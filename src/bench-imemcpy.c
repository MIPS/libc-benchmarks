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
# define TEST_NAME "inline-memcpy"
# define MAX_BUFSIZE (1 << 16)
# include "bench-string.h"

#include <stdio.h>
#define typedef_st(T,X) typedef struct __attribute__((packed)) { T words[(X)/sizeof(T)]; } st_##T##X

typedef_st(int,8);
typedef_st(int,16);
typedef_st(int,32);
typedef_st(int,64);
typedef_st(int,128);
typedef_st(int,256);
typedef_st(int,512);
typedef_st(int,1024);

typedef_st(char,15);
typedef_st(char,29);
typedef_st(char,59);
typedef_st(char,121);
typedef_st(char,247);
typedef_st(char,499);
typedef_st(char,1009);

typedef_st(short,10);
typedef_st(short,20);
typedef_st(short,38);
typedef_st(short,72);
typedef_st(short,138);
typedef_st(short,268);
typedef_st(short,524);
typedef_st(short,1040);

#define ALIGN_OFFSET(P,T) (((unsigned long)(P)) % sizeof(T))
#define ALIGN_PTR(P,T) ((ALIGN_OFFSET(P,T) == 0) ? (P) : (void *)((unsigned long)(P) +  sizeof (T) - ALIGN_OFFSET(P,T)))
#define MISALIGN_PTR(P,T,N) ((ALIGN_OFFSET(P,T) == (N)) ? (P) : (void *)((unsigned long)ALIGN_PTR(P,T) + (N)))

#define TIMING_LOOP(D,S) {				\
  size_t i, iters = INNER_LOOP_ITERS;			\
  unsigned long start, stop, cur;			\
  TIMING_NOW (start);					\
  for (i = 0; i < iters; i++)				\
    (D) = (S);						\
  TIMING_NOW (stop);					\
  TIMING_DIFF (cur, start, stop);			\
  TIMING_PRINT_MEAN ((double) cur, (double) iters);	\
  printf ("\n");					\
  }							\

#define PRINT_SETUP1(T,D)					\
  printf ("%4zd,Y,0,N,%2zd,",sizeof(T),ALIGN_OFFSET (D,int));	\

#define PRINT_SETUP2(T,S)					\
  printf ("%4zd,N,%2zd,Y,0,",sizeof(T),ALIGN_OFFSET (S,int));	\

#define PRINT_SETUP3(T,S,D)					\
  printf ("%4zd,N,%2zd,N,%2zd,",sizeof(T),ALIGN_OFFSET (S,int), \
	  ALIGN_OFFSET (D,int));				\

#define PRINT_SETUP4(T)						\
  printf ("%4zd,Y,0,Y,0,",sizeof(T));				\
	  
#define CHECK_RESULT(T,D,S)				\
  if (memcmp ((void *)D, (void *)S, sizeof(T)) != 0)	\
    {							\
      fprintf (stderr, "Assignment failed\n");		\
      return;						\
    }							\

#define TIMING_SETUP1(T,A)						\
  T __attribute__((aligned (2))) src_##T;				\
  T *dst_##T = (T *) MISALIGN_PTR (buf2, int, A);			\
  *dst_##T = src_##T;							\
  CHECK_RESULT (T,dst_##T,&src_##T)					\
  PRINT_SETUP1 (T,dst_##T)						\

#define TIMING_SETUP2(T,A)						\
  T *src_##T = (T *) MISALIGN_PTR (buf1, int, A);			\
  T __attribute__((aligned (2))) dst_##T;				\
  dst_##T = *src_##T;							\
  CHECK_RESULT (T,&dst_##T,src_##T)					\
  PRINT_SETUP2 (T,src_##T)						\

#define TIMING_SETUP3(T,A1,A2)						\
  T *src_##T = (T *) MISALIGN_PTR (buf1, int, A1);		\
  T *dst_##T = (T *) MISALIGN_PTR (buf2, int, A2);		\
  *dst_##T = *src_##T;							\
  CHECK_RESULT (T,dst_##T,src_##T)					\
  PRINT_SETUP3 (T,src_##T,dst_##T)					\

#define TIMING_SETUP4(T)						\
  T __attribute__((aligned (2))) src_##T;				\
  T __attribute__((aligned (2))) dst_##T;				\
  dst_##T = src_##T;							\
  CHECK_RESULT (T,&dst_##T,&src_##T)					\
  PRINT_SETUP4 (T)							\

#define TIMING_LOOP1(T,A) {			\
    TIMING_SETUP1(T,A)				\
      TIMING_LOOP(*dst_##T,src_##T)		\
      }

#define TIMING_LOOP2(T,A) {			\
    TIMING_SETUP2(T,A)				\
      TIMING_LOOP(dst_##T,*src_##T)		\
      }

#define TIMING_LOOP3(T,A1,A2) {			\
    TIMING_SETUP3(T,A1,A2)			\
      TIMING_LOOP(*dst_##T,*src_##T)		\
      }						\

#define TIMING_LOOP4(T) {			\
    TIMING_SETUP4(T)				\
      TIMING_LOOP(dst_##T,src_##T)		\
      }						\

/* src aligned, dest declaratively misaligned, but may be aligned */

void do_test_1 (int len, int dst_align)
{
  switch (len)
    {
      case 8: 	TIMING_LOOP1 (st_int8,dst_align); break;
      case 16: 	TIMING_LOOP1 (st_int16,dst_align); break;
      case 32: 	TIMING_LOOP1 (st_int32,dst_align); break;
      case 64: 	TIMING_LOOP1 (st_int64,dst_align); break;
      case 128: TIMING_LOOP1 (st_int128,dst_align); break;
      case 256: TIMING_LOOP1 (st_int256,dst_align); break;
      case 512: TIMING_LOOP1 (st_int512,dst_align); break;
      case 1024: TIMING_LOOP1 (st_int1024,dst_align); break;
      case 10:	TIMING_LOOP1 (st_short10,dst_align); break;
      case 15:	TIMING_LOOP1 (st_char15,dst_align); break;
      case 20:	TIMING_LOOP1 (st_short20,dst_align); break;
      case 29:	TIMING_LOOP1 (st_char29,dst_align); break;
      case 38:	TIMING_LOOP1 (st_short38,dst_align); break;
      case 59:	TIMING_LOOP1 (st_char59,dst_align); break;
      case 72:	TIMING_LOOP1 (st_short72,dst_align); break;
      case 121:	TIMING_LOOP1 (st_char121,dst_align); break;
      case 138:	TIMING_LOOP1 (st_short138,dst_align); break;
      case 247:	TIMING_LOOP1 (st_char247,dst_align); break;
      case 268:	TIMING_LOOP1 (st_short268,dst_align); break;
      case 499:	TIMING_LOOP1 (st_char499,dst_align); break;
      case 524:	TIMING_LOOP1 (st_short524,dst_align); break;
      case 1009: TIMING_LOOP1 (st_char1009,dst_align); break;
      case 1040: TIMING_LOOP1 (st_short1040,dst_align); break;
      default:
	fprintf (stderr,"Unsupported test length %d\n", len);
	return;
    }
}

/* src declaratively misaligned but may be aligned, dest aligned  */

void do_test_2 (int len, int src_align)
{
  switch (len)
    {
      case 8: 	TIMING_LOOP2 (st_int8,src_align); break;
      case 16: 	TIMING_LOOP2 (st_int16,src_align); break;
      case 32: 	TIMING_LOOP2 (st_int32,src_align); break;
      case 64: 	TIMING_LOOP2 (st_int64,src_align); break;
      case 128: TIMING_LOOP2 (st_int128,src_align); break;
      case 256: TIMING_LOOP2 (st_int256,src_align); break;
      case 512: TIMING_LOOP2 (st_int512,src_align); break;
      case 1024: TIMING_LOOP2 (st_int1024,src_align); break;
      case 10:	TIMING_LOOP2 (st_short10,src_align); break;
      case 15:	TIMING_LOOP2 (st_char15,src_align); break;
      case 20:	TIMING_LOOP2 (st_short20,src_align); break;
      case 29:	TIMING_LOOP2 (st_char29,src_align); break;
      case 38:	TIMING_LOOP2 (st_short38,src_align); break;
      case 59:	TIMING_LOOP2 (st_char59,src_align); break;
      case 72:	TIMING_LOOP2 (st_short72,src_align); break;
      case 121:	TIMING_LOOP2 (st_char121,src_align); break;
      case 138:	TIMING_LOOP2 (st_short138,src_align); break;
      case 247:	TIMING_LOOP2 (st_char247,src_align); break;
      case 268:	TIMING_LOOP2 (st_short268,src_align); break;
      case 499:	TIMING_LOOP2 (st_char499,src_align); break;
      case 524:	TIMING_LOOP2 (st_short524,src_align); break;
      case 1009: TIMING_LOOP2 (st_char1009,src_align); break;
      case 1040: TIMING_LOOP2 (st_short1040,src_align); break;
      default:
	fprintf (stderr,"Unsupported test length %d\n", len);
	return;
    }
}

/* src declaratively misaligned but actually aligned, dest declaratively misaligned but actually aligned  */

void do_test_3 (int len, int src_align, int dst_align)
{
    switch (len)
    {
      case 8: 	TIMING_LOOP3 (st_int8, src_align, dst_align); break;
      case 16: 	TIMING_LOOP3 (st_int16, src_align, dst_align); break;
      case 32: 	TIMING_LOOP3 (st_int32, src_align, dst_align); break;
      case 64: 	TIMING_LOOP3 (st_int64, src_align, dst_align); break;
      case 128: TIMING_LOOP3 (st_int128, src_align, dst_align); break;
      case 256: TIMING_LOOP3 (st_int256, src_align, dst_align); break;
      case 512: TIMING_LOOP3 (st_int512, src_align, dst_align); break;
      case 1024: TIMING_LOOP3 (st_int1024, src_align, dst_align); break;
      case 10:	TIMING_LOOP3 (st_short10, src_align, dst_align); break;
      case 15:	TIMING_LOOP3 (st_char15, src_align, dst_align); break;
      case 20:	TIMING_LOOP3 (st_short20, src_align, dst_align); break;
      case 29:	TIMING_LOOP3 (st_char29, src_align, dst_align); break;
      case 38:	TIMING_LOOP3 (st_short38, src_align, dst_align); break;
      case 59:	TIMING_LOOP3 (st_char59, src_align, dst_align); break;
      case 72:	TIMING_LOOP3 (st_short72, src_align, dst_align); break;
      case 121:	TIMING_LOOP3 (st_char121, src_align, dst_align); break;
      case 138:	TIMING_LOOP3 (st_short138, src_align, dst_align); break;
      case 247:	TIMING_LOOP3 (st_char247, src_align, dst_align); break;
      case 268:	TIMING_LOOP3 (st_short268, src_align, dst_align); break;
      case 499:	TIMING_LOOP3 (st_char499, src_align, dst_align); break;
      case 524:	TIMING_LOOP3 (st_short524, src_align, dst_align); break;
      case 1009: TIMING_LOOP3 (st_char1009, src_align, dst_align); break;
      case 1040: TIMING_LOOP3 (st_short1040, src_align,  dst_align); break;
      default:
	fprintf (stderr,"Unsupported test length %d\n", len);
	return;
    }
}

/* src aligned, dest aligned  */

void do_test_4 (int len)
{
    switch (len)
    {
      case 8: 	TIMING_LOOP4 (st_int8); break;
      case 16: 	TIMING_LOOP4 (st_int16); break;
      case 32: 	TIMING_LOOP4 (st_int32); break;
      case 64: 	TIMING_LOOP4 (st_int64); break;
      case 128: TIMING_LOOP4 (st_int128); break;
      case 256: TIMING_LOOP4 (st_int256); break;
      case 512: TIMING_LOOP4 (st_int512); break;
      case 1024: TIMING_LOOP4 (st_int1024); break;
      case 10:	TIMING_LOOP4 (st_short10); break;
      case 15:	TIMING_LOOP4 (st_char15); break;
      case 20:	TIMING_LOOP4 (st_short20); break;
      case 29:	TIMING_LOOP4 (st_char29); break;
      case 38:	TIMING_LOOP4 (st_short38); break;
      case 59:	TIMING_LOOP4 (st_char59); break;
      case 72:	TIMING_LOOP4 (st_short72); break;
      case 121:	TIMING_LOOP4 (st_char121); break;
      case 138:	TIMING_LOOP4 (st_short138); break;
      case 247:	TIMING_LOOP4 (st_char247); break;
      case 268:	TIMING_LOOP4 (st_short268); break;
      case 499:	TIMING_LOOP4 (st_char499); break;
      case 524:	TIMING_LOOP4 (st_short524); break;
      case 1009: TIMING_LOOP4 (st_char1009); break;
      case 1040: TIMING_LOOP4 (st_short1040); break;
      default:
	fprintf (stderr,"Unsupported test length %d\n", len);
	return;
    }
}


/* Main */

int
main (int argc, char **argv)
{
  size_t i;
  int testsizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 10, 15, 20, 29,
		     38, 59, 72, 121, 138, 247, 268, 499, 524, 1009, 1040};

  printf ("Starting %s\n", TEST_NAME);
  printf ("Struct length,Src declared align,Src mis-align, Dest declared align, Dest mis-align,Time(nsecs)\n");
  test_init (MAX_BUFSIZE);

  for (i = 0; i < (sizeof(testsizes) / sizeof(int)); i++)
    {
      do_test_1 (testsizes[i], 0);
      do_test_1 (testsizes[i], 1);
      do_test_1 (testsizes[i], 2);
      do_test_1 (testsizes[i], 3);
    }
  
  for (i = 0; i < (sizeof(testsizes) / sizeof(int)); i++)
    {
      do_test_2 (testsizes[i], 0);
      do_test_2 (testsizes[i], 1);
      do_test_2 (testsizes[i], 2);
      do_test_2 (testsizes[i], 3);
    }
  
  for (i = 0; i < (sizeof(testsizes) / sizeof(int)); i++)
    {
      do_test_3 (testsizes[i], 1, 1);
      do_test_3 (testsizes[i], 1, 2);
      do_test_3 (testsizes[i], 1, 3);
      do_test_3 (testsizes[i], 2, 1);
      do_test_3 (testsizes[i], 2, 2);
      do_test_3 (testsizes[i], 2, 3);
      do_test_3 (testsizes[i], 3, 1);
      do_test_3 (testsizes[i], 3, 2);
      do_test_3 (testsizes[i], 3, 3);
    }
  
  for (i = 0; i < (sizeof(testsizes) / sizeof(int)); i++)
    do_test_4 (testsizes[i]);
  
  test_cleanup ();
  return ret;
}
