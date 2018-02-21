
#include "music.h"
#include <stdint.h>

int Play_Song = 0;
unsigned int I = 0;

void Music_Init(void){
  Play_Song = 0;
  I = 0;
}

void Play(int song){
  Play_Song = 1;
}

void Rewind(void){
  I = 0;
}

void Stop(void){
  Play_Song = 0;
}

