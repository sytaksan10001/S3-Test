#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

const int STEP = 11;
const int DIR = 10;
const int  steps_per_rev = 700;

void setup()
{
  delay(2000);
  Serial.begin(115200);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);

  digitalWrite(DIR, LOW);

  for(int i = 0; i<steps_per_rev; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(7000);
    digitalWrite(STEP, LOW);
    delayMicroseconds(7000);
  }
  delay(1000);

}
void loop()
{
  // digitalWrite(DIR, HIGH);
  // Serial.println("Spinning Clockwise...");
  
  // for(int i = 0; i<steps_per_rev; i++)
  // {
  //   digitalWrite(STEP, HIGH);
  //   delayMicroseconds(3000);
  //   digitalWrite(STEP, LOW);
  //   delayMicroseconds(3000);
  // }
  // delay(1000); 
  
  // digitalWrite(DIR, LOW);
  // Serial.println("Spinning Anti-Clockwise...");

  // for(int i = 0; i<steps_per_rev; i++)
  // {
  //   digitalWrite(STEP, HIGH);
  //   delayMicroseconds(1000);
  //   digitalWrite(STEP, LOW);
  //   delayMicroseconds(1000);
  // }
  // delay(1000);
}