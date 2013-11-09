#include "FastSPI_LED2.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// test code
//
//////////////////////////////////////////////////

#define NUM_LEDS 15

CRGB leds[NUM_LEDS];
int ledsX[NUM_LEDS][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)
int BOTTOM_INDEX = 0;
int TOP_INDEX = int(NUM_LEDS/2);
int EVENODD = NUM_LEDS%2;


void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(2000);

   	LEDS.setBrightness(255);
   	LEDS.addLeds<WS2811, 3, GRB>(leds, NUM_LEDS);
}

void rainbow_loop(){                       //-m88-RAINBOW FADE FROM FAST_SPI2
  static byte ihue = 0;
  ihue -= 1;
  fill_rainbow( leds, NUM_LEDS, ihue );
  LEDS.show();
  delay(5);
}

//---FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
  if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
  if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
  return NUM_LEDS - i;  
}

//---FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % NUM_LEDS; }
  return iN;
}

//---FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < NUM_LEDS - 1) {r = i + 1;}
  else {r = 0;}
  return r;
}

//---FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {r = i - 1;}
  else {r = NUM_LEDS - 1;}
  return r;
}


void copy_led_array(){
  for(int i = 0; i < NUM_LEDS; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }  
}


void rwb_march() {                    //-m15-R,W,B MARCH CCW
  copy_led_array();
  int iCCW;
  static int idex = 0;
  idex++;
  if (idex > 2) {idex = 0;}
  switch (idex) {
    case 0:
      leds[0] = CRGB::Red;
    break;
    case 1:
      leds[0] = CRGB::White;
    break;
    case 2:
      leds[0] = CRGB::Green;
    break;
  }
    for(int i = 1; i < NUM_LEDS; i++ ) {
      iCCW = adjacent_ccw(i);
      leds[i].r = ledsX[iCCW][0];
      leds[i].g = ledsX[iCCW][1];
      leds[i].b = ledsX[iCCW][2];    
    }
  LEDS.show();  
  delay(125);
}

void twinkle()
{
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  //print("seed drop ");println();
  for(int i = 0;i<NUM_LEDS;i++)
  {
    if (random(0,30) == 0) 
    {
      leds[i] = CHSV(0, 0, random_bright);
    }
    else
    {
      leds[i] = CRGB::Black;
    }
  }
  LEDS.show();
  delay(200);
}

void color_bounce() {                        //-m5-BOUNCE COLOR (SINGLE LED)
  static byte idex = 0;
  static byte hue = 0;
  static byte val = 255;
  static boolean bounceForward = true;
  if (bounceForward) {
    idex = idex + 1;
    if (idex == NUM_LEDS) {
      bounceForward = false;
      idex = idex - 1;
    }
  } 
  else
  {
    idex = idex - 1;
    if (idex == 0) {
      bounceForward = true;
    }
  }  
  for(int i = 0; i < NUM_LEDS; i++ ) 
  {
    if (i == idex) 
    {
      leds[adjacent_cw(i)] = CHSV(hue, 255, val/2);
      leds[i] = CHSV(hue, 255, val);
      leds[adjacent_ccw(i)] = CHSV(hue, 255, val/2);
    }
    else 
    {
      leds[i] = CHSV(0, 0, 0);
    }
    hue = getFilteredHue(hue);
    //val+=2;
  }
  LEDS.show();
  delay(75);
}

byte getFilteredHue(byte hue)
{
  hue++;
 
 if (hue > 16 && hue < 64)
    hue = 64; 
  // filter out teal and blue
  if (hue > 100 && hue < 238)
    hue = 238;
    
  return hue;
}

void loop()
{
  for (int i = 0;i<250;i++)
    rwb_march();
  for (int i = 0;i<1000;i++)
    flicker();
  for (int i = 0;i<250;i++)
    color_bounce();
  for (int i = 0;i<250;i++)
    twinkle();
  for (int i = 0;i<1000;i++)
    rainbow_loop();
}

