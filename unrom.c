/* unrom.c */
/* loads an HR/SR sample ROM and splits it into .wav files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>


#define ROMSIZE 1048576

int main(char *argv, int argc) {
    FILE *file;
    SNDFILE *out;
    SF_INFO info;
    signed char *rom;
    float *sample;
    
    float gain=1;
    int rom_p=0, sample_p=0;
    int got_marker;
    
    info.format=SF_FORMAT_WAV | SF_FORMAT_PCM_16;  
    info.channels=1;  
    info.samplerate=32000;  
    
    

    file = fopen("srsample.bin","rb");
    rom = malloc(sizeof(char)*ROMSIZE);
    fread(rom,1,ROMSIZE,file);
    fclose(file);
    
    sample = malloc(sizeof(float)*ROMSIZE); // way bigger than it needs to be
    memset(sample, 0, sizeof(float)*ROMSIZE); // may not be safe on all systems
    for(rom_p=0; rom_p<ROMSIZE; rom_p++) {
        if (rom[rom_p] == -128) {
            if (got_marker) {
                printf("sample end %05x\n",rom_p);
                rom_p = (rom_p & 0xffff0) + 0x10; // 16-byte boundaries
                gain = 2;
            }
            got_marker = 1;
            gain = gain / 2;
            printf("marker (gain=%f)\n",gain);
            rom_p++;
        } else {
            got_marker = 0;
        }
        sample[sample_p]=(float)rom[rom_p]/128.0f*gain;
        sample_p++;
    }
    
    out=sf_open("srsample.wav", SFM_WRITE, &info);
    sf_writef_float(out, sample, ROMSIZE);
    sf_close(out);
    free(sample);
    free(rom);
    return(0);

}
