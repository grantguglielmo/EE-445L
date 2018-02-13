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
#include "ADCTestMain.h"
#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Menu.h"
#include "DigitalClock.h"
#include "AnalogClock.h"
#include "SetAlarm.h"
#include "SetClock.h"

#define PE2             (*((volatile uint32_t *)0x40024010))
#define PE1             (*((volatile uint32_t *)0x40024008))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern volatile uint32_t AlarmTime;

volatile uint32_t TimeSecs;
volatile uint16_t SW1;
volatile uint16_t SW2;
volatile uint16_t SW3;
volatile uint32_t Debounce;
volatile uint16_t AlarmOn;
volatile uint16_t TimeOut;

int16_t flash = 0;
int16_t alarmSound = 0;
int16_t Menu = 1;   //1: main menu, 2: digital clk, 3: analog clk, 4: set alarm, 5: set clock

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
  TIMER0_TAILR_R = 79999999;                        // start value for 100 Hz interrupts
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
  TimeSecs = (TimeSecs + 1)%86400;
  if(AlarmOn && (AlarmTime == TimeSecs)){
    alarmSound = 1;
  }
  if(alarmSound){
    PF2 = 0;
  }
  else{
    PF2 ^= 0x04;
  }
  if(TimeOut > 1){
    TimeOut--;
  }
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

void Switch_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCGPIO_R |= 0x00000010; // activate port E
  SW1 = 0;                    // clear semaphores
  SW2 = 0;
  SW3 = 0;
  GPIO_PORTE_AMSEL_R = ~0x3E;// disable analog function on PE5-2
  GPIO_PORTE_PCTL_R = ~0x00FFFFF0; // configure PE5-2 as GPIO 
  GPIO_PORTE_DIR_R = ~0x38;  // make PE5-2 in 
  GPIO_PORTE_AFSEL_R &= ~0x3E;// disable alt funct on PE5-2 
  GPIO_PORTE_DEN_R |= 0x3E;   // enable digital I/O on PE5-2
  GPIO_PORTE_IS_R &= ~0x38;   // PE5-2 is edge-sensitive 
  GPIO_PORTE_IBE_R &= ~0x38;  // PE5-2 is not both edges 
  GPIO_PORTE_IEV_R |= 0x38;   // PE5-2 rising edge event
  GPIO_PORTE_ICR_R = 0x38;    // clear flag5-2
  GPIO_PORTE_IM_R |= 0x38;    // enable interrupt on PE5-2
                              // GPIO PortE=priority 2
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000040; // bits 5-7
  NVIC_EN0_R = 0x00000010; // enable interrupt 4 in NVIC
}

void GPIOPortE_Handler(void){
  if(GPIO_PORTE_RIS_R&0x08){  // poll PE3
    GPIO_PORTE_ICR_R = 0x08;  // acknowledge flag3
    if(alarmSound){
      alarmSound = 0;
      Debounce = 15;
      AlarmTime = (TimeSecs + 60)%86400;
      return;
    }
    if(Debounce){
      return;
    }
    SW1 = 1;                  // signal SW1 occurred
  }
  if(GPIO_PORTE_RIS_R&0x10){  // poll PE4
    GPIO_PORTE_ICR_R = 0x10;  // acknowledge flag4
    if(alarmSound){
      alarmSound = 0;
      Debounce = 15;
      AlarmTime = (TimeSecs + 60)%86400;
      return;
    }
    if(Debounce){
      return;
    }
    SW2 = 1;                  // signal SW2 occurred
  }
  if(GPIO_PORTE_RIS_R&0x20){  // poll PE5
    GPIO_PORTE_ICR_R = 0x20;  // acknowledge flag5
    if(alarmSound){
      alarmSound = 0;
      Debounce = 15;
      return;
    }
    if(Debounce){
      return;
    }
    SW3 = 1;                  // signal SW3 occurred
  }
  Debounce = 30;
}

void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
  NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
  // enable interrupts after all initialization is finished
}

void SysTick_Handler(void){
  TimeSecs = (TimeSecs + 1)%86400;
}

int main(void){
  PLL_Init(Bus80MHz);                                             // 80 MHz
  DisableInterrupts();
  TimeSecs = 0;
  SYSCTL_RCGCGPIO_R |= 0x20;                                      // activate port F
  Timer0A_Init100HzInt();
  ST7735_InitR(INITR_REDTAB);
  GPIO_PORTF_DIR_R |= 0x06;                                       // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;                                    // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;                                       // enable digital I/O on PF2, PF1
                                                                  // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;                                         // disable analog functionality on PF
  PF2 = 0;                                                        // turn off LED
  PF1 = 0;
  Switch_Init();                                                  // PE2-5
  
  
  AlarmOn = 0;
  Debounce = 0;
  TimeOut = 0;
  
  //int16_t rtn = 5;
  EnableInterrupts();
  //PE1 = 0x1;
  Draw_Menu();
  while(1){
    if(Debounce){
      Debounce--;
    }
    if(alarmSound){
      if(!flash){
        PF1 ^= 0x02;
        flash = 10;
      }
      else{
        flash--;
      }
      PE2 ^= 0x04;
    }
    else{
      PF1 = 0;
    }
    if(TimeOut == 1){
      TimeOut--;
      Menu = 1;
      Output_Clear();
      Draw_Menu();
    }
    switch(Menu){
      case 1:
        Refresh_Menu();
        break;
      case 2:
        Refresh_Digclk();
        break;
      case 3:
        Refresh_Anlgclk();
        break;
      case 4:
        Refresh_Alarmset();
        break;
      case 5:
        Refresh_Clockset();
    }
  }
}


