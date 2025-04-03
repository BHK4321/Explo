#include <WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

#define MOTOR1_ENA  5
#define MOTOR1_IN1  18
#define MOTOR1_IN2  19
#define MOTOR2_ENB  21
#define MOTOR2_IN3  22
#define MOTOR2_IN4  23
#define TRIG_PIN    12
#define ECHO_PIN    14
#define SERVO_PIN   27

const char* ssid = "your_hotspot_ssid";
const char* password = "your_hotspot_password";
const char* mqtt_server = "your_laptop_ip";

WiFiClient espClient;
PubSubClient client(espClient);
Servo scanServo;

int botID = 0;
bool idAssigned = false;
float leftBotDistance, rightBotDistance;

void setup_wifi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void callback(char* topic, byte* message, unsigned int length) {
    String command = "";
    for (int i = 0; i < length; i++) {
        command += (char)message[i];
    }
    
    if (command == "align_straight") {
        alignStraightLine();
    }
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect("ESP32_Bot")) {
            client.subscribe("bot/command");
            client.subscribe("bot/positions");
        } else {
            delay(5000);
        }
    }
}

void moveForward() {
    digitalWrite(MOTOR1_IN1, HIGH);
    digitalWrite(MOTOR1_IN2, LOW);
    digitalWrite(MOTOR2_IN3, HIGH);
    digitalWrite(MOTOR2_IN4, LOW);
    analogWrite(MOTOR1_ENA, 150);
    analogWrite(MOTOR2_ENB, 150);
}

void moveBackward() {
    digitalWrite(MOTOR1_IN1, LOW);
    digitalWrite(MOTOR1_IN2, HIGH);
    digitalWrite(MOTOR2_IN3, LOW);
    digitalWrite(MOTOR2_IN4, HIGH);
    analogWrite(MOTOR1_ENA, 150);
    analogWrite(MOTOR2_ENB, 150);
}

void stopMotors() {
    digitalWrite(MOTOR1_IN1, LOW);
    digitalWrite(MOTOR1_IN2, LOW);
    digitalWrite(MOTOR2_IN3, LOW);
    digitalWrite(MOTOR2_IN4, LOW);
}

long measureDistance(int angle) {
    scanServo.write(angle);
    delay(500);
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
}

void assignBotID() {
    if (!idAssigned) {
        long frontDistance = measureDistance(90);
        if (frontDistance > 50) {
            botID = 2;
        } else {
            botID = (measureDistance(90) < 30) ? 3 : 1;
        }
        idAssigned = true;
        String msg = "Bot ID: " + String(botID);
        Serial.println(msg);
        client.publish("bot/positions", msg.c_str());
    }
}

void alignStraightLine() {
    Serial.println("Aligning in a straight line...");
    assignBotID();
    
    if (botID == 2) {
        return; // Middle bot remains stationary
    }
    
    long middleToLeft = measureDistance(180);
    long middleToRight = measureDistance(0);
    
    if (botID == 1) {
        while (measureDistance(90) > middleToLeft) {
            moveForward();
        }
        while (measureDistance(90) < middleToLeft) {
            moveBackward();
        }
    }
    else if (botID == 3) {
        while (measureDistance(90) > middleToRight) {
            moveForward();
        }
        while (measureDistance(90) < middleToRight) {
            moveBackward();
        }
    }
    stopMotors();
}

void setup() {
    pinMode(MOTOR1_ENA, OUTPUT);
    pinMode(MOTOR1_IN1, OUTPUT);
    pinMode(MOTOR1_IN2, OUTPUT);
    pinMode(MOTOR2_ENB, OUTPUT);
    pinMode(MOTOR2_IN3, OUTPUT);
    pinMode(MOTOR2_IN4, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    Serial.begin(115200);
    scanServo.attach(SERVO_PIN);
    
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    alignStraightLine();
    delay(3000);
}
i