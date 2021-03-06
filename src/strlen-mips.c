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

#define do_strlen_word(__av) {\
    if (detect_zero(x,x,_01s,_80s)) break;\
    x = __av;\
    cnt += sizeof (unsigned);\
    }

#define do_strlen_byte(__x) {\
  if ((bx.b.B##__x) == 0) break;\
  ++cnt;\
  }

#if SMOKE_TEST_MIPS_STRLEN
#define strlen my_strlen
#endif

#ifndef __mips_using_dsp
def_and_set_01 (_01s);
def_and_set_80 (_80s);
#endif

int
strlen (const void *_a)
{
  int cnt = 0;
  unsigned x;
#ifdef __mips_using_dsp
  def_and_set_01 (_01s);
  def_and_set_80 (_80s);
#endif

  /* align the string to word boundary so we can do word at a time.  */
  if ((cvt_ptr_to (unsigned, _a) & (sizeof (unsigned) - 1)) != 0)
    {
      if ((cvt_ptr_to (unsigned, _a) & 1) != 0)
	{
	  if (get_byte (_a, 0) == 0)
	    return cnt;
	  /* set bit 1 so 2-bytes are checked and incremented. */
	  inc_ptr_as (char *, _a, 1);
	  ++cnt;
	}
      if ((cvt_ptr_to (unsigned, _a) & 2) != 0)
	{
	  if (get_byte (_a, 0) == 0)
	    return cnt + 0;
	  if (get_byte (_a, 1) == 0)
	    return cnt + 1;
	  inc_ptr_as (char *, _a, 2);
	  cnt += 2;
	}
    }

#if __mips64
  //#error strlen: mips64 check for 4-byte alignment not implemented.
#endif

  if (1)
    {

      /* as advantagous as it is to performance, this code cannot pre-load
         the following word, nor can it prefetch the next line at the start
         of the loop since the string can be at the end of a page with the
         following page unmapped. There are tests in the suite to catch
         any attempt to go beyond the current word. */
      x = get_word (_a, 0);
      while (1)
	{
	  /* doing 8 words should cover most strings.  */
	  do_strlen_word (get_word (_a, 1));
	  do_strlen_word (get_word (_a, 2));
	  do_strlen_word (get_word (_a, 3));
	  do_strlen_word (get_word (_a, 4));
	  do_strlen_word (get_word (_a, 5));
	  do_strlen_word (get_word (_a, 6));
	  do_strlen_word (get_word (_a, 7));
	  do_strlen_word (get_word (_a, 8));
	  inc_ptr_as (unsigned *, _a, 8);
	}
    }
  while (1)
    {
      /* pull apart the last word processed and find the zero.  */
      bitfields_t bx;
      bx.v = x;
#if __mips64
      do_strlen_byte (0);
      do_strlen_byte (1);
      do_strlen_byte (2);
      do_strlen_byte (3);
      do_strlen_byte (4);
      do_strlen_byte (5);
      do_strlen_byte (6);
#else
      do_strlen_byte (0);
      do_strlen_byte (1);
      do_strlen_byte (2);
#endif
      /* last byte is zero */
      break;
    }
  return cnt;
}

#undef do_strlen_byte
#undef do_strlen_word

#if SMOKE_TEST_MIPS_STRLEN
#include <stdio.h>
char str1[] = "DHRYSTONE PROGRAM, 1'ST STRING";
char str2[] = "DHRYSTONE PROGRAM, 2'ST STRING";

char str3[] = "another string";
char str4[] = "another";

char str5[] = "somes tring";
char str6[] = "somes_tring";

char str7[16], str8[16];

static char *
chk (unsigned mine, unsigned libs, int *errors)
{
  static char answer[1024];
  char *result = mine == libs ? "PASS" : "FAIL";
  sprintf (answer, "new_strlen=%d: lib_strlen=%d: %s!", mine, libs, result);
  if (mine != libs)
    (*errors)++;
  return answer;
}

int
main (int argc, char **argv)
{
  int errors = 0;
  /* set -1 in one position */
  str6[5] = 0xff;
  /* set zero in same position with junk in following 3 */
  str7[0] = str8[0] = 0;
  str7[1] = 0xff;
  str7[2] = 'a';
  str7[3] = 2;
  str8[1] = 's';
  str8[2] = -2;
  str8[3] = 0;

  fprintf (stderr, "========== mips_strlen%s test...\n",
	   argv[0] ? argv[0] : "unknown strlen");
#define P(__x,__y) {\
    int a = my_strlen(__x + __y);\
    int b = (strlen)(__x + __y) /* library version */;\
    fprintf(stderr,"%s+%d: %s\n",#__x,__y,chk(a,b,&errors));\
    }

  P (str1, 0);
  P (str1, 1);
  P (str1, 2);
  P (str1, 3);

  P (str2, 0);
  P (str2, 1);
  P (str2, 2);
  P (str2, 3);

  P (str3, 0);
  P (str3, 1);
  P (str3, 2);
  P (str3, 3);

  P (str4, 0);
  P (str4, 1);
  P (str4, 2);
  P (str4, 3);

  P (str5, 0);
  P (str5, 1);
  P (str5, 2);
  P (str5, 3);

  P (str6, 0);
  P (str6, 1);
  P (str6, 2);
  P (str6, 3);

  P (str7, 0);
  P (str7, 1);
  P (str7, 2);
  P (str7, 3);

  P (str8, 0);
  P (str8, 1);
  P (str8, 2);
  P (str8, 3);

  return errors;
}
#endif
