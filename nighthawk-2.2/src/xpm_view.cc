/***************************************************************************
*
* For viewing xpm's..
*
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
}
#include "nighthawk_defs.h"
#include "misc.h"

/***************************************************************************
*
***************************************************************************/
int main(void)
{
  if(x_init())
  {
    tbm bm;

    XMapWindow(display,window);
    XSync(display,0);
    if(loadxpm("data/Tobruk/map.xpm",&bm))
    {
      register int running = 1;

      while(running)
      {
        drawxpm_ani(&bm,0,0,0,1);
        XCopyArea(display,render_screen,window,gc_bgblt,0,0,
          window_width,window_height,0,0);
        while(XPending(display))
        {
          XEvent event;

          XNextEvent(display,&event);
          switch(event.type)
          {
            case ButtonPress:
              int win_x_return,win_y_return;
              Window a,b;
              int c,d;
              unsigned int e;

              XQueryPointer(display,window,&a,&b,&c,&d,
                &win_x_return,&win_y_return,&e);
              printf("%d,%d\n",win_x_return,win_y_return);
              break;
            case KeyPress:
              switch(XLookupKeysym((XKeyEvent *)&event,0))
              {
                case KEY_QUIT:
                  running = 0;
                  break;
              }
              break;
          }
        }
        sleep(1);
      }
      free_bm(&bm);
    }
    Xreaper();
  }
  return 1;
}
