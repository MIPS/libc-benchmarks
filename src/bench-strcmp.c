/* Measure strcmp and wcscmp functions.
   Copyright (C) 2018-2020 MIPS Tech LLC
   This is a modification of glibc test-case for bench-marking.

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

# define TEST_NAME "strcmp"
# define MIN_PAGE_SIZE (2 << 16)
# define BUFX2
# define MAX_BUFSIZE MIN_PAGE_SIZE
# define STATIC
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
# define CHARBYTESLOG 0
# define CHARALIGN 1
# define MIDCHAR 0x7f
# define LARGECHAR 0xfe
# define CHAR__MAX CHAR_MAX
# define CHAR__MIN CHAR_MIN

extern int strcmp (const char *s1, const char *s2);

/* Strcmp uses unsigned semantics for comparison. */
int
simple_strcmp (const char *s1, const char *s2)
{
  int ret;

  while ((ret = *(unsigned char *) s1 - *(unsigned char*) s2++) == 0 && *s1++);
  return ret;
}

static void
do_one_test (const CHAR *s1, const CHAR *s2,
	     int exp_result)
{
  size_t i, iters = INNER_LOOP_ITERS;
  unsigned long start, stop, cur;
  int result = strcmp (s1,s2);
  
  if ((result > 0 && exp_result < 0)
      || (result < 0 && exp_result > 0)
      || (result == 0 && exp_result != 0)
      || (result != 0 && exp_result == 0))
    {
      printf ("Wrong result in function strcmp %s,%s: got %x, expected %x", s1, s2, result, exp_result);
      return;
    }
  
  TIMING_NOW (start);
  for (i = 0; i < iters; ++i)
    strcmp (s1, s2);
  TIMING_NOW (stop);

  TIMING_DIFF (cur, start, stop);

  TIMING_PRINT_MEAN ((double) cur, (double) iters);
}

static void
do_test (size_t align1, size_t align2, size_t len, int max_char,
	 int exp_result)
{
  size_t i;

  CHAR *s1, *s2;

  if (len == 0)
    return;

  align1 &= 63;
  if (align1 + (len + 1) * CHARBYTES >= page_size)
    return;

  align2 &= 63;
  if (align2 + (len + 1) * CHARBYTES >= page_size)
    return;

  /* Put them close to the end of page.  */
  i = align1 + CHARBYTES * (len + 2);
  s1 = (char *) (buf1 + ((page_size - i) / 16 * 16) + align1);
  i = align2 + CHARBYTES * (len + 2);
  s2 = (char *) (buf2 + ((page_size - i) / 16 * 16)  + align2);

  for (i = 0; i < len; i++)
    s1[i] = s2[i] = 1 + (23 << ((CHARBYTES - 1) * 8)) * i % max_char;

  s1[len] = s2[len] = 0;
  s1[len + 1] = 23;
  s2[len + 1] = 24 + exp_result;
  s2[len - 1] -= exp_result;
  s1[len + 2] = 23 + exp_result;
  s2[len + 2] = 24;
  s1[len + 3] = 24;
  s2[len + 3] = 24;

  printf ("%4zd,%d,%d,", len, align1, align2);

  do_one_test (s1, s2, exp_result);

  putchar ('\n');
}

static void
check (void)
{
  CHAR *s1 = (CHAR *) (buf1 + 0xb2c);
  CHAR *s2 = (CHAR *) (buf1 + 0xfd8);

  STRCPY(s1, L("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs"));
  STRCPY(s2, L("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkLMNOPQRSTUV"));

  /* Check correct working for negatives values */

  s1[0] = 1;
  s2[0] = 1;
  s1[1] = 1;
  s2[1] = 1;
  s1[2] = -1;
  s2[2] = 3;
  s1[3] = 0;
  s2[3] = -1;

  /* Check possible overflow bug, actual more for wcscmp */

  s1[7] = CHAR__MIN;
  s2[7] = CHAR__MAX;

  size_t l1 = STRLEN (s1);
  size_t l2 = STRLEN (s2);

  for (size_t i1 = 0; i1 < l1; i1++)
    for (size_t i2 = 0; i2 < l2; i2++)
      {
		int exp_result = simple_strcmp (s1 + i1, s2 + i2);
		printf ("%4zd,%d,%d,", strlen(s1+i1), (unsigned)(s1+i1)%4, (unsigned)(s2+i2)%4);
		do_one_test (s1+i1, s2+i2, exp_result);
		putchar ('\n');
      }

  /* Test cases where there are multiple zero bytes after the first.  */

  for (size_t i = 0; i < 16 + 1; i++)
    {
      s1[i] = 0x00;
      s2[i] = 0x00;
    }

  for (size_t i = 0; i < 16; i++)
    {
      int exp_result;

      for (int val = 0x7c; val < 0x82; val++)
	{
	  for (size_t j = 0; j < i; j++)
	    {
	      s1[j] = val;
	      s2[j] = val;
	    }

	  s2[i] = val;

	  exp_result = simple_strcmp (s1, s2);
	  printf ("%4zd,%d,%d,", strlen (s1), (unsigned)s1 % 4, (unsigned)s2 % 4);
	  do_one_test (s1, s2, exp_result);
	  putchar ('\n');
	}
    }
}

int
main (int argc, char **argv)
{
  size_t i;

  test_init (MIN_PAGE_SIZE);

  printf ("Length,Alignment1,Alignment 2,Time(nsecs)\n");

  check();

  for (i = 1; i < 32; ++i)
    {
      do_test (CHARBYTES * i, CHARBYTES * i, i, MIDCHAR, 0);
      do_test (CHARBYTES * i, CHARBYTES * i, i, MIDCHAR, 1);
      do_test (CHARBYTES * i, CHARBYTES * i, i, MIDCHAR, -1);
    }

  for (i = 1; i < 11 + CHARBYTESLOG; ++i)
    {
      do_test (0, 0, 2 << i, MIDCHAR, 0);
      do_test (0, 0, 2 << i, LARGECHAR, 0);
      do_test (0, 0, 2 << i, MIDCHAR, 1);
      do_test (0, 0, 2 << i, LARGECHAR, 1);
      do_test (0, 0, 2 << i, MIDCHAR, -1);
      do_test (0, 0, 2 << i, LARGECHAR, -1);
      do_test (0, CHARBYTES * i, 2 << i, MIDCHAR, 1);
      do_test (CHARBYTES * i, CHARBYTES * (i + 1), 2 << i, LARGECHAR, 1);
    }

  for (i = 1; i < 8; ++i)
    {
      do_test (CHARBYTES * i, 2 * CHARBYTES * i, 8 << i, MIDCHAR, 0);
      do_test (2 * CHARBYTES * i, CHARBYTES * i, 8 << i, LARGECHAR, 0);
      do_test (CHARBYTES * i, 2 * CHARBYTES * i, 8 << i, MIDCHAR, 1);
      do_test (2 * CHARBYTES * i, CHARBYTES * i, 8 << i, LARGECHAR, 1);
      do_test (CHARBYTES * i, 2 * CHARBYTES * i, 8 << i, MIDCHAR, -1);
      do_test (2 * CHARBYTES * i, CHARBYTES * i, 8 << i, LARGECHAR, -1);
    }
  test_cleanup ();
  
  return ret;
}
