#include "FastSPI_LED2.h"

const byte ledCount = 15;
const byte PIN = 3;



CRGB leds[ledCount];
int BOTTOM_INDEX = 0;
int TOP_INDEX = int(ledCount/2);
int EVENODD = ledCount%2;

/*
constexpr int factorial(int n)
{
    return n <= 1 ? 1 : (n * factorial(n-1));
}
*/

class CPattern 
{  
  protected: 
    CRGB *m_pLeds; 
    int m_ledCount; 
  public: 
    CPattern(CRGB *pLeds, int numLeds) : m_pLeds(pLeds), m_ledCount(numLeds) 
    {
      //Serial.println("Create CPattern");
    }
    virtual ~CPattern() 
    {
      //Serial.println("Delete CPattern");
    }
    virtual void draw() = 0; 
}; 

class CSolidDotPattern : public CPattern
{
  int mFrame;
  CRGB mColor;
  public:
    CSolidDotPattern(CRGB *pStartLed, int nLeds, CRGB color) : CPattern(pStartLed, nLeds), mFrame(0), mColor(color) {}   

  virtual void draw()
  {
    if (m_ledCount == mFrame)
    {
      mFrame = 0;
    }

    m_pLeds[mFrame] = mColor;
    mFrame++;
  }
};

class CColorBouncePattern : public CPattern
{
  private:
    byte idex;
    byte hue;
    byte val;

  public:
    CColorBouncePattern(CRGB *pStartLed, int nLeds) : CPattern(pStartLed, nLeds),idex(0), hue(0), val(255) {}

    virtual void draw()
    {

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
    }
};

class CMarchPattern : public CPattern 
{ 
  public: 
    CMarchPattern(CRGB *pStartLed, int nLeds) : CPattern(pStartLed, nLeds) {} 

    virtual void draw() 
    {
      CRGB temp = leds[m_ledCount-1];
      for(int i = m_ledCount-1; i > 0; i-- ) 
      {
        m_pLeds[i] = m_pLeds[i-1];
      }
      m_pLeds[0] = temp;
    }
}; 

class CSolidPattern : public CPattern
{
  private: CRGB mColor;
  public:   
    CSolidPattern(CRGB *pStartLed, int nLeds, CRGB color) : CPattern(pStartLed, nLeds), mColor(color) {} 

    virtual void draw() 
    {
      fill_solid(m_pLeds, m_ledCount, mColor);
    }

};

class CRainbowMarchPattern : public CPattern
{
  private:
   byte ihue;
  public: 
    CRainbowMarchPattern(CRGB *pStartLed, int nLeds) : CPattern(pStartLed, nLeds), ihue(0) {} 

    virtual void draw() 
    {
      ihue -= 1;
      fill_rainbow( m_pLeds, m_ledCount, ihue );
    }
};

class CStarPattern : public CPattern
{
  private:
      int starPos;
  public: 
    CStarPattern(CRGB *pStartLed, int nLeds) : CPattern(pStartLed, nLeds), starPos(random(0,nLeds-1)) 
    {
      //Serial.println("Create CStarPattern");
    } 

    ~CStarPattern() 
    {
      //Serial.println("Delete CStarPattern");
    }

    virtual void draw() 
    {
      m_pLeds[starPos] = CHSV(0, 0, random(0,255));
    }
};


class CThreeColorMarchPattern : public CMarchPattern
{
  CRGB mFirst;
  CRGB mSecond;
  CRGB mThird;

  public: 
    CThreeColorMarchPattern(CRGB *pStartLed, int nLeds, CRGB first, CRGB second, CRGB third) : CMarchPattern(pStartLed, nLeds), mFirst(first), mSecond(second), mThird(third) 
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

    virtual void draw() 
    {
      CMarchPattern::draw();
    }
};


//CPattern *pPatterns[10] = {NULL};
CPattern *pPatterns[10] = {NULL};


void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
 	delay(1000);
  //Serial.begin(9600);
    LEDS.setBrightness(8);


  // global brightness doesn't work on the attiny85
 	//LEDS.setBrightness(8);
 	LEDS.addLeds<WS2811, PIN, GRB>(leds, ledCount);

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
int adjacent_cw(int i) 
{
  int r;
  if (i < ledCount - 1) 
  {
    r = i + 1;
  }
  else 
  {
    r = 0;
  }
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
  CRGB temp = leds[ledCount-1];
  for(int i = ledCount-1; i > 0; i-- ) 
  {
    leds[i] = leds[i-1];
  }
  leds[0] = temp;
}

void marchCCW()
{

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

void runPattern(int frameCount, int duration)
{
//  unsigned long tick = millis();
  unsigned long millsPerFrame = duration/frameCount;
  

  for (int i = 0;i<frameCount;i++)
  {
    int iPattern=0; 
    while(pPatterns[iPattern] != NULL) 
    {  
      pPatterns[iPattern++]->draw(); 
    } 
    LEDS.show(); 
    delay(millsPerFrame);
  }  
}

void loop()
{
  const byte brightness = 32;
  const CRGB red = CRGB(brightness,0,0);
  const CRGB green = CRGB(0,brightness,0);
  const CRGB blue = CRGB(0,0,brightness);
  const CRGB white = CRGB(brightness,brightness,brightness);

  pPatterns[0] = new CRainbowMarchPattern(leds, ledCount);
  runPattern(1000, 5000);
  delete pPatterns[0];

  pPatterns[0] = new CSolidDotPattern(leds, ledCount, red);
  runPattern(15, 1500);
  delete pPatterns[0];

  pPatterns[0] = new CSolidDotPattern(leds, ledCount, white);
  runPattern(15, 1500);
  delete pPatterns[0];

  pPatterns[0] = new CSolidDotPattern(leds, ledCount, green);
  runPattern(15, 1500);
  delete pPatterns[0];

  //pPatterns[0] = new CColorBouncePattern(leds, ledCount, CRGB::Red, CHSV(200, 255, 128));
  //runPattern(100, 5000);
  //delete pPatterns[0];

  pPatterns[0] = new CThreeColorMarchPattern(leds, ledCount, white, red, green);
  runPattern(30, 3000);
  delete pPatterns[0];

  for (int i = 0;i<5;i++)
  {
    pPatterns[0] = new CSolidPattern(leds, ledCount, CHSV(200, 255, 32));
    pPatterns[1] = new CStarPattern(leds, ledCount);
    pPatterns[2] = new CStarPattern(leds, ledCount);
    pPatterns[3] = new CStarPattern(leds, ledCount);
    runPattern(20, 400);
    delete pPatterns[0];
    delete pPatterns[1];
    delete pPatterns[2];
    delete pPatterns[3];
  }

  //for (int i = 0;i<75;i++)
  //  twinkle();

}

