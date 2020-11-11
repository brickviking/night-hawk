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
* This is inline data (as opposed to loaded data). I've got old man
* eyes now and got sick of scrolling though pages of droid_stats array in
* misc.c. So this is to make the code easier to scroll through. This file
* shouldn't ever get touched again now.- JN, 25AUG20
*
****************************************************************************
****************************************************************************
***************************************************************************/
#include "defs.h"

const unsigned int division_table[] = {
	10000000,
	1000000,
	100000,
	10000,
	1000,
	100,
	10,
	1,
};

/*
 * Circumfence of a droid. Used for colision detection in tentity. The
 * "tentity" is the reference, so substract the droid position from the
 * other entity you want to compare before testing these points. JN, 26SEP20
 */
col_droid_perimeter_st col_droid_perimeter[COL_DROID_PER_SIZE] = {
	{11, 0}, /*0 deg*/
	{-11, -0}, /*180 deg*/
	{-0, 11}, /*90 deg*/
	{-0, -11}, /*270 deg*/
	{8, 8}, /*45 deg*/
	{-8, -8}, /*225 deg*/
	{-8, 8}, /*135 deg*/
	{8, -8}, /*315 deg*/
	{-5, 10}, /*118 deg*/
	{7, 9}, /*51 deg*/
	{10, -5}, /*332 deg*/
	{10, -4}, /*337 deg*/
	{10, 5}, /*28 deg*/
	{-2, -11}, /*259 deg*/
	{9, -7}, /*321 deg*/
	{11, 1}, /*6 deg*/
	{-10, -5}, /*208 deg*/
	{-10, -4}, /*203 deg*/
	{4, 10}, /*68 deg*/
	{-3, 11}, /*107 deg*/
	{3, 11}, /*73 deg*/
	{11, -2}, /*349 deg*/
	{-11, -1}, /*186 deg*/
	{11, -3}, /*343 deg*/
	{2, -11}, /*281 deg*/
	{-10, 5}, /*152 deg*/
	{5, -10}, /*298 deg*/
	{-2, 11}, /*101 deg*/
	{2, 11}, /*79 deg*/
	{-1, -11}, /*264 deg*/
	{-7, 9}, /*129 deg*/
	{-6, -9}, /*236 deg*/
	{10, 4}, /*23 deg*/
	{-10, 4}, /*158 deg*/
	{-11, 2}, /*169 deg*/
	{-11, -2}, /*191 deg*/
	{-9, 6}, /*146 deg*/
	{7, -9}, /*309 deg*/
	{-3, -11}, /*253 deg*/
	{-6, 9}, /*124 deg*/
	{-9, 7}, /*141 deg*/
	{11, -1}, /*354 deg*/
	{5, 10}, /*62 deg*/
	{9, -6}, /*326 deg*/
	{-7, -9}, /*231 deg*/
	{-11, -3}, /*197 deg*/
	{-9, -7}, /*219 deg*/
	{-5, -10}, /*242 deg*/
	{11, 3}, /*17 deg*/
	{4, -10}, /*292 deg*/
	{9, 7}, /*39 deg*/
	{-1, 11}, /*96 deg*/
	{1, -11}, /*276 deg*/
	{-11, 3}, /*163 deg*/
	{-4, -10}, /*248 deg*/
	{-9, -6}, /*214 deg*/
	{6, -9}, /*304 deg*/
	{-11, 1}, /*174 deg*/
	{-4, 10}, /*113 deg*/
	{11, 2}, /*11 deg*/
	{3, -11}, /*287 deg*/
	{6, 9}, /*56 deg*/
	{1, 11}, /*84 deg*/
	{9, 6} /*34 deg*/
};

const tdroid_stats droid_stats[] = {
  {
    "002",
    "Paradroid",
    1,
    1.0,
    27.0,
    "Neurologic",
    LASER_LINARITE,
    10,
    10,
    5,
    0
  },
  {
    "108",
    "Minor Cleaning Droid",
    1,
    0.6,
    38.3,
    "Embedded Logic",
    LASER_NONE,
    1,
    1,
    2,
    -1
  },
  {
    "176",
    "Standard Cleaning Droid",
    1,
    1.2,
    80.6,
    "Embedded Logic",
    LASER_NONE,
    5,
    5,
    3,
    -1
  },
  {
    "261",
    "Cargo Paletting Droid",
    2,
    3,
    800.75,
    "Embedded Logic",
    LASER_NONE,
    500,
    500,
    1,
    -1
  },
  {
    "275",
    "Servant Droid",
    2,
    0.4,
    15.3,
    "Embedded Logic",
    LASER_NONE,
    15,
    15,
    5,
    -1
  },
  {
    "355",
    "Messenger (Class C) Droid",
    3,
    0.5,
    10.0,
    "Embedded Logic",
    LASER_NONE,
    10,
    10,
    6,
    -1
  },
  {
    "368",
    "Messenger (Class D) Droid",
    3,
    0.76,
    11.0,
    "Embedded Logic",
    LASER_NONE,
    20,
    20,
    5,
    -1
  },
  {
    "423",
    "Standard Maintenance Droid",
    4,
    2,
    60.0,
    "Embedded Logic",
    LASER_NONE,
    20,
    20,
    3,
    -1
  },
  {
    "467",
    "Heavy Duty Maintenance Droid",
    4,
    1.5,
    100.0,
    "Embedded Logic",
    LASER_CROC_BENZ,
    25,
    25,
    3,
    -1
  },
  {
    "489",
    "Aerial Maintenance Droid",
    4,
    0.2,
    5.0,
    "Embedded Logic",
    LASER_LINARITE,
    25,
    25,
    6,
    -1
  },
  {
    "513",
    "Trauma Technician",
    5,
    1,
    40.0,
    "Neurologic",
    LASER_CROC_BENZ,
    30,
    30,
    3,
    0
  },
  {
    "520",
    "Nurse Droid",
    5,
    0.6,
    50.0,
    "Neurologic",
    LASER_CROC_BENZ,
    35,
    35,
    4,
    0
  },
  {
    "599",
    "Surgeon Droid",
    5,
    2,
    56.80,
    "Neurologic",
    LASER_UVAROVITE,
    45,
    45,
    4,
    0
  },
  {
    "606",
    "Guard Monitor",
    6,
    1.1,  
    20.0,
    "Neurologic",
    LASER_CROC_BENZ,
    15,
    15,
    4,
    20
  },
  {
    "691",
    "Sentinel Droid",
    6,
    1.9,
    45.0,
    "Neurologic",
    LASER_CROC_BENZ,
    40,
    40,
    5,
    17
  },
  {
    "693",
    "Centurion Droid",
    6,
    2.0,  
    50.0,
    "Neurologic",
    LASER_UVAROVITE,
    50,
    50,
    3,
    20
  },
  {
    "719",
    "Battle Droid",
    7,
    1.5,
    40.92,
    "Neurologic",
    LASER_CROC_BENZ,
    40,
    40,
    2,
    20
  },
  {
    "720",
    "Flank Droid",
    7,
    1.0,
    30.10,
    "Neurologic",
    LASER_CROC_BENZ,
    30,
    30,
    6,
    20
  },
  {
    "766",
    "Attack Droid",
    7,
    1.9,
    67.45,
    "Neurologic",
    LASER_UVAROVITE,
    70,
    70,
    4,
    15
  },
  {
    "799",
    "Heavy Battle Droid",
    7,
    2.0,
    90.98,
    "Neurologic",
    LASER_UVAROVITE,
    100,
    100,
    3,
    12
  },
  {
    "805",
    "Communications Droid",
    8,
    0.8,
    50.10,
    "Neurologic",
    LASER_UVAROVITE,
    70,
    70,
    3,
    15
  },
  {
    "810",
    "Engineer Droid",
    8,
    1.74,
    60.00,
    "Neurologic",
    LASER_UVAROVITE,
    80,
    80,
    2,
    14
  },
  {
    "820",
    "Science Droid",
    8,
    1.0,
    40.00,
    "Neurologic",
    LASER_UVAROVITE,
    90,
    90,
    3,
    13
  },
  {
    "899",
    "Pilot Droid",
    8,
    1.74,
    60.00,
    "Neurologic",
    LASER_UVAROVITE,
    90,
    90,
    2,
    12
  },
  {
    "933",
    "Minor Cyborg",
    9,
    3.1,
    120.0,
    "Neurologic",
    LASER_TIGER_EYE,
    20,
    20,
    4,
    20
  },
  {
    "949",
    "Command Cyborg",
    9,
    2.6,
    100.0,
    "Neurologic",
    LASER_TIGER_EYE,
    70,
    70,
    5,
    15
  },
  {
    "999",
    "Fleet Cyborg",
    9,
    3.1,
    90.95,
    "Neurologic",
    LASER_TIGER_EYE,
    200,
    200,
    6,
    10
  }
};
