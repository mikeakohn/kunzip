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

#ifndef _ZIPFILE_H
#define _ZIPFILE_H

#define VERSION "Version August 15, 2015"
#define COPYRIGHT "Copyright 2005-2015 - Michael Kohn <mike@mikekohn.net>"

struct zip_local_file_header_t
{
  unsigned int signature;
  int version;
  int general_purpose_bit_flag;
  int compression_method;
  int last_mod_file_time;
  int last_mod_file_date;
  unsigned int crc_32;
  int compressed_size;
  int uncompressed_size;
  int file_name_length;
  int extra_field_length;
  char *file_name;
  unsigned char *extra_field;
};

#endif

