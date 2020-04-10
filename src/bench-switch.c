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

# define BUFX2
# define TEST_NAME "switch"
# define MAX_BUFSIZE (1 << 16)
# include "bench-string.h"

#define MAX_JUMP_CASES 14
#define CACHE_SIZE 64*1024

#include <stdio.h>
#include <stdlib.h>

static int uniform(int x, int limit) {return limit;};
static int uniform_alt(int x, int limit) {return (x%2 == 0) ? limit : limit*5;};
static int linear_inc(int x, int limit) {return limit * x;};
static int linear_dec(int x, int limit) {return linear_inc (limit, limit) - linear_inc (x, limit);};
static int sqr(int x, int limit) {return (x * x);};
static int sqr_rev(int x, int limit) {return sqr ((x - limit), limit);};
static int quad(int x, int limit) {return sqr ((x - limit/2) * 2,limit);};
static int quad_inv(int x, int limit) {return  quad (0, limit) - quad(x, limit);};

static int rand_sample (const int limit, int (*f)(int,int))
{
  int max = 0;
  int sample;
  static int range[MAX_JUMP_CASES]; // = calloc (limit+1, sizeof (int));
  static int match[MAX_JUMP_CASES]; // = calloc (limit+1, sizeof (int));
  int matchcount = 0;
  int i;
  range[0] = 1;
  for (i = 1; i <= limit; i++)
    {
      range[i] = range[i-1] + f (i,limit);
      
      if (range[i] > max)
	max = range[i];
    }
  
  sample = rand () % max;

  /* Find which bucket the sample belongs to.  */
  for (i = 0; i < limit; i++)
    if ((range[i] <= sample &&
	sample < range[i+1])
	|| (range[i] >= sample &&
	    sample > range[i+1]))
      match[matchcount++] = i;

  if (matchcount > 1)
    /* Pick one out of matching buckets with equal probability.  */
    sample = match[rand() % matchcount];
  else
    /* Only one bucket.  */
    sample = match[0];
  
/*   free (range); */
/*   free (match); */
  return sample;
}

int jumptab6 (int a)  __attribute__ ((noinline));
int jumptab7 (int a)  __attribute__ ((noinline));
int jumptab8 (int a)  __attribute__ ((noinline));
int jumptab9 (int a)  __attribute__ ((noinline));
int jumptab10 (int a)  __attribute__ ((noinline));
int jumptab11 (int a)  __attribute__ ((noinline));
int jumptab12 (int a)  __attribute__ ((noinline));
int jumptab13 (int a)  __attribute__ ((noinline));

/* int jumptab18 (int a) */
/* { */
/*   int b; */
/*   switch (a) */
/*     { */
/*     case 0: */
/*       b = 10898;break; */
/*     case 1: */
/*       b = 11818;break; */
/*     case 2: */
/*       b = 12803;break; */
/*     case 3: */
/*       b = 13811;break; */
/*     case 4: */
/*       b = 14825;break; */
/*     case 5: */
/*       b = 15827;break; */
/*     case 6: */
/*       b = 13832;break; */
/*     case 7: */
/*       b = 12942;break; */
/*     case 8: */
/*       b = 11849;break; */
/*     case 9: */
/*       b = 11805;break; */
/*     case 10: */
/*       b = 11850;break; */
/*     case 11: */
/*       b = 11855;break; */
/*     case 12: */
/*     case 13: */
/*     case 14: */
/*       b = 12827;break; */
/*     case 15: */
/*       b = 13867;break; */
/*     case 16: */
/*       b = 14867;break; */
/*     case 17: */
/*       b = 15867;break; */
/*     } */
/*   return b; */
/* } */

/* int jumptab17 (int a) */
/* { */
/*   int b; */
/*   switch (a) */
/*     { */
/*     case 0: */
/*       b = 10798;break; */
/*     case 1: */
/*       b = 10718;break; */
/*     case 2: */
/*       b = 10703;break; */
/*     case 3: */
/*       b = 10711;break; */
/*     case 4: */
/*       b = 10725;break; */
/*     case 5: */
/*       b = 10727;break; */
/*     case 6: */
/*       b = 10732;break; */
/*     case 7: */
/*       b = 10742;break; */
/*     case 8: */
/*       b = 10749;break; */
/*     case 9: */
/*       b = 10705;break; */
/*     case 10: */
/*     case 11: */
/*     case 12: */
/*       b = 10791;break; */
/*     case 13: */
/*       b = 10787;break; */
/*     case 14: */
/*       b = 10777;break; */
/*     case 15: */
/*       b = 10767;break; */
/*     case 16: */
/*       b = 10767;break; */
/*     } */
/*   return b; */
/* } */

/* int jumptab16 (int a) */
/* { */
/*   int b; */
/*   switch (a) */
/*     { */
/*     case 0: */
/*       b = 6198;break; */
/*     case 1: */
/*       b = 6118;break; */
/*     case 2: */
/*       b = 6103;break; */
/*     case 3: */
/*       b = 6111;break; */
/*     case 4: */
/*       b = 6125;break; */
/*     case 5: */
/*       b = 6127;break; */
/*     case 6: */
/*       b = 6132;break; */
/*     case 7: */
/*       b = 6142;break; */
/*     case 8: */
/*       b = 6149;break; */
/*     case 9: */
/*       b = 6105;break; */
/*     case 10: */
/*       b = 6150;break; */
/*     case 11: */
/*       b = 6155;break; */
/*     case 12: */
/*       b = 6191;break; */
/*     case 13: */
/*       b = 6187;break; */
/*     case 14: */
/*       b = 6177;break; */
/*     case 15: */
/*       b = 6167;break; */
/*     } */
/*   return b; */
/* } */


/* int jumptab15 (int a) */
/* { */
/*   int b; */
/*   switch (a) */
/*     { */
/*     case 0: */
/*       b = 15198;break; */
/*     case 1: */
/*       b = 15018;break; */
/*     case 2: */
/*       b = 15003;break; */
/*     case 3: */
/*       b = 15311;break; */
/*     case 4: */
/*       b = 15125;break; */
/*     case 5: */
/*       b = 15327;break; */
/*     case 6: */
/*       b = 15332;break; */
/*     case 7: */
/*       b = 15342;break; */
/*     case 8: */
/*       b = 15849;break; */
/*     case 9: */
/*       b = 15905;break; */
/*     case 10: */
/*       b = 15455;break; */
/*     case 11: */
/*       b = 15655;break; */
/*     case 12: */
/*       b = 15651;break; */
/*     case 13: */
/*       b = 15957;break; */
/*     case 14: */
/*       b = 15957;break; */
/*     } */
/*   return b; */
/* } */

/* int jumptab14 (int a) */
/* { */
/*   int b; */
/*   switch (a) */
/*     { */
/*     case 0: */
/*       b = 14198;break; */
/*     case 1: */
/*       b = 14018;break; */
/*     case 2: */
/*       b = 14003;break; */
/*     case 3: */
/*       b = 14311;break; */
/*     case 4: */
/*       b = 14125;break; */
/*     case 5: */
/*       b = 14327;break; */
/*     case 6: */
/*       b = 14332;break; */
/*     case 7: */
/*       b = 14342;break; */
/*     case 8: */
/*       b = 14849;break; */
/*     case 9: */
/*       b = 14905;break; */
/*     case 10: */
/*       b = 14455;break; */
/*     case 11: */
/*       b = 14655;break; */
/*     case 12: */
/*       b = 14651;break; */
/*     case 13: */
/*       b = 14957;break; */
/*     } */
/*   return b; */
/* } */

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

int jumptab4 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 4045;break;
    case 1:
      b = 4235;break;
    case 2:
      b = 4425;break;
    case 3:
      b = 4615;break;
    }
  return b;
}

int jumptab3 (int a)
{
  int b;
  switch (a)
    {
    case 0:
      b = 3136;break;
    case 1:
      b = 3526;break;
    case 2:
      b = 3716;break;
    }
  return b;
}

#define DO_GEN_VALUES(SIZE,FUNC,SAVE)		\
  for (i = 0; i < iters; i++) \
    SAVE[i] = rand_sample(SIZE,FUNC);

#define DO_TIME_LOOP(SIZE,FUNC,SAVE)			\
  DO_GEN_VALUES(SIZE,FUNC,SAVE);			\
  *retval = jumptab##SIZE(0);				\
  TIMING_NOW (start);					\
  for (i = 0; i < iters; i++)				\
    *retval = jumptab##SIZE(SAVE[i]);			\
  TIMING_NOW (stop);					\
  TIMING_DIFF (cur, start, stop);			\
  TIMING_PRINT_MEAN ((double) cur, (double) iters);	\
  printf ("\n");

void do_test (int size, int(*f)(int,int), char *name, int *retval)
{
  size_t i, iters = 10000;
  unsigned long start, stop, cur;
  int *values = calloc (iters, sizeof(int));  
  printf ("%d,%s,",size,name);
  switch (size)
    {
/*       case 18: DO_TIME_LOOP(18,f,values); break; */
/*       case 17: DO_TIME_LOOP(17,f,values); break; */
/*       case 16: DO_TIME_LOOP(16,f,values); break; */
/*       case 15: DO_TIME_LOOP(15,f,values); break; */
/*       case 14: DO_TIME_LOOP(14,f,values); break; */
      case 13: DO_TIME_LOOP(13,f,values); break;
      case 12: DO_TIME_LOOP(12,f,values); break;
      case 11: DO_TIME_LOOP(11,f,values); break;
      case 10: DO_TIME_LOOP(10,f,values); break;
      case 9: DO_TIME_LOOP(9,f,values); break;
      case 8: DO_TIME_LOOP(8,f,values); break;
      case 7: DO_TIME_LOOP(7,f,values); break;
      case 6: DO_TIME_LOOP(6,f,values); break;
      case 5: DO_TIME_LOOP(5,f,values); break;
      case 4: DO_TIME_LOOP(4,f,values); break;
      case 3: DO_TIME_LOOP(3,f,values); break;
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
  int (*func[])(int,int)  = { uniform, uniform_alt, linear_inc,
			      linear_dec, sqr, sqr_rev, quad, quad_inv };
  char *fname[] = {"uniform", "uniform alt",
		   "linear increasing", "linear decreasing", "squared",
		   "squared reversed", "quadratic", "quadratic inverted"};
  
  printf ("Starting %s\n", TEST_NAME);
  printf ("Size,Dist,Time\n");
  test_init (MAX_BUFSIZE);
  for (i = 0; i < sizeof (func) / sizeof (func[0]); i++)
    for (j = 5; j <= 13; j++)
      do_test (j, func[i], fname[i], &globl);

  test_cleanup ();
  return ret;
}
