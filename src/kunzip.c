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

#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"
#include "kinflate.h"
#include "kunzip.h"
#include "zipfile.h"

int main(int argc, char *argv[])
{
  int i;

  printf("\nkunzip ZIP decompression routines\n");
  printf(COPYRIGHT"\n");
  printf(VERSION"\n\n");

  if (argc != 2)
  {
    printf("Usage: kunzip <infile>\n");
    exit(0);
  }

  kunzip_inflate_init();

  i = kunzip_count_files(argv[1]);
  printf("There are %d files in this archive.\n", i);

  if (i > 0)
  {
    i = kunzip_all(argv[1], ".");
    //printf("Offset = %d\n",kunzip_get_offset_by_name(argv[1],"support/adobe",0,-1));
  }

  kunzip_inflate_free();

  if (i != 0)
  {
    printf("Problem decompressing\n");
  }

  return 0;
}



