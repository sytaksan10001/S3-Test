#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESP32Encoder.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <TaskScheduler.h>
#include <WebSerial.h>
#include <WiFi.h>

#include <PCF8574.h>

#include "Encoder.h"
#include "L298N.h"
#include <QuickPID.h>

#define CLK 35
#define DT 36

#define SDA 12
#define SCL 11

const char *ssid = "ESP32";
const char *password = "mimimimimi";

void testing();
void RPK();
void countering();
void printer();
void IRread();
void belokKiri();
void belokKanan();

AsyncWebServer server(80);
Scheduler ts;

Task RPKT(10, TASK_FOREVER, &RPK);
Task PWXX(1, TASK_FOREVER, &testing);
Task CNT(100, TASK_FOREVER, &countering);
Task PRT(1000, TASK_FOREVER, &printer);
Task IRR(1, TASK_FOREVER, &IRread);

float Input1, Output1, Input2, Output2, Input3, Output3, Input4, Output4, Setpoint;
float Kp = 5, Ki = 1, Kd = 0;
bool state, stateRight, stateLeft, stateRightMax = 0, stateLeftMax = 0, lastState = 0;
int count = 0, lastCount;
bool counting = false;
int on = 1;

int speed1 = 170;
int speed2 = 170;
int speed3 = 170;
int speed4 = 170;

QuickPID myPID1(&Input1, &Output1, &Setpoint);
QuickPID myPID2(&Input2, &Output2, &Setpoint);
QuickPID myPID3(&Input3, &Output3, &Setpoint);
QuickPID myPID4(&Input4, &Output4, &Setpoint);

L298N motor1(0, 40000, 8, 13, P6, P5, true);
L298N motor2(1, 40000, 8, 47, 42, 41);
L298N motor3(2, 40000, 8, 14, P4, P3, true);
L298N motor4(3, 40000, 8, 21, 40, 39);

Encoder enc1(motor1, 17, 18);
Encoder enc2(motor2, 37, 38);
Encoder enc3(motor3, 10, 9);
Encoder enc4(motor4, 36, 35);

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.softAP(ssid, password);
    ElegantOTA.begin(&server);
    WebSerial.begin(&server);
    server.begin();

    pinMode(15, INPUT);
    pinMode(2, INPUT);
    pinMode(6, INPUT);
    pinMode(16, INPUT);
    pinMode(1, INPUT);

    _pcf.begin();
    // ts.addTask(PWX0);
    // PWX0.enable();
    delay(15000);

    ts.addTask(PWXX);
    // PWXX.enable();

    // enc4.init();
    ts.addTask(RPKT);
    ts.addTask(CNT);
    ts.addTask(PRT);
    ts.addTask(IRR);
    // RPKT.enable();
    CNT.enable();
    // PRT.enable();
    PWXX.enable();
    IRR.enable();
    // Serial.println("first print");

    // myPID4.SetTunings(Kp, Ki, Kd);
    // myPID4.SetMode(1);
    // myPID4.SetOutputLimits(-255, 255);
    // myPID4.SetSampleTimeUs(10000);

    // myPID3.SetTunings(Kp, Ki, Kd);
    // myPID3.SetMode(1);
    // myPID3.SetOutputLimits(-255, 255);
    // myPID3.SetSampleTimeUs(10000);

    // myPID2.SetTunings(Kp, Ki, Kd);
    // myPID2.SetMode(1);
    // myPID2.SetOutputLimits(-255, 255);
    // myPID2.SetSampleTimeUs(10000);

    // myPID1.SetTunings(Kp, Ki, Kd);
    // myPID1.SetMode(1);
    // myPID1.SetOutputLimits(-255, 255);
    // myPID1.SetSampleTimeUs(10000);

    enc1._encoder.clearCount();
    enc2._encoder.clearCount();
    enc3._encoder.clearCount();
    enc4._encoder.clearCount();

    // motor1.forward(151);
    // motor2.backward(150);
    // motor3.forward(151);
    // motor4.backward(150);

    // delay(5000);

    count = enc2._encoder.getCount();

    // while (count <= 2000) {
    //     motor1.forward(200);
    //     motor2.forward(150);
    //     motor3.forward(150);
    //     motor4.forward(200);

    //     count = enc2._encoder.getCount();
    //     Serial.println("loop");
    // }

    // Serial.print("enc 1: ");
    // Serial.println(enc1._encoder.getCount());
    // Serial.print("enc 2: ");
    // Serial.println(enc2._encoder.getCount());
    // Serial.print("enc 3: ");
    // Serial.println(enc3._encoder.getCount());
    // Serial.print("enc 4: ");
    // Serial.println(enc4._encoder.getCount());
}

void loop() {
    ElegantOTA.loop();
    ts.execute();
    /////////////////////////////////////////

    // Serial.print("depan: ");
    // Serial.print(digitalRead(15));
    // Serial.print("\tkiri: ");
    // Serial.print(digitalRead(6));
    // Serial.print("\tkanan: ");
    // Serial.print(digitalRead(2));
    // Serial.print("\tkiri max: ");
    // Serial.print(digitalRead(16));
    // Serial.print("\tkanan max: ");
    // Serial.println(digitalRead(1));

    ///////////////////////////////////////

    // Serial.print("depan: ");
    // Serial.print(analogRead(15));
    // Serial.print("\tkiri: ");
    // Serial.print(analogRead(6));
    // Serial.print("\tkanan: ");
    // Serial.print(analogRead(2));
    // Serial.print("\tkiri max: ");
    // Serial.print(analogRead(16));
    // Serial.print("\tkanan max: ");
    // Serial.println(analogRead(1));

    /////////////////////////////////////////

    // Serial.print("\tdepan: ");
    // Serial.print(state);
    // Serial.print("\tkiri: ");
    // Serial.print(stateLeft);
    // Serial.print("\tkanan: ");
    // Serial.println(stateRight);

    //////////////////////////////////////////////////////////////

    // motor1.forward(speed1);
    // motor2.forward(speed2);
    // motor3.forward(speed3);
    // motor4.forward(speed4);

    ////////////////////////////
    //////////////////////////

    // Setpoint = enc4.Setpoint;

    // myPID4.Compute();
    // myPID3.Compute();
    // myPID2.Compute();
    // myPID1.Compute();
}

void testing() {
    if (stateRight) {
        speed1 += 1;
        speed3 += 1;
        speed2 -= 1;
        speed4 -= 1;
        // Serial.println("kanan");
    } else if (stateLeft) {
        speed1 -= 1;
        speed3 -= 1;
        speed2 += 1;
        speed4 += 1;
        // Serial.println("kiri");
    } else if (stateLeft && !state) {
        speed1 -= 5;
        speed3 -= 5;
        speed2 += 5;
        speed4 += 5;
    } else if (stateRight && !state) {
        speed1 += 5;
        speed3 += 5;
        speed2 -= 5;
        speed4 -= 5;
    } else {
        speed1 = 170;
        speed3 = 170;
        speed2 = 170;
        speed4 = 170;
    }

    if (stateLeftMax) {
        belokKiri();
    }
    // if(stateRightMax){
    //     belokKanan();
    //     // Serial.println("belok kanan");
    // }
}

void RPK() {
    Setpoint = 50;

    Input4 = enc4.Input;
    enc4.Output = Output4;
    enc4.RPM(50);
    Serial.print("encoder_4: ");
    Serial.print(Input4);
    Serial.print(" ");

    Input3 = enc3.Input;
    enc3.Output = Output3;
    enc3.RPM(50);
    Serial.print("encoder_3: ");
    Serial.print(Input3);
    Serial.print(" ");

    Input2 = enc2.Input;
    enc2.Output = Output2;
    enc2.RPM(50);
    Serial.print("encoder_2: ");
    Serial.print(Input2);
    Serial.print(" ");

    Input1 = enc1.Input;
    enc1.Output = Output1;
    enc1.RPM(50);
    Serial.print("encoder_1: ");
    Serial.print(Input1);
    Serial.print(" ");

    Serial.println();
}

void countering() {
    if ((counting == false) && !((stateRight == 1) && (stateLeft == 1))) {
        counting = true;
    }

    if ((counting == true) && ((stateRight == 1) && (stateLeft == 1))) {
        count++;
        // Serial.println("counting");
        counting = false;
    }

    if (count != lastCount) {
        WebSerial.println(count);
    }

    lastCount = count;

    // if (state) {
    //     Serial.println("tengah");
    // }
    // if (stateLeft) {
    //     Serial.println("kiri");
    // }
    // if (stateRight) {
    //     Serial.println("kanan");
    // }
    // if (stateLeftMax) {
    //     Serial.println("kiri max");
    // }
    // if (stateRightMax) {
    //     Serial.println("kanan max");
    // }
}

void printer() {
    WebSerial.println(count);
}

void IRread() {
    state = digitalRead(15);
    stateLeft = digitalRead(6);
    stateRight = digitalRead(2);
    stateLeftMax = digitalRead(16);
    stateRightMax = digitalRead(1);

    // if (analogRead(15) > 200) {
    //     state = 1;
    // } else {
    //     state = 0;
    // }

    // if (analogRead(2) > 200) {
    //     stateRight = 1;
    // } else {
    //     stateRight = 0;
    // }

    // if (analogRead(6) > 200) {
    //     stateLeft = 1;
    // } else {
    //     stateLeft = 0;
    // }

    // if (analogRead(16) > 200) {
    //     stateLeftMax = 1;
    // } else {
    //     stateLeftMax = 0;
    // }

    // if (analogRead(1) > 200) {
    //     stateRightMax = 1;
    // } else {
    //     stateRightMax = 0;
    // }
}

void belokKanan() {
    count = 0;
    enc2._encoder.clearCount();
    while (count >= -1040) {
        motor1.forward(151);
        motor2.backward(150);
        motor3.forward(151);
        motor4.backward(150);

        count = enc2._encoder.getCount();
    }

    state = digitalRead(15);
    while (!state) {
        motor1.backward(151);
        motor2.forward(150);
        motor3.forward(151);
        motor4.backward(150);

        state = digitalRead(15);
    }

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
}

void belokKiri() {
    count = 0;
    enc2._encoder.clearCount();
    while (count <= 1040) {
        motor1.backward(151);
        motor2.forward(150);
        motor3.backward(151);
        motor4.forward(150);
        WebSerial.println("true");
        count = enc2._encoder.getCount();
    }

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();

    state = digitalRead(15);

    while (!state) {
        motor1.forward(151);
        motor2.backward(150);
        motor3.backward(151);
        motor4.forward(150);

        state = digitalRead(15);
    }

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
}