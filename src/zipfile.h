/*

kunzip (unzipping library)

Author: Michael Kohn
 Email: mike@mikekohn.net
   Web: https://www.mikekohn.net/

Copyright 2005-2023 by Michael Kohn

This package is licensed under the LGPL. You are free to use this library
in both commercial and non-commercial applications as long as you dynamically
link to it. If you statically link this library you must also release your
software under the LGPL. If you need more flexibility in the license email
me and we can work something out.

*/

#ifndef _ZIPFILE_H
#define _ZIPFILE_H

#include <stdint.h>

#define VERSION "Version October 17, 2023"
#define COPYRIGHT "Copyright 2005-2023 - Michael Kohn <mike@mikekohn.net>"

struct zip_local_file_header_t
{
  uint32_t signature;
  int version;
  int general_purpose_bit_flag;
  int compression_method;
  int last_mod_file_time;
  int last_mod_file_date;
  uint32_t crc_32;
  int compressed_size;
  int uncompressed_size;
  int file_name_length;
  int extra_field_length;
  char *file_name;
  unsigned char *extra_field;
};

struct zip_central_directory_t
{
  uint32_t signature;
  uint16_t version;
  uint16_t version_needed;
  uint16_t flags;
  uint16_t compression;
  uint16_t mod_time;
  uint16_t mod_date;
  uint32_t crc_32;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
  uint16_t file_name_length;
  uint16_t extra_field_length;
  uint16_t file_comment_length;
  uint16_t disk_num_start;
  uint16_t internal_attr;
  uint32_t external_attr;
  uint32_t offset_of_local_header;
};

#endif

