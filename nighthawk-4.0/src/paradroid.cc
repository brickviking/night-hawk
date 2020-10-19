/***************************************************************************
****************************************************************************
****************************************************************************
*
* Nighthawk Copyright (C) 1997 Jason Nunn
* Nighthawk is under the GNU General Public License.
*
* Sourceforge admin: Eric Gillespie
* night-hawk.sourceforge.net
*
* See README file for a list of contributors.
*
* ----------------------------------------------------------------
* paradroid object routines
*
****************************************************************************
****************************************************************************
***************************************************************************/
extern "C" {
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "math.h"
#include "defs.h"
#include "linked_data_externs.h"
#include "misc_externs.h"
#include "sprite_loader_externs.h"
#include "opengl_externs.h"
#include "scores_externs.h"
}
#include "tparadroid.h"

/***************************************************************************
*
***************************************************************************/
tparadroid::tparadroid(void)
{
  memcpy(&stats, &droid_stats[DROID_002], sizeof(tdroid_stats));
  transferring = 0;

 /*Rafael Laboissiere*/
  cascade = 0;

  captured_droid = NULL;
  low_shields_trig = 0;
}

tparadroid::~tparadroid(void)
{ 
}

void tparadroid::init(tfloor *f, tentity **dp, float px, float py)
{
  tentity::init(f, &paradroid_bm[0], dp, 10, px, py);
}

void tparadroid::draw(void)
{
  int        x;
  static int pos[4][2] =
     {{-16 + SCREEN_HSIZE_X, -17 + SCREEN_HSIZE_Y},
      {-17 + SCREEN_HSIZE_X,  15 + SCREEN_HSIZE_Y},
      { 14 + SCREEN_HSIZE_X,  15 + SCREEN_HSIZE_Y},
      { 14 + SCREEN_HSIZE_X, -17 + SCREEN_HSIZE_Y}};

  draw_laser(floor_ptr, (int)pos_x, (int)pos_y);

  switch(state)
  {
    case 0:
      draw_droid(SCREEN_HSIZE_X, SCREEN_HSIZE_Y);

      if(transferring)
      {
        glColor4f(0.8, 0.8, 0.8, fabs(sinf(shield_wave_inc))); // Fixed. 03SEP20
        for(x = 0; x < 4; x++)
          blit_rect(pos[x][X], pos[x][Y], 3, 3);

        if(captured_droid != NULL)
        {
          int np, nv;

          np = SCREEN_HSIZE_X - (44 >> 1);

          glEnable(GL_LINE_STIPPLE);
          glLineWidth(5.0);
          glLineStipple(2, 0xf7f7);
          glColor3fv(red_pixel[0]);
          blit_line(
            SCREEN_HSIZE_X,
            SCREEN_HSIZE_Y,
            SCREEN_HSIZE_X + captured_droid->spos_x - spos_x,
            SCREEN_HSIZE_Y + captured_droid->spos_y - spos_y);
          glDisable(GL_LINE_STIPPLE);

          glColor4fv(bm_colour);
          blit_bm(&neg_bm, np, 50);

          nv = ((tug1 + -tug2) >> 2) + 20;
          if(nv < 0) nv = 0;
          if(nv > 40) nv = 40;
          nv += 2;

          glLineWidth(8.0);
          glLineStipple(1, 0xffff);
          glColor3fv(green_pixel[0]);
          blit_rect(np + 2, 61, nv, 3);
        }
      }
      break;

    case 1:
      glColor4fv(bm_colour);
      blit_bm(
        &explosion_bm[ss_expl_anim_ptr >> 4],  // JN, 04SEP20
        SCREEN_HSIZE_X - 20, SCREEN_HSIZE_Y - 20);
      break;
  }
}

int tparadroid::within_capt_dist(void)
{
  int d1, d2;

  d1 = (int)(captured_droid->pos_x - pos_x);
  d2 = (int)(captured_droid->pos_y - pos_y);
  if(((d1 * d1) + (d2 * d2)) > (100 * 100))
  {
    captured_droid->captured = 0;
    captured_droid = NULL;
    return 0;
  }

  return 1;
}

/*
 * Added special keys detection here. JN, 01SEP20
 */
void tparadroid::bg_calc(void)
{
  static float handicap[10] =
    {6.0, 5.5, 5.0, 4.5, 4.0, 3.5, 3.2, 3.0, 2.7, 2.5};

  tdroid::bg_calc();

  if(state)  // Reduce nesting. I was such a Pascal programmer in 1996 LOL. JN, 01SEP20
    return;

  friction();

 /*
  * Added special key handling here. Update pos_dx and pos_dyy and after
  * friction().
  * JN, 01SEP20
  */
  float das = stats.speed * DIAGONAL_MOVE_CO;
  switch(get_special_key_state())
  {
    case SP_KEY_STATE_UP:
    pos_dx = 0;
    pos_dy = -stats.speed;
    break;

    case SP_KEY_STATE_DOWN:
    pos_dx = 0;
    pos_dy = stats.speed;
    break;

    case SP_KEY_STATE_LEFT:
    pos_dx = -stats.speed;
    pos_dy = 0;
    break;

    case SP_KEY_STATE_RIGHT:
    pos_dx = stats.speed;
    pos_dy = 0;
    break;

   /*
    * This is based on Eric's diagonal keyboard code.
    * JN, 01SEP20
    */
    case SP_KEY_STATE_UP_LF:
      pos_dx = -das; // One little thing Eric forgot to add in 2.3
      pos_dy = -das; // Diagonal adjusted speed. Pythagoras's theorm.
      break;

    case SP_KEY_STATE_UP_RT:
      pos_dx = das;
      pos_dy = -das;
      break;

    case SP_KEY_STATE_DN_LF:
      pos_dx = -das;
      pos_dy = das;
      break;

    case SP_KEY_STATE_DN_RT:
      pos_dx = das;
      pos_dy = das;
      break;
  }

  if(stats.shielding <= (stats.max_shielding >> 2))
  {
    if(!low_shields_trig)
    {
      low_shields_trig = 25;
      sound_cmd(SND_CMD_FX_MIXED, FX_LOW_SHIELDS, 0.3, 1.2, 0.0, 0.0);
    }
    else
      low_shields_trig--;
  }

  if(captured_droid != NULL)
  {
    if(!within_capt_dist())
      console_message_add((char *)"lost contact");
    else
    {
      if(captured_droid->state)
      {
        console_message_add((char *)"Forced detach");
        captured_droid = NULL;
      }
      else
      {
        int tt;

        tug1 +=
          (int)rintf(handicap[stats.entry] *
          (float)stats.entry * ((float)random() / RAND_MAX));

        tug2 +=
          (int)rintf(2.0 * (float)captured_droid->stats.entry *
         ((float)random() / RAND_MAX));

        tt = tug1 + -tug2;
        if(tt <= -80)
        {
          console_message_add((char *)"Burn out!");
          state = 1;
          captured_droid = NULL;
          do_sound(FX_EXPLOSION_1, 1.0, 1.0);
        }
        else
        {
          if(tt >= 80)
          {
            float tx, ty, tdx, tdy;

            if(!god_mode)
              score += captured_droid->stats.entry * 100; // 2x score. JN, 10SEP20

            memcpy(&stats,
              &(captured_droid->stats), sizeof(tdroid_stats));
            tx = pos_x;
            ty = pos_y;
            tdx = pos_dx;
            tdy = pos_dy;
            pos_x = captured_droid->pos_x;
            pos_y = captured_droid->pos_y;
            pos_dx = captured_droid->pos_dx;
            pos_dy = captured_droid->pos_dy;
            captured_droid->pos_x = tx;
            captured_droid->pos_y = ty;
            captured_droid->pos_dx = tdx;
            captured_droid->pos_dy = tdy;
            captured_droid->state = 1;
            captured_droid = NULL;
            do_sound(FX_EXPLOSION_1, 1.0, 1.0);
            do_sound(FX_POWER_UP, 1.0, 1.2);

           /*Rafael Laboissiere*/
            if(!cascade)
              transferring = 0;

            console_message_add((char *)"Transferred!!");
            tdroid::display_nd();
          }
        }
      }
    }
  }
}

void tparadroid::capture_droid(int cx, int cy)
{
  int  x;
  char str[STR_LABEL_LEN + 1];

  for(x = 0; x < MAX_DROIDS; x++)
    if(*(droids + x) != NULL)
    {
      if((*(droids + x) != this) && !(*(droids + x))->state &&
        ((*(droids + x))->floor_ptr == floor_ptr))
      {
        float ix, iy;

        ix = cx - (*(droids + x))->pos_x + SPRITE_HSIZE_X;
        iy = cy - (*(droids + x))->pos_y + SPRITE_HSIZE_Y;
        if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
           (iy >= 0) && (iy < SPRITE_SIZE_Y))
        {
          captured_droid = (tparadroid *)*(droids + x);
          captured_droid->captured = 1;
          tug1 = tug2 = 0;

          if(within_capt_dist())
            snprintf(str, STR_LABEL_LEN, "%s captured",
				captured_droid->stats.type);
          else
          {
            snprintf(str, STR_LABEL_LEN, "Out of range");
            sound_cmd_primary(FX_NO_WEAPON, 0.5, 2.0);
          }
          console_message_add(str);
          return;
        }
      }
    }
    else
      break;

  console_message_add((char *)"Empty Space");
  sound_cmd_primary(FX_NO_WEAPON, 0.5, 2.0);
}

int tparadroid::action(tkevent *kp)
{
  int         s = 0;
  const char *no_weapon_mess = "No weapon!";

  if(state)
    return 0;

  switch(kp->type)
  {
    case MOUSE_EVENT:
      switch(kp->val)
      {
        case GLUT_LEFT_BUTTON:
          if(transferring)
          {
            if(captured_droid == NULL)
              capture_droid(
                kp->mouse_x + ((int)pos_x - SCREEN_HSIZE_X),
                kp->mouse_y + ((int)pos_y - SCREEN_HSIZE_Y));
          }
          else
          {
            if(stats.armament)
            {
              create_laser(
                (kp->mouse_x - SCREEN_HSIZE_X),
                (kp->mouse_y - SCREEN_HSIZE_Y));
            }
            else
            {
              console_message_add((char *)no_weapon_mess);
              sound_cmd_primary(FX_NO_WEAPON, 0.5, 2.0);
            }
          }
          break;

        case GLUT_RIGHT_BUTTON:
          if(!transferring)
          {
            if(stats.armament)
            {
              create_laser(-10000,     0);
              create_laser( 10000,     0);
              create_laser(     0,-10000);
              create_laser(     0, 10000);
              create_laser(-10000,-10000);
              create_laser( 10000,-10000);
              create_laser(-10000, 10000);
              create_laser( 10000, 10000);
            }
            else
            {
              console_message_add((char *)no_weapon_mess);
              sound_cmd_primary(FX_NO_WEAPON, 0.5, 2.0); // JN 16OCT20
            }
          }
          break;
      }
      break;

    case KEYBOARD_EVENT:
      switch(tolower(kp->val))  // tolower(). Tanel's influence. JN, 05SEP20
      {
        case KEY_STATUS:
          display_nd();
          sound_cmd_primary(FX_SELECT, 1.0, 1.0);
          break;

       /*Rafael Laboissiere*/
        case KEY_CASCADE_MODE:
          cascade = 1;

        case KEY_TRANSFER_MODE:
        case KEY_TRANSFER_MODE_ALT: // Eric's key preference. JN, 01SEP20
          if(captured_droid == NULL)
          {
            transferring ^= 1;
            if(transferring)
            {
              sound_cmd_primary(FX_SELECT, 1.0, 1.0);
            }
            else
            {
             /*Rafael Laboissiere*/
              cascade = 0;
              sound_cmd_primary(FX_WEAPON_RELOAD, 1.0, 1.0);
            }
          }
          else
          {
            console_message_add((char *)"You're commited!");
            sound_cmd_primary(FX_NO_WEAPON, 0.5, 2.0);
          }
          break;

        case KEY_SELECT:
          if(captured_droid != NULL)
          {
            console_message_add((char *)"Can't trans.");
            sound_cmd_primary(FX_NO_WEAPON, 0.5, 2.0);
          }
          else
          {
            int x, rpx, rpy;

            rpx = spos_x + SPRITE_HSIZE_X;
            rpy = spos_y + SPRITE_HSIZE_Y;

            for(x = 0; x < MAX_TRANSPORTS; x++)
              if(floor_ptr->transport[x].x != -1)
              {
                int ix, iy;

                ix = rpx - floor_ptr->transport[x].x;
                iy = rpy - floor_ptr->transport[x].y;
                if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
                   (iy >= 0) && (iy < SPRITE_SIZE_Y))
                {
                  pos_dx = 0;
                  pos_dy = 0;
                  s = 1;
                  sound_cmd_primary(FX_SELECT, 1.0, 1.0);
                  break;
                }
              }

            if(floor_ptr->console.x != -1)
            {
              int ix, iy;

              ix = rpx - floor_ptr->console.x;
              iy = rpy - floor_ptr->console.y;
              if((ix >= 0) && (ix < SPRITE_SIZE_X) &&
                 (iy >= 0) && (iy < SPRITE_SIZE_Y))
              {
                pos_dx = 0;
                pos_dy = 0;
                s = 2;
                sound_cmd_primary(FX_SELECT, 1.0, 1.0);
                break;
              }
            }
          }
          break;
      }
      break;

//    case SPECIAL_EVENT:  See bg_calc(). JN, 01SEP20
//      break;
  }

  return s;
}

void tparadroid::display_nd(void)
{
  char str[STR_LABEL_LEN + 1];

  snprintf(str, STR_LABEL_LEN, "Shield=%d", stats.shielding);
  console_message_add(str);
  tdroid::display_nd();
}

void tparadroid::hit(tlaser *l)
{
  char str[STR_LABEL_LEN + 1];

  if(!god_mode)
  {
    tdroid::hit(l);
    return;
  }

  snprintf(str, STR_LABEL_LEN,
    "%s h %s(god)", l->owner->stats.type, stats.type);
  console_message_add(str);

  do_sound(FX_DROID_HIT_1, 0.8, 1.0);
}
