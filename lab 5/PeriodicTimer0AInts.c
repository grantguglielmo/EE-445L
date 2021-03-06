// PeriodicTimer0AInts.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// oscilloscope or LED connected to PF3-1 for period measurement
// When using the color wheel, the blue LED on PF2 is on for four
// consecutive interrupts then off for four consecutive interrupts.
// Blue is off for: dark, red, yellow, green
// Blue is on for: light blue, blue, purple, white
// Therefore, the frequency of the pulse measured on PF2 is 1/8 of
// the frequency of the Timer0A interrupts.

#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Timer0A.h"
#include "Switch.h"
#include "DAC.h"
#include "music.h"
#include "Song.h"
#include "Timer1A.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define WHEELSIZE 8           // must be an integer multiple of 2
                              //    red, yellow,    green, light blue, blue, purple,   white,          dark
const long COLORWHEEL[WHEELSIZE] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern int Play_Song;
extern unsigned int I;
extern unsigned int Note_I;
#define Song_arr Wave
extern const unsigned short Song_arr[];
extern int Note_Reload[];
extern Note Song_Notes[];

void ChangeNote(void){
  if(Play_Song){
    Note_I = (Note_I + 1)%NUM_NOTES;
    TIMER0_TAILR_R = Note_Reload[Song_Notes[Note_I]];
  }
}

void SingSong(void){
  if(Play_Song){
    I = (I+1)%SONG_SIZE;
    DAC_Out(Song_arr[I]);
  }
}
// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f
#define F2HZ (80000000/2)
#define F20KHZ (80000000/20000)
//debug code
int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
  LED_Init();                      // enable buttons and leds on port F
  LEDS = 0;                        // turn all LEDs off
  DAC_Init();
  Music_Init();
//  Timer0A_Init(&SingSong, F20KHZ);     // initialize timer0A (20,000 Hz)
  Timer0A_Init(&SingSong, F20KHZ);  // initialize timer0A (20 kHz)
  Timer1A_Init(&ChangeNote, F2HZ);
  TIMER0_TAILR_R = Note_Reload[Song_Notes[0]];
  EnableInterrupts();
  int playing = 0;

  while(1){
    switch(Board_Input()){
      case 1:     // right button
        if(playing)
          Stop();
        else
          Play(SONG);
        playing ^= 1;
        break;
      case 2:     // left button
        Rewind();
        break;
    }
  }
}

