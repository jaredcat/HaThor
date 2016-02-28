/*************************************************************
  SparkFun SX1509 I/O Expander Example: pwm output (analogWrite)
  Jim Lindblom @ SparkFun Electronics
  Original Creation Date: September 21, 2015
  https://github.com/sparkfun/SparkFun_SX1509_Arduino_Library

  Connect an LED to the SX1509's pin 15 (or any other pin, they
  can all PWM!). The SX1509 can either sink or source current,
  just don't forget your limiting resistor!

  Hardware Hookup:
	SX1509 Breakout ------ Arduino -------- Breadboard
	      GND -------------- GND
	      3V3 -------------- 3.3V
		  SDA ------------ SDA (A4)
		  SCL ------------ SCL (A5)
		  15 -------------------------------- LED+
		                                 LED- -/\/\/\- GND
                                                330
*************************************************************/

#include <Wire.h> // Include the I2C library (required)
#include <SparkFunSX1509.h> // Include SX1509 library

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address
SX1509 io; // Create an SX1509 object to be used throughout

// Class to hold all the functions related to indavidual light controlls
class Leg {
  public:
    uint8_t red, green, blue = 0;
    byte RPIN, GPIN, BPIN;
    byte brightness = 255;
    Leg(byte, byte, byte, byte);
    void fadeIn(uint8_t, uint8_t, uint8_t, int);
    void fadeOut(int);
    void fadeRed(uint8_t, int);
    void fadeGreen(uint8_t, int);
    void fadeBlue(uint8_t, int);
    void off();
    void set(uint8_t, uint8_t, uint8_t);
};

class Tower {
  private:
    Leg *legs[4];
    uint8_t countLegs(byte);
  public:
    Tower(byte);
    Leg *leg(byte);
    void rainbowCycle(int);
    void rainbow(int);
    void randomColors(int);
    void fadeOut(int);
    void fadeIn(byte, uint8_t, uint8_t, uint8_t, int);
};

void setup()
{
  // Call io.begin(<address>) to initialize the SX1509. If it
  // successfully communicates, it'll return 1.
  if (!io.begin(SX1509_ADDRESS))
  {
    while (1) ; // If we fail to communicate, loop forever.
  }
  // Setup output pins on the SX1509
  for(int i = 0; i < 16; i++) {
    io.pinMode(i, ANALOG_OUTPUT);
  }
  Serial.begin(9600);
}

// Create new Tower object
//  input sets the brightness
Tower tower(70);

void loop()
{
//  for(byte i = 0; i < 1; i++){
//    tower.rainbow(10);
//  }
//  tower.fadeOut(4);
  for(byte i = 0; i < 1; i++){
    tower.rainbowCycle(20);
  }
//  tower.fadeOut(4);
  
  
  // Fades in all 4 legs to white
//  tower.fadeIn(byte(B0101), uint8_t(255), uint8_t(255), uint8_t(255), 50);
//  delay(500);
//  tower.leg(0)->off();
//  tower.leg(1)->off();
//  tower.leg(2)->off();
//  tower.leg(3)->off();
//  delay(500);
//  // Accesses leg0 and calls built-in fade function
//  tower.leg(0)->fadeIn(255, 0, 150, 10);
//  delay(100);
//  tower.leg(0)->fadeOut(10);
//  delay(200);
//  tower.leg(0)->set(100,250,50);
//  delay(100);
//  tower.leg(0)->off();
//  delay(50);
//  tower.leg(0)->set(255,255,255);
//  delay(150);
}

/*************************************
 * LEG OBJECT FUNCTION DEFINITIONS
 ************************************/
// Object constructor
Leg::Leg(byte pin1, byte pin2, byte pin3, byte newBrightness) {
  RPIN = pin1;
  GPIN = pin2;
  BPIN = pin3;
  brightness = newBrightness;
}

// Takes values for red, green, blue, and a timer
// Finds the largest of the three and smoothly transitions between current and new colors
//   with the delay of timer (in ms) between each step
void Leg::fadeIn(uint8_t newRed = 0, uint8_t newGreen = 0, uint8_t newBlue = 0, int timer = 0) {
  uint8_t largestCur = findLargest(red, green, blue);
  uint8_t largest = findLargest(newRed, newGreen, newBlue);
  for (int i = largestCur; i <= largest; i++) {
    // Scale the values to the largest so they all smoothly fade in
    io.analogWrite(RPIN, invert(i*(float(newRed)/float(largest))));
    io.analogWrite(GPIN, invert(i*(float(newGreen)/float(largest))));
    io.analogWrite(BPIN, invert(i*(float(newBlue)/float(largest))));
    delay(timer);
    break;
  }
  // Update internal states to keep track for later
  red = newRed;
  green = newGreen;
  blue = newBlue;
}

// Fades out all colors smoothly
void Leg::fadeOut(int timer = 0) {
  uint8_t largest = findLargest(red, green, blue);
  for (int i = largest; i >= 0; i--) {
    // Scale the values to the largest so they all smoothly fade in
    io.analogWrite(RPIN, invert(i*(float(red)/float(largest))));
    io.analogWrite(GPIN, invert(i*(float(green)/float(largest))));
    io.analogWrite(BPIN, invert(i*(float(blue)/float(largest))));
    delay(timer);
    // Update internal states
    red -= 1;
    green -= 1;
    blue -= 1;
    if (red < 0) { red = 0; }
    if (green < 0) { green = 0; }
    if (blue < 0) { blue = 0; }
    break;
  }
}


// Fades a red between two values intelligently, timer probably shouldn't be used
void Leg::fadeRed(uint8_t newRed = 0, int timer = 0) {
  if (red != newRed){
    if (red < newRed) {
      red += 1;
      io.analogWrite(RPIN, invert(red));
    } else {
      red -= 1;
      io.analogWrite(RPIN, invert(red));
    }
    delay(timer);
  }
}

// Fades a green between two values intelligently, timer probably shouldn't be used
void Leg::fadeGreen(uint8_t newGreen = 0, int timer = 0) {
  if (green != newGreen){
    if (green < newGreen) {
      green += 1;
      io.analogWrite(GPIN, invert(green));
    } else {
      green -= 1;
      io.analogWrite(GPIN, invert(green));
    }
    delay(timer);
  }
}

// Fades a blue between two values intelligently, timer probably shouldn't be used
void Leg::fadeBlue(uint8_t newBlue = 0, int timer = 0) {
  if (blue != newBlue){
    if (blue < newBlue) {
      blue += 1;
      io.analogWrite(BPIN, invert(blue));
    } else {
      blue -= 1;
      io.analogWrite(BPIN, invert(blue));
    }
    delay(timer);
  }
}

void Leg::off(){
  io.analogWrite(RPIN, 255);
  io.analogWrite(GPIN, 255);
  io.analogWrite(BPIN, 255);
  // Update internal states
  red = 0;
  green = 0;
  blue = 0;
}

void Leg::set(uint8_t newRed = 0, uint8_t newGreen = 0, uint8_t newBlue = 0){
  io.analogWrite(RPIN, invert(newRed*brightness/255));
  io.analogWrite(GPIN, invert(newGreen*brightness/255));
  io.analogWrite(BPIN, invert(newBlue*brightness/255));
  // Update internal states
//  red = newRed;
//  green = newGreen;
//  blue = newBlue;
}


/*************************************
 * TOWER OBJECT FUNCTION DEFINITIONS
 ************************************/
// Object consturctor
Tower::Tower(byte brightness){
  // Loads 4 leg objects into the tower
//  for(int i = 0; i < 4; i++){
//    // Create a new leg object
//    // R, G, B are attached to pins 3 consecutive pins, respectivly
//    legs[i] = new Leg(i, i+1, i+2);
//  }
  legs[0] = new Leg(0, 1, 2, brightness);
  legs[1] = new Leg(4, 5, 6, brightness);
  legs[2] = new Leg(8, 9, 10, brightness);
  legs[3] = new Leg(12, 13, 14, brightness);
}

uint8_t Tower::countLegs(byte leg_select) {
  // (1) 1 = leg(0)
  // (10) 2 = leg(1)
  // (100) 4 = leg(2)
  // (1000) 8 = leg(3)
  uint8_t remaining = uint8_t(leg_select);
  uint8_t count = 0;
  
  if (remaining > 7) {
    count += 1;
    remaining -= 8;
  }
  if(remaining > 3){
    count += 1;
    remaining -= 4;
  }
  if (remaining > 1) {
    count += 1;
    remaining -= 2;
  }
  if (remaining > 0){
    count += 1;
    // remaining -= 1;
  }
  return count;
}

// Returns a leg of the tower
Leg *Tower::leg(byte leg){
  return legs[leg];
}

void Tower::fadeIn(byte legs, uint8_t newRed = 0, uint8_t newGreen = 0, uint8_t newBlue = 0, int timer = 0) {
  uint8_t largest = findLargest(newRed, newGreen, newBlue);
  uint8_t leg_count = countLegs(legs);
  
  for(int i = 0; i < largest; i++) {
    uint8_t remainingLegs = uint8_t(legs);
    if (remainingLegs > 7) {
      remainingLegs -= 8;
      tower.leg(3)->set(i*(float(newRed)/float(largest)), i*(float(newGreen)/float(largest)), i*(float(newBlue)/float(largest)));
    }
    if (remainingLegs > 3) {
      remainingLegs -= 4;
      tower.leg(2)->set(i*(float(newRed)/float(largest)), i*(float(newGreen)/float(largest)), i*(float(newBlue)/float(largest)));
    }
    if (remainingLegs > 1) {
      remainingLegs -= 2;
      tower.leg(1)->set(i*(float(newRed)/float(largest)), i*(float(newGreen)/float(largest)), i*(float(newBlue)/float(largest)));
    }
    if (remainingLegs > 0) {
      // remainingLegs -= 1;
      tower.leg(0)->set(i*(float(newRed)/float(largest)), i*(float(newGreen)/float(largest)), i*(float(newBlue)/float(largest)));
    }
    delay(timer/float(leg_count)); 
  }
}

void Tower::rainbowCycle(int timer = 0) {
  uint8_t leg0[3];
  uint8_t leg1[3];
  uint8_t leg2[3];
  uint8_t leg3[3];
  for (uint16_t i = 0; i < 256; i++) {
    Wheel(i+64/3 & 255, leg0);
    Wheel(i+128/3 & 255, leg1);
    Wheel(i+192/3 & 255, leg2);
    Wheel(i+256/3 & 255, leg3);
    
    tower.leg(0)->red = leg0[0];
    tower.leg(0)->green = leg0[1];
    tower.leg(0)->blue = leg0[2];
    
    tower.leg(1)->red = leg1[0];
    tower.leg(1)->green = leg1[1];
    tower.leg(1)->blue = leg1[2];
    
    tower.leg(2)->red = leg2[0];
    tower.leg(2)->green = leg2[1];
    tower.leg(2)->blue = leg2[2];
    
    tower.leg(3)->red = leg3[0];
    tower.leg(3)->green = leg3[1];
    tower.leg(3)->blue = leg3[2];
    
    tower.leg(0)->set(leg0[0], leg0[1], leg0[2]);
    tower.leg(1)->set(leg1[0], leg1[1], leg1[2]);
    tower.leg(2)->set(leg2[0], leg2[1], leg2[2]);
    tower.leg(3)->set(leg3[0], leg3[1], leg3[2]);
    delay(timer); 
  }
}

void Tower::rainbow(int timer = 0) {
  uint8_t leg0[3];
  uint8_t leg1[3];
  uint8_t leg2[3];
  uint8_t leg3[3];
  for (uint16_t i = 0; i < 256; i++) {
    Wheel(i & 255, leg0);
    Wheel(i & 255, leg1);
    Wheel(i & 255, leg2);
    Wheel(i & 255, leg3);

    tower.leg(0)->red = leg0[0];
    tower.leg(0)->green = leg0[1];
    tower.leg(0)->blue = leg0[2];
    
    tower.leg(1)->red = leg1[0];
    tower.leg(1)->green = leg1[1];
    tower.leg(1)->blue = leg1[2];
    
    tower.leg(2)->red = leg2[0];
    tower.leg(2)->green = leg2[1];
    tower.leg(2)->blue = leg2[2];
    
    tower.leg(3)->red = leg3[0];
    tower.leg(3)->green = leg3[1];
    tower.leg(3)->blue = leg3[2];
    
    tower.leg(0)->set(leg0[0], leg0[1], leg0[2]);
    tower.leg(1)->set(leg1[0], leg1[1], leg1[2]);
    tower.leg(2)->set(leg2[0], leg2[1], leg2[2]);
    tower.leg(3)->set(leg3[0], leg3[1], leg3[2]);
    delay(timer); 
  }
}

void Tower::randomColors(int timer = 0) {
  tower.leg(0)->set(random(0,256), random(0,256), random(0,256));
  tower.leg(1)->set(random(0,256), random(0,256), random(0,256));
  tower.leg(2)->set(random(0,256), random(0,256), random(0,256));
  tower.leg(3)->set(random(0,256), random(0,256), random(0,256));
  delay(timer);
}

void Tower::fadeOut(int timer = 0) {
  for (int i = 0; i < 256; i++){
    if(tower.leg(0)->red != 0){
      tower.leg(0)->red -= 1;
    }
    if(tower.leg(0)->green != 0){
      tower.leg(0)->green -= 1;
    }
    if(tower.leg(0)->blue != 0){
      tower.leg(0)->blue -= 1;
    }
    
    tower.leg(0)->set(tower.leg(0)->red, tower.leg(0)->green, tower.leg(0)->blue);

    if(tower.leg(1)->red != 0){
      tower.leg(1)->red -= 1;
    }
    if(tower.leg(1)->green != 0){
      tower.leg(1)->green -= 1;
    }
    if(tower.leg(1)->blue != 0){
      tower.leg(1)->blue -= 1;
    }
    tower.leg(1)->set(tower.leg(1)->red, tower.leg(1)->green, tower.leg(1)->blue);

    if(tower.leg(2)->red != 0){
      tower.leg(2)->red -= 1;
    }
    if(tower.leg(2)->green != 0){
      tower.leg(2)->green -= 1;
    }
    if(tower.leg(2)->blue != 0){
      tower.leg(2)->blue -= 1;
    }
    tower.leg(2)->set(tower.leg(2)->red, tower.leg(2)->green, tower.leg(2)->blue);

    if(tower.leg(3)->red != 0){
      tower.leg(3)->red -= 1;
    }
    if(tower.leg(3)->green != 0){
      tower.leg(3)->green -= 1;
    }
    if(tower.leg(3)->blue != 0){
      tower.leg(3)->blue -= 1;
    }
    tower.leg(3)->set(tower.leg(3)->red, tower.leg(3)->green, tower.leg(3)->blue);
    delay(timer);
  }
}


/*************************************
 * HELPER  FUNCTION DEFINITIONS
 ************************************/
// Helper to invert uint8_t numbers because the SX1509 has inverted logic
uint8_t invert(uint8_t num){
  return 255-num;
}

// Helper function to find largest value to be used to calculate step sizes
uint8_t findLargest(uint8_t num1, uint8_t num2, uint8_t num3) {
  uint8_t largest = num1;
  if (num2 > largest) {
    largest = num2;
  }
  if (num3 > largest) {
    largest = num3;
  }
  return largest;
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(byte WheelPos, uint8_t colors[]) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    colors[0] = 255 - WheelPos * 3;
    colors[1] = 0;
    colors[2] = WheelPos * 3;
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    colors[0] = 0;
    colors[1] = WheelPos * 3;
    colors[2] = 255 - WheelPos * 3;
  } else {
    WheelPos -= 170;
    colors[0] = WheelPos * 3;
    colors[1] = 255 - WheelPos * 3;
    colors[2] = 0;
  }
}
