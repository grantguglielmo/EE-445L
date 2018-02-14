/* ** Menu.c **
* Grant Guglielmo, Malek Al Sukhni
* Created: 1/20/18
* Contains the code for printing the main menu to the screen, as well as the code for navigating 
* menu modes. Contains code for updating based on user input
* Lab 3
* Jamie Campos
* Last modified: 2/13/18
* outputs to ST7735 LCD screen connected to pins: PA2, PA3, PA5, PA6, PA7, 3.3 V and GND
*/

#include "Menu.h"
#include "ADCTestMain.h"
#include "ST7735.h"
#include "AnalogClock.h"
#include "DigitalClock.h"
#include "SetClock.h"
#include "SetAlarm.h"

#define YPOS 40

extern volatile uint32_t TimeSecs;
extern volatile uint16_t SW3;
extern volatile uint16_t SW2;
extern volatile uint16_t SW1;
extern volatile uint16_t AlarmOn;
extern volatile uint16_t Debounce;
extern uint16_t Menu;

const unsigned short arrow[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2E9F, 0x2E9F,
 0x369F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x0000, 0x0000, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x0000, 0x0000, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0000, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

const unsigned short arrowclean[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};


uint16_t pos = 4;
uint16_t clocktype = 0;

void Refresh_Menu(void){
  if(clocktype){
    Refresh_Anlgclk();
  }
  else{
    Refresh_Digclk();
  }
  if(SW3 | SW2 | SW1){
    if(SW3){          // enter
      switch(pos){
        case 4:       // display digital clock
          clocktype = 0;
          ST7735_FillRect(0, 0, 128, 100, ST7735_BLACK);
          Refresh_Digclk();
          break;
        case 3:       // display analog clock
          clocktype = 1;
          ST7735_FillRect(0, 0, 128, 100, ST7735_BLACK);
          Draw_Analog();
          Refresh_Anlgclk();
          break;
        case 2:       // set alarm
          Menu = 4;
          SW3 = SW2 = SW1 = 0;
          Debounce = 25;
          Draw_Alarmset();
          return;
        case 1:       // set clock
          Menu = 5;
          SW3 = SW2 = SW1 = 0;
          Debounce = 25;
          Draw_Clockset();
          return;
        case 0:
          ST7735_SetCursor(3, 14);
          if(AlarmOn){
            ST7735_OutString("Alarm On ");
            AlarmOn = 0;
          }
          else{
            ST7735_OutString("Alarm Off");
            AlarmOn = 1;
          }
          break;
      }
    }
    else if(SW2){     // up
      ST7735_DrawBitmap(5, 148 - pos*10, arrowclean, 10, 11);
      if(pos!= 4){
        pos++;
      }
      ST7735_DrawBitmap(5, 148 - pos*10, arrow, 10, 11);
    }
    else if(SW1){     // down
      ST7735_DrawBitmap(5, 148 - pos*10, arrowclean, 10, 11);
      if(pos){
        pos = pos-1;
      }
      ST7735_DrawBitmap(5, 148 - pos*10, arrow, 10, 11);
    }
    SW3 = SW2 = SW1 = 0;
  }
}

void Draw_Menu(void){
  if(clocktype){
    Draw_Analog();
  }
  Refresh_Menu();
  ST7735_SetCursor(3, 10);
  ST7735_OutString("Digital Clock");
  ST7735_SetCursor(3, 11);
  ST7735_OutString("Analog Clock");
  ST7735_SetCursor(3, 12);
  ST7735_OutString("Set Alarm");
  ST7735_SetCursor(3, 13);
  ST7735_OutString("Set Clock");
  ST7735_SetCursor(3, 14);
  if(AlarmOn){
    ST7735_OutString("Alarm Off");
  }
  else{
    ST7735_OutString("Alarm On ");
  }
  ST7735_DrawBitmap(5, 148 - pos*10, arrow, 10, 11);
}

