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

#ifndef _KINFLATE_H
#define _KINFLATE_H

int inflate(FILE *in, FILE *out, unsigned int *checksum);
int inflate_init();
int inflate_free();
unsigned int crc32(unsigned char *buffer, int len, unsigned int crc);

#endif

