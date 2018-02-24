
#ifndef _MUSIC_H_
#define _MUSIC_H_

#define SONG 1
#define NUM_NOTES 12

typedef enum Note{C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B} Note;

void Music_Init(void);

void Play(int song);

void Rewind(void);

void Stop(void);

#endif  // _MUSIC_H_
