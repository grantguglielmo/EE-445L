/* ** DigitalClock.c **
* Grant Guglielmo, Malek Al Sukhni
* Created: 1/20/18
* Contains the code for printing the digital clock to the lcd screen. Decides what and where to print numbers
* corresponding to hours and minutes.
* Lab 3
* Jamie Campos
* Last modified: 2/13/18
* outputs to ST7735 LCD screen connected to pins: PA2, PA3, PA5, PA6, PA7, 3.3 V and GND
*/

#include "DigitalClock.h"
#include "ST7735.h"

#define YPOS 40

extern volatile uint32_t TimeSecs;

void Refresh_Digclk(void){
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
  uint16_t x = hrs/10;
  if(x){
    ST7735_DrawChar(0, 0 + YPOS, x + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  }
  else{
    ST7735_DrawChar(0, 0 + YPOS, ' ', ST7735_YELLOW, ST7735_BLACK, 4);
  }
  ST7735_DrawChar(25, 0 + YPOS, hrs%10 + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  ST7735_FillRect(50, 7 + YPOS, 5, 5, ST7735_YELLOW);
  ST7735_FillRect(50, 17 + YPOS, 5, 5, ST7735_YELLOW);
  ST7735_DrawChar(60, 0 + YPOS, mins/10 + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  ST7735_DrawChar(85, 0 + YPOS, mins%10 + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  ST7735_DrawChar(113, 20 + YPOS, 'a' + meridiem, ST7735_YELLOW, ST7735_BLACK, 1);
  ST7735_DrawChar(120, 20 + YPOS, 'm', ST7735_YELLOW, ST7735_BLACK, 1);
}

