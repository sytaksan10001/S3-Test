#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESP32Encoder.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <SPIFFS.h>
#include <TaskScheduler.h>
#include <WebSerial.h>
#include <WiFi.h>

#include <PCF8574.h>

#include "Encoder.h"
#include "L298N.h"

#define CLK 35
#define DT 36

#define SDA 12
#define SCL 11

#define DIR 4
#define STEP 5

const char *ssid = "ESP32";
const char *password = "mimimimimi";

void line();
void RPK();
void countering();
void printer();
void IRread();
void belokKiri();
void belokKanan();
void recvMsg(uint8_t *data, size_t len);
void reset();
void code();
void forkUp();
void forkDown();
void putarBalik();
void ambil();
void letakkan();
void maju();
void mundur();

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Scheduler ts;

Task LINE(2, TASK_FOREVER, &line);
Task CNT(100, TASK_FOREVER, &countering);
Task PRT(1000, TASK_FOREVER, &printer);
Task IRR(1, TASK_FOREVER, &IRread);
Task RES(5000, TASK_FOREVER, &reset);
Task MAIN(100, TASK_FOREVER, &code);

bool state, stateRight, stateLeft, stateRightMax = 0, stateLeftMax = 0, lastState = 0, stateFork = 0;
int count = 0, lastCount;
bool counting = false;
int on = 1;
int SPEED = 170;
int step = 1;
int pulse = 0;
int pd;
int start = 1;
int pos = 0;
bool lastPos = false;

int speed1 = SPEED;
int speed2 = SPEED;
int speed3 = SPEED;
int speed4 = SPEED;

L298N motor1(0, 40000, 8, 13, P6, P5, true);
L298N motor2(1, 40000, 8, 47, 42, 41);
L298N motor3(2, 40000, 8, 14, P4, P3, true);
L298N motor4(3, 40000, 8, 21, 40, 39);

Encoder enc1(motor1, 17, 18);
Encoder enc2(motor2, 37, 38);
Encoder enc3(motor3, 10, 9);
Encoder enc4(motor4, 36, 35);

void processCommand(String inputValue) {
    WebSerial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());
    switch (inputValue.toInt()) {
    case 1:
        MAIN.enable();
        break;

    case 2:
        break;

    default:
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        break;
    }
}

void handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Error 404 : File Not Found");
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        // client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
        WebSerial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        WebSerial.printf("WebSocket client #%u disconnected\n", client->id());
        processCommand("0");
        break;
    case WS_EVT_DATA:
        AwsFrameInfo *info;
        info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            std::string myData = "";
            myData.assign((char *)data, len);
            processCommand(myData.c_str());
        }
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    default:
        break;
    }
}

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.softAP(ssid, password);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/homepage.html"); });
    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/control.html"); });
    server.onNotFound(handleNotFound);

    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    ElegantOTA.begin(&server);
    WebSerial.begin(&server);
    WebSerial.msgCallback(recvMsg);
    server.begin();

    pinMode(15, INPUT);
    pinMode(2, INPUT);
    pinMode(6, INPUT);
    pinMode(16, INPUT);
    pinMode(1, INPUT);
    pinMode(7, INPUT);
    pinMode(STEP, OUTPUT);
    pinMode(DIR, OUTPUT);

    _pcf.pinMode(P1, INPUT);
    _pcf.begin();
    delay(5000);

    ts.addTask(LINE);
    ts.addTask(CNT);
    ts.addTask(PRT);
    ts.addTask(IRR);
    ts.addTask(RES);
    ts.addTask(MAIN);

 

    IRR.enable();
    RES.enable();


    enc1._encoder.clearCount();
    enc2._encoder.clearCount();
    enc3._encoder.clearCount();
    enc4._encoder.clearCount();

    pulse = enc2._encoder.getCount();

    WebSerial.println("setup completed");
    Serial.println("setup completed");
    // MAIN.enable();
}

void loop() {
    ElegantOTA.loop();
    ts.execute();
}

void line() {

    if (stateRight) {
        speed1 = 180;
        speed3 = 180;
        speed2 = 160;
        speed4 = 160;
        // Serial.println("kanan");
    } else if (stateLeft) {
        speed1 = 160;
        speed3 = 160;
        speed2 = 180;
        speed4 = 180;
        // Serial.println("kiri");
    } else if (stateLeft && !state) {
        speed1 = 200;
        speed3 = 200;
        speed2 = 150;
        speed4 = 150;
    } else if (stateRight && !state) {
        speed1 = 150;
        speed3 = 150;
        speed2 = 200;
        speed4 = 200;
    } else {
        speed1 = 170;
        speed3 = 170;
        speed2 = 170;
        speed4 = 170;
    }
    motor1.forward(speed1);
    motor2.forward(speed2);
    motor3.forward(speed3);
    motor4.forward(speed4);
}

void countering() {
    if ((counting == false) && !((stateRight == 1) && (stateLeft == 1))) {
        counting = true;
    }

    if ((counting == true) && ((stateRight == 1) && (stateLeft == 1))) {
        count++;
        // WebSerial.println(count);
        counting = false;
    }

    if (count != lastCount) {
        WebSerial.println(count);
    }

    lastCount = count;
}

void printer() {
    // WebSerial.println(count);
    static int a, b, c, d;
    a = ledcRead(0);
    b = ledcRead(1);
    c = ledcRead(2);
    d = ledcRead(3);
}

void IRread() {
    state = digitalRead(15);
    stateLeft = digitalRead(7);
    stateRight = digitalRead(2);
    stateLeftMax = digitalRead(16);
    stateRightMax = digitalRead(1);
    
}

void putarBalik() {
   
    motor1.forward(151);
    motor2.backward(150);
    motor3.forward(151);
    motor4.backward(150);
    delay(2800);

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
    delay(1000);
    WebSerial.println(pulse);
    RES.enable();
}

void recvMsg(uint8_t *data, size_t len) {
    WebSerial.println("data received");
    String d = "";
    for (int i = 0; i < len; i++) {
        d += char(data[i]);
    }
}

void reset() {
    enc1._encoder.clearCount();
    enc2._encoder.clearCount();
    enc3._encoder.clearCount();
    enc4._encoder.clearCount();
}

void code() {
    if (step == 1) {

        CNT.enable();

        motor1.forward(160);
        motor2.forward(161);
        motor3.forward(160);
        motor4.forward(161);

        if (count >= 5) {
            step++;
            CNT.disable();
        }

        
    } else if (step == 2) {
        motor1.forward(160);
        motor2.forward(162);
        motor3.forward(160);
        motor4.forward(162);
        delay(500);

        pos = 0;
        step++;

    } else if (step == 3) {

        motor1.forward(158);
        motor2.backward(150);
        motor3.backward(150);
        motor4.forward(153);

        if ((pos == 1) && state) {
            motor1.stop();
            motor2.stop();
            motor3.stop();
            motor4.stop();
            step++;
            pos = 0;
        }
        if (stateLeftMax && !lastPos) {
            pos++;
            lastPos = true;
        }
        if (!stateLeftMax) {
            lastPos = false;
        }
    } else if (step == 4) {

        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        delay(1500);
        ambil();
        step++;
    } else if (step == 5) {
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        delay(1000);
        mundur();

        motor1.backward(150);
        motor2.forward(153);
        motor3.forward(150);
        motor4.backward(153);
        delay(3000);

        delay(500);
        putarBalik();
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        // delay(50000);
        step++;
    } else if (step == 6) {
        count = 0;
        step++;
    } else if (step == 7) {
        CNT.enable();

        motor1.forward(160);
        motor2.forward(162);
        motor3.forward(160);
        motor4.forward(162);

        if (count >= 5) {
            step++;
            CNT.disable();
            count = 0;
        }
    } else if (step == 8) {
        motor1.forward(160);
        motor2.forward(163);
        motor3.forward(160);
        motor4.forward(163);
        delay(500);

        pos = 0;
        step++;
    } else if (step == 9) {

        motor1.backward(150);
        motor2.forward(156);
        motor3.forward(153);
        motor4.backward(150);

        if ((pos == 1) && state) {
            motor1.stop();
            motor2.stop();
            motor3.stop();
            motor4.stop();
            step++;
        }
        if (stateRightMax && !lastPos) {
            pos++;
            lastPos = true;
        }
        if (!stateRightMax) {
            lastPos = false;
        }
    } else if (step == 10) {
        motor1.forward(160);
        motor2.forward(163);
        motor3.forward(160);
        motor4.forward(163);
        delay(1600);
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        forkDown();
        mundur();
        delay(800);
        step++;
    } else if (step == 11) {
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        // delay(50000);
        putarBalik();
        step++;
    } else if (step == 12) {
        mundur();
        delay(500);
        motor1.backward(150);
        motor2.forward(158);
        motor3.forward(153);
        motor4.backward(155);
        delay(2900);
        step++;
    } else if (step == 13) {

        CNT.enable();

        motor1.forward(160);
        motor2.forward(162);
        motor3.forward(160);
        motor4.forward(162);

        if (count >= 5) {
            step++;
            CNT.disable();
            count = 0;
        }
    } else if (step == 14) {
        motor1.forward(160);
        motor2.forward(161);
        motor3.forward(160);
        motor4.forward(161);
        delay(1000);
        step++;
    } else if (step == 15) {
        motor1.forward(153);
        motor2.backward(150);
        motor3.backward(150);
        motor4.forward(153);

        if ((pos == 3) && state) {
            motor1.stop();
            motor2.stop();
            motor3.stop();
            motor4.stop();
            step++;
            pos = 0;
        }
        if (stateLeftMax && !lastPos) {
            pos++;
            lastPos = true;
        }
        if (!stateLeftMax) {
            lastPos = false;
        }
    } else if (step == 16) {
        ambil();
        motor1.backward(150);
        motor2.forward(150);
        motor3.forward(150);
        motor4.backward(150);
        delay(6000);
        putarBalik();
        step++;
    } else if (step == 17) {
        CNT.enable();

        motor1.forward(160);
        motor2.forward(162);
        motor3.forward(160);
        motor4.forward(162);

        if (count >= 5) {
            step++;
            CNT.disable();
            count = 0;
        }

    } else if (step == 18) {
        motor1.forward(160);
        motor2.forward(162);
        motor3.forward(160);
        motor4.forward(162);
        delay(500);
        step++;
    } else if (step == 19) {
        motor1.backward(150);
        motor2.forward(153);
        motor3.forward(153);
        motor4.backward(150);

        if ((pos == 3) && state) {
            motor1.stop();
            motor2.stop();
            motor3.stop();
            motor4.stop();
            step++;
        }
        if (stateRightMax && !lastPos) {
            pos++;
            lastPos = true;
        }
        if (!stateRightMax) {
            lastPos = false;
        }
    } else if (step == 20) {
        motor1.forward(160);
        motor2.forward(163);
        motor3.forward(160);
        motor4.forward(163);
        delay(1500);
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        forkDown();
        mundur();
        delay(800);
    }else if (step == 21) {
        
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
    }
}

void resSPD() {
    speed1 = SPEED;
    speed3 = SPEED;
    speed2 = SPEED;
    speed4 = SPEED;
}

void forkUp() {
    digitalWrite(DIR, HIGH);

    for (int i = 0; i < 300; i++) {
        digitalWrite(STEP, HIGH);
        delayMicroseconds(2000);
        digitalWrite(STEP, LOW);
        delayMicroseconds(2000);
    }
    delay(1000);
}

void forkDown() {
    digitalWrite(DIR, LOW);

    for (int i = 0; i < 300; i++) {
        digitalWrite(STEP, HIGH);
        delayMicroseconds(1000);
        digitalWrite(STEP, LOW);
        delayMicroseconds(1000);
    }
    delay(1000);
}

void ambil() {
    stateFork = digitalRead(6);

    while (stateFork) {
        maju();

        stateFork = digitalRead(6);
    }

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();

    forkUp();
}

void letakkan() {

    forkDown();

    motor1.backward(170);
    motor2.backward(170);
    motor3.backward(170);
    motor4.backward(170);

    delay(500);

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
}

void belokKananReg() {
    RES.disable();
    pulse = 0;
    enc4._encoder.clearCount();

    motor1.forward(151);
    motor2.backward(150);
    motor3.forward(151);
    motor4.backward(150);
    delay(1800);

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
    delay(1500);

    pulse = enc4._encoder.getCount();
    WebSerial.println(pulse);
    RES.enable();
}

void belokKiriReg() {

    motor1.backward(151);
    motor2.forward(150);
    motor3.backward(151);
    motor4.forward(150);
    delay(1800);

    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
    delay(1500);

    pulse = enc2._encoder.getCount();
    WebSerial.println(pulse);
    RES.enable();
}

void maju() {
    motor1.forward(160);
    motor2.forward(161);
    motor3.forward(160);
    motor4.forward(161);
}

void mundur() {
    motor1.backward(160);
    motor2.backward(161);
    motor3.backward(160);
    motor4.backward(161);
}