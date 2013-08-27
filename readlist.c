/* HR16 ROM tools
 * readlist.c - save a flipped copy of the ROM
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
#include <argp.h>
#include <string.h>
#include <sndfile.h>
#include "romtools.h"
#include "flip.h"



int main() {
  FILE *file;
  // processing each line
  char line[128];
  char title[16];
  char centre[2];
  char filename[80];
  char t_str[10];
  int truncate;
  int frames;
  SNDFILE *wave_in;
  SF_INFO *wave_info;
  
  // running state
  int sample = 0;
  int pointer = 0;
  
  int i;
  
  // buffers
  unsigned char *buffer;
  float *wave_buffer;
  unsigned char *firmware;
  
  firmware = malloc(FIRMWARE_SIZE*sizeof(char));

  buffer=malloc(1024*1024*sizeof(unsigned char));
  memset(buffer,0x80,1024*1024*sizeof(unsigned char));
  
  wave_buffer = malloc(1024*1024*sizeof(float));
  wave_info = (SF_INFO *) malloc(sizeof(SF_INFO));  

  file = fopen("hr16b.bin", "rb");
  if(!file) {
    printf("can't open firmware image\n");
    exit(1);
  }
  
  fread(firmware,1,FIRMWARE_SIZE,file);
  fclose(file);
  flip(firmware);
  
  file = fopen("samplelist.txt", "rb");
  if(!file) {
    printf("can't open sample list\n");
    exit(1);
  }

  
  


  
  while (fgets(line, 128, file)) {

    centre[0]=0x00;

    sscanf(line, "%[^:]:%[^:]:%[^:]:%[^:]", title, centre, filename, t_str);
    
    //sscanf(line, "%[^:]:%[^:]:%[^:]", title, centre, filename);


    if (!(wave_in = sf_open(filename,SFM_READ,wave_info))) {
      printf("Failed to read sample %s\n", filename);
    }

    frames = wave_info->frames;
    truncate = atoi(t_str);
    if (truncate>0) frames=truncate;

    sf_readf_float(wave_in,wave_buffer,frames);

    sf_close(wave_in);

    for (i=0; i< frames; i++) {
      buffer[i+pointer] = 127*(wave_buffer[i]);
    }
    set_title(firmware, sample+1, title, 1);

    printf("pointer = %d (%05x)\nframes = %d (%05x)\n",pointer,pointer,frames, frames);
    frames += 2; // make sure there are at least two stop bytes
    frames = (frames & 0xffff0)+0x10;
    printf("frames = %d (%05x)\n\n",frames, frames);
    
    set_offset(firmware, sample+1, pointer);
    
    pointer += frames;
    sample++;

    //printf("%s:\n%d frames\n%d channels\n", title, wave_info->channels, wave_info->frames);

  }



  fclose(file);
  
  file = fopen("sample1.bin","wb");
  fwrite(buffer,1,524288,file);
  fclose(file);

  file = fopen("sample2.bin","wb");
  fwrite(buffer+524288,1,524288,file);
  fclose(file);


  
  flip(firmware);
  file = fopen("out.bin","wb");
  fwrite(firmware, sizeof(char), FIRMWARE_SIZE, file);
  fclose(file);
  
  printf("%d samples with total size %d, %d%% used\n",sample, pointer, (int)(pointer/(1024*10.24)));
    printf("%x\n",firmware);
  free(firmware);
  free(wave_buffer);
  free(buffer);
  free(wave_info);

}

