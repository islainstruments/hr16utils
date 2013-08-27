/* HR16 ROM tools
 * flip.c - flip the lower 8 bits of the address in a buffer
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

int flip(char *buffer) {
// reverse the lower 8 bits of the address
  int ah, al;
  int i, j,t,u;
  char *temp;
  // give ourselves space to work
  temp = malloc(sizeof(char)*FIRMWARE_SIZE);
  memcpy(temp, buffer, FIRMWARE_SIZE);

  for(i=0; i<FIRMWARE_SIZE; i++) {
    // this will fail for FIRMWARE_SIZE > 0xffff but since we're only dealing with 32kB that's fine
    ah = i & 0xff00;
    al = i & 0x00ff;
    // I'm sure there are more elegant ways to do this
    u = 0;
    for (j=0; j<8; j++) {
      t = al & 1;
      u = (u << 1) | t;
      al = al >> 1;
    }
    al = u;
    buffer[ah+al]=temp[i];
  }
  free(temp);
}

int set_title(char *buffer, int sample, char *title, int centre) {
      int c = 0, l = 0;
      memset(&buffer[TITLE_ADDR+(16*sample)], ' ', 16);
      l=strlen(title);
      if (l>16) l=16;
      if (centre) c=(16-l)/2;
      strncpy(&buffer[TITLE_ADDR+(16*sample)+c], title, l);
}

int set_offset(char *buffer, int sample, int offset) {
  if (sample < 1 || sample > 49) {
    return 1; // FIXME - define proper error codes
  }
  sample -= 1; // run from 0 to 48
  sample *= 2; // two byte value
  // should probably check that offset is a multiple of 16, rather than just truncating
  offset = offset / 16;
  buffer[OFFSET_ADDR+sample] = offset/256;
  buffer[OFFSET_ADDR+sample+1] = offset & 255;
  return 0;
}


