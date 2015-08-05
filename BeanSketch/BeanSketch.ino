#include <Adafruit_NeoPixel.h>
#include <avr/power.h> // Comment out this line for non-AVR boards (Arduino Due, etc.)

#define PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

int cRelax[] = {0, 255, 0};
int cCon[] = {0, 0, 255};

int Gn = 0;
int Bn = 0;

bool warmed = false;
bool cooled = false;
int n=100;

long num = 10;

/* Serial Events */

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
float current = 0;

/* - */

void setup() 
{
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  //#if defined (__AVR_ATtiny85__)
  //  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  //#endif
  // End of trinket special code
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() 
{ 
  /* Serial Works */
   serialEvent();

   if (stringComplete) {
    current = inputString.toFloat();
    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  /* - */
   
//  //read data from bean
//  num = Bean.readScratchNumber(1);
//  Bean.setScratchNumber(2, num);

  num = current;
  
  //map data to LED color
  Gn = map(num, 0, 10, 50, 5);
  Bn = map(num, 0, 10, 5, 50);
  
  //sparkle when below Concentration Threshold™
  if ( num < 5) 
  {
    //set sparkle variables
    int numChange = random(15,25);
    int numChase = 3;
  
    //begin sparkle chase pattern
    for (int q=0; q < numChase; q++) 
    {
      for (int i=0; i < strip.numPixels(); i=i+numChase) 
      {
       strip.setPixelColor(i+q, 0, Gn, Bn);    //turn every third pixel on
      }
      strip.show();
  
      delay(50);
  
      for (int i=0; i < strip.numPixels(); i=i+numChase) 
      {
        strip.setPixelColor(i+q, 0, Gn + numChange, Bn);        //turn every third pixel off
      }
      //strip.show();
    }
    delay(50);
  }
  
  //begin pulse when above Concentration Threshold™
  else 
  {
    //set fade intensity
    int numFade = 35;
    
    //begin fade out
    for(int i=0; i<numFade; i++)
    {
       for (int q=0; q < strip.numPixels(); q++)
       {
         strip.setPixelColor(q, 0, Gn, Bn - i);
       }
  
      strip.show();
      delay(100);
    }
    
    delay(500);
    
    //being fade in
    for (int i=0; i<numFade; i++)
    {
      for (int q=0;q < strip.numPixels(); q++)
      {
         strip.setPixelColor(q, 0, Gn, (Bn - numFade) + i);
      }
  
      strip.show();
      delay(100);
    }
    
    delay(300);
  }
  
  delay(10);

}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
