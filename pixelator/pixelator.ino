#include "FastLED.h"
#include <time.h>
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
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow,floweradvancement, rainbowWithGlitter, confetti, sinelon, juggle, bpm, snakechase };

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
  srand(time(NULL));
  CRGB snake1color=CRGB(rand()%256;rand()%256;rand()%256);
  CRGB snake2color=CRGB(rand()%256;rand()%256;rand()%256);
  for(int timestep = 0; timestep < 145; timestep++){
    leds[timestep]=snake1color;
    leds[288-timestep]=snake2color;
    FastLED.show();
    delay(5);
  }
  
}

void floweradvancement(){
  //The idea here is that you start with a randomly colored pixel in one of the corners. 
  //At each timestep a fixed amount of the pixels attached to currently active pixels turn into a random color.
  //After a fixed amount of timesteps the pattern ends.
  for(int ledlength = 0; ledlength <289; ledlength++){
    leds[ledlength]=CRGB::Black;
  }
  srand(time(NULL));
  int frontier[];
  frontier[0] = rand()%289;
  leds[frontier[0]]=CRGB(rand()%256;rand()%256;rand()%256);
  FastLED.show();
  delay(100);
  //while loop picks how many new elements it wants to add to frontier
  //sees how many are feasible
  
  while(sizeof(frontier<250)){
    int randomadvance = rand()%5;
    int possiblearray[];
    for(int j=0; j < randomadvance; j++){
      possiblearray.push_back(floweradvancementhandler(std::random_shuffle(frontier.begin(),frontier.end())[0]));
    }
    for(int p=0;p < sizeof(possiblearray);p++){
      if(std::find(frontier.begin(),frontier.end(),possiblearray[p]){
      }
      else{
        leds[possiblearray[p]]=CRGB(rand()%256;rand()%256;rand()%256);
      }
  }
     delay(5);
     FastLED.show();
}

int floweradvancementhandler(int frontier){
  srand(time(NULL));
  //check if on edge or border
  //return random one around it
  //if 0 , 16 ,17, 33 mod 34
  if(frontier == 0){
    rand2=rand()%2;
    if(rand2==0){
      return 1
    }
    if(rand2==1){
      return 33
    }
  }
  else if(frontier==16){
    rand2=rand()%2;
    if(rand2==0){
      return 15
    }
    if(rand2==1){
      return 17
    }
  }
  else if(frontier ==271){
    rand2=rand()%2;
    if(rand2==0){
      return 270
    }
    if(rand2==1){
      return 272
    }
  }
  else if(frontier == 288){
    rand2=rand()%2;
    if(rand2==0){
      return 287
    }
    if(rand2==1){
      return 265
    }
  }
  else if(frontier<17){
    rand3=rand()%3;
    if(rand3==0){
      return frontier-1;
    }
    else if(rand3==1){
      return frontier+17;
    }
    else{
      return frontire+1;
    }
  }
  else if(frontier>=271){
    rand3=rand()%3;
    if(rand3==0){
      return frontier-1;
    }
    else if(rand3==1){
      return frontier-17;
    }
    else{
      return frontire+1;
    }
  }
  else{
    rand4=rand()%4;
    if(rand4==0){
      return frontier-17;
    }
    else if(rand4==1){
      return frontier+1;
    }
    else if(rand4==2){
      return frontier+17;
    }
    else{
      return frontier-1;
    }
  }
  
}



int rectangulartodisplay(int x,int y){
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



