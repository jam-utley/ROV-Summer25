#include <SoftwareSerial.h>

#define DE 2
#define RE 3

SoftwareSerial RS485Serial(10, 11); // TX, RX to Uno
int joyPin = A0;
int joyNeutral = 512;
const int deadzone = 20;

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

  Serial.begin(2400);      // for debugging
  RS485Serial.begin(2400); // RS485 TX
  joyNeutral = analogRead(joyPin); // calibrate neutral
}

void loop() {
  int joyVal = analogRead(joyPin);
  char direction = 0;

  if (joyVal < joyNeutral - deadzone) direction = 'F';
  else if (joyVal > joyNeutral + deadzone) direction = 'B';

  if (direction != 0) {
    uint8_t packet[2];
    packet[0] = direction;
    packet[1] = crc8(&packet[0], 1);

    digitalWrite(DE, HIGH);
    digitalWrite(RE, HIGH);
    RS485Serial.write(packet, 2);
    RS485Serial.flush();
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
  }

  delay(50);
}
