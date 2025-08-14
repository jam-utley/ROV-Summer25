#include <SoftwareSerial.h>
#define joyL_pin A0
//#define joyR_pin A1
#define DE 2
#define RE 3

SoftwareSerial RS485Serial(10, 11);

int joyL = 512;
//int joyR = 512;

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
}

void loop() {
  // Read joystick
  joyL = analogRead(joyL_pin);
  //joyR = analogRead(joyR_pin);

  // Prepare packet: header + joyL + joyR + CRC
  uint8_t packet[6];
  packet[0] = 0xAA;
  packet[1] = highByte(joyL);
  packet[2] = lowByte(joyL);
  //packet[3] = highByte(joyR);
  //packet[4] = lowByte(joyR);
  packet[5] = crc8(packet, 5);

  // Send to UNO
  digitalWrite(DE, HIGH); 
  digitalWrite(RE, HIGH);
  Serial.write(packet, 6);
  Serial.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // Receive direction from UNO (2 bytes: dir, CRC)
  if (Serial.available() >= 2) {
    uint8_t dir = Serial.read();
    uint8_t recvCRC = Serial.read();
    uint8_t calcCRC = crc8(&dir, 1);

    if (recvCRC == calcCRC) {
      if (dir == 'F') Serial.println("Forward");
      else if (dir == 'B') Serial.println("Backward");
    } else {
      Serial.println("CRC ERROR");
    }
  }

  delay(100);
}
