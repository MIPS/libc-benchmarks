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

#include "mips-string-ops.h"

#define do_strcpy_aligned_word(__dst,__dst_idx,__x_next)\
{\
  unsigned long x__last = x;\
  if (detect_zero (x__last, x__last, _01s, _80s))\
    {\
      inc_ptr_as (unsigned long *, dst, __dst_idx); \
      break;\
    }\
  x = __x_next;\
  set_word (__dst, __dst_idx, x__last);\
}

#define do_strcpy_aligned_first_word(__dst,__dst_idx,__x_next)\
{\
  unsigned long x__last = x;\
  if (detect_zero (x__last, x__last, _01s, _80s))\
    {\
      break;\
    }\
  x = __x_next;\
  set_word (__dst, __dst_idx, x__last);\
}

#define do_strcpy_maligned_word(__dst,__dst_idx,__x_next)\
{\
  unsigned long x__last = x;\
  if (detect_zero (x__last, x__last, _01s, _80s))\
    {\
      inc_ptr_as (unsigned long *, dst, __dst_idx); \
      break;\
    }\
  x = __x_next;\
  set_maligned_word (__dst, __dst_idx, x__last);\
}

#define do_strcpy_maligned_first_word(__dst,__dst_idx,__x_next)\
{\
  unsigned long x__last = x;\
  if (detect_zero (x__last, x__last, _01s, _80s))\
    {\
      break;\
    }\
  x = __x_next;\
  set_maligned_word (__dst, __dst_idx, x__last);\
}

#define do_strcpy_byte(__dst,__x)\
{\
  unsigned char v##__x = (unsigned char) bx.b.B##__x;\
  set_byte (__dst, __x, v##__x);\
  if (v##__x == 0)\
    {\
      break;\
    }\
}

#define do_strcpy_last_byte(__dst, __x)\
{\
  unsigned char v##__x = (unsigned char) bx.b.B##__x;\
  set_byte (__dst, __x, v##__x);\
  break;\
}

#if SMOKE_TEST_MIPS_STRCPY
#define strcpy my_strcpy
#endif

#ifndef __mips_using_dsp
  def_and_set_01 (_01s);
  def_and_set_80 (_80s);
#endif

void *
strcpy (void *__restrict__ dst, const void *__restrict__ _a)
{
  unsigned long x;
  void *res = dst;

  /* align the string to word boundary so we can do word at a time.
     Due to N32 ABI, addresses can be 32-bit while data is 64-bit.
   */
  if ((cvt_ptr_to (unsigned long, _a) & (sizeof (unsigned long) - 1)) != 0)
    {
      if ((cvt_ptr_to (unsigned long, _a) & 1) != 0)
	{
	  unsigned char b0 = get_byte (_a, 0);
	  set_byte (dst, 0, b0);
	  if (b0 == 0)
	    {
	      return res;
	    }
	  inc_ptr_as (char *, _a, 1);
	  inc_ptr_as (char *, dst, 1);
	}
      if ((cvt_ptr_to (unsigned long, _a) & 2) != 0)
	{
	  unsigned char b1 = get_byte (_a, 0);
	  set_byte (dst, 0, b1);
	  if (b1 == 0)
	    {
	      return res;
	    }
	  unsigned char b2 = get_byte (_a, 1);
	  set_byte (dst, 1, b2);
	  if (b2 == 0)
	    {
	      return res;
	    }
	  inc_ptr_as (char *, _a, 2);
	  inc_ptr_as (char *, dst, 2);
	}

#if __mips64
      if ((cvt_ptr_to (unsigned long, _a) & 4) != 0)
	{
	  unsigned char b0 = get_byte (_a, 0);
	  set_byte (dst, 0, b0);
	  if (b0 == 0)
	    {
	      return res;
	    }
	  unsigned char b1 = get_byte (_a, 1);
	  set_byte (dst, 1, b1);
	  if (b1 == 0)
	    {
	      return res;
	    }
	  unsigned char b2 = get_byte (_a, 2);
	  set_byte (dst, 0, b2);
	  if (b2 == 0)
	    {
	      return res;
	    }
	  unsigned char b3 = get_byte (_a, 3);
	  set_byte (dst, 0, b3);
	  if (b3 == 0)
	    {
	      return res;
	    }
	  inc_ptr_as (char *, _a, 4);
	  inc_ptr_as (char *, dst, 4);
	}
#endif
    }

  /* as advantagous as it is to performance, this code cannot pre-load
     the following word, nor can it prefetch the next line at the start
     of the loop since the string can be at the end of a page with the
     following page unmapped. There are tests in the suite to catch
     any attempt to go beyond the current word. */
#ifdef __mips_using_dsp
  def_and_set_01 (_01s);
  def_and_set_80 (_80s);
#endif
  x = get_word (_a, 0);
  if ((cvt_ptr_to (unsigned long, dst) & (sizeof (unsigned long) - 1)) != 0)
    {
      while (1)
	{
	  do_strcpy_maligned_first_word (dst, 0, get_word (_a, 1));
	  do_strcpy_maligned_word (dst, 1, get_word (_a, 2));
	  do_strcpy_maligned_word (dst, 2, get_word (_a, 3));
	  do_strcpy_maligned_word (dst, 3, get_word (_a, 4));
	  inc_ptr_as (unsigned long *, _a, 4);
	  inc_ptr_as (unsigned long *, dst, 4);
	}
    }
  else
    {
      while (1)
	{
	  do_strcpy_aligned_first_word (dst, 0, get_word (_a, 1));
	  do_strcpy_aligned_word (dst, 1, get_word (_a, 2));
	  do_strcpy_aligned_word (dst, 2, get_word (_a, 3));
	  do_strcpy_aligned_word (dst, 3, get_word (_a, 4));
	  inc_ptr_as (unsigned long *, _a, 4);
	  inc_ptr_as (unsigned long *, dst, 4);
	}
    }

  while (1)
    {
      /* pull apart the last word processed and find the zero.  */
      bitfields_t bx;
      bx.v = x;
      do_strcpy_byte (dst, 0);
      do_strcpy_byte (dst, 1);
      do_strcpy_byte (dst, 2);
#if __mips64
      do_strcpy_byte (dst, 3);
      do_strcpy_byte (dst, 4);
      do_strcpy_byte (dst, 5);
      do_strcpy_byte (dst, 6);
      do_strcpy_last_byte (dst, 7)
#else
      do_strcpy_last_byte (dst, 3);
#endif
    }
  return res;
}
