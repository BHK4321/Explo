#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// Ethernet settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 50); // Static IP for Arduino
IPAddress esp32_1(192, 168, 1, 101); // First bot
IPAddress esp32_2(192, 168, 1, 102); // Second bot
IPAddress esp32_3(192, 168, 1, 103); // Third bot

const unsigned int udpPort = 4210;
EthernetUDP udp;

void setup() {
    Serial.begin(115200);  // Start Serial for GUI communication
    Ethernet.begin(mac, ip);
    udp.begin(udpPort);
    Serial.println("Arduino Ready. Listening for Serial Commands...");
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');  // Read command from GUI
        command.trim();

        Serial.print("Processing command: ");
        Serial.println(command);

        // Send command to all ESP32s
        sendUDP(command, esp32_1);
        sendUDP(command, esp32_2);
        sendUDP(command, esp32_3);
        
        Serial.println("Command sent to all ESP32 bots.");
    }
}

// Function to send UDP packets
void sendUDP(String command, IPAddress botIP) {
    udp.beginPacket(botIP, udpPort);
    udp.print(command);
    udp.endPacket();
}
