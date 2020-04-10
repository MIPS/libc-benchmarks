/* Copyright (C) 2018-2020 MIPS Tech LLC
   Snarfed from glibc/string/test-strcpy.c, modified for bench-marking.  */

/* Test and measure strcpy functions.
   Copyright (C) 1999-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Written by Jakub Jelinek <jakub@redhat.com>, 1999.
   Added wcscpy support by Liubov Dmitrieva <liubov.dmitrieva@gmail.com>, 2011

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

# define STRCPY_RESULT(dst, len) dst
# define TEST_MAIN
#  define TEST_NAME "strcpy"
#  define SIMPLE_STRCPY simple_strcpy
#  define STRCPY strcpy
# define MIN_PAGE_SIZE (2 << 16)
# define BUFX2
# define MAX_BUFSIZE MIN_PAGE_SIZE
# include "bench-string.h"
# include <limits.h>
#include <stdlib.h>

# define CHAR char
# define UCHAR unsigned char
# define sfmt "s"
# define BIG_CHAR CHAR_MAX
# define SMALL_CHAR 127
# define STRCMP strcmp
# define MEMCMP memcmp
# define MEMSET memset



CHAR *SIMPLE_STRCPY (CHAR *, const CHAR *);

CHAR *
SIMPLE_STRCPY (CHAR *dst, const CHAR *src)
{
  CHAR *ret = dst;
  while ((*dst++ = *src++) != '\0');
  return ret;
}

typedef CHAR *(*proto_t) (CHAR *, const CHAR *);

static void
do_one_test (CHAR *dst, const CHAR *src,
	     size_t len __attribute__((unused)))
{
  size_t i, iters = INNER_LOOP_ITERS;
  unsigned long start, stop, cur;
  char *result = SIMPLE_STRCPY (dst,src);

  if (strcpy (dst, src) != result)
    {
      fprintf (stderr, "\nWrong return in function strcpy %p %p\n",
	     strcpy (dst, src), result);
      ret = 1;
      fflush (stdout); fflush (stderr);
      return;
    }

  if (strcmp (dst, src) != 0)
    {
      fprintf (stderr,
	     "\nWrong result in function strcpy dst %s src %s\n",
	     dst, src);
      ret = 1;
      return;
    }

  TIMING_NOW (start);
  for (i = 0; i < iters; ++i)
    strcpy(dst, src);
  TIMING_NOW (stop);

  TIMING_DIFF (cur, start, stop);

  TIMING_PRINT_MEAN ((double) cur, (double) iters);
  return;
}

static void
do_test (size_t align1, size_t align2, size_t len, int max_char)
{
  size_t i;
  CHAR *s1, *s2;
/* For wcscpy: align1 and align2 here mean alignment not in bytes,
   but in wchar_ts, in bytes it will equal to align * (sizeof (wchar_t))
   len for wcschr here isn't in bytes but it's number of wchar_t symbols.  */
  align1 &= 7;
  if ((align1 + len) * sizeof(CHAR) >= page_size)
    return;

  align2 &= 7;
  if ((align2 + len) * sizeof(CHAR) >= page_size)
    return;

  s1 = (CHAR *) (buf1) + align1;
  s2 = (CHAR *) (buf2) + align2;

  for (i = 0; i < len; i++)
    s1[i] = 32 + 23 * i % (max_char - 32);
  s1[len] = 0;

  printf ("%4d,%d,%d,", len, align1, align2);
  do_one_test (s2, s1, len);
  putchar ('\n');
}

int
main (void)
{
  size_t i;

  test_init (MIN_PAGE_SIZE);

  printf ("Length,Alignment1,Alignment 2,Time(nsecs)\n");

  for (i = 0; i <= 16; ++i)
    {
      do_test (0, 0, i, SMALL_CHAR);
      do_test (0, 0, i, BIG_CHAR);
      do_test (0, i, i, SMALL_CHAR);
      do_test (i, 0, i, BIG_CHAR);
    }

  for (i = 2; i < 16; ++i)
    {
      do_test (i, 2 * i, 8 << i, SMALL_CHAR);
      do_test (2 * i, i, 8 << i, BIG_CHAR);
      do_test (i, i, 8 << i, SMALL_CHAR);
      do_test (i, i, 8 << i, BIG_CHAR);
    }

  test_cleanup ();
  return ret;
}
