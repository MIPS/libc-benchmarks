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

	.file	1 "memset-c.c"
	.linkrelax
	.module	softfloat
	.text
	.align	1
	.globl	memset
	.ent	memset
	.type	memset, @function
memset:
	.frame	$sp,32,$ra		# vars= 0, regs= 5/0, args= 0, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	save	32,$ra,$s0-$s3
	move	$s1,$a2
	movep	$s3,$s2,$a0,$a1
	bgeiuc	$a2,16,$L2
	beqzc	$a2,$L21
	andi	$a1,$s2,0x00ff
	balc	memset
$L21:
	move	$a0,$s3
	restore.jrc	32,$ra,$s0-$s3
$L2:
	andi	$a3,$s3,0x3
	bnezc	$a3,$L39
	move	$a0,$s3
	move	$a3,$zero
	beqzc	$s2,$L12
$L40:
	andi	$s2,$s2,0x00ff
	ins	$a3,$s2,0,8
	ins	$a3,$s2,8,8
	ins	$a3,$s2,16,8
	ins	$a3,$s2,24,8
	andi	$a1,$a3,0x00ff
$L6:
	srl	$a4,$s1,2
	andi	$a2,$s1,0x3
	andi	$s1,$a4,0x7
	subu	$a4,$a4,$s1
	beqzc	$a4,$L7
	move	$s0,$zero
$L8:
	addiu	$s0,$s0,8
	sw	$a3,0($a0)
	sw	$a3,4($a0)
	sw	$a3,8($a0)
	sw	$a3,12($a0)
	sw	$a3,16($a0)
	sw	$a3,20($a0)
	sw	$a3,24($a0)
	sw	$a3,28($a0)
	addiu	$a0,$a0,32
	bltuc	$s0,$a4,$L8
$L7:
	beqzc	$s1,$L9
	sw	$a3,0($a0)
	beqic	$s1,1,$L10
	sw	$a3,4($a0)
	beqic	$s1,2,$L10
	sw	$a3,8($a0)
	beqic	$s1,3,$L10
	sw	$a3,12($a0)
	beqic	$s1,4,$L10
	sw	$a3,16($a0)
	beqic	$s1,5,$L10
	sw	$a3,20($a0)
	bneic	$s1,7,$L10
	sw	$a3,24($a0)
$L10:
	lsa	$a0,$s1,$a0,2
$L9:
	beqzc	$a2,$L21
	balc	memset
	move	$a0,$s3
	restore.jrc	32,$ra,$s0-$s3
$L39:
	li	$s0,4			# 0x4
	andi	$a1,$s2,0x00ff
	subu	$s0,$s0,$a3
	move	$a2,$s0
	subu	$s1,$s1,$s0
	balc	memset
	addu	$a0,$s3,$s0
	move	$a3,$zero
	bnezc	$s2,$L40
$L12:
	move	$a1,$zero
	move	$a3,$zero
	bc	$L6
	.set	macro
	.set	reorder
	.end	memset
	.size	memset, .-memset
	.ident	"GCC: (Codescape GNU Tools NON-PRODUCTION nanoMIPS v86 for nanoMIPS IMG Bare Metal) 6.3.0"
