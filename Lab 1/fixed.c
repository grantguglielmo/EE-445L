/* ** fixed.c **
* Grant Guglielmo, Malek Al Sukhni
* Created: 1/20/18
* contains fixed point conversion functions and diagram plotting functions
* Lab 1
* Jamie Campos
* Last modified: 1/23/18
* outputs to ST7735 LCD screen connected to pins: PA2, PA3, PA5, PA6, PA7, 3.3 V and GND
*/

#include <stdint.h>
#include <stdio.h>
#include "ST7735.h"

void writeVal(int32_t n){
  
}

/****************ST7735_sDecOut2***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.01
 range -99.99 to +99.99
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " **.**"
  2345    " 23.45"  
 -8100    "-81.00"
  -102    " -1.02" 
    31    "  0.31" 
-12345    "-**.**"
 */ 
void ST7735_sDecOut2(int32_t n){
  if(n < 0){                                                // output sign and take absolute value
    n *= -1;
    if((n > 9999) || (n < -9999)){                            // out of range
      printf("-**.**");
      return;
    }

    if(n/1000){                                               // output all 4 digits and decimal point
      printf("-%d", n/1000);
      n %= 1000;
    }
    else{
      printf(" -");                                            // not signifacnt figure
    }
    printf("%d.", n/100);
    n %= 100;
    printf("%d", n/10);
    n %= 10;
    printf("%d", n);
  }
  else{
    if((n > 9999) || (n < -9999)){                            // out of range
      printf(" **.**");
      return;
    }
  
    if(n/1000){                                               // output all 4 digits and decimal point
      printf(" %d", n/1000);
      n %= 1000;
    }
    else{
      printf("  ");                                            // not signifacnt figure
    }
    printf("%d.", n/100);
    n %= 100;
    printf("%d", n/10);
    n %= 10;
    printf("%d", n);
  }
}


/**************ST7735_uBinOut6***************
 unsigned 32-bit binary fixed-point with a resolution of 1/64. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 256000, it signifies an error. 
 The ST7735_uBinOut6 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     1	  "  0.01"
    16    "  0.25"
    25	  "  0.39"
   125	  "  1.95"
   128	  "  2.00"
  1250	  " 19.53"
  7500	  "117.19"
 63999	  "999.99"
 64000	  "***.**"
*/
void ST7735_uBinOut6(uint32_t n){
  if(n >= 64000){
    printf("***.**");                                     // out of range
    return;
  }
  
  int32_t digFlag = 0;
  
  uint32_t intDigits = n >> 6;                              // right shift n for only whole integer values
  if(intDigits/100){                                        // output all 3 integer digits and decimal point
    printf("%u", intDigits/100);
    intDigits %= 100;
    digFlag = 1;
  }
  else{
    printf(" ");                                            // not signifacnt figure
  }
  if(intDigits/10 || digFlag){
    printf("%u", intDigits/10);
    intDigits %= 10;
  }
  else{
    printf(" ");                                            // not signifacnt figure
  }
  printf("%u.", intDigits);
  
  n &= 0x3F;                                                // bit mask for "decimal" values
  uint32_t binDigit = 1000000/64;                           // calculate 1/64 digits
  binDigit *= n;
  printf("%u", binDigit/100000);
  binDigit %= 100000;
  printf("%u", binDigit/10000);
}

int32_t Xmi;
int32_t Xma;
int32_t Ymi;
int32_t Yma;

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
  Output_Clear();                                           // clear display
  ST7735_SetCursor(0, 0);                                   // reset cursor
  printf("%s", title);                                      // display title
  Xmi = minX;                                              // save X, Y boundraries
  Xma = maxX;
  Ymi = minY;
  Yma = maxY;
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
  int32_t i;
  int32_t x, y;
  int32_t rangeX = Xma - Xmi;               // calculate range of x and y from min and max
  int32_t rangeY = Yma - Ymi;
  
  for(i = 0; i < num; i++){
    if(bufX[i] < Xmi || bufX[i] > Xma || bufY[i] < Ymi || bufY[i] > Yma){         // out of x,y range
      continue;
    }
    x = (bufX[i] - Xmi) * 127;              // calculate x and y coord
    x /= rangeX;                            // within the given x/y min/max
    y = (Yma - bufY[i]) * 127;              // and fitting within 128 pixels
    y /= rangeY;
    y += 32;                                // offset y, because y has 160 pixels to x 128
    
    ST7735_DrawPixel(x, y, ST7735_MAGENTA); // draw pixel
  }
}


