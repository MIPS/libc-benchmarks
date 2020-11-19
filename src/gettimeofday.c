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

#include <time.h>
#include <sys/time.h>

#ifdef __riscv
#  define read_CCRes() 1
#  define read_Count() ({ unsigned long __tmp; \
       __asm__ volatile("csrr %0, mcycle" : "=r"(__tmp)); \
       __tmp; })
#elif (__mips_isa_rev >= 2)
#  define read_CCRes()			\
({ int __ccres;				\
	__asm__ __volatile__(		\
	"rdhwr\t%0,$3\n\t"		\
	: "=r" (__ccres));		\
	__ccres;			\
})

#  define read_Count()			\
({ int __count;				\
	__asm__ __volatile__(		\
	"rdhwr\t%0,$2\n\t"		\
	: "=r" (__count));		\
	__count;			\
})
# else
#  define read_CCRes() 0
#  define read_Count() 0
#endif

#ifndef FREQ_MHZ
#define FREQ_MHZ 1000
#endif

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
  unsigned int ccres = read_CCRes();
  unsigned int count = read_Count();
  unsigned int cycles = (ccres * count);
  tp->tv_sec = cycles / ((unsigned int)FREQ_MHZ * 1000000);
  tp->tv_nsec = cycles % ((unsigned int)FREQ_MHZ * 1000000);
  return 1000;
}
