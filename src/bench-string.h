/* Copyright (C) 2018-2020 MIPS Tech LLC
   Snarfed from glibc/string/test-string.h, modified for bench-marking.  */

/* memcpy common header
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

# define MEMCPY_RESULT(dst, len) dst
# define OPT_ITERATIONS 10000

# define INNER_LOOP_ITERS 64
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>

#if 1
# define TIMING_NOW(var)						\
  ({									\
    static  struct timespec tv = {0,0};					\
    clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &tv);			\
    (var) = (uint64_t) (tv.tv_nsec + (uint64_t) 1000000000 * tv.tv_sec); \
})
#else
#define TIMING_NOW(var)
#endif

# define TIMING_DIFF(diff, start, end) (diff) = (end) - (start)
# define TIMING_ACCUM(sum, diff) (sum) += (diff)

#define TIMING_PRINT_MEAN(d_total_s, d_iters) \
  printf ("%g", (d_total_s) / (d_iters))

static int getpagesize (void) { return 4096; }

int ret, do_srandom;
unsigned int seed;
size_t page_size;
#ifdef STATIC
static unsigned char buf1[MAX_BUFSIZE+64]
#if 1
__attribute__((aligned(1<<18)))
#endif
  ;
#ifdef BUFX2  
static unsigned char buf2[MAX_BUFSIZE+64]
#if 1
__attribute__((aligned(1<<18)))
#endif
  ;
#endif
#else
static unsigned char *buf1, *buf2;
static unsigned char *alloc1, *alloc2;
#endif

# ifndef BUF1PAGES
#  define BUF1PAGES 1
# endif

static void
test_init (unsigned bufsize)
{
  page_size = 2 * getpagesize ();
# ifdef MIN_PAGE_SIZE
  if (page_size < MIN_PAGE_SIZE)
    page_size = MIN_PAGE_SIZE;
# endif
#ifndef STATIC
  alloc1 = malloc (bufsize + 64);
  if (alloc1 == NULL)
    fprintf (stderr, "mmap failed");
  else if ((unsigned long) alloc1 % 64)
    buf1 = alloc1 + (64 - ((unsigned long) alloc1 % 64));
  else
    buf1 = alloc1;
#ifdef BUFX2  
  alloc2 = malloc (bufsize + 64);
  if (alloc2 == NULL)
    fprintf(stderr, "mmap failed");
  else if ((unsigned long) alloc2 % 64)
    buf2 = alloc2 + (64 - ((unsigned long) alloc2 % 64));
  else
    buf2 = alloc2;
#endif
  memset (alloc1, 0xa5, bufsize+64);
#ifdef BUFX2
  memset (alloc2, 0x5a, bufsize+64);
#endif
#else
  memset (buf1, 0xa5, bufsize+64);
#ifdef BUFX2
  memset (buf2, 0x5a, bufsize+64);
#endif
#endif
}

static void
test_cleanup (void)
{
#ifndef STATIC
  free (alloc1);
#ifdef BUFX2
  free (alloc2);
#endif
#endif
}
