#include "Encoder.h"
#include "Arduino.h"
#include <QuickPID.h>

Encoder::Encoder(ESP32Encoder &enc) {
    _enc = enc;
}

float Encoder::RPM() {
    position = _enc.getCount()

    rpm = ((position / (4 * 11 * 45 * 2)) * (60 / 0.1));
    Input = rpm;

    static QuickPID myPID(&Input, &Output, &Setpoint);
    myPID.Compute();

    
    _enc.clearCount();

    if (Output > 0) {
        digitalWrite(40, LOW);
        digitalWrite(39, HIGH);
    } else {
        digitalWrite(40, HIGH);
        digitalWrite(39, LOW);
    }

    mapped = map(abs(Output), 0, 255, 115, 240);
    ledcWrite(0, mapped);

    // Serial.print("  rpm: ");
    // Serial.print(Input);
    return Input;
}