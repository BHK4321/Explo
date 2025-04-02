#include <WiFi.h>
#include <WiFiUdp.h>

WiFiUDP udp;
IPAddress esp32IP(192, 168, 1, 101);
const unsigned int udpPort = 4210;

const int motor1Pin1 = 14;
const int motor1Pin2 = 12;
const int motor2Pin1 = 27;
const int motor2Pin2 = 26;

void setup() {
  Serial.begin(115200);
  WiFi.begin();
  WiFi.config(esp32IP);
  udp.begin(udpPort);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    udp.read(incomingPacket, 255);
    String command = String(incomingPacket);

    if (command.startsWith("MOVE_STRAIGHT")) {
      moveStraight(command.substring(14).toInt());
    } else if (command.startsWith("FORM_CIRCLE")) {
      formCircle(command.substring(12).toInt());
    } else if (command.startsWith("FORM_TRIANGLE")) {
      formTriangle(command.substring(14).toInt());
    }
  }
}

void moveStraight(int distance) {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin2, LOW);
  delay(distance * 10);
  stopMotors();
}

void formCircle(int radius) {
  for (int i = 0; i < 360; i++) {
    analogWrite(motor1Pin1, map(i, 0, 360, 0, 255));
    analogWrite(motor2Pin1, map(i, 0, 360, 255, 0));
    delay(10);
  }
  stopMotors();
}

void formTriangle(int sideLength) {
  for (int i = 0; i < 3; i++) {
    moveStraight(sideLength);
    turnRight(120);
  }
}

void turnRight(int angle) {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor2Pin2, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  delay(angle * 10);
  stopMotors();
}

void stopMotors() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

// Formation Control for Swarm Bots
void maintainStraightLine(int leaderSpeed) {
  int followerSpeed = leaderSpeed - 10; // Adjust based on distance sensors
  analogWrite(motor1Pin1, leaderSpeed);
  analogWrite(motor2Pin1, leaderSpeed);
  delay(5000); // Move forward for a duration
  stopMotors();
}

void maintainTriangleFormation(int leaderSpeed) {
  int leftBotSpeed = leaderSpeed - 10;
  int rightBotSpeed = leaderSpeed - 10;
  analogWrite(motor1Pin1, leaderSpeed);
  analogWrite(motor2Pin1, leaderSpeed);
  delay(2000);
  turnRight(60); // Adjust to form the triangle
  delay(2000);
  stopMotors();
}

void synchronizedCircularRotation(int speed) {
  analogWrite(motor1Pin1, speed);
  analogWrite(motor2Pin1, speed - 20); // outer wheel makes faster
  delay(5000);
  stopMotors();
}
