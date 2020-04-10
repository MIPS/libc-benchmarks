/* Copyright (C) 2018-2020 MIPS Tech LLC
   This file is part of c-library bench-mark

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA. */

# define TEST_NAME "jump opt"
# define MAX_BUFSIZE (1 << 16)
# include "bench-string.h"

#define MAX_JUMP_CASES 14
#define CACHE_SIZE 64*1024

#ifndef TEST_CFLAGS
#define TEST_CFLAGS " "
#endif

#include <stdio.h>
#include <stdlib.h>

int jumptab13 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 13198;break;
    case 1:
      b = 13018;break;
    case 2:
      b = 13703;break;
    case 3:
      b = 13361;break;
    case 4:
      b = 13175;break;
    case 5:
      b = 13427;break;
    case 6:
      b = 13532;break;
    case 7:
      b = 13352;break;
    case 8:
      b = 13839;break;
    case 9:
      b = 13904;break;
    case 10:
      b = 13457;break;
    case 11:
      b = 13755;break;
    case 12:
      b = 13951;break;
    }
  return b;
}

int jumptab12 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 12198;break;
    case 1:
      b = 12818;break;
    case 2:
      b = 12773;break;
    case 3:
      b = 12365;break;
    case 4:
      b = 12575;break;
    case 5:
      b = 12467;break;
    case 6:
      b = 12537;break;
    case 7:
      b = 12852;break;
    case 8:
      b = 12819;break;
    case 9:
      b = 12900;break;
    case 10:
      b = 12257;break;
    case 11:
      b = 12792;break;
    }
  return b;
}

int jumptab11 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 11998;break;
    case 1:
      b = 11888;break;
    case 2:
      b = 11777;break;
    case 3:
      b = 11665;break;
    case 4:
      b = 11555;break;
    case 5:
      b = 11464;break;
    case 6:
    case 7:
    case 8:
      b = 11811;break;
    case 9:
      b = 11000;break;
    case 10:
      b = 11222;break;
    }
  return b;
}

int jumptab10 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 10198;break;
    case 1:
      b = 10828;break;
    case 2:
      b = 10773;break;
    case 3:
      b = 10465;break;
    case 4:
      b = 10555;break;
    case 5:
    case 6:
    case 7:
      b = 10882;break;
    case 8:
      b = 11819;break;
    case 9:
      b = 10010;break;
    }
  return b;
}

int jumptab9 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 9198;break;
    case 1:
      b = 9827;break;
    case 2:
      b = 9873;break;
    case 3:
      b = 9465;break;
    case 4:
    case 5:
      b = 9866;break;
    case 6:
      b = 9737;break;
    case 7:
      b = 9881;break;
    case 8:
      b = 9819;break;
    }
  return b;
}

int jumptab8 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 8981;break;
    case 1:
      b = 8871;break;
    case 2:
      b = 8761;break;
    case 3:
    case 4:
      b = 8541;break;
    case 5:
      b = 8431;break;
    case 6:
      b = 8321;break;
    case 7:
      b = 8211;break;
    }
  return b;
}

int jumptab7 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 7672;break;
    case 1:
      b = 7262;break;
    case 2:
      b = 7852;break;
    case 3:
      b = 7442;break;
    case 4:
      b = 7032;break;
    case 5:
      b = 7622;break;
    case 6:
      b = 7212;break;
    }
  return b;
}

int jumptab6 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 6463;break;
    case 1:
      b = 6853;break;
    case 2:
      b = 6243;break;
    case 3:
      b = 6633;break;
    case 4:
      b = 6023;break;
    case 5:
      b = 6413;break;
    }
  return b;
}


int jumptab5 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 5354;break;
    case 1:
      b = 5644;break;
    case 2:
      b = 5934;break;
    case 3:
      b = 5324;break;
    case 4:
      b = 5614;break;
    }
  return b;
}


static int rand_sample (const int limit)
{
  return (rand () % limit);
}

#define DO_GEN_VALUES(SIZE,SAVE)		\
  for (i = 0; i < iters; i++) \
    SAVE[i] = rand_sample(SIZE);

#define DO_TIME_LOOP(SIZE,SAVE)			\
  DO_GEN_VALUES(SIZE,SAVE);			\
  *retval = jumptab##SIZE(0);				\
  TIMING_NOW (start);					\
  for (i = 0; i < iters; i++)				\
    *retval = jumptab##SIZE(SAVE[i]);			\
  TIMING_NOW (stop);					\
  TIMING_DIFF (cur, start, stop);			\
  TIMING_PRINT_MEAN ((double) cur, (double) iters);	\
  printf ("\n");

void do_test (int size, int *retval)
{
  size_t i, iters = 10000;
  unsigned long start, stop, cur;
  char *s = TEST_CFLAGS;
  int *values = calloc (iters, sizeof(int));  
  printf (TEST_CFLAGS ",%d,", size);
  switch (size)
    {
/*       case 13: DO_TIME_LOOP(13,values); break; */
/*       case 12: DO_TIME_LOOP(12,values); break; */
/*       case 11: DO_TIME_LOOP(11,values); break; */
/*       case 10: DO_TIME_LOOP(10,values); break; */
/*       case 9: DO_TIME_LOOP(9,values); break; */
/*       case 8: DO_TIME_LOOP(8,values); break; */
      case 7: DO_TIME_LOOP(7,values); break;
      case 6: DO_TIME_LOOP(6,values); break;
      case 5: DO_TIME_LOOP(5,values); break;
      default: break;
    }
  free (values);
}

static int rand_in_range (int range) {return (rand ()%(range+1));}
int globl;

/* Main */

int
main (int argc, char **argv)
{
  size_t i, j;
  printf ("Starting %s\n", TEST_NAME);
  printf ("CFLAGS,Size,Time\n");
  test_init (MAX_BUFSIZE);
  for (j = 5; j <= 13; j++)
    do_test (j, &globl);

  test_cleanup ();
  return ret;
}
