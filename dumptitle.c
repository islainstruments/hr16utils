/* HR16 ROM tools
 * dumptitle.c - dump out the HR16B sample names and offsets
 *
 * Copyright (C) 2008 Gordon JC Pearce
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "romtools.h"
#include "flip.h"


FILE *in;

unsigned char *buffer;
int i;

int main() {
  
  unsigned char msg[16];
  signed int off;

  in = fopen("hr16b.bin", "rb");
  if(!in) {
    printf("\ncan't open file (requires an HR16B ROM image called \"hr16b.bin\")\n");
    exit(1);
  }

  buffer = malloc(sizeof(char)*FIRMWARE_SIZE);
  
  fread(buffer,1,FIRMWARE_SIZE,in);

  // we need to flip the bits in the lower byte of the address
  flip(buffer);

  for (i=0; i<49; i++) {
  
    off = 256 * buffer[0x6bfd+(i*2)];
    off += buffer[0x6bfd+(1+i*2)];
    off *= 16;
    strncpy(msg, &buffer[0x6c5f+(16*i)],16);
    printf("%05x = '%s'\n",(unsigned int)off,msg); 
  }

  free(buffer);
  fclose(in);
}
