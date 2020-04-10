/* Copyright (C) 2018-2020 MIPS Tech LLC
   Snarfed from glibc/string/test-strcmp.c, modified for bench-marking.  */

/* Copyright (C) 2010 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include "mips-string-ops.h"

#define do_strcmp_iteration(__av,__bv) {\
    int hz = detect_zero(x,y,_01s,_80s);\
    if (x != y) break;\
    if (hz) return 0;\
    x = __av;\
    y = __bv;\
    }

#define do_unaligned_strcmp_iteration(__x,__y) {\
    if (b1 == 0 || b1 - b2 != 0) return b1 - b2;\
    b1 = __x;\
    b2 = __y;\
    }

#define do_strcmp_byte(__x) {\
    if (bx.b.B##__x - by.b.B##__x != 0 || bx.b.B##__x == 0) \
	return bx.b.B##__x - by.b.B##__x;\
    }

#define do_final_strcmp_byte(__x) {\
    return bx.b.B##__x - by.b.B##__x;\
    }

#if SMOKE_TEST_MIPS_STRCMP
#define strcmp my_strcmp
#endif

int
strcmp (const void *_a, const void *_b)
{
  unsigned x, y;
  def_and_set_01 (_01s);
  def_and_set_80 (_80s);

  /* Only check for both aligned. To determine and align the pointers
     otherwise creates too much overhead. If unaligned, then use lwl/lwr. */
#if __mips_isa_rev < 6 && defined (__nanomips__)
  if (((cvt_ptr_to (unsigned, _a) | cvt_ptr_to (unsigned, _b)) &
       (sizeof (unsigned) - 1)) == 0)
#endif
    {
      /* as advantagous as it is to performance, this code cannot pre-load
         the following word, nor can it prefetch the next line at the start
         of the loop since the string can be at the end of a page with the
         following page unmapped. There are tests in the suite to catch
         any attempt to go beyond the current word. */

      x = get_word (_a, 0);
      y = get_word (_b, 0);;
      while (1)
	{
	  /* doing 8 words should cover most strings.  */
	  do_strcmp_iteration (get_word (_a, 1), get_word (_b, 1));
	  do_strcmp_iteration (get_word (_a, 2), get_word (_b, 2));
	  do_strcmp_iteration (get_word (_a, 3), get_word (_b, 3));
	  do_strcmp_iteration (get_word (_a, 4), get_word (_b, 4));
	  do_strcmp_iteration (get_word (_a, 5), get_word (_b, 5));
	  do_strcmp_iteration (get_word (_a, 6), get_word (_b, 6));
	  do_strcmp_iteration (get_word (_a, 7), get_word (_b, 7));
	  do_strcmp_iteration (get_word (_a, 8), get_word (_b, 8));
	  inc_ptr_as (unsigned *, _a, 8);
	  inc_ptr_as (unsigned *, _b, 8);
	}
      /* don't put any code here */
    }
#if __mips_isa_rev < 6 && defined (__nanomips__)
  else
    {
#if 0				/* allow unaliged loads for bare metal for Don's testing.  */

      /* really want to use the unaligned loads, but that can run off
         the end of memory, or run into a DMA buffer etc.; this is the
         same issue as the aligned code above. When -Ofast is added for
         the MIPS processors, as well as -minline-stringops, we can use
         the commented out code below. */

      unsigned char b1 = get_byte (_a, 0);
      unsigned char b2 = get_byte (_b, 0);
      while (1)
	{
	  /* doing 8 bytes should cover many strings.  */
	  do_unaligned_strcmp_iteration (get_byte (_a, 1), get_byte (_b, 1));
	  do_unaligned_strcmp_iteration (get_byte (_a, 2), get_byte (_b, 2));
	  do_unaligned_strcmp_iteration (get_byte (_a, 3), get_byte (_b, 3));
	  do_unaligned_strcmp_iteration (get_byte (_a, 4), get_byte (_b, 4));
	  do_unaligned_strcmp_iteration (get_byte (_a, 5), get_byte (_b, 5));
	  do_unaligned_strcmp_iteration (get_byte (_a, 6), get_byte (_b, 6));
	  do_unaligned_strcmp_iteration (get_byte (_a, 7), get_byte (_b, 7));
	  do_unaligned_strcmp_iteration (get_byte (_a, 8), get_byte (_b, 8));
	  inc_ptr_as (unsigned char *, _a, 8);
	  inc_ptr_as (unsigned char *, _b, 8);
	}
#else
      x = _ULW (get_word (_a, 0));
      y = _ULW (get_word (_b, 0));;
      while (1)
	{
	  do_strcmp_iteration (_ULW (get_word (_a, 1)),
			       _ULW (get_word (_b, 1)));
	  do_strcmp_iteration (_ULW (get_word (_a, 2)),
			       _ULW (get_word (_b, 2)));
	  do_strcmp_iteration (_ULW (get_word (_a, 3)),
			       _ULW (get_word (_b, 3)));
	  do_strcmp_iteration (_ULW (get_word (_a, 4)),
			       _ULW (get_word (_b, 4)));
	  inc_ptr_as (unsigned *, _a, 4);
	  inc_ptr_as (unsigned *, _b, 4);
	}
      /* don't put any code here */
#endif
    }
#endif

  if (1)
    {
      /* pull apart the last words processed and find byte difference.
         This code assumes that the compiler can handle small structs
         in regs.  It also does not use the cvt_to_ptr macro since
         that would force the variables into memory.  */
      bitfields_t bx, by;
      bx.v = x;
      by.v = y;
      do_strcmp_byte (0);
      do_strcmp_byte (1);
      do_strcmp_byte (2);
#if __mips64
      do_strcmp_byte (3);
      do_strcmp_byte (4);
      do_strcmp_byte (5);
      do_strcmp_byte (6);
      do_final_strcmp_byte (7);	/* always returns */
#else
      do_final_strcmp_byte (3);	/* always returns */
#endif
    }
  /* Never reached.  */
}

#undef do_strcmp_byte
#undef do_final_strcmp_byte
#undef do_strcmp_iteration

#if SMOKE_TEST_MIPS_STRCMP
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
  char mine_answer = (int) mine < 0 ? '<' : ((int) mine > 0 ? '>' : '=');
  char libs_answer = (int) libs < 0 ? '<' : ((int) libs > 0 ? '>' : '=');
  char *result = mine_answer == libs_answer ? "PASS" : "FAIL";
  sprintf (answer, "new_strcmp %c 0: lib_strcmp %c 0: %s!", mine_answer,
	   libs_answer, result);
  if (mine_answer != libs_answer)
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

#define P(__x,__y) {\
    int x = my_strcmp(__x,__y);\
    int y = (strcmp)(__x,__y) /* library version */;\
    printf("x=0x%08x,y=0x%08x: %s\n",x,y,chk(x,y,&errors));\
    x = my_strcmp(__y,__x);\
    y = (strcmp)(__y,__x) /* library version */;\
    printf("x=0x%08x,y=0x%08x: %s\n",x,y,chk(x,y,&errors));\
    }

  /* aligned strings */
  P (str1, str2);
  P (str3, str4);
  P (str5, str6);
  P (str7, str8);

  /* unaligned strings */
  P (str1 + 1, str2 + 1);
  P (str3 + 1, str4 + 1);
  P (str5 + 1, str6 + 1);
  P (str7 + 1, str8 + 1);
  return errors;
}
#endif
