#include <SoftwareSerial.h>

#define DE 2
#define RE 3

SoftwareSerial RS485Serial(10, 11); // RX from Nano

// CRC8 function
uint8_t crc8(const uint8_t *data, uint8_t len) {
  uint8_t crc = 0;
  while (len--) {
    uint8_t extract = *data++;
    for (uint8_t i = 8; i; i--) {
      uint8_t sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) crc ^= 0x8C;
      extract >>= 1;
    }
  }
  return crc;
}

void setup() {
  pinMode(DE, OUTPUT);
  pinMode(RE, OUTPUT);
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  Serial.begin(2400);      // print Forward/Backward
  RS485Serial.begin(2400); // receive from Nano
}

void loop() {
  if (RS485Serial.available() >= 2) {
    uint8_t packet[2];
    RS485Serial.readBytes(packet, 2);

    if (crc8(&packet[0], 1) == packet[1]) {
      if (packet[0] == 'F') Serial.println("Forward");
      else if (packet[0] == 'B') Serial.println("Backward");
    }
  }
}
