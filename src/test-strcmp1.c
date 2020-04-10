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


#include <stdio.h>
#include <string.h>

char str1[] = "DHRYSTONE PROGRAM, 1'ST STRING";
char str2[] = "DHRYSTONE PROGRAM, 2'ST STRING";

char str3[] = "another string";
char str4[] = "another";

char str5[] = "somes tring";
char str6[] = "somes_tring";

char str7[16], str8[16];

/* Strcmp uses unsigned semantics for comparison. */
int
simple_strcmp (const char *s1, const char *s2)
{
  int ret;

  while ((ret = *(unsigned char *) s1 - *(unsigned char*) s2++) == 0 && *s1++);
  return ret;
}


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
    int x = simple_strcmp(__x,__y);\
    int y = (strcmp)(__x,__y) /* library version */;\
    printf("x=0x%08x,y=0x%08x: %s\n",x,y,chk(x,y,&errors));\
    x = simple_strcmp(__y,__x);\
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
