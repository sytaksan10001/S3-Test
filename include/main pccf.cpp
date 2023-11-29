#include "Arduino.h"
#include "PCF8574.h"

// Set i2c address
PCF8574 pcf8574(0x38, 4, 5);

void setup()
{
	Serial.begin(115200);
	delay(1000);

	// Set pinMode to OUTPUT
	pcf8574.pinMode(P0, OUTPUT);
	// pcf8574.pinMode(P1, INPUT);

	Serial.print("Init pcf8574...");
	if (pcf8574.begin()){
		Serial.println("OK");
	}else{
		Serial.println("KO");
	}
}

void loop()
{
	pcf8574.digitalWrite(P0, HIGH);
	delay(1000);
	pcf8574.digitalWrite(P0, LOW);
	delay(1000);
}