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

int cCon[] = {255, 104, 0};
int cRelax[] = {102, 255, 255};

bool warmed = false;
bool cooled = false;
int n=100;

long num = 0;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  //#if defined (__AVR_ATtiny85__)
  //  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  //#endif
  // End of trinket special code

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  
  num = Bean.readScratchNumber(1);
  //Bean.setScratchNumber(2, num);
  
  if ( num > 5)
  {
    if (!warmed)
    {
       warm();
       warmed = true;
       cooled = false;
    }
    concentrated();
    Bean.setScratchNumber(2, (num + 1));
  }
  if (num <= 5) 
  {
    if (!cooled)
    {
      cool();
      warmed = false;
      cooled = true;
    }    
    relaxed();
    Bean.setScratchNumber(2, 1);
  }
  
  Bean.sleep(50);

}

void warm() {
  
  for(int i = 0; i < n; i++) // larger values of 'n' will give a smoother/slower transition.
  {
     int Rn = cRelax[0] + (cCon[0] - cRelax[0]) * i / n;
     int Gn = cRelax[1] + (cCon[1] - cRelax[1]) * i / n;
     int Bn = cRelax[2] + (cCon[2] - cRelax[2]) * i / n;
     
     //serial.println( Rn 
   
    for(int i = 0; i < strip.numPixels();i++) {
      strip.setPixelColor(i, strip.Color(Rn, Gn, Bn));
      strip.show();
      delay(10);
    }
  }

}

void concentrated() {
  
  for(int i = 0; i < strip.numPixels();i++) {
    strip.setPixelColor(i, strip.Color(cCon[0], cCon[1],cCon[2]));
    strip.show();
    delay(100);
  }
  
}

void cool() {
  
  for(int i = 0; i < n; i++) // larger values of 'n' will give a smoother/slower transition.
  {
     int Rn = cCon[0] + (cRelax[0] - cCon[0]) * i / n;
     int Gn = cCon[1] + (cRelax[1] - cCon[1]) * i / n;
     int Bn = cCon[2] + (cRelax[2] - cCon[2]) * i / n;
     
     //serial.println( Rn 
   
    for(int i = 0; i < strip.numPixels();i++) {
      strip.setPixelColor(i, strip.Color(Rn, Gn, Bn));
      strip.show();
      delay(10);
    }
  }
  
}

void relaxed() {
  
  for(int i = 0; i < strip.numPixels();i++) {
    strip.setPixelColor(i, strip.Color(cRelax[0], cRelax[1], cRelax[2]));
    strip.show();
    delay(100);
  }
  
}
