/* Copyright (C) 2018-2020 MIPS Tech LLC
   This is a modification of glibc test-case for bench-marking.

   Measure memset functions.
   Copyright (C) 2013-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#define START_SIZE (128 * 1024)
#ifdef LARGEBUF
#define STATIC
#define MIN_PAGE_SIZE (4096 + 8 * 1024 * 1024)
#else
# define MIN_PAGE_SIZE 131072
#endif
# define MAX_BUFSIZE MIN_PAGE_SIZE
#include "bench-string.h"

char *
SIMPLE_MEMSET (char *s, int c, size_t n)
{
  char *r = s, *end = s + n;
  while (r < end)
    *r++ = c;
  return s;
}

extern char *
memset1 (char *s, int c, size_t n);


static void
do_one_test (char *s, int c __attribute ((unused)), size_t n)
{
  size_t i, iters = INNER_LOOP_ITERS;
  unsigned long start, stop, cur;
  char tstbuf[n];
  char *res = memset(s, c, n);
  if (res != s
      || SIMPLE_MEMSET (tstbuf, c, n) != tstbuf
      || memcmp (s, tstbuf, n) != 0)
    {
      fprintf (stderr, "Wrong result in function memset");
      ret = 1;
      return;
    }

  TIMING_NOW (start);
  for (i = 0; i < iters; ++i)
    {
      memset(s, c, n);
    }
  TIMING_NOW (stop);

  TIMING_DIFF (cur, start, stop);

  TIMING_PRINT_MEAN ((double) cur, (double) iters);
}

static void
do_test (size_t align, int c, size_t len)
{
  align &= 63;
  if ((align + len) * sizeof (char) > page_size)
    return;

  printf ("%4zd,%2zd,c %2d,", len, align, c);
  fflush (stdout);
  do_one_test ((buf1) + align, c, len);

  putchar ('\n');
}

int
main (int argc, char **argv)
{
  size_t i;
  int c = 0;
  char x[10];

  test_init (MAX_BUFSIZE);
  printf ("Length,Alignment,Data,Time(nsecs)\n");

#ifndef LARGEBUF
  for (c = -65; c <= 130; c += 65)
    {
      for (i = 0; i < 18; ++i)
	do_test (0, c, 1 << i);
      for (i = 1; i < 32; ++i)
	{
	  do_test (i, c, i);
	  if (i & (i - 1))
	    do_test (0, c, i);
	}
      for (i = 32; i < 512; i+=32)
	{
	  do_test (0, c, i);
	  do_test (i, c, i);
	}
      do_test (1, c, 14);
      do_test (3, c, 1024);
      do_test (4, c, 64);
      do_test (2, c, 25);
    }
  for (i = 33; i <= 256; i += 4)
    {
      do_test (0, c, 32 * i);
      do_test (i, c, 32 * i);
    }
#else
  c = 65;
  for (i = START_SIZE; i < MIN_PAGE_SIZE; i <<= 1)
    {
      do_test (0, c, i);
      do_test (3, c, i);
    }
#endif

  test_cleanup ();

  return ret;
}
