#ifndef ENCODER_H
#define ENCODER_H

#include "L298N.h"
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <QuickPID.h>

class Encoder {
  public:
    Encoder(L298N &motor, int phaseA, int phaseB);
    void RPM(float setpoint);
    ESP32Encoder _encoder;

  private:
    L298N _motor;
};

void move(float jarak, Encoder enc1, Encoder enc2, Encoder enc3, Encoder enc4);

#endif