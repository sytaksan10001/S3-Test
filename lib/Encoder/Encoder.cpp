#include "Encoder.h"

Encoder::Encoder(L298N &motor, int phaseA, int phaseB) : _motor(motor) {
    ESP32Encoder encoder;
    encoder.attachSingleEdge(phaseA, phaseB);
    encoder.setCount(0);
    _encoder = encoder;
    Serial.begin(115200);
}