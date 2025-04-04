
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <L298N.h>

WiFiClient espClient;
PubSubClient client(espClient);

#define AIN1 8
#define AIN2 9
#define PWMA 20
#define BIN1 10
#define BIN2 11
#define PWMB 33
#define STBY 12

L298N motor1(PWMA, AIN1, AIN2);
L298N motor2(PWMB, BIN1, BIN2);

void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0'; // Null terminate the received string
    String message = String((char*)payload);
    
    int commaIndex = message.indexOf(',');
    if (commaIndex != -1) {
        int leftSpeed = message.substring(0, commaIndex).toInt();
        int rightSpeed = message.substring(commaIndex + 1).toInt();
        
        motor_drive(leftSpeed, rightSpeed);
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP32_Bot_Receiver")) {
            Serial.println("connected");
            client.subscribe("bot/speed");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void loop() {
    if (!client.connected()) {
        while (!client.connect("ESP32_Bot_Receiver")) {
            Serial.print(".");
            delay(500);
        }
        Serial.println("Reconnected to MQTT");
        client.subscribe("bot/speed");
    }
    client.loop();
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
