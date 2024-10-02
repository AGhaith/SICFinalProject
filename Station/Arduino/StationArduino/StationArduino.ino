#include <SPI.h>
#include <LoRa.h>

#define ss 10
#define rst 9
#define dio0 8

void setup() {
  Serial.begin(9600);

  while (!Serial);
  Serial.println("LoRa Two Way Communication");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
  Serial.println("LoRa Started");
}

void loop() {
    Serial.flush();  // Clear the serial buffer

  // Check for incoming LoRa packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    onReceive(packetSize);
  }

  // Read from Serial if available
  if (Serial.available()) {
    char input = Serial.read();  // Read one byte
    Serial.print(input);          // Print the byte to the Serial Monitor
    sendMessage(input);           // Send the character via LoRa
    sendMessage('.');            // Send a newline as a delimiter
  }
}

void sendMessage(char outgoing) {
  LoRa.beginPacket();
  LoRa.print(outgoing);          // Send the single character
  LoRa.endPacket();

}

void onReceive(int packetSize) {
  if (packetSize == 0) return;

  String LoRaData = LoRa.readStringUntil('\n');  // Read until newline
  Serial.println(LoRaData);  // Print received LoRa data
}
