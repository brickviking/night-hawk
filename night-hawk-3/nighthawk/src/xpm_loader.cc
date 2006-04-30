/****************************************************************************
* Small Precludes OpenGL demo
*
* By Jason Nunn <jsno8192@yahoo.com.au>
* (C) Copyright 2000-2003
* Darwin, NT, Australia
* http://jsno.arafuraconnect.com.au
*
* =====================================================================
* XPM loader... a quick and dirty hack.
*
* note// the formal xpm library normalises data.. and RGB pics turn out to
* be something else, and i can't feed 'em to glut texture routines.
*
* so i wrote my own.
***************************************************************************/
#include "defs.h"
#include "protos.p"

#define XPM_STR_LEN 512

typedef struct
{
  int id, r, g, b;
} tcolour;

PRIVATE FILE          *fp;
PRIVATE unsigned int  size_x,
                      size_y,
                      no_colours,
                      id_size;

PRIVATE tcolour       *colour_tab;
PRIVATE unsigned char *bitmap_buf;
PRIVATE int           transparent_pixel, transparent_f;

/***************************************************************************
*
***************************************************************************/
PRIVATE int read_ln(char *strln)
{
  if(fgets(strln,XPM_STR_LEN,fp) == NULL)
    return 0;
  return 1;
}

PRIVATE void parse_error(void)
{
  fprintf(stderr,"Error: Parsing XPM file, Aborting.\n");
  end_game(-1);
}

PRIVATE void rm_inverted_commas(char *str)
{
  char *s;

  s = strchr(str,'\"');
  if(s == NULL)
    return;
  *s = ' ';

  s = strchr(str,'\"');
  if(s == NULL)
    return;
  *s = 0;
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void parse_header(void)
{
  char strln[XPM_STR_LEN + 1];

  for(;;)
  {
    if(feof(fp))
      return;

    if(!read_ln(strln))
      parse_error();

    if(strchr(strln,'\"') != NULL)
      break;
  }

  rm_inverted_commas(strln);

  if(sscanf(strln,"%d %d %d %d",
    &size_x, &size_y, &no_colours, &id_size) != 4)
    parse_error();
}

/***************************************************************************
*
***************************************************************************/
PRIVATE int quan_id(char *str)
{
  unsigned int r = 0, x;

  for(x = 0; x < id_size; x++)
    r |= str[x] << (8 * x);

  return r;
}

PRIVATE int qsort_compar(const void *n1, const void *n2)
{
  return ((tcolour *)n1)->id - ((tcolour *)n2)->id;
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void parse_colour_block(void)
{
  char               strln[XPM_STR_LEN + 1],
                     type[32], value_str[32], *s;
  unsigned int       n = 0;
  unsigned long long x, t;

  for(n = 0; n < no_colours; n++)
  {

    if(feof(fp))
      return;
   
    if(!read_ln(strln))
      parse_error();

    s = strchr(strln, '\"');
    if(s == NULL)
      parse_error();
    s++;

    colour_tab[n].id = quan_id(s);
    s += id_size;

    if(sscanf(s, "%s %s", type, value_str) != 2)
      parse_error();

    s = strchr(value_str, '\"');
    if(s == NULL)
      parse_error();
    *s = 0;

    if((*type != 'c') && (*type != 'g'))
    {
      fprintf(stderr, "Warning: unknown colour type found. Aborting.\n");
      end_game(-1);
    }

    if(!strcmp(value_str, "None"))
    {
      transparent_f = 1;
      transparent_pixel = colour_tab[n].id;
      colour_tab[n].r = 0;
      colour_tab[n].g = 0;
      colour_tab[n].b = 0;
    }
    else
    {
      if(value_str[0] != '#')
      {
        fprintf(stderr, "Warning: Invalid colur value found. Aborting.\n");
        end_game(-1);
      }

      s = value_str + 1;
      if(sscanf(s, "%llx", &x) == 1)
      {
        switch(strlen(s))
        {
          case 6:
            colour_tab[n].r = x >> 16;
            colour_tab[n].g = (x >> 8) & 0xff;
            colour_tab[n].b = x & 0xff;
            break;

          case 12:
            t = x >> (16 * 2);
            colour_tab[n].r = t >> 8;

            t = (x >> 16) & 0xffff;
            colour_tab[n].g = t >> 8;

            t = x & 0xffff;
            colour_tab[n].b = t >> 8;
            break;
        }
      }
    }
  }
}

/***************************************************************************
*
***************************************************************************/
PRIVATE void parse_bitmap(void)
{
  unsigned char *bm_ptr;
  char          strln[XPM_STR_LEN + 1], *s;
  tcolour       key, *c;

  bm_ptr = bitmap_buf;
  for(;;)
  {
    if(feof(fp))
      return;
  
    if(!read_ln(strln))
      return;

    s = strchr(strln,'\"');
    if(s != NULL)
    {
      s++;
      for(;;)
      {
        if(*s == '\"')
          break;

        key.id = quan_id(s);
        c = (tcolour *)bsearch(
          &key, colour_tab, no_colours, sizeof(tcolour), qsort_compar);
        if(c == NULL)
        {
          fprintf(stderr,"Error: couldn't match colour. Aborting.\n");
          end_game(-1);
        }
        s += id_size;
        bm_ptr[0] = c->r;
        bm_ptr[1] = c->g;
        bm_ptr[2] = c->b;

        if(transparent_f && key.id == transparent_pixel)
          bm_ptr[3] = 0;
        else
          bm_ptr[3] = 0xff;

        bm_ptr += 4;
      }
    }
  }
}

/***************************************************************************
*
***************************************************************************/
#ifdef WIN32
PRIVATE void winshit_convert(void)
{
  unsigned char *ptr, temp;
  int i;

  for(i = ssize, ptr = bits; i > 0; i--, ptr += 3)
  {
    temp = ptr[0];
    ptr[0] = ptr[2];
    ptr[2] = temp;
  }
}
#endif

/***************************************************************************
*
***************************************************************************/
PUBLIC void load_xpm(
  char *filename, unsigned char **buf, unsigned int *sx, unsigned int *sy)
{
  char str[2048 + 1];

  transparent_f = 0;
  strcpy(str, INSTALL_DIR "/data/");
  strcat(str, filename);
  fp = fopen(str, "r");
  if(fp == NULL)
  {
    fprintf(stderr, "Error: load_xpm()::fopen()\n");
    end_game(-1);
  }

  parse_header();

  if(!size_x || !size_y || (id_size > 4))
  {
    fprintf(stderr, "Error: Invalid header parameters, Aborting.\n");
    end_game(-1);
  }

  colour_tab = (tcolour *)alloc_mem(no_colours * sizeof(tcolour));
  parse_colour_block();
  qsort(colour_tab, no_colours, sizeof(tcolour), qsort_compar);
  *buf = bitmap_buf = (unsigned char *)alloc_mem(size_x * size_y * 4);
  parse_bitmap();

#ifdef WIN32
  winshit_convert();
#endif

  fclose(fp);

  if(!size_x || !size_y)
    fprintf(stderr, "Error: invalid XPM dimensions.\n");
  free(colour_tab);

  *sx = size_x;
  *sy = size_y;
}
