#include <SoftwareSerial.h>

#define DE 2
#define RE 3

SoftwareSerial RS485Serial(10, 11);

int joyL = 512;
int joyLneutral = 512;
const int deadzone = 20;

// CRC8 function
uint8_t crc8(const uint8_t *data, uint8_t len) {
  uint8_t crc = 0;
  while (len--) {
    uint8_t extract = *data++;
    for (uint8_t tempI = 8; tempI; tempI--) {
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
  digitalWrite(DE, LOW); // RX mode
  digitalWrite(RE, LOW);

  Serial.begin(2400);
  RS485Serial.begin(2400);

  // Initial calibration
  bool calibrated = false;
  while (!calibrated) {
    if (Serial.available() >= 6) {
      uint8_t packet[6];
      Serial.readBytes(packet, 6);
      if (packet[0] == 0xAA && crc8(packet, 5) == packet[5]) {
        joyL = (packet[3] << 8) | packet[4];
        joyLneutral = joyL;
        calibrated = true;
      }
    }
  }
}

void loop() {
  // Receive joystick packet
  if (Serial.available() >= 6) {
    uint8_t packet[6];
    Serial.readBytes(packet, 6);

    if (packet[0] == 0xAA && crc8(packet, 5) == packet[5]) {
      joyL = (packet[3] << 8) | packet[4];

      char direction = 0;
      if ((joyL - joyLneutral) < -deadzone) direction = 'F';
      else if ((joyL - joyLneutral) > deadzone) direction = 'B';

      if (direction != 0) {
        uint8_t sendPacket[2];
        sendPacket[0] = direction;
        sendPacket[1] = crc8(&sendPacket[0], 1);

        digitalWrite(DE, HIGH);
        digitalWrite(RE, HIGH);
        Serial.write(sendPacket, 2);
        Serial.println(direction);
        Serial.flush();
        digitalWrite(DE, LOW);
        digitalWrite(RE, LOW);
      }
    }
  }
}
