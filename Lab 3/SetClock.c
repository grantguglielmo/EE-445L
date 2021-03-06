/* ** SetClock.c **
* Grant Guglielmo, Malek Al Sukhni
* Created: 1/20/18
* Contains the code for updating the clocks current time
* Lab 3
* Jamie Campos
* Last modified: 2/13/18
* outputs to ST7735 LCD screen connected to pins: PA2, PA3, PA5, PA6, PA7, 3.3 V and GND
*/

#include "SetClock.h"
#include "ST7735.h"
#include "Menu.h"

#define YPOS 40

const unsigned short arrowup[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x24F8, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x259A, 0x0082, 0x25DC, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x25FC, 0x00E4, 0x0000, 0x0105, 0x2E3E, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E3E, 0x0967, 0x0000, 0x0000, 0x0000, 0x09A8, 0x2E7F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E7F,
 0x0A0A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0A4B, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x12CD, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x132F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x1391, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x1BF3, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x369F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x1C75, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x1CB6, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x24F8, 0x0021, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0041, 0x2559, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x259A, 0x0082, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0083, 0x25DC, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x25FC, 0x00E4,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0105,
 0x2E3E, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E3E, 0x0967, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x09A8, 0x2E7F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E7F, 0x0A0A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0A4B, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x12CD, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x132F, 0x2E9F, 0x2E7F, 0x2E9F,
 0x2E9F, 0x2E5F, 0x2E9F, 0x1391, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1BF3, 0x2E9F, 0x2E5E, 0x2E3E, 0x2E9F,
 0x1C75, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1CB6, 0x2E9F, 0x2E9F, 0x24F8, 0x0021, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0041, 0x2559, 0x259A, 0x0082, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0083, 0x0083, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

const unsigned short arrowdown[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0083,
 0x0083, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0082, 0x259A, 0x2559, 0x0041,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0021, 0x24F8, 0x2E9F, 0x2E9F, 0x1CB6, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1C75, 0x2E9F, 0x2E3E, 0x2E5E, 0x2E9F, 0x1BF3, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x1391, 0x2E9F, 0x2E5F, 0x2E9F, 0x2E9F, 0x2E7F, 0x2E9F, 0x132F, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x12CD, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0A4B, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0A0A, 0x2E7F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E7F, 0x09A8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0967, 0x2E3E, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E3E, 0x0105, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00E4, 0x25FC, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x25DC, 0x0083, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0082, 0x259A, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2559, 0x0041, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0021, 0x24F8, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x1CB6, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x1C75, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x369F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x1BF3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1391, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x132F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x12CD, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x0A4B, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0A0A, 0x2E7F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E7F,
 0x09A8, 0x0000, 0x0000, 0x0000, 0x0967, 0x2E3E, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E3E, 0x0105,
 0x0000, 0x00E4, 0x25FC, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x25DC, 0x0082, 0x259A,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x24F8, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F,
 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x2E9F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

extern volatile uint32_t TimeSecs;
extern volatile uint16_t SW3;
extern volatile uint16_t SW2;
extern volatile uint16_t SW1;
extern uint16_t Menu;
extern volatile uint16_t TimeOut;

uint16_t hrs = 0;
uint16_t mins = 0;
uint16_t meridiem = 0;

uint16_t hours = 0;

void Draw_Clockset(void){
  hours = 0;
  uint32_t time = TimeSecs;
  hrs = time/3600;
  mins = (time%3600)/60;
  meridiem = 0;
  if(hrs >= 12){
    meridiem = 15;
  }
  Output_Clear();
  ST7735_DrawBitmap(65, 95, arrowdown, 30, 20);
  ST7735_DrawBitmap(65, 35, arrowup, 30, 20);
  TimeOut = 61;
  Refresh_Clockset();
}

void Refresh_Clockset(void){
  if(SW3 | SW2 | SW1){
    if(SW3){          // enter
      if(hours){
        uint32_t time = 0;
        time += hrs*60*60;
        time += mins*60;
        TimeOut = 0;
        Menu = 1;
        SW3 = SW2 = SW1 = 0;
        Output_Clear();
        Draw_Menu();
        TimeSecs = time;
        return;
      }
      Output_Clear();
      ST7735_DrawBitmap(10, 95, arrowdown, 30, 20);
      ST7735_DrawBitmap(10, 35, arrowup, 30, 20);
      hours = 1;
    }
    else if(SW2){     // up
      if(hours){
        hrs = (hrs + 1)%24;
        if(hrs >= 12){
          meridiem = 15;
        }
        else{
          meridiem = 0;
        }
      }
      else{
        mins = (mins + 1)%60;
      }
    }
    else if(SW1){     // down
      if(hours){
        if(!hrs){
          hrs = 23;
        }
        else{
          hrs--;
        }
        if(hrs >= 12){
          meridiem = 15;
        }
        else{
          meridiem = 0;
        }
      }
      else{
        if(!mins){
          mins = 59;
        }
        else{
          mins--;
        }
      }
    }
    SW3 = SW2 = SW1 = 0;
  }
  
  uint16_t hrs_dis = hrs;
  if(hrs_dis >= 12){
    hrs_dis -= 12;
  }
  if(!hrs_dis){
    hrs_dis = 12;
  }
  uint16_t x = hrs_dis/10;
  if(x){
    ST7735_DrawChar(0, 0 + YPOS, x + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  }
  else{
    ST7735_DrawChar(0, 0 + YPOS, ' ', ST7735_YELLOW, ST7735_BLACK, 4);
  }
  ST7735_DrawChar(25, 0 + YPOS, hrs_dis%10 + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  ST7735_FillRect(50, 7 + YPOS, 5, 5, ST7735_YELLOW);
  ST7735_FillRect(50, 17 + YPOS, 5, 5, ST7735_YELLOW);
  ST7735_DrawChar(60, 0 + YPOS, mins/10 + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  ST7735_DrawChar(85, 0 + YPOS, mins%10 + '0', ST7735_YELLOW, ST7735_BLACK, 4);
  ST7735_DrawChar(113, 20 + YPOS, 'a' + meridiem, ST7735_YELLOW, ST7735_BLACK, 1);
  ST7735_DrawChar(120, 20 + YPOS, 'm', ST7735_YELLOW, ST7735_BLACK, 1);
}

