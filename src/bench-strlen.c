/* Copyright (C) 2018-2020 MIPS Tech LLC
   Snarfed from glibc/string/test-strlen.c, modified for bench-marking.  */

/* Measure strlen
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

# define TEST_NAME "strlen"
# define MIN_PAGE_SIZE (2 << 16)
# define BUFX2
# define MAX_BUFSIZE MIN_PAGE_SIZE
# include "bench-string.h"

# include <limits.h>

# define L(str) str
# define STRCMP strcmp
# define STRCPY strcpy
# define STRLEN strlen
# define MEMCPY memcpy
# define STUPID_STRCMP stupid_strcmp
# define CHAR char
# define UCHAR unsigned char
# define CHARBYTES 1
# define CHARALIGN 1
# define MIDCHAR 0x7f
# define LARGECHAR 0xfe
# define CHAR__MAX CHAR_MAX
# define CHAR__MIN CHAR_MIN

int globl;

static void
do_one_test (const CHAR *s1, int exp_result)
{
  size_t i, iters = INNER_LOOP_ITERS;
  unsigned long start, stop, cur;
  int result = strlen (s1);
  
  if (result != exp_result)
    {
      fprintf (stderr, "\nWrong result in function strlen, got %d, expect %d\n",
	       result, exp_result);
      fflush (stdout); fflush (stderr);
      return;
    }
  
  TIMING_NOW (start);
  for (i = 0; i < iters; ++i)
    globl += strlen(s1);
  TIMING_NOW (stop);

  TIMING_DIFF (cur, start, stop);

  TIMING_PRINT_MEAN ((double) cur, (double) iters);
}

static void
do_test (size_t align1, size_t len)
{
  size_t i;

  CHAR *s1, *s2;

  if (len == 0)
    return;

  align1 &= 63;
  if (align1 + (len + 1) * CHARBYTES >= page_size)
    return;

  /* Put them close to the end of page.  */
  i = align1 + CHARBYTES * (len + 2);
  s1 = (char *) (buf1 + ((page_size - i) / 16 * 16) + align1);

  for (i = 0; i < len; i++)
    s1[i] = 1 + (23 << ((CHARBYTES - 1) * 8)) * i % LARGECHAR;

  s1[len] = 0;
  s1[len + 1] = 23;

  printf ("%4zd,%2zd,", len, align1);

  do_one_test (s1, len);

  putchar ('\n');
}

int
main (int argc, char **argv)
{
  size_t i;

  test_init (MIN_PAGE_SIZE);

  printf ("Length,Alignment1,Time(nsecs)\n");

  for (i = 1; i < 32; ++i)
    do_test (CHARBYTES * i, i);

  for (i = 4; i < 20; ++i)
    do_test (0, 2 << i);

  test_cleanup ();
  return ret;
}
