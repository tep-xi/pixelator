#include "FastLED.h"
FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    7
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    289
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  srand(analogRead(0));
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, solidXi, prideXi, floweradvancement, rainbowWithGlitter, confetti, sinelon, juggle, bpm, snakechase };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void snakechase(){
  //The goal is to have a random color be choose and for snakes to chase each other between 5 and 10 times. 
  //Snakes chasing each other means that two trails of light come in from either end.
  for(int ledamount = 0; ledamount<289; ledamount++){
    leds[ledamount]=CRGB::Black;
  }
  CRGB snake1color;
  snake1color = CRGB(rand()%256,rand()%256,rand()%256);
  CRGB snake2color=CRGB(rand()%256,rand()%256,rand()%256);
  for(int timestep = 0; timestep < 145; timestep++){
    leds[timestep]=snake1color;
    leds[288-timestep]=snake2color;
    FastLED.show();
    delay(5);
  }
  
}

void floweradvancement()
{
  CRGB nextleds[NUM_LEDS];
  int fullCount = 0;
  int emptyCount = 0;
  for(int x = 0; x < 17; x++)
  {
    for(int y = 0; y <17; y++)
    {
        if(leds[pxmap(x, y)] != CRGB(0,0,0))
        {
            nextleds[pxmap(x,y)] = leds[pxmap(x,y)];
            fullCount++;
            continue;
        }
        emptyCount++;
          
        bool above = y > 0 && leds[pxmap(x,y - 1)] != CRGB(0,0,0);
        bool below = y < 16 && leds[pxmap(x,y + 1)] != CRGB(0,0,0);
        bool left = x > 0 && leds[pxmap(x - 1,y)] != CRGB(0,0,0);
        bool right = x < 16 && leds[pxmap(x + 1,y)] != CRGB(0,0,0);

        if((above && below && left && right) && rand()%100 > 80)
        {
            CRGBPalette16 palette = PartyColors_p;
            uint8_t beat = beatsin8( 62, 64, 255);
            nextleds[pxmap(x, y)] = ColorFromPalette(palette, gHue+rand()%500, beat-gHue+rand()%2000);
        }
        else if(((above && below && right) | (above && below && left) | (above&&left&&right) | (below && left && right)) && rand()%100 > 70){
          CRGBPalette16 palette = PartyColors_p;
            uint8_t beat = beatsin8( 62, 64, 255);
            nextleds[pxmap(x, y)] = ColorFromPalette(palette, gHue+rand()%500, beat-gHue+rand()%2000);
        }
        else if(((above&&right)|(above&&left)|(above&&below)|(below&&right)|(below&&left)|(left&&right))&&rand()%100 > 60){
          CRGBPalette16 palette = PartyColors_p;
            uint8_t beat = beatsin8( 62, 64, 255);
            nextleds[pxmap(x, y)] = ColorFromPalette(palette, gHue+rand()%500, beat-gHue+rand()%2000);
        }
        else if((above|below|right|left)&&rand()%100>50){
          CRGBPalette16 palette = PartyColors_p;
            uint8_t beat = beatsin8( 62, 64, 255);
            nextleds[pxmap(x, y)] = ColorFromPalette(palette, gHue+rand()%500, beat-gHue+rand()%2000);
        }
        else
        {
          nextleds[pxmap(x, y)] = CRGB(0,0,0);
        }
    }   
  }  
 
  if(fullCount == 289 || emptyCount == 289)
  {
    for(int i = 0; i < 289; i++)
    {
      nextleds[i] = CRGB(0,0,0);
    }
    nextleds[pxmap(rand()%17,rand()%17)] = CRGB::Blue;
  }
  
  for(int i = 0; i < 289; i++)
  {
    leds[i] = nextleds[i];
  }
}


//has a Xi in one solid color and changes like a rainbow pattern
void solidXi(){
  int bytemap[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 // fill_rainbow(leds,289,hue,7);
  for(int i=0;i<289;i++){
    if(bytemap[i]==0){
      leds[pxmap(i%17,i/17)]=CRGB::Black;
    }
    else{
      leds[pxmap(i%17,i/17)]=CRGB::Purple;
    }
  }
}

//has a rainbow pattern that is constantly changing on a Xi
//convert bytemap matrix into array of addresses 
void prideXi(){
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  int bytemap[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 // fill_rainbow(leds,289,hue,7);
  for(int i=0;i<289;i++){
    if(bytemap[i]==0){
      leds[pxmap(i%17,i/17)]=CRGB::Black;
    }
  }
}

boolean isinarray(int arrays[],int val){
  for(int i=0;i<sizeof(arrays);i++){
    if(arrays[i]==val){
      return true;
    }
  }
  return false;
}


int pxmap(int x,int y){
  //if y even then x normal
  //if y odd then 17-x is desired
  if(y%2==0){
     int p = x%17;
     return 17*y+p;
  }
  else{
    return 17*(y+1)-x-1;
  }
}







