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
  private:
    uint8_t red, green, blue = 0;
  public:
    byte RPIN, GPIN, BPIN;
    Leg(byte, byte, byte);
    void fadeIn(uint8_t, uint8_t, uint8_t, int);
    void fadeOut(int);
    void off();
    void set(uint8_t, uint8_t, uint8_t);
};

class Tower {
  private:
    Leg *legs[4];
    uint8_t countLegs(byte);
  public:
    Tower();
    Leg *leg(byte);
    void fadeIn(byte, uint8_t, uint8_t, uint8_t, int);
//    void fadeOut(int, int);
};

void setup()
{
  Serial.begin(9600);
  Serial.print("starting");
  // Call io.begin(<address>) to initialize the SX1509. If it
  // successfully communicates, it'll return 1.
  if (!io.begin(SX1509_ADDRESS))
  {
    Serial.print("fail");
    while (1) ; // If we fail to communicate, loop forever.
  }
  // Setup output pins on the SX1509
  io.pinMode(0, ANALOG_OUTPUT);
  io.pinMode(1, ANALOG_OUTPUT);
  io.pinMode(2, ANALOG_OUTPUT);
  io.pinMode(3, ANALOG_OUTPUT);
  io.pinMode(4, ANALOG_OUTPUT);
  io.pinMode(5, ANALOG_OUTPUT);
  io.pinMode(6, ANALOG_OUTPUT);
  io.pinMode(7, ANALOG_OUTPUT);
  io.pinMode(8, ANALOG_OUTPUT);
  io.pinMode(9, ANALOG_OUTPUT);
  io.pinMode(10, ANALOG_OUTPUT);
  io.pinMode(11, ANALOG_OUTPUT);
  io.pinMode(12, ANALOG_OUTPUT);
}

// Create new Tower object
Tower tower;

void loop()
{
  // Fades in all 4 legs to white
  tower.fadeIn(1111, 255, 255, 255, 50);
  delay(500);
  tower.leg(0)->off();
  tower.leg(1)->off();
  tower.leg(2)->off();
  tower.leg(3)->off();
  delay(500);
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
Leg::Leg(byte pin1, byte pin2, byte pin3) {
  RPIN = pin1;
  GPIN = pin2;
  BPIN = pin3;
}

// Takes values for red, green, blue, and a timer
// Finds the largest of the three and smoothly transitions between current and new colors
//   with the delay of timer (in ms) between each step
void Leg::fadeIn(uint8_t newRed = 0, uint8_t newGreen = 0, uint8_t newBlue = 0, int timer = 0) {
  uint8_t largest = findLargest(newRed, newGreen, newBlue);
  for (int i = 0; i <= largest; i++) {
    // Scale the values to the largest so they all smoothly fade in
    io.analogWrite(RPIN, invert(i*(float(newRed)/float(largest))));
    io.analogWrite(GPIN, invert(i*(float(newGreen)/float(largest))));
    io.analogWrite(BPIN, invert(i*(float(newBlue)/float(largest))));
    delay(timer);
  }
  // Update internal states to keep track for later
  red = newRed;
  green = newGreen;
  blue = newBlue;
}

void Leg::fadeOut(int timer = 0) {
//  Serial.println("Fadeout values: " + String(*red) + "  " + String(*green) + "   " + String(*blue));
  uint8_t largest = findLargest(red, green, blue);
  for (int i = largest; i >= 0; i--) {
    // Scale the values to the largest so they all smoothly fade in
    io.analogWrite(RPIN, invert(i*(float(red)/float(largest))));
    io.analogWrite(GPIN, invert(i*(float(green)/float(largest))));
    io.analogWrite(BPIN, invert(i*(float(blue)/float(largest))));
    delay(timer);
  }
  // Update internal states
  red = 0;
  green = 0;
  blue = 0;
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
  io.analogWrite(RPIN, invert(newRed));
  io.analogWrite(GPIN, invert(newGreen));
  io.analogWrite(BPIN, invert(newBlue));
  // Update internal states
  red = newRed;
  green = newGreen;
  blue = newBlue;
}


/*************************************
 * TOWER OBJECT FUNCTION DEFINITIONS
 ************************************/
// Object consturctor
Tower::Tower(){
  // Loads 4 leg objects into the tower
  for(int i = 0; i < 4; i++){
    // Create a new leg object
    // R, G, B are attached to pins 3 consecutive pins, respectivly
    legs[i] = new Leg(i, i+1, i+2);
  }
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
  uint8_t remainingLegs = uint8_t(legs);
  for(int i = 0; i < largest; i++) {
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


/*************************************
 * HELPER  FUNCTION DEFINITIONS
 ************************************/
// Helper to invert uint8_t numbers because the SX1509 has inverted logic
uint8_t invert(uint8_t num){
  return 255-num;
}

// Helper function to find largest value to be used to calculate step sizes
uint8_t findLargest(uint8_t num1, uint8_t num2, uint8_t num3) {
  uint8_t temp = 0;
  if (num1 >= num2 && num1 >= num3) {
    temp = num1;
  } else if (num2 >= num1 && num2 >= num3) {
    temp = num2;
  } else {
    temp = num3;
  }
  return temp;
}

