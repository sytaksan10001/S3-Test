#include <Arduino.h>
#include <PCF8574.h>

const int STEP = 15;
const int DIR = 16;
const int steps_per_rev = 800;

#define STEP P0
#define DIR P1

// Set the i2c HEX address
PCF8574 pcf8574(0x20, 4, 5);

void setup(){
  Serial.begin(115200);

  // Set the pinModes
  pcf8574.pinMode(STEP, OUTPUT);
  pcf8574.pinMode(DIR, OUTPUT);
  pcf8574.begin();
}

void loop(){
  // pcf8574.digitalWrite(P0, HIGH);
  // pcf8574.digitalWrite(P1, LOW);
  // delay(1000);
  // pcf8574.digitalWrite(P0, LOW);
  // pcf8574.digitalWrite(P1, HIGH);
  // delay(1000);

  pcf8574.digitalWrite(DIR, HIGH);
  Serial.println("Spinning Clockwise...");
  // Serial.println("Spinning Clockwise...2");
  
  for(int i = 0; i<steps_per_rev; i++)
  {
    pcf8574.digitalWrite(STEP, HIGH);
    delayMicroseconds(3000);
    pcf8574.digitalWrite(STEP, LOW);
    delayMicroseconds(3000);
  }
  delay(1000); 
  
  pcf8574.digitalWrite(DIR, LOW);
  Serial.println("Spinning Anti-Clockwise...");

  for(int i = 0; i<steps_per_rev; i++)
  {
    pcf8574.digitalWrite(STEP, HIGH);
    delayMicroseconds(3000);
    pcf8574.digitalWrite(STEP, LOW);
    delayMicroseconds(3000);
  }
  delay(1000);
}
