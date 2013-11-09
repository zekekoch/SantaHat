#include "FastSPI_LED2.h"

#define ledCount 15
#define PIN 3

CRGB leds[ledCount];
int BOTTOM_INDEX = 0;
int TOP_INDEX = int(ledCount/2);
int EVENODD = ledCount%2;


void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(1000);

    // global brightness doesn't work on the attiny85
   	//LEDS.setBrightness(8);
   	LEDS.addLeds<WS2811, PIN, GRB>(leds, ledCount);
}

void rainbow_loop(){                       //-m88-RAINBOW FADE FROM FAST_SPI2
  static byte ihue = 0;
  ihue -= 1;
  fill_rainbow( leds, ledCount, ihue );
  LEDS.show();
  delay(5);
}

//---FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
  if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
  if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
  return ledCount - i;  
}

//---FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % ledCount; }
  return iN;
}

//---FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < ledCount - 1) {r = i + 1;}
  else {r = 0;}
  return r;
}

//---FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {r = i - 1;}
  else {r = ledCount - 1;}
  return r;
}

void marchCW()
{
  for(int i = ledCount; i > 0; i-- ) 
  {
    leds[i] = leds[i-1];
  }
  leds[0] = leds[ledCount-1];
}

void marchCCW()
{

}

void threeColorFill(CRGB first, CRGB second, CRGB third)
{
  for (int i = 0;i<ledCount;i++)
  {
    switch(i%3)
    {
      case 0:
        leds[i] = first;
        break;
      case 1:
        leds[i] = second;
        break;
      case 2:
        leds[i] = third;
        break;
    }
  }
}

void rwb_march(boolean firstRun)
{
  if (firstRun) 
  {
    threeColorFill(CRGB::White, CRGB::Red, CRGB::Green);
    firstRun = false;
  }

  marchCW();
  LEDS.show();
  delay(125);
}

void flicker() {            //-m9-FLICKER EFFECT
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  if (random_bool < 10) {
    for(int i = 0 ; i < ledCount; i++ ) {
      leds[i] += random_bright - 180;
    }
    LEDS.show();
    delay(random_delay);
  }
}


void twinkle()
{
  int random_bright = random(0,255);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  //print("seed drop ");println();
  for(int i = 0;i<ledCount;i++)
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
    if (idex == ledCount) {
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

  // reset everything to black 
  memset(leds, 0, sizeof(struct CRGB) * ledCount); 

  // set the leds that we want set 
  leds[adjacent_cw(idex)] = CHSV(hue, 255, val/2);
  leds[idex] = CHSV(hue, 255, val);
  leds[adjacent_ccw(idex)] = CHSV(hue, 255, val/2);
  hue = getFilteredHue(hue);

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
  rwb_march(true);
  for (int i = 0;i<150;i++)
    rwb_march(false);
//  for (int i = 0;i<1000;i++)
//    flicker();
  for (int i = 0;i<250;i++)
    color_bounce();
  for (int i = 0;i<250;i++)
    twinkle();
  for (int i = 0;i<1000;i++)
    rainbow_loop();
}

