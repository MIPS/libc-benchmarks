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

# define TEST_NAME "jump opt"
# define MAX_BUFSIZE (1 << 16)
# include "bench-string.h"

#define MAX_JUMP_CASES 14
#define CACHE_SIZE 64*1024

#ifndef TEST_CFLAGS
#define TEST_CFLAGS " "
#endif

#include <stdio.h>
#include <stdlib.h>


/* Main */

int Buffer[32];
int Buffer2[32];

#define read_CCRes()			\
({ int __ccres;				\
	__asm__ __volatile__(		\
	"rdhwr\t%0,$3\n\t"		\
	: "=r" (__ccres));		\
	__ccres;			\
})

#define read_Count()			\
({ int __count;				\
	__asm__ __volatile__(		\
	"rdhwr\t%0,$2\n\t"		\
	: "=r" (__count));		\
	__count;			\
})


int
main (int argc, char **argv)
{
  size_t i, j;
  int cur, start, stop, res;
  for (i = 0; i < 32; i++)
    j = Buffer[i];
  for (i = 0; i < 32; i++)
    j = Buffer[i];
  for (i = 0; i < 32; i++)
    j = Buffer[i];
  for (i = 0; i < 20; i++)
    j = Buffer[i];
  for (i = 0; i < 20; i++)
    j = Buffer[i];

  
  for (j = 0; j < 25; j++)
    {
      start = read_Count ();
  for (i = 0; i < 10; i++)
    {
      #if 0
      j += Buffer[i];
      #else
      j += Buffer[0];
      j += Buffer[1];
      j += Buffer[2];
      j += Buffer[3];
      j += Buffer[4];
      j += Buffer[5];
      j += Buffer[6];
      j += Buffer[7];
      j += Buffer[8];
      j += Buffer[9];
      #endif
    }
  
  stop = read_Count ();
  res = read_CCRes ();
  printf ("%d\n", (stop - start) * res);
    }
  
  return j;
}
