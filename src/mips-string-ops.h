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

#ifndef __MIPS_STRING_OPS_H
#define __MIPS_STRING_OPS_H
    /* This definition of the byte bitfields uses the 
       assumption that the layout of the bitfields is
       equivalent to the layout in memory.  Generally,
       for the MIPS ABIs, this is true. If you compile
       the strcmp.c file with -DSMOKE_TEST_NEW_STRCMP,
       this assumption will be tested.

       Also, regardless of char signedness, ANSI C dictates that
       strcmp() treats each character as unsigned char.  For
       strlen and the like, signedness doesn't matter.

       Also, this code assumes that there are 8-bits per 'char'.  */

#if __mips64
typedef struct bits
{
  unsigned B0:8, B1:8, B2:8, B3:8, B4:8, B5:8, B6:8, B7:8;
} bits_t;
#else
typedef struct bits
{
  unsigned B0:8, B1:8, B2:8, B3:8;
} bits_t;
#endif

#ifndef _ULW
#define _ULW
    /* for MIPS GCC, there are no unaligned builtins - so this code forces
       the compiler to treat the pointer access as unaligned.  */
union ulw
{
  unsigned char uc;
  unsigned short us;
  unsigned int ui;
  unsigned long int uli;
} __attribute__ ((packed));

#define get_maligned_2byte(__x,__x__idx) ((union ulw *) ((unsigned short *)(__x) + (__x_idx)))->us
#define get_maligned_word(__x,__x__idx) ((union ulw *) ((unsigned long *)(__x)))->uli

#define set_maligned_2byte(__x,__x_idx,__v) ((union ulw *) (((unsigned short *)(__x)) + __x_idx))->us = __v
#define set_maligned_word(__x,__x_idx,__v) ((union ulw *) (((unsigned long *)(__x)) + __x_idx))->uli = __v

#endif

/* This union assumes that small structures can be in registers.  If
   not, then memory accesses will be done - not optimal, but ok.  */
typedef union
{
  unsigned v;
  bits_t b;
} bitfields_t;

#ifndef detect_zero
/* __mips_dsp, __mips_dspr2, and __mips64 are predefined by
   the compiler, based on command line options.  */
#if (__mips_dsp || __mips_dspr2) && !__mips64
#define __mips_using_dsp 1

/* DSP 4-lane (8 unsigned bits per line) subtract and saturate
 * Intrinsic operation. How this works:
 *     Given a 4-byte string of "ABC\0", subtract this as
 *     an unsigned integer from 0x01010101:
 *	   0x01010101
 *       - 0x41424300
 *        -----------
 (         0xbfbebe01 <-- answer without saturation
 *	   0x00000001 <-- answer with saturation
 * When this 4-lane vector is treated as an unsigned int value,
 * a non-zero answer indicates the presence of a zero in the
 * original 4-byte argument.  */

typedef signed char v4i8 __attribute__ ((vector_size (4)));

#define detect_zero(__x,__y,__01s,__80s)\
       ((unsigned) __builtin_mips_subu_s_qb((v4i8) __01s,(v4i8) __x))

    /* sets all 4 lanes to requested byte.  */
#define set_byte_lanes(__x,__fill) (__x = (unsigned) __builtin_mips_repl_qb(__fill))

    /* sets all 4 lanes to 0x01.  */
#define def_and_set_01(__x) unsigned __x = (unsigned) __builtin_mips_repl_qb(0x01)

    /* sets all 4 lanes to 0x80. Not needed when subu_s.qb used. */
#define def_and_set_80(__x)	/* do nothing */

#else
    /* this version, originally published in the 80's, does the
       following steps for __x = 0x31ff0001:
       __x - _01s = 0x30fdff00
       ~__x = 0xce00fffe
       ((__x - _01s) & ~__x) = 0x0000ff00
       x & _80s = 0x00008000 <- byte 3 was zero
       Some implementaions naively assume that characters are
       always 7-bit unsigned ASCII. With that assumption, the
       "& ~x" is usually discarded. Since character strings
       are 8-bit, the and is needed to catch the case of
       a false positive when the byte is 0x80. */

#define detect_zero(__x,__y,_01s,_80s)\
	((unsigned long) (((__x) - _01s) & ~(__x)) & _80s)

#define set_byte_lanes(__x,__fill)\
{\
  __x = __fill;\
  if (__x != 0)\
    {\
      bitfields_t f;\
      __fill &= 0xff;\
      f.b.B0 = f.b.B1 = f.b.B2 = f.b.B3 = __fill;\
      __x = f.v;\
    }\
}

#if __mips64
#define def_and_set_80(__x) unsigned long __x =  0x8080808080808080ul
#define def_and_set_01(__x)  unsigned long __x = 0x0101010101010101ul
#else
#define def_and_set_80(__x) unsigned long __x = 0x80808080ul
#define def_and_set_01(__x) unsigned long __x = 0x01010101ul
#endif

#endif
#endif

/* dealing with 'void *' conversions without using extra variables. */
#define get_byte(__x,__idx) (((unsigned char *) (__x))[__idx])
#define set_byte(__x,__idx,__fill) ((unsigned char *) (__x))[__idx] = (__fill)

static inline unsigned long __attribute__ ((always_inline))
get_word(const void *__x,int __idx) {
  return ((const unsigned long *) (__x))[__idx];
}

#define set_word(__x,__idx,__fill) ((unsigned long *) (__x))[__idx] = (__fill)
#define inc_ptr_as(__type,__x,__inc) __x = (void *) (((__type) __x) + (__inc))
#define cvt_ptr_to(__type,__x) ((__type) (__x))

#endif
