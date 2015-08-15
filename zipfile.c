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
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef DLL
#include <utime.h>
#endif
#include <time.h>
#include <unistd.h>

#include "fileio.h"
#include "zipfile.h"
#include "kinflate.h"
#include "kunzip.h"

#ifdef DLL

struct utimbuf
{
  int actime;
  int modtime;
};

int utime(char *outname, struct utimbuf *my_utimbuf);

char *strcasestr_m(char *hejstack, char *needle)
{
  int hejlen,neelen;
  int t,r;

  hejlen = strlen(hejstack);
  neelen = strlen(needle);
  r = hejlen - neelen;

  for(t = 0; t <= r; t++)
  {
    if (strncasecmp(&hejstack[t], needle, neelen) == 0)
    { return &hejstack[t]; }
  }

  return NULL;
}

#endif

#define BUFFER_SIZE 16738

/* #define _GNU_SOURCE */

char *strcasestr(const char *haystack, const char *needle);

/* These CRC32 functions were taken from the gzip spec and kohninized */
/*

int crc_built=0;
unsigned int crc_table[256];

int build_crc32()
{
  unsigned int c;
  int n,k;

  for (n=0; n<256; n++)
  {
    c=(unsigned int)n;
    for (k=0; k<8; k++)
    {
      if (c&1)
      { c=0xedb88320^(c>>1); }
        else
      { c=c>>1; }
    }
    crc_table[n]=c;
  }

  crc_built=1;

  return 0;
}

unsigned int crc32(FILE *in)
{
unsigned int crc;
int ch;

  if (crc_built==0) build_crc32();

  crc=0xffffffff;

  while(1)
  {
    ch=getc(in);
    if (ch==EOF) break;

    crc=crc_table[(crc^ch)&0xff]^(crc>>8);
  }

  return crc^0xffffffff;
}
*/

unsigned int copy_file(FILE *in, FILE *out, int len)
{
  unsigned char buffer[BUFFER_SIZE];
  uint32_t checksum;
  int t,r;

  checksum = 0xffffffff;

  t = 0;

  while(t < len)
  {
    if (t + BUFFER_SIZE < len)
    { r = BUFFER_SIZE; }
      else
    { r = len - t; }

    read_buffer(in, buffer, r);
    write_buffer(out, buffer, r);
    checksum = crc32(buffer, r, checksum);
    t = t + r;
  }

  return checksum ^ 0xffffffff;
}

int create_dirs(char *filename)
{
  struct stat buf;
  char path[1024];
  int s;

  strcpy(path, filename);

  s = strlen(path);
  if (s == 0) { return -1; }

  while (path[s] != '/' && path[s] != '\\') { s--; }
  path[s] = 0;

#ifdef DEBUG_ZIP
printf("path=%s\n", path);
#endif

  if (stat(path, &buf) != 0)
  {
    if (create_dirs(path) == -1) { return -1; }
#ifdef DEBUG_ZIP
    printf("Creating directory: %s\n", path);
#endif
#ifndef DLL
    if (mkdir(path, 0777) != 0) { return -1; }
#else
    if (mkdir(path) != 0) { return -1; }
#endif
  }

  return 0;
}

int read_zip_header(FILE *in, struct zip_local_file_header_t *local_file_header)
{
  local_file_header->signature = read_int(in);
  if (local_file_header->signature != 0x04034b50) { return -1; }

  local_file_header->version = read_word(in);
  local_file_header->general_purpose_bit_flag = read_word(in);
  local_file_header->compression_method = read_word(in);
  local_file_header->last_mod_file_time = read_word(in);
  local_file_header->last_mod_file_date = read_word(in);
  local_file_header->crc_32 = read_int(in);
  local_file_header->compressed_size = read_int(in);
  local_file_header->uncompressed_size = read_int(in);
  local_file_header->file_name_length = read_word(in);
  local_file_header->extra_field_length = read_word(in);

  return 0;
}

#ifdef DEBUG_ZIP
int print_zip_header(struct zip_local_file_header_t *local_file_header)
{
  const char *compression_method = "";
  const char *compression_methods[] =
  {
    "No Compression",
    "Shrunk",
    "Reduced, Compression Factor 1",
    "Reduced, Compression Factor 2",
    "Reduced, Compression Factor 3",
    "Reduced, Compression Factor 4",
    "Imploded",
    "reserved",
    "Deflated",
    "Enhanced Deflated",
    "PKWare DCL imploded",
    "reserved",
    "LZMA"
  };

  int flags = local_file_header->general_purpose_bit_flag;

  if (local_file_header->compression_method < sizeof(compression_methods) / sizeof (char *))
  {
    compression_method = compression_methods[local_file_header->compression_method];
  }

  printf("ZIP LOCAL FILE HEADER\n");
  printf("----------------------------------\n");
  printf("Signature: %02x%02x%02x%02x\n",
                               ((local_file_header->signature >> 24) & 255),
                               ((local_file_header->signature >> 16) & 255),
                               ((local_file_header->signature >> 8) & 255),
                               (local_file_header->signature & 255));
  printf("Version: %d\n", local_file_header->version);
  printf("General Purpose Bit Flag: %d (",
    local_file_header->general_purpose_bit_flag);
  if ((flags & (1<<0)) != 0) { printf(" encrypted"); }
  if ((flags & (1<<1)) != 0) { printf(" compression1"); }
  if ((flags & (1<<2)) != 0) { printf(" compression2"); }
  if ((flags & (1<<3)) != 0) { printf(" data_desc"); }
  if ((flags & (1<<4)) != 0) { printf(" enhanced_deflate"); }
  if ((flags & (1<<5)) != 0) { printf(" compressed_patch"); }
  if ((flags & (1<<6)) != 0) { printf(" strong_encryption"); }
  if ((flags & (1<<11)) != 0) { printf(" language_encoding"); }
  if ((flags & (1<<12)) != 0) { printf(" reserved12"); }
  if ((flags & (1<<13)) != 0) { printf(" masked_header"); }
  printf(" )\n");
  printf("Compression Method: %d (%s)\n",
    local_file_header->compression_method,
    compression_method);
  printf("Last Mod File Time: %d\n", local_file_header->last_mod_file_time);
  printf("Last Mod File Date: %d\n", local_file_header->last_mod_file_date);
  printf("CRC-32: 0x%08x\n", local_file_header->crc_32);
  printf("Compressed Size: %d\n", local_file_header->compressed_size);
  printf("Uncompressed Size: %d\n", local_file_header->uncompressed_size);
  printf("File Name Length: %d\n", local_file_header->file_name_length);
  printf("Extra Field Length: %d\n", local_file_header->extra_field_length);
  printf("File Name: %s\n", local_file_header->file_name);

  return 0;
}
#endif

int kunzip_file(FILE *in, char *base_dir)
{
  char outname[1024];
  FILE *out;
  struct zip_local_file_header_t local_file_header;
  int ret_code;
  time_t date_time;
  uint32_t checksum;
  long marker;
  struct utimbuf my_utimbuf;
  struct tm my_tm;

  ret_code=0;

  if (read_zip_header(in, &local_file_header) == -1) { return -1; }

  // FIXME - Use alloca().
  local_file_header.file_name = (char *)malloc(local_file_header.file_name_length + 1);
  local_file_header.extra_field = (unsigned char *)malloc(local_file_header.extra_field_length + 1);

  read_chars(in, local_file_header.file_name, local_file_header.file_name_length);
  read_chars(in, (char *)local_file_header.extra_field, local_file_header.extra_field_length);

  marker = ftell(in);

#ifdef DEBUG_ZIP
  print_zip_header(&local_file_header);
#endif

  if (base_dir[strlen(base_dir) - 1] != '/' &&
      base_dir[strlen(base_dir) - 1] != '\\')
  {
    sprintf(outname, "%s/%s", base_dir, local_file_header.file_name);
  }
    else
  {
    sprintf(outname, "%s%s", base_dir, local_file_header.file_name);
  }

  if (create_dirs(outname) != 0)
  {
    printf("Could not create directories\n");
    return -2;
  }

#ifndef QUIET
  printf("unzipping: %s\n", outname);
#endif

  if (local_file_header.uncompressed_size != 0)
  {
    out = fopen(outname, "wb+");
    if (out == NULL)
    {
      printf("Error: Cannot open output file: %s\n",outname);
      return -3;
    }

    if (local_file_header.compression_method == 0)
    {
      checksum = copy_file(in, out, local_file_header.uncompressed_size);
    }
      else
    {
      inflate(in, out, &checksum);
/* printf("start=%d end=%d total=%d should_be=%d\n",marker,(int)ftell(in),(int)ftell(in)-marker,local_file_header.compressed_size); */
    }

    fclose(out);

/*

From google groups:

The time is store in a WORD (2 bytes/16bits) where :
Bit  0 -  4  Seconds / 2
     5 - 10  Minutes
    11 - 15  Hours

and the date in another WORD with the following structure :
Bit  0 -  4  Day
     5 -  8  Month
     9 - 15  Years since 1980 

^^^ PUKE!!!!!!!!!!!!

That's MS-DOS time format btw.. which zip files use.. 

*/
    /* memset(&my_tm,0,sizeof(struct tm)); */

    my_tm.tm_sec = (local_file_header.last_mod_file_time & 31) * 2;
    my_tm.tm_min = (local_file_header.last_mod_file_time >> 5) & 63;
    my_tm.tm_hour = (local_file_header.last_mod_file_time >> 11);
    my_tm.tm_mday = (local_file_header.last_mod_file_date & 31);
    my_tm.tm_mon = ((local_file_header.last_mod_file_date >> 5) & 15) - 1;
    my_tm.tm_year = (local_file_header.last_mod_file_date >> 9) + 80;
    my_tm.tm_wday = 0;
    my_tm.tm_yday = 0;
    my_tm.tm_isdst = 0;

    date_time = mktime(&my_tm);

    my_utimbuf.actime = date_time;
    my_utimbuf.modtime = date_time;
    utime(outname, &my_utimbuf);

    if (checksum != local_file_header.crc_32)
    {
      printf("Checksums don't match: %d %d\n",
             checksum, local_file_header.crc_32);
      ret_code = -4;
    }
  }

  free(local_file_header.file_name);
  free(local_file_header.extra_field);

  fseek(in, marker + local_file_header.compressed_size, SEEK_SET);

  if ((local_file_header.general_purpose_bit_flag & 8) != 0)
  {
    read_int(in);
    read_int(in);
    read_int(in);
  }

  return ret_code;
}

int kunzip_all(char *zip_filename, char *base_dir)
{
  FILE *in;
  int i;

  in = fopen(zip_filename, "rb");
  if (in == NULL) { return -1; }

  while(1)
  {
    i = kunzip_file(in, base_dir);
    if (i != 0) { break; }
  }

  fclose(in);

  return 0;
}

long kunzip_next(char *zip_filename, char *base_dir, int offset)
{
  FILE *in;
  int i;
  long marker;

  in=fopen(zip_filename, "rb");
  if (in == NULL) { return -1; }

  fseek(in, offset, SEEK_SET);

  i = kunzip_file(in, base_dir);
  marker = ftell(in);
  fclose(in);
  if (i < 0) { return i; }

  return marker;
}

int kunzip_count_files(char *zip_filename)
{
  FILE *in;
  struct zip_local_file_header_t local_file_header;
  uint32_t checksum;
  int count;
  int i;

  in = fopen(zip_filename,"rb");
  if (in == NULL) { return -1; }

  count = 0;

  while(1)
  {
    i = read_zip_header(in, &local_file_header);
    if (i == -1) break;

    if ((local_file_header.general_purpose_bit_flag & (1<<3)) == 0)
    {
      // Data descriptor flag is not set so compressed size is accurate
      // and the file can be skipped.
      fseek(in, local_file_header.compressed_size +
                local_file_header.file_name_length +
                local_file_header.extra_field_length, SEEK_CUR);
    }
      else
    {
      fseek(in, local_file_header.file_name_length +
                local_file_header.extra_field_length, SEEK_CUR);

      FILE *out = fopen("/dev/null", "wb");
      if (out == NULL)
      {
        break;
      }

#ifdef DEBUG_ZIP
printf("offset=%ld\n", ftell(in));
#endif
      inflate(in, out, &checksum);
#ifdef DEBUG_ZIP
printf("offset=%ld\n", ftell(in));
#endif
      fclose(out);

      read_int(in);
      local_file_header.crc_32 = read_int(in);
      local_file_header.compressed_size = read_int(in);
      local_file_header.uncompressed_size = read_int(in);

#ifdef DEBUG_ZIP
      printf("*CRC-32: 0x%08x\n", local_file_header.crc_32);
      printf("*Compressed Size: %d\n", local_file_header.compressed_size);
      printf("*Uncompressed Size: %d\n", local_file_header.uncompressed_size);
#endif
    }

    count++;
  }

  fclose(in);

  return count;
}

int kunzip_get_offset_by_number(char *zip_filename, int file_count)
{
  FILE *in;
  struct zip_local_file_header_t local_file_header;
  int count;
  int i=0,curr;

  in = fopen(zip_filename, "rb");
  if (in == 0)
  { return -1; }

  count = 0;

  while(1)
  {
    curr = ftell(in);
    if (count == file_count) { break; }

    i = read_zip_header(in, &local_file_header);
    if (i == -1) break;

    fseek(in, local_file_header.compressed_size +
              local_file_header.file_name_length +
              local_file_header.extra_field_length, SEEK_CUR);

    count++;
  }

  fclose(in);

  if (i!=-1)
  { return curr; }
    else
  { return -1; }
}

/*
  Match Flags:
    bit 0: set to 1 if it should be exact filename match
           set to 0 if the archived filename only needs to contain that word
    bit 1: set to 1 if it should be case sensitive
           set to 0 if it should be case insensitive
*/

int kunzip_get_offset_by_name(char *zip_filename, char *compressed_filename, int match_flags, int skip_offset)
{
  FILE *in;
  struct zip_local_file_header_t local_file_header;
  int i=0,curr;
  char *name=0;
  int name_size=0;
  long marker;

  in = fopen(zip_filename, "rb");
  if (in == 0) { return -1; }

  if (skip_offset != -1) { fseek(in, skip_offset, SEEK_SET); }

  while(1)
  {
    curr = ftell(in);
    i = read_zip_header(in,&local_file_header);
    if (i == -1) break;

    if (skip_offset < 0 || curr > skip_offset)
    {
      marker = ftell(in);  /* nasty code.. please make it nice later */

      if (name_size < local_file_header.file_name_length + 1)
      {
        if (name_size!=0) { free(name); }
        name_size = local_file_header.file_name_length + 1;
        // FIXME - alloca this.
        name = malloc(name_size);
      }

      read_chars(in, name, local_file_header.file_name_length);
      name[local_file_header.file_name_length]=0;

      fseek(in,marker,SEEK_SET); /* and part 2 of nasty code */

      if ((match_flags&1)==1)
      {
        if ((match_flags&2)==2)
        { if (strcmp(compressed_filename,name)==0) break; }
          else
        { if (strcasecmp(compressed_filename,name)==0) break; }
      }
        else
      {
        if ((match_flags&2)==2 || strlen(compressed_filename)==strlen(name))
        {
          if (strstr(name,compressed_filename)!=0) break;
        }
          else
#ifdef DLL
        { if (strcasestr_m(name, compressed_filename) !=0 ) { break; } }
#else
        { if (strcasestr(compressed_filename, name) != 0) { break; } }
#endif
      }
    }

    fseek(in, local_file_header.compressed_size +
              local_file_header.file_name_length +
              local_file_header.extra_field_length, SEEK_CUR);

  }

  if (name_size != 0) free(name);

  fclose(in);

  if (i != -1)
  { return curr; }
    else
  { return -1; }
}

int kunzip_get_name(char *zip_filename, char *name, int offset)
{
  FILE *in;
  struct zip_local_file_header_t local_file_header;
  int i;

  in = fopen(zip_filename, "rb");
  if (in == NULL) { return -1; }

  fseek(in, offset, SEEK_SET);

  i = read_zip_header(in, &local_file_header);
  if (i < 0) { return i; }

  read_chars(in, name, local_file_header.file_name_length);

  fclose(in);

  return 0;
}

int kunzip_get_filesize(char *zip_filename, int offset)
{
  FILE *in;
  struct zip_local_file_header_t local_file_header;
  int i;

  in = fopen(zip_filename, "rb");
  if (in == NULL) { return -1; }

  fseek(in, offset, SEEK_SET);

  i = read_zip_header(in,&local_file_header);
  if (i < 0) { return i; }

  fclose(in);

  return local_file_header.uncompressed_size;
}

time_t kunzip_get_modtime(char *zip_filename, int offset)
{
  FILE *in;
  struct zip_local_file_header_t local_file_header;
  time_t date_time;
  struct tm my_tm;
  int i;

  in = fopen(zip_filename, "rb");
  if (in == NULL) { return -1; }

  fseek(in, offset, SEEK_SET);

  i = read_zip_header(in, &local_file_header);
  if (i < 0) { return i; }

  fclose(in);

  my_tm.tm_sec = (local_file_header.last_mod_file_time & 31) * 2;
  my_tm.tm_min = (local_file_header.last_mod_file_time >> 5) & 63;
  my_tm.tm_hour = (local_file_header.last_mod_file_time >> 11);
  my_tm.tm_mday = (local_file_header.last_mod_file_date & 31);
  my_tm.tm_mon = ((local_file_header.last_mod_file_date >> 5) & 15) - 1;
  my_tm.tm_year = (local_file_header.last_mod_file_date >> 9) + 80;
  my_tm.tm_wday = 0;
  my_tm.tm_yday = 0;
  my_tm.tm_isdst = 0;

  date_time = mktime(&my_tm);

  return date_time;
}


