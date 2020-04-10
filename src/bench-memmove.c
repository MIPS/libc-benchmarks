/* Measure memmove functions.

   Copyright (C) 2018-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

# define BUFX2
#define START_BITS 18
/* #ifdef LARGEBUF */
#define START_SIZE (64 * 1024)
#define STATIC
#define MIN_PAGE_SIZE (4096 + 4096 * 1024)
/* # define TEST_NAME "memmove-large" */
/* #else */
/* # define MAX_BUF_BITS 18 */
/* # define MIN_PAGE_SIZE (1<<MAX_BUF_BITS) */
# define TEST_NAME "memmove"
/* #endif */
# define MAX_BUFSIZE MIN_PAGE_SIZE
# include "bench-string.h"

#define CACHE_LINE 32
#define CACHE_OFFSET 4096

void dcache_flush (const void *addr, unsigned size)
{
  unsigned size_by_off = size / CACHE_OFFSET; 
  unsigned size_by_block = (size % CACHE_OFFSET) / CACHE_LINE;
  int i, j;

  if ((size % CACHE_OFFSET) % CACHE_LINE != 0)
    size_by_block++;
  if (size_by_block < CACHE_LINE)
    return;

  #ifdef __mips
  for (i = 0; i < size_by_off; i++, addr += CACHE_OFFSET)
    {
      for (j = 0; j < CACHE_OFFSET/CACHE_LINE; j++, addr += CACHE_LINE)
	{
	  asm volatile ("cache 21,%0" :: "ZC" (addr));
	  asm volatile ("cache 19,%0" :: "ZC" (addr));
	}
    }

  for (j = 0; j < size_by_block; j++, addr += CACHE_LINE)
    {
      asm volatile ("cache 17,%0" :: "ZC" (addr));
      asm volatile ("cache 19,%0" :: "ZC" (addr));
    }
  #endif
}

static void
do_one_test (char *dst, const char *src,
	     size_t len)
{
  size_t i, iters = INNER_LOOP_ITERS;
  unsigned long start, stop, cur;
  unsigned misses = 0;

  /* Must clear the destination buffer set by the previous run.  */
  for (i = 0; i < len; i++)
    dst[i] = 0;

  if (memmove (dst, src, len) != MEMCPY_RESULT (dst, len))
    {
      fprintf (stderr, "Wrong return in function %p %p",
	     memmove(dst, src, len), MEMCPY_RESULT (dst, len));
      ret = 1;
      return;
    }

  if (memcmp (dst, src, len) != 0)
    {
      fprintf (stderr,"Wrong result in function %p, %p", dst, src);
	for (i = 0; i < len; i++)
	  fprintf (stderr, "i = %d, [%p] = %x, [%p] = %x\n", i, src+i, src[i], dst+i, dst[i]);
	
      ret = 1;
      return;
    }

  /* Flush code leads to memory fault  */
  /*   dcache_flush (src, len); */

  TIMING_NOW (start);
  for (i = 0; i < iters; ++i)
    memmove(dst, src, len);
  TIMING_NOW (stop);
  
  TIMING_DIFF (cur, start, stop);

  TIMING_PRINT_MEAN ((double) cur, (double) iters);
}

static void
do_test (size_t align1, size_t align2, size_t len)
{
  size_t i, j;
  char *s1, *s2;

  align1 &= 63;
  if (align1 + len >= page_size)
    return;

  align2 &= 63;
  if (align2 + len >= page_size)
    return;

  s1 = (char *) (buf1 + align1);
  s2 = (char *) (buf2 + align2);

  for (i = 0, j = 1; i < len; i++, j += 23)
    s1[i] = j;

  printf ("%4zd,%2zd,%2zd,", len, align1, align2);

  do_one_test (s2, s1, len);

  printf ("\n");
}

int
main (int argc, char **argv)
{
  size_t i;
  printf ("Starting %s\n", TEST_NAME);
  printf ("Length,Alignment1,Alignment2,Time(nsecs)\n");
  test_init (MAX_BUFSIZE);

  for (i = 0; i < START_BITS; ++i)
    {
      do_test (0, 0, 1 << i);
#if __mips_isa_rev < 6
      do_test (i, 0, 1 << i);
      do_test (0, i, 1 << i);
#endif
      do_test (i, i, 1 << i);
    }
  
  for (i = 0; i < 32; ++i)
    {
      do_test (0, 0, i);
      do_test (i, 0, i);
      do_test (0, i, i);
      do_test (i, i, i);
    }

  for (i = 3; i < 32; ++i)
    {
      if ((i & (i - 1)) == 0)
	continue;
      do_test (0, 0, 16 * i);
      do_test (i, 0, 16 * i);
      do_test (0, i, 16 * i);
      do_test (i, i, 16 * i);
    }

  for (i = 32; i < 64; ++i)
    {
      do_test (0, 0, 32 * i);
      do_test (i, 0, 32 * i);
      do_test (0, i, 32 * i);
      do_test (i, i, 32 * i);
    }

  do_test (0, 0, getpagesize ());

  do_test (0, 0, 1 << 16);
#if __mips_isa_rev < 6
  do_test (15, 0, 1 << 16);
  do_test (0, 15, 1 << 16);
#endif
  do_test (15, 15, 1 << 16);
  do_test (0, 0, 1 << 16);
  do_test (16, 0, 1 << 16);
  do_test (0, 16, 1 << 16);
  do_test (16, 16, 1 << 16);

  for (i = START_SIZE<<1; i <= MIN_PAGE_SIZE; i <<= 1)
    {
      do_test (0, 0, i + 7);
#if __mips_isa_rev < 6
      do_test (0, 3, i + 15);
      do_test (3, 0, i + 31);
      do_test (3, 5, i + 63);
#endif
    }

  test_cleanup ();
  return ret;
}
