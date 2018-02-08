/* ** ADCTestMain.c **
* Grant Guglielmo, Malek Al Sukhni
* Created: 1/20/18
* Initializes timers 0 and 1 and keeps track of ADC values when an interrupt occurs (every 12.5 ns). 
* it then takes these values and constructs a PMF and plots it to the ST7735. Built off of files from
* projects ADCSWTrigger_4C123 and PeriodicTimer1AInts_4C123. 
* Lab 2
* Jamie Campos
* Last modified: 1/31/18
* outputs to ST7735 LCD screen connected to pins: PA2, PA3, PA5, PA6, PA7, 3.3 V and GND
*/

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

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

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include "ADCSWTrigger.h"
#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define ADC             (*((volatile uint32_t *)0x40038030))
#define ADCCTR          (*((volatile uint32_t *)0x40038038))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int Time[1000];               // Time array for storing the time at which the timer interrupts
int Adc[1000];                // ADC array for storing adc values at timer interrupts
int I = 0;                    // indexx used for accessing the above arrays

volatile uint32_t ADCvalue;
volatile uint32_t TIMEvalue0;
volatile uint32_t TIMEvalue1;

// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;                       // activate timer0
  delay = SYSCTL_RCGCTIMER_R;                       // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN;                  // disable timer0A during setup
  TIMER0_CFG_R = 0;                                 // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                                    // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;                          // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;                 // enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;                // clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;                   // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                                    // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000;// top 3 bits
  NVIC_EN0_R = 1<<19;                               // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;                // acknowledge timer0A timeout
  PF2 ^= 0x04;                                      // profile
  PF2 ^= 0x04;                                      // profile
  ADCvalue = ADC0_InSeq3();
  if(I < 1000){                                     // this loop will fill the ADC and Time arrays until Timer0 has interrupted 1000 times
    //ADCvalue = ADC0_InSeq3();
    TIMEvalue0 = TIMER1_TAR_R;
    TIMEvalue1 = TIMER1_TAR_R;
    Time[I] = TIMEvalue0 - TIMEvalue1;              // setting Time array value
    Adc[I] = ADCvalue;                              // setting adc array value
    I++;                                            // incrementing I
  }
  PF2 ^= 0x04;                                      // profile
}

void Timer1_Init(){
  SYSCTL_RCGCTIMER_R |= 0x02;                        // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;                         // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;                         // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;                        // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 0xFFFFFFFF;                       // 4) reload value
  TIMER1_TAPR_R = 0;                                 // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;                         // 6) clear TIMER1A timeout flag
                                                     // 7) skip
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4

  // interrupts enabled in the main program after all devices initialized
  // vector number 37, interrupt number 21
  
  NVIC_EN0_R = 1<<21;                                // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;                         // 10) enable TIMER1A
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT; // acknowledge TIMER1A timeout
}

/****************PMFplot***************
 converts the ADC array into a PMF which is outputting to the LCD screen
 Inputs:  uses the global ADC array
 Outputs: none
 prints a PMF to the LCD display
 */ 
void PMFplot(void){
  int vals[1000];
  int cnt[1000];
  for(int j = 0; j < 1000; j++){                        // initializing cnt array to all zeroes
    cnt[j] = 0;
  }
  int x = 1;
  for(int j = 0; j < 1000; j++){                        // counting the amount of times that and ADC value appears
    x = Adc[j];
    for(int k = 0; k < 1000; k++){
      if(x == vals[k]){
        cnt[k]++;
        break;
      }
      if(cnt[k] == 0){
        vals[k] = x;
        cnt[k]++;
        break;
      }
    }
  }
  int j;
  for(j = 0; cnt[j] != 0 && j < 1000; j++){              // bubble sort algorithm that sorts the vals and cnt array so that the values can be 
    int x = vals[j];                                     // plotted in the correct order along the x axis
    int x_cnt = cnt[j];
    for(int k = j + 1; cnt[k] != 0 && k < 1000; k++){
      if(vals[k] < x){
        vals[j] = vals[k];
        cnt[j] = cnt[k];
        vals[k] = x;
        cnt[k] = x_cnt;
        x = vals[j];
        x_cnt = cnt[j];
      }
    }
  }
  
  Output_Clear();
  ST7735_FillScreen(ST7735_BLACK);                        // clear the screen
  ST7735_SetCursor(0,0);                                  // start cursor at 0, 0
  for(int k = 0; k < j; k++){
    int x = vals[k];
    int y = cnt[k];
    ST7735_Line(k, 0, k, cnt[k], ST7735_MAGENTA);         // for each entry in array val, plot val vs. cnt creating pmf
  }
}

int Jitter[999];

int main(void){
  PLL_Init(Bus80MHz);                                             // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;                                      // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();                                   // allow time to finish activating
  Timer0A_Init100HzInt();                                         // set up Timer0A for 100 Hz interrupts
  ST7735_InitR(INITR_REDTAB);
  GPIO_PORTF_DIR_R |= 0x06;                                       // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;                                    // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;                                       // enable digital I/O on PF2, PF1
                                                                  // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;                                         // disable analog functionality on PF
  PF2 = 0;                                                        // turn off LED
  Timer1_Init();
  //ADCCTR = 0x40;
  //ADC = 0x6;
  EnableInterrupts();
  int min = 0;                                                    // initializing the min variable where we will store the current min jitter
  int max = 0;                                                    // initializing the max variable where we will store the current max jitter
  int jitter;                                                     // variable for storing the final jitter value
  ST7735_Line(50, 160 - 50, 40, 160 - 100, ST7735_MAGENTA);
  
  ST7735_Line(50, 160 - 50, 20, 160 - 80, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 0, 160 - 60, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 0, 160 - 50, ST7735_MAGENTA);
  
  ST7735_Line(50, 160 - 50, 50, 160 - 0, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 50, 160 - 120, ST7735_MAGENTA);
  
  ST7735_Line(50, 160 - 50, 60, 160 - 100, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 80, 160 - 80, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 100, 160 - 60, ST7735_MAGENTA);
  
  ST7735_Line(50, 160 - 50, 100, 160 - 50, ST7735_MAGENTA);
  
  ST7735_Line(50, 160 - 50, 20, 160 - 20, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 0, 160 - 40, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 40, 160 - 0, ST7735_MAGENTA);
  
  ST7735_Line(50, 160 - 50, 60, 160 - 0, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 80, 160 - 20, ST7735_MAGENTA);
  ST7735_Line(50, 160 - 50, 100, 160 - 40, ST7735_MAGENTA);
  while(1){
    PF1 ^= 0x02;                                                  // toggles when running in main
    /*if(I == 1000){                                                // (A)  
      min = Time[1] - Time[0];                                    
      max = min;                                                  // the max will be equal to the first min
      for(int j = 0; j < 999; j++){
        Jitter[j] = Time[j+1] - Time[j];
        if(Jitter[j] < min){                                      // if the current jitter is less than the min, it becomes the new min
          min = Jitter[j];
        }
        if(Jitter[j] > max){                                      // if the current jitter is greater than the max, it becomes the new max
          max = Jitter[j];
        }
      }
      jitter = max - min;                                         // calculating the final jitter
      I++;                                                        // incrementing I so that if statement A is only executed once
      
      PMFplot();                                                  // PMF plot will plot all the ADC values and create a PMF
    }*/
  }
}


