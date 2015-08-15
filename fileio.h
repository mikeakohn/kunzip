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

#ifndef _FILEIO_H
#define _FILEIO_H

int write_int(FILE *out, int n);
int write_word(FILE *out, int n);

int read_int(FILE *in);
int read_word(FILE *in);

int read_chars(FILE *in, char *s, int count);
int write_chars(FILE *out, char *s);

int write_int_b(FILE *out, int n);
int write_word_b(FILE *out, int n);

int read_int_b(FILE *in);
int read_word_b(FILE *in);

int read_buffer(FILE *in, unsigned char *buffer, int len);
int write_buffer(FILE *in, unsigned char *buffer, int len);

#endif

