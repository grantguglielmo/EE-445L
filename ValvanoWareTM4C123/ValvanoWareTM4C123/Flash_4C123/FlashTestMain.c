// FlashTestMain.c
// Runs on LM4F120/TM4C123
// Test the functions provided by FlashProgram.c by writing
// constant values to the flash memory and then finding
// them in the memory viewer in the debugger.
// Daniel Valvano
// October 21, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

#include <stdint.h>
#include "FlashProgram.h"
#include "PLL.h"
#include "Scoreboard.h"
#include "UART.h"

#define GPIO_PORTF321           (*((volatile uint32_t *)0x40025038))
#define GPIO_PORTF_DIR_R        (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile uint32_t *)0x40025420))
#define GPIO_PORTF_DEN_R        (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile uint32_t *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile uint32_t *)0x4002552C))
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_RCGCGPIO_R5      0x00000020  // GPIO Port F Run Mode Clock
                                            // Gating Control
#define SYSCTL_PRGPIO_R         (*((volatile uint32_t *)0x400FEA08))
#define SYSCTL_PRGPIO_R5        0x00000020  // GPIO Port F Peripheral Ready
// Flash ROM addresses must be 1k byte aligned, e.g., 0x8000, 0x8400, 0x8800...
#define FLASH                   0x00008000  // location in flash to write; make sure no program code is in this block
#define SCOREBOARD              0x00008800  // location in flash for scoreboard

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// Print the top scores from the scoreboard to the UART.
// Assumes that the UART is initialized.
void printscores(void){
  SBEType* Scoreboard;                              // pointer to array of ScoreboardElements holding top scores
  int i;
  UART_OutString("High Scores:\r\n");
  Scoreboard = Scoreboard_Get();
  for(i=0; i<SCOREBOARDSIZE; i=i+1){
    UART_OutChar(Scoreboard[i].first);              // print first initial
    UART_OutChar(Scoreboard[i].middle);             // print middle initial
    UART_OutChar(Scoreboard[i].last);               // print last initial
    UART_OutChar(' ');
    UART_OutChar(' ');
    UART_OutUDec(Scoreboard[i].score);              // print the score
    UART_OutChar('\r');
    UART_OutChar('\n');
  }
}

// Get new initials and score and record them in the
// scoreboard.  Input characters from the UART until three
// letters (a through z) are entered, which are the
// player's initials.  Convert them to upper case if
// needed.  Ignore non-letter characters, including
// backspace.  Then, input a 32-bit number, which is the
// score that the player would have earned in a game.
// Assumes that the UART is initialized.
void getnewscore(void){
  char first, middle, last;
  uint32_t score;
  UART_OutString("Enter three letters: ");
  // get the first initial
  first = UART_InChar();
  if(first >= ' '){
    UART_OutChar(first);                            // echo the character
  }
  while((first < 'A') || (('Z' < first ) && (first < 'a')) || ('z' < first)){
    if(first >= ' '){
      UART_OutChar(BS);                             // backspace
    }
    first = UART_InChar();                          // repeat until a letter is pressed
    if(first >= ' '){
      UART_OutChar(first);                          // echo the character
    }
  }
  if(('a' <= first) && (first <= 'z')){
    first = first - ' ';                            // convert to upper case
  }
  // get the middle initial
  middle = UART_InChar();
  if(middle >= ' '){
    UART_OutChar(middle);                           // echo the character
  }
  while((middle < 'A') || (('Z' < middle ) && (middle < 'a')) || ('z' < middle)){
    if(middle >= ' '){
      UART_OutChar(BS);                             // backspace
    }
    middle = UART_InChar();                         // repeat until a letter is pressed
    if(middle >= ' '){
      UART_OutChar(middle);                         // echo the character
    }
  }
  if(('a' <= middle) && (middle <= 'z')){
    middle = middle - ' ';                          // convert to upper case
  }
  // get the last initial
  last = UART_InChar();
  if(last >= ' '){
    UART_OutChar(last);                             // echo the character
  }
  while((last < 'A') || (('Z' < last ) && (last < 'a')) || ('z' < last)){
    if(last >= ' '){
      UART_OutChar(BS);                             // backspace
    }
    last = UART_InChar();                           // repeat until a letter is pressed
    if(last >= ' '){
      UART_OutChar(last);                           // echo the character
    }
  }
  if(('a' <= last) && (last <= 'z')){
    last = last - ' ';                              // convert to upper case
  }
  // get the score
  UART_OutString("\r\nEnter your score as a number and hit ENTER: ");
  score = UART_InUDec();
  UART_OutChar('\r');
  UART_OutChar('\n');
  Scoreboard_Record(first, middle, last, score);
}

// Perform a long test by writing stuff to each word of
// memory.  Once each word has been written, go back and
// check that each memory location has the expected
// contents.  After that, clear the flash.
//
// According to the discussion found in the forum thread:
// http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/367605.aspx
// The TM4C123 microcontroller may sometimes skip
// instructions following a flash memory write or erase.
// However, I have not been able to repeat the error using
// the programs and test functions in this file.  This test
// has functioned correctly ('errors' == 0 and no crashes)
// for nearly 24 hours of continuous writing and erasing
// operations and over 16,000 writes and erases to each
// memory location ('runs' > 16000).  Both a TM4C123 and an
// LM4F120 microcontroller were tested with the same
// results.  Nevertheless, it may still be better to move
// the contents of Flash_Write(), Flash_FastWrite(), and
// Flash_Erase() to SRAM, where code execution can continue
// during flash writes and erases.
// Assumes that the UART is initialized.
#define LTTSTART  0x00008000
#define LTTEND    0x0003FFFC
void longtermtest(void){
  uint32_t volatile *flashPtr;
  uint32_t static addr = LTTSTART;
  uint32_t static data = 0xFFFFFFFF;
  uint32_t static errors = 0;
  uint32_t static runs = 0;
  int i;
  Flash_Write(addr, data);
  addr = addr + 4;
  data = data - 1;
  if(addr > LTTEND){
    runs = runs + 1;
    UART_OutString("Run #");
    UART_OutUDec(runs);
    UART_OutString(" complete.  ");
    flashPtr = (uint32_t volatile*)LTTSTART;
    i = 0;
    while(flashPtr <= (uint32_t volatile*)LTTEND){
      if(*flashPtr != (data + 0xE000 - i)){
        errors = errors + 1;
      }
      flashPtr = flashPtr + 1;
      i = i + 1;
    }
    UART_OutUDec(errors);
    UART_OutString(" errors.\r\n");
    addr = LTTSTART;
    UART_OutString("Erasing 0x00008000 through 0x0003FFFF.\r\n");
    for(i=LTTSTART; i<LTTEND; i=i+1024){
      Flash_Erase(i);
    }
  }
}

volatile float T;    // temperature in C
volatile uint32_t N; // 12-bit ADC value
void Test1(void){
  for(N=0; N<4096; N++){
    T = 10.0+ 0.009768*N; 	
  }
}

void Test2(void){
  for(N=0; N<4096; N++){
    T = 1000 + ((125*N+64)>>7); 	
  }
}


uint32_t DataArray[10];
uint32_t volatile *FlashPtr = (uint32_t volatile*)FLASH;
int SuccessfulWrites;

int main(void){
  PLL_Init(Bus80MHz);
  Test1();
}

// Note on writing to location 0x00008400:
// The way flash memory works is that an erase sets all bits
// in a block to 1, and writing clears desired bits to 0.
// You cannot "write" individual bits back to 1's.  When
// FlashTestMain is compiled and run, there may already be
// data stored at 0x00008400.  After the line
// Flash_Write(FLASH + 0x400, 0x45464153);
// the contents of flash location 0x00008400 will be the
// bitwise AND of the previous data contents and 0x45464153.
//
// To deal with this issue, you should erase the block once. E.g., 
// Flash_Erase(0x00008400);   // erase 0x00008400 through 0x000087FF
  
// Another way to erase Flash is to use the Keil debugger
// 1) Open the "Flash" menu and select "Configure Flash Tools..."
// 2) Select the "Utilities" sub-tab
// 3) Click the "Setting" button near the pull-down menu that says
//  "Stellaris ICDI"
// 4) Click the radio button that says "Mass Erase (erase entire Flash)"
// 5) Click both "OK" buttons
// 6) Clear the flash and download your program with the regular
//  "Download" button.  In my experience, the debugger failed
//  after the full erase finished.  If this happens, repeat
//  Steps 1, 2, and 3.  Then in Step 4, click the radio button
//  that says "Page Erase (erase only necessary pages)".  You
//  can also try the ultra-scientific process of repeatedly
//  disconnecting and re-connecting the USB cable or restarting.
//
// This process guarantees that the flash memory at 0x0000.C000 is clear
//  (set to all 1's) in case a previously loaded program left data there.
