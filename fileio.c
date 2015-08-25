/*

kunzip (unzipping library)

Author: Michael Kohn
 Email: mike@mikekohn.net
   Web: http://www.mikekohn.net/

Copyright 2005-2015 by Michael Kohn

This package is licensed under the LGPL. You are free to use this library
in both commercial and non-commercial applications as long as you dynamically
link to it. If you statically link this library you must also release your
software under the LGPL. If you need more flexibility in the license email
me and we can work something out. 

*/

#include <stdio.h>
#include <stdlib.h>

int write_int32(FILE *out, int n)
{
  putc((n & 0xff), out);
  putc(((n >> 8) & 0xff), out);
  putc(((n >> 16) & 0xff), out);
  putc(((n >> 24) & 0xff), out);

  return 0;
}

int write_int16(FILE *out, int n)
{
  putc((n & 0xff), out);
  putc(((n >> 8) & 0xff), out);

  return 0;
}

int read_int32(FILE *in)
{
  int c;

  c = getc(in);
  c = c | (getc(in) << 8);
  c = c | (getc(in) << 16);
  c = c | (getc(in) << 24);

  return c;
}

int read_int16(FILE *in)
{
  int c;

  c = getc(in);
  c = c | (getc(in) << 8);

  return c;
}

int read_chars(FILE *in, char *s, int count)
{
  int t;

  for (t = 0; t < count; t++)
  {
    s[t] = getc(in);
  }

  s[t] = 0;

  return 0;
}

int write_chars(FILE *out, char *s)
{
  int t;

  t = 0;
  while(s[t] != 0 && t < 255)
  {
    putc(s[t++], out);
  }

  return 0;
}

int write_int32_b(FILE *out, int n)
{
  putc(((n >> 24) & 0xff), out);
  putc(((n >> 16) & 0xff), out);
  putc(((n >> 8) & 0xff), out);
  putc((n & 0xff), out);

  return 0;
}

int write_int16_b(FILE *out, int n)
{
  putc(((n >> 8) & 0xff), out);
  putc((n & 0xff), out);

  return 0;
}

int read_int32_b(FILE *in)
{
  int c;

  c = getc(in);
  c = (c << 8) | getc(in);
  c = (c << 8) | getc(in);
  c = (c << 8) | getc(in);

  return c;
}

int read_int16_b(FILE *in)
{
  int c;

  c = getc(in);
  c = (c << 8) | getc(in);

  return c;
}

int read_buffer(FILE *in, unsigned char *buffer, int len)
{
  int t = 0;

  while (t < len)
  {
    t = t + fread(buffer + t, 1, len - t, in);
  }

  return t;
}

int write_buffer(FILE *out, unsigned char *buffer, int len)
{
  int t = 0;

  while (t < len)
  {
    t = t + fwrite(buffer + t, 1, len - t, out);
  }

  return t;
}



