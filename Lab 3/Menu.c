#include "Menu.h"
#include "ADCTestMain.h"
#include "ST7735.h"

extern volatile uint32_t TimeSecs;
extern volatile uint16_t SW3;
extern volatile uint16_t SW2;
extern volatile uint16_t SW1;
extern volatile uint16_t SW0;

uint16_t pos = 3;

void Refresh_Menu(void){
  uint32_t time = TimeSecs;
  uint16_t hrs = time/3600;
  uint16_t mins = (time%3600)/60;
  uint16_t meridiem = 0;
  if(hrs >= 12){
    meridiem = 15;
    hrs -= 12;
  }
  if(!hrs){
    hrs = 12;
  }
  ST7735_SetCursor(0, 0);
  ST7735_OutChar(hrs/10 + '0');
  ST7735_OutChar(hrs%10 + '0');
  ST7735_OutChar(':');
  ST7735_OutChar(mins/10 + '0');
  ST7735_OutChar(mins%10 + '0');
  ST7735_OutChar('a' + meridiem);
  ST7735_OutChar('m');
  if(SW3 | SW2 | SW1 | SW0){
    if(SW3){          // enter
      switch(pos){
        case 3:
          break;
        case 2:
          break;
        case 1:
          break;
        case 0:
          break;
      }
    }
    else if(SW2){     // up
      pos = (pos+1)%4;
    }
    else if(SW1){     // down
      if(pos){
        pos = pos-1;
      }
    }
    else{             // reserved
    }
    SW3 = SW2 = SW1 = SW0 = 0;
  }
}

void Draw_Menu(void){
  Refresh_Menu();
  ST7735_SetCursor(3, 10);
  ST7735_OutString("Digital Clock");
  ST7735_SetCursor(3, 11);
  ST7735_OutString("Analog Clock");
  ST7735_SetCursor(3, 12);
  ST7735_OutString("Set Alarm");
  ST7735_SetCursor(3, 13);
  ST7735_OutString("Set Clock");
}

