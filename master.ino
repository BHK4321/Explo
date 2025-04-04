#include <WiFi.h>
#include <WebServer.h>
#include <QTRSensors.h>
#include <L298N.h>
#include <PubSubClient.h>  // Include MQTT library

// WiFi and MQTT Broker details
const char* ssid = "Aa";
const char* password = "12345678";
const char* mqtt_server = "192.168.1.100"; // Set the correct MQTT broker IP

WiFiClient espClient;
PubSubClient client(espClient);

QTRSensors qtr;

#define AIN1 8
#define AIN2 9
#define PWMA 20
#define BIN1 10
#define BIN2 11
#define PWMB 33
#define STBY 12

const int offsetA = 1;
const int offsetB = 1;

L298N motor1(PWMA, AIN1, AIN2);
L298N motor2(PWMB, BIN1, BIN2);

const uint8_t SensorCount = 6;
uint16_t sensorValues[SensorCount];

float Kp = 1.25, Ki = 0.03, Kd = 0.5;
uint16_t position;
int P, D, I, PIDvalue, error;
int previousError = 0;
int lsp, rsp;
uint8_t multiP = 1;
uint8_t multiI  = .01;
uint8_t multiD = 1;
int lfspeed = 230;
int threshold = 0;

float Pvalue;
float Ivalue;
float Dvalue;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    // MQTT Setup
    client.setServer(mqtt_server, 1883);  // NEW LINE: Setting up MQTT broker
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP32_Client")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void loop() {
    if (!client.connected()) {  // NEW LINE: Ensuring MQTT connection
        while (!client.connect("ESP32_Client")) {
            Serial.print(".");
            delay(500);
        }
        Serial.println("Reconnected to MQTT");
    }
    client.loop();

    robot_control();
    delay(50);
}

void robot_control() {
    position = qtr.readLineBlack(sensorValues);
    error = 2500 - position;
    
    bool detected[6] = {0};
    for (uint8_t i = 0; i < SensorCount; i++) {
        detected[i] = sensorValues[i] > threshold;
    }

    bool hardRight = detected[0] && detected[1] && detected[2];
    bool hardLeft = detected[3] && detected[4] && detected[5];
    bool crossSection = detected[0] && detected[1] && detected[2] && detected[3] && detected[4] && detected[5];

    if (crossSection) PID_Linefollow(-error);
    else if (hardLeft) HardLeft();
    else if (hardRight) HardRight();
    else PID_Linefollow(-error);

    // Publish speed data over MQTT
    String speedData = String(lsp) + "," + String(rsp);  // NEW LINE: Formatting speed data
    client.publish("bot/speed", speedData.c_str());  // NEW LINE: Publishing speed data
}

void PID_Linefollow(int error) {
    P = error;
    I += error;
    I = constrain(I, -1000, 1000);
    D = error - previousError;

    Pvalue = (Kp / pow(10, multiP)) * P;
    Ivalue = (Ki / pow(10, multiI)) * I;
    Dvalue = (Kd / pow(10, multiD)) * D;

    float PIDvalue = Pvalue + Ivalue + Dvalue;
    previousError = error;

    lsp = constrain(lfspeed - PIDvalue, -255, 255);
    rsp = constrain(lfspeed + PIDvalue, -255, 255);

    motor_drive(lsp, rsp);
}

void HardLeft() {
    motor_drive(-200, 200);
    delay(100);
}

void HardRight() {
    motor_drive(200, -200);
    delay(100);
}

void motor_drive(int left, int right) {
    if (right > 0) {
        motor2.setSpeed(right);
        motor2.forward();
    } else {
        motor2.setSpeed(abs(right));
        motor2.backward();
    }
    if (left > 0) {
        motor1.setSpeed(left);
        motor1.forward();
    } else {
        motor1.setSpeed(abs(left));
        motor1.backward();
    }
}
