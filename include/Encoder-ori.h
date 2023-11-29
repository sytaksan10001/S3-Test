#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <ESP32Encoder.h>
#include <QuickPID.h>

class Encoder {
  public:
    Encoder(ESP32Encoder &enc);
    float RPM();
  private:
    ESP32Encoder _enc;
    float position, mapped, rpm;
    float Kp = 5, Ki = 1, Kd = 0;
    float Setpoint, Input, Output;
    // QuickPID myPID(float &input, float &output, float &setpoint);
};

#endif