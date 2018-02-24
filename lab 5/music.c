
#include "music.h"
#include <stdint.h>
#include "Song.h"

#define freq 80000000

int Play_Song;
unsigned int I;
unsigned int Note_I;

Note Song_Notes[NUM_NOTES] = {B, B, B, F, G, G, F, D, C, C, B, F};
int Not_len[] = {1};
int Note_Reload[] = {freq/(262*SONG_SIZE), freq/(277*SONG_SIZE), freq/(294*SONG_SIZE), freq/(311*SONG_SIZE), freq/(330*SONG_SIZE),
    freq/(349*SONG_SIZE), freq/(370*SONG_SIZE), freq/(392*SONG_SIZE), freq/(415*SONG_SIZE), freq/(440*SONG_SIZE),
    freq/(466*SONG_SIZE), freq/(494*SONG_SIZE)};

void Music_Init(void){
  Play_Song = 1;
  I = 0;
  Note_I = 0;
}

void Play(int song){
  Play_Song = 1;
}

void Rewind(void){
  I = 0;
  Note_I = 0;
}

void Stop(void){
  Play_Song = 0;
}

