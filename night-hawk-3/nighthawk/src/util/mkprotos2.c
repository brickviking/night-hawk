/****************************************************************************
* mkprotos- prototype generation utility, 2nd Rev, patch 1
* By Jason Nunn <jsno@leal.com.au>, Sept 2003
*
* it couldn't hack the small precludes code.. i had to
* revise it
****************************************************************************/
#include <stdio.h>
#include <string.h>
/* we'll comment this out, as it's probably no longer needed
#include <malloc.h>
*/
#include <stdlib.h>

FILE *fp_in;

char *tok_public = "PUBLIC ",
     *tok_private = "PRIVATE ";
int  tok_public_size,
     tok_private_size;

int  c, record_f;
char *buf;
int  buf_size, buf_data_size;

/****************************************************************************
*
****************************************************************************/
void reset_buf(void)
{
  if(buf == NULL)
    return;
  free(buf);
  buf = NULL;
  buf_size = buf_data_size = 0;
}

void add_buf(void)
{
  if(buf_data_size >= buf_size)
  {
    buf_size += 1024;
    buf = realloc(buf, buf_size);
    if(buf == NULL)
    {
      perror("realloc()");
      exit(-1);
    }
  }

  buf[buf_data_size] = c;
  buf_data_size++;
}

int get_char(void)
{
  if(feof(fp_in))
    return -1;
  return getc(fp_in);
}

/****************************************************************************
*
****************************************************************************/
int ignore_comment(void)
{
  for(;;)
  {
    c = get_char();
    if(c == -1)
      return 0;

    if(c == '*')
    {
      c = get_char();
      if(c == -1)
        return 0;
      if(c == '/')
        return 1;
      ungetc(c, fp_in);
    }
  }
}

int ignore_comment2(void)
{
  for(;;)
  {
    c = get_char();
    if(c == -1)
      return 0;

    if(c == '\n')
      return 1;
  }
}

/****************************************************************************
*
****************************************************************************/
void remove_string(char **s, char *e)
{
  **s = ' ';
  for(; *s <= e; (*s)++)
  {
    if(**s == '"')
    {
      **s = ' ';
      break;
    }

    **s = ' ';
  }
}

void remove_array(char **s, char *e)
{
  char *s2;

  **s = ' ';
  for(; *s <= e; (*s)++)
  {
    if(**s == '{')
    {
      s2 = *s;
      remove_array(&s2, e);
    }

    if(**s == '}')
    {
      **s = ' ';
      break;
    }

    **s = ' ';
  }
}

void remove_value(char **s, char *e)
{
  for(; *s <= e; (*s)++)
  {
    if(**s == ',' || **s == ';')
      break;

    if(**s == '"')
    {
      remove_string(s, e);
      continue;
    }

    if(**s == '{')
    {
      remove_array(s, e);
      continue;
    }

    **s = ' ';
  }
}

void remove_values(char *s, char *e)
{
  char *ss, *sss, oc = 0;

  for(ss = s; ss <= e; ss++)
  {
    if(*ss == '=')
      remove_value(&ss, e);
    if(*ss == ';')
      break;
  }

  for(sss = ss = s; ss <= e; ss++)
  {
    if(!(oc == ' ' && *ss == ' '))
    {
      *sss = *ss;
      sss++;
    }
    oc = *ss;
  }
  *sss = 0;
}

/****************************************************************************
*
****************************************************************************/
void extract_protos(void)
{
  int c2, i, i2, oldc;

restart:
  oldc = 0;
  record_f = 0;
  reset_buf();
  for(;;)
  {
    c = get_char();
    if(c == -1)
      break;

    if(c == '/')
    {
      c2 = get_char();
      if(c2 == -1)
        break;

      if(c2 == '*')
      {
        if(!ignore_comment())
          break;
        continue;
      }
      else if(c2 == '/')
      {
        if(!ignore_comment2())
          break;
        continue;
      }
      else
        ungetc(c2, fp_in);
    }

    if(c == '\n' || c == '\t')
      c = ' ';

    if(oldc == ' ' && c == ' ')
      continue;
    oldc = c;

    add_buf();

    if(!record_f)
    {
      i = buf_data_size - tok_private_size;
      if(i >= 0)
        if(!strncmp(tok_private, buf + i, tok_private_size))
          goto restart;

      i = buf_data_size - tok_public_size;
      if(i >= 0)
        if(!strncmp(tok_public, buf + i, tok_public_size))
        {
          printf("extern ");
          record_f = 1;
          reset_buf();
        }

      continue;
    }

    if(c == ';')
    {
      for(i = 0; i < (buf_data_size - 2); i++)
      {
        if(buf[i] == '=')
          break;

        if(buf[i] == ')' && buf[i + 2] == '{')
        {
          fwrite(buf, 1, 1 + i, stdout);
          printf(";\n");

          for(i2 = buf_data_size - 1; i2 >= i + 3; i2--)
            ungetc(buf[i2], fp_in);

          goto restart;
        }
      }

      remove_values(buf, buf + (buf_data_size - 1));
      printf("%s\n", buf);
      goto restart;
    }
  }
}

/****************************************************************************
*
****************************************************************************/
int main(int c, char *v[])
{
  int i;

  tok_public_size  = strlen(tok_public);
  tok_private_size  = strlen(tok_private);

  for(i = 1; i < c; i++)
  {
    fp_in = fopen(v[i], "r");
    if(fp_in == NULL)
      continue;
    extract_protos();
    fclose(fp_in);
  }

  return 0;
}
