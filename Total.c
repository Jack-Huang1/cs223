/* 
 CS 223b, hw1
 Name: Jack Huang
 netid: jh2533
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define ungetchar(c)  ungetc(c, stdin) // unread char read from stdin

int mygetchar() 
{
  int ch = getchar();
  while (ch == '\\') // gets rid of line splices
  {
    ch = getchar();
    ch = getchar();
  }
  return ch;
}

int main(int argc, char **argv) 
{
  int ch;
  bool inorout = false; // checks if ch being read is part of a term
  bool basecheck, neg; // checks for the base and sign of a term
  int base;
  long hold = 0; // variable to hold current term
  long total = 0;
  while ((ch = mygetchar()) != EOF)
  { 
    if (ch == '-' && inorout == false) // checks for negative signs
    {
      neg = true;
      continue;
    }
    if (basecheck == true)
    {
      basecheck = false;
      if (ch <= '7' && ch >= '1') // base is octal
      {
        base = 8;
        hold = ch - '0';
        inorout = true;
        continue;
      }
      switch(ch)
      {
        case 'X': case 'x': // base is hexadecimal
          base = 16;
          inorout = true;
          continue;
        case 'B': case 'b': // base is binary
          base = 2;
          inorout = true;
          continue;
      }
      if (ch == '0') // checks for consecutive zeroes
      {
        basecheck = true;
        continue;
      }
    }
    if (base == 16 && inorout == true) // accounts for letters in hex
    {
      if (ch > '9') // checks if ch is a letter
      {
        if (isxdigit(ch)) // checks if ch is a relevant letter (a-f, A-F)
        {
          if (ch >= 'A' && ch <= 'F') // checks for uppercase
            ch = tolower(ch);
          hold = (hold * 16) + (10 + ch - 'a');
          continue;
        } 
      }
    }
    if (isdigit(ch))
    {
      if (base == 2 && ch > '1') // catches invalid numbers within binary 
        goto invalid;
      if (base == 8 && ch > '7') // catches invalid numbers within octal
      {
        invalid:
        inorout = false;
        if (neg == true) // treats hold as a negative value if necessary
        {
          neg = false;
          total -= hold;
          goto finish;
        }
        total += hold;
        finish:
          base = 10;
          ungetchar(ch); // avoids skipping over a number
          continue;
      }
      if (ch == '0' && inorout == false) // initiates basechecking
      {
        basecheck = true;
        continue;
      }
      if (inorout == false) // new number term at base 10
      {
        inorout = true;
        base = 10;
        hold = ch - '0';
        continue;
      }
      hold = (hold * base) + (ch - '0');  // adds the value of ch appropriately
    }
    else // invalid value and/or end of a term
    {
      if (ch == '\"') // checks for quotes
      {
        while ((ch = mygetchar()) != EOF)
        {
          if (ch == '\"')
            break;
        }
        if (ch == EOF)
        {
          exit(0); // checks for an uneven number of quotation marks
        }
      }
      inorout = false;
      if (neg == true) // treats hold as a negative value if necessary
      {
        neg = false;
        total -= hold;
        goto end;
      }
      total += hold;
      end:
        hold = 0;
        base = 10; // resets the base to decimal by default
        if (ch == '-')
          ungetchar(ch);
    }
  }
  printf("Total: %ld\n", total);
}
