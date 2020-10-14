/***************************************************************************
****************************************************************************
****************************************************************************
* Nighthawk
* Copyright (C) 1996-2004 Jason Nunn et al
* Developed by Jason Nunn et al (see README for Acknowledgements)
*
* Project head and maintainer--
*   Eric Gillespie (The Viking)
*   Email: viking667@users.sourceforge.net
*   http://night-hawk.sourceforge.net
*
* Original Author--
*   Jason Nunn
*   http://jsno.leal.com.au
*
*   C/- Jason Nunn
*   PO Box 15
*   Birdwood SA 5234
*   Australia
*
* This software is Open Source GPL. Please read COPYING notice.
* =====================================================================
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"
#include "protos.p"

//keyboard events
PUBLIC tkevent kevent[KEVENT_SIZE];
PUBLIC int     kevent_r_ptr, kevent_w_ptr;

/****************************************************************************
*
****************************************************************************/
PUBLIC void malloc_error_h(void)
{
  printf("Memory Error: aborting.\n");
  end_game(-1);
}

PUBLIC void *alloc_mem(int mem_size)
{
  void *r;

  r = malloc(mem_size);
  if(r == NULL)
    malloc_error_h();

  return r;
}

PUBLIC void *realloc_mem(void *old_ptr, int mem_size)
{
  void *r;

  r = realloc(old_ptr, mem_size);
  if(r == NULL)
    malloc_error_h();

  return r;
}

/****************************************************************************
*
****************************************************************************/
PUBLIC void put_kb_event(tkevent *k)
{
  memcpy(&kevent[kevent_w_ptr], k, sizeof(tkevent));
  kevent_w_ptr++;
  kevent_w_ptr &= (KEVENT_SIZE - 1);
}

PUBLIC int get_kb_event(tkevent *k)
{
  if(kevent_r_ptr == kevent_w_ptr)
    return 0;

  memcpy(k, &kevent[kevent_r_ptr], sizeof(tkevent));
  kevent_r_ptr++;
  kevent_r_ptr &= (KEVENT_SIZE - 1);
  return 1;
}
