/* Measure memcpy functions.
   Copyright (C) 2018-2020, MIPS Tech LLC.

   This is a modification of a glibc test case for bench-marking.

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

#ifndef SCOND
#define SCOND(S) ((S) <= 256)
#endif

#ifndef ACOND
#define ACOND(A1,A2,A3) (((A1) != 0) && ((A1) % 32 == 0) && ((A2) != 0) && ((A2) % 32 == 0) && ((A3) >= 32) && ((A3) % 32 == 0))
#endif
#include "bench-memcpy-common.c"

