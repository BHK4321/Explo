// ESP32 Swarm Robot Firmware
// Handles motor control, sensor readings, and UDP communication with formation control

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// Wi-Fi Settings
const char* ssid = "Nagendra Kumar";
const char* password = "Nagendra122";
WiFiUDP udp;
const int udpPort = 4210;
char packetBuffer[255];

// Motor Pins
const int motor1A = 18, motor1B = 19;
const int motor2A = 23, motor2B = 22;
const int pwm1 = 5, pwm2 = 4; // PWM for speed control

// Ultrasonic Sensor Pins
const int trigPin = 32;
const int echoPin = 33;

// MPU6050 (IMU) Setup
Adafruit_MPU6050 mpu;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to Wi-Fi");
    udp.begin(udpPort);

    pinMode(motor1A, OUTPUT);
    pinMode(motor1B, OUTPUT);
    pinMode(motor2A, OUTPUT);
    pinMode(motor2B, OUTPUT);
    pinMode(pwm1, OUTPUT);
    pinMode(pwm2, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
    }
}

// Motor Control Function
void moveRobot(int speedL, int speedR) {
    digitalWrite(motor1A, speedL > 0);
    digitalWrite(motor1B, speedL < 0);
    digitalWrite(motor2A, speedR > 0);
    digitalWrite(motor2B, speedR < 0);
    analogWrite(pwm1, abs(speedL));
    analogWrite(pwm2, abs(speedR));
}

// Read Ultrasonic Sensor
float getDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
}

// Read IMU Sensor Data
void readIMU() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    Serial.print("Angle X: "); Serial.println(a.acceleration.x);
    Serial.print("Angle Y: "); Serial.println(a.acceleration.y);
}

// Handle Incoming UDP Commands
void receiveUDP() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        udp.read(packetBuffer, 255);
        String command = String(packetBuffer);
        if (command == "FORWARD") moveRobot(150, 150);
        else if (command == "BACKWARD") moveRobot(-150, -150);
        else if (command == "LEFT") moveRobot(-150, 150);
        else if (command == "RIGHT") moveRobot(150, -150);
        else if (command == "STOP") moveRobot(0, 0);
        else if (command == "LINE") straightLineFormation();
        else if (command == "CIRCLE") circleFormation();
        else if (command == "TRIANGLE") triangleFormation();
    }
}

// Formation Control: Maintain distance from leader bot
void formationControl(float targetDistance) {
    float distance = getDistance();
    if (distance > targetDistance + 5) {
        moveRobot(100, 100); // Move forward to close the gap
    } else if (distance < targetDistance - 5) {
        moveRobot(-100, -100); // Move backward to maintain spacing
    } else {
        moveRobot(0, 0); // Maintain position
    }
}

// Straight Line Formation
void straightLineFormation() {
    formationControl(20.0); // Maintain 20cm distance from leader
}

// Circular Formation (Bots move in a circular path)
void circleFormation() {
    moveRobot(100, 50); // Different speeds for circular motion
}

// Triangle Formation (Bots maintain 2 distances)
void triangleFormation() {
    float distance = getDistance();
    if (distance > 25) moveRobot(100, 100); // Maintain 25cm distance for triangle
    else if (distance < 15) moveRobot(-100, -100);
    else moveRobot(0, 0);
}

void loop() {
    receiveUDP();
    readIMU();
    delay(100);
}
