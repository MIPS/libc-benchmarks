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

#include <string.h>

#if !defined(UNALIGNED_INSTR_SUPPORT)
/* does target have unaligned lw/ld/ualw/uald instructions? */
#define UNALIGNED_INSTR_SUPPORT 0
#if __mips_isa_rev < 6 && !__mips1
#undef UNALIGNED_INSTR_SUPPORT
#define UNALIGNED_INSTR_SUPPORT 1
#endif
#endif

#if !defined(HW_UNALIGNED_SUPPORT)
/* Does target have hardware support for unaligned accesses?  */
#define HW_UNALIGNED_SUPPORT 0
#if _MIPS_ARCH_I6400
#undef HW_UNALIGNED_SUPPORT
#define HW_UNALIGNED_SUPPORT 1
#endif
#endif

#define SIZEOF_reg_t 4
#if _MIPS_SIM == _ABIO32
typedef unsigned long reg_t;
typedef struct bits
{
  reg_t B0:8, B1:8, B2:8, B3:8;
} bits_t;
#else
#undef SIZEOF_reg_t
#define SIZEOF_reg_t 8
typedef unsigned long long reg_t;
typedef struct bits
{
  reg_t B0:8, B1:8, B2:8, B3:8, B4:8, B5:8, B6:8, B7:8;
} bits_t;
#endif

/* This union assumes that small structures can be in registers.  If
   not, then memory accesses will be done - not optimal, but ok.  */
typedef union
{
  reg_t v;
  bits_t b;
} bitfields_t;

#define do_bitfield(__i)\
if (x.b.B##__i != y.b.B##__i) return x.b.B##__i - y.b.B##__i;

/* pull apart the words to find the first differing unsigned byte.  */
static int __attribute__ ((noinline)) do_by_bitfields (reg_t a, reg_t b)
{
  bitfields_t x, y;
  x.v = a;
  y.v = b;
  do_bitfield (0);
  do_bitfield (1);
  do_bitfield (2);
#if SIZEOF_reg_t == 4
  return x.b.B3 - y.b.B3;
#else
  do_bitfield (3);
  do_bitfield (4);
  do_bitfield (5);
  do_bitfield (6);
  return x.b.B7 - y.b.B7;
#endif
}

/* This code is called when aligning a pointer, there are remaining bytes
   after doing word compares, or architecture does not have some form
   of unaligned support.  */
static inline int __attribute__ ((always_inline))
do_bytes (const void *a, const void *b, unsigned long len)
{
  unsigned char *x = (unsigned char *) a;
  unsigned char *y = (unsigned char *) b;
  unsigned long i;

  /* 'len' might be zero here, so preloading the first two values
     before the loop may access unallocated memory.  */
  for (i = 0; i < len; i++)
    {
      if (*x != *y)
	return *x - *y;
      x++;
      y++;
    }
  return 0;
}

#if !HW_UNALIGNED_SUPPORT
#if UNALIGNED_INSTR_SUPPORT
/* for MIPS GCC, there are no unaligned builtins - so this struct forces
   the compiler to treat the pointer access as unaligned.  */
struct ulw
{
  reg_t uli;
} __attribute__ ((packed));

/* first pointer is not aligned while second pointer is.  */
static int
unaligned_words (const struct ulw *a, const reg_t * b,
		 unsigned long words, unsigned long bytes)
{
  unsigned long i, words_by_4, words_by_1;

  words_by_1 = words % 4;
  words_by_4 = words - words_by_1;
  for (i = 0; i < words_by_4; i += 4)
    {
      reg_t x0 = a[0].uli, x1 = a[1].uli;
      reg_t x2 = a[2].uli, x3 = a[3].uli;
      reg_t y0 = b[0], y1 = b[1], y2 = b[2], y3 = b[3];
      if (x0 != y0)
	return do_by_bitfields (x0, y0);
      if (x1 != y1)
	return do_by_bitfields (x1, y1);
      if (x2 != y2)
	return do_by_bitfields (x2, y2);
      if (x3 != y3)
	return do_by_bitfields (x3, y3);
      a += 4;
      b += 4;
    }

  /* do remaining words.  */
  for (i = 0; i < words_by_1; i++)
    {
      reg_t x0 = a->uli;
      reg_t y0 = *b;
      a += 1;
      b += 1;
      if (x0 != y0)
	return do_by_bitfields (x0, y0);
    }

  /* mop up any remaining bytes.  */
  return do_bytes (a, b, bytes);
}
#else
/* no HW support or unaligned lw/ld/ualw/uald instructions.  */
static int
unaligned_words (const reg_t * a, const reg_t * b,
		 unsigned long words, unsigned long bytes)
{
  return do_bytes (a, b, (sizeof (reg_t) * words) + bytes);
}
#endif /* UNALIGNED_INSTR_SUPPORT */
#endif /* HW_UNALIGNED_SUPPORT */

/* both pointers are aligned, or first isn't and HW support for unaligned.  */
static int
aligned_words (const reg_t * a, const reg_t * b,
	       unsigned long words, unsigned long bytes)
{
  unsigned long i, words_by_4, words_by_1;

  words_by_1 = words % 4;
  words_by_4 = words - words_by_1;
  for (i = 0; i < words_by_4; i += 4)
    {
      reg_t x0 = a[0], x1 = a[1], x2 = a[2], x3 = a[3];
      reg_t y0 = b[0], y1 = b[1], y2 = b[2], y3 = b[3];
      if (x0 != y0)
	return do_by_bitfields (x0, y0);
      if (x1 != y1)
	return do_by_bitfields (x1, y1);
      if (x2 != y2)
	return do_by_bitfields (x2, y2);
      if (x3 != y3)
	return do_by_bitfields (x3, y3);
      a += 4;
      b += 4;
    }

  /* do remaining words.  */
  for (i = 0; i < words_by_1; i++)
    {
      reg_t x0 = *a;
      reg_t y0 = *b;
      a += 1;
      b += 1;
      if (x0 != y0)
	return do_by_bitfields (x0, y0);
    }

  /* mop up any remaining bytes.  */
  return do_bytes (a, b, bytes);
}

int
memcmp (const void *a, const void *b, size_t len)
{
  unsigned long bytes, words;

  /* shouldn't hit that often.  */
  if (len < sizeof (reg_t) * 4)
    {
      return do_bytes (a, b, len);
    }

  /* Align the second pointer to word/dword alignment.
     Note that the pointer is only 32-bits for o32/n32 ABIs. For
     n32, loads are done as 64-bit while address remains 32-bit.   */
  bytes = ((unsigned long) b) % sizeof (reg_t);
  if (bytes)
    {
      int res;
      bytes = sizeof (reg_t) - bytes;
      if (bytes > len)
	bytes = len;
      res = do_bytes (a, b, bytes);
      if (res || len == bytes)
	return res;
      len -= bytes;
      a = (const void *) (((unsigned char *) a) + bytes);
      b = (const void *) (((unsigned char *) b) + bytes);
    }

  /* Second pointer now aligned.  */
  words = len / sizeof (reg_t);
  bytes = len % sizeof (reg_t);
#if HW_UNALIGNED_SUPPORT
  /* treat possible unaligned first pointer as aligned.  */
  return aligned_words (a, b, words, bytes);
#else
  if (((unsigned long) a) % sizeof (reg_t) == 0)
    {
      return aligned_words (a, b, words, bytes);
    }
  /* need to use unaligned instructions on first pointer.  */
  return unaligned_words (a, b, words, bytes);
#endif
}
