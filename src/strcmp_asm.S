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

#ifdef ANDROID_CHANGES
# include "machine/asm.h"
# include "machine/regdef.h"
#elif _LIBC
# include <sysdep.h>
# include <regdef.h>
# include <sys/asm.h>
#elif _COMPILING_NEWLIB
# include "machine/asm.h"
# include "machine/regdef.h"
#else
# include <regdef.h>
# include <sys/asm.h>
#endif

/* Testing on a little endian machine showed using CLZ was a
   performance loss, so we are not turning it on by default.  */
#if !defined(__nanomips_subset) && defined(ENABLE_CLZ) && (__mips_isa_rev > 1)
# define USE_CLZ
#else
/* Using ALIGN when EXT is unavailable.  */
# if defined(__nanomips_subset) || defined(DISABLE_EXT)
#  define USE_ALIGN_INST
# endif
#endif

/* Some asm.h files do not have the L macro definition.  */
#ifndef L
# if _MIPS_SIM == _ABIO32
#  define L(label) $L ## label
# else
#  define L(label) .L ## label
# endif
#endif

/* Some asm.h files do not have the PTR_ADDIU macro definition.  */
#ifndef PTR_ADDIU
# ifdef USE_DOUBLE
#  define PTR_ADDIU       daddiu
# else
#  define PTR_ADDIU       addiu
# endif
#endif

/* Allow the routine to be named something else if desired.  */
#ifndef STRCMP_NAME
# define STRCMP_NAME strcmp
#endif

#ifdef __nanomips__
	.section .text.strcmp,"ax",@progbits
	.align 1
#else
	.text
	.align 2
#endif

#ifdef ANDROID_CHANGES
LEAF(STRCMP_NAME, 0)
#else
LEAF(STRCMP_NAME)
#endif
	.set	nomips16

	or	t0, a0, a1
	andi	t0, t0, 0x3
	bne	t0, zero, L(byteloop)

/* Both strings are 4 byte aligned at this point.  */

	li	t8, 0x01010101
#if !defined(__mips_dsp)
	li	t9, 0x7f7f7f7f
#endif

#ifdef __mips_dsp
# define STRCMP32(OFFSET) \
	lw	a2, OFFSET(a0); \
	lw	a3, OFFSET(a1); \
	subu_s.qb t0, t8, a2; \
	bne	a2, a3, L(worddiff); \
	bne	t0, zero, L(returnzero)
#else  /* !__mips_dsp */
# define STRCMP32(OFFSET) \
	lw	a2, OFFSET(a0); \
	lw	a3, OFFSET(a1); \
	subu	t0, a2, t8; \
	nor	t1, a2, t9; \
	and	t1, t0, t1; \
	bne	a2, a3, L(worddiff); \
	bne	t1, zero, L(returnzero)
#endif /* __mips_dsp */

	.align 2
L(wordloop):
	STRCMP32(0)
	STRCMP32(4)
	STRCMP32(8)
	STRCMP32(12)
	STRCMP32(16)
	STRCMP32(20)
	STRCMP32(24)
	lw	a2, 28(a0)
	lw	a3, 28(a1)
#ifndef __mips_dsp
	subu	t0, a2, t8
	nor	t1, a2, t9
	and	t1, t0, t1
#else
	subu_s.qb t0, t8, a2
#endif

	PTR_ADDIU a0, a0, 32
	bne	a2, a3, L(worddiff)
	PTR_ADDIU a1, a1, 32
	beq	t1, zero, L(wordloop)	

L(returnzero):
	move	va0, zero
	jr	ra

	.align 2
L(worddiff):
#ifdef USE_CLZ
	xor	t0, a2, a3
	or	t0, t0, t1
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  if defined(__nanomips__) && !defined(__nanomips_subset)
	byterevw t0, t0
#  else /* ! __nanomips__ */
	wsbh	t0, t0
	rotr	t0, t0, 16
#  endif /* __nanomips__ */
# endif /* LITTLE_ENDIAN */
	clz	t1, t0
	or	t0, t1, 24	/* Only care about multiples of 8.  */
	xor 	t1, t1, t0	/* {0,8,16,24} => {24,16,8,0}  */
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	sllv	a2,a2,t1
	sllv	a3,a3,t1
#else
	srlv	a2,a2,t1
	srlv	a3,a3,t1
#endif
	subu	va0, a2, a3
	jr	ra
#else /* USE_CLZ */
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	andi	a0, a2, 0xff	/* abcd => d */
	andi	a1, a3, 0xff
	beq	a0, zero, L(wexit01)
#  ifdef USE_ALIGN_INST
	align	t0, a2, a2, 2 	/* abcd => cdab */
	bne	a0, a1, L(wexit01)
	align	t1, a3, a3, 2
	srl	t8, t0, 24 	/* cdab => c */
	srl	t9, t1, 24
	beq	t8, zero, L(wexit89)
	andi	a0, t0, 0xff	/* cdab => b */
	bne	t8, t9, L(wexit89)
	andi	a1, t1, 0xff
#  else  /* !USE_ALIGN_INST */
	ext	t8, a2, 8, 8
	bne	a0, a1, L(wexit01)
	ext	t9, a3, 8, 8
	beq	t8, zero, L(wexit89)
	ext	a0, a2, 16, 8
	bne	t8, t9, L(wexit89)
	ext	a1, a3, 16, 8
#  endif  /* !USE_ALIGN_INST */

# else /* __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ */
	srl	a0, a2, 24	/* abcd => a */
	srl	a1, a3, 24
	beq	a0, zero, L(wexit01)

#  ifdef USE_ALIGN_INST
	align	t0, a2, a2, 2 	/* abcd => cdab */
	bne	a0, a1, L(wexit01)
	align	t1, a3, a3, 2
	andi	t8, t0, 0xff 	/* cdab => b */
	andi	t9, t1, 0xff
	beq	t8, zero, L(wexit89)
	srl	a0, t0, 24	/* cdab => c */
	bne	t8, t9, L(wexit89)
	srl	a1, t1, 24
#  else /* ! USE_ALIGN_INST */
	ext	t8, a2, 16, 8
	bne	a0, a1, L(wexit01)
	ext	t9, a3, 16, 8
	beq	t8, zero, L(wexit89)
	ext	a0, a2, 8, 8
	bne	t8, t9, L(wexit89)
	ext	a1, a3, 8, 8
#  endif  /* USE_ALIGN_INST */

# endif /* __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ */

	beq	a0, zero, L(wexit01)
	bne	a0, a1, L(wexit01)

	/* The other bytes are identical, so just subract the 2 words
	  and return the difference.  */
# if defined(__nanomips__) && !defined(__nanomips_subset)
	movep a0,a1,a2,a3
# else
	move a0, a2
	move a1, a3
# endif

L(wexit01):
	subu	va0, a0, a1
	jr	ra

L(wexit89):
	subu	va0, t8, t9
	jr	ra

#endif /* USE_CLZ */

#if __nanomips__
 #define DELAY_NOP
#else
 #define DELAY_NOP nop
#endif	
	
#define BYTECMP01(OFFSET) \
	lbu	a3, OFFSET(a1); \
	DELAY_NOP; \
	beq	a2, zero, L(bexit01); \
	lbu	t8, OFFSET+1(a0); \
	bne	a2, a3, L(bexit01)

#define BYTECMP89(OFFSET) \
	lbu	t9, OFFSET(a1); \
	DELAY_NOP; \
	beq	t8, zero, L(bexit89); \
	lbu	a2, OFFSET+1(a0); \
	bne	t8, t9, L(bexit89)

	.align 2
L(byteloop):
	lbu	a2, 0(a0)
	BYTECMP01(0)
	BYTECMP89(1)
	BYTECMP01(2)
	BYTECMP89(3)
	BYTECMP01(4)
	BYTECMP89(5)
	BYTECMP01(6)
	lbu	t9, 7(a1)

	PTR_ADDIU a0, a0, 8
	beq	t8, zero, L(bexit89)
	PTR_ADDIU a1, a1, 8
	beq	t8, t9, L(byteloop)

L(bexit89):
	subu	va0, t8, t9
	jr	ra

L(bexit01):
	subu	va0, a2, a3
	jr	ra
	
	.set	at

END(STRCMP_NAME)
#ifndef ANDROID_CHANGES
# ifdef _LIBC
libc_hidden_builtin_def (STRCMP_NAME)
# endif
#endif
