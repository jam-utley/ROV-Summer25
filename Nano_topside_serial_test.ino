#include <SoftwareSerial.h>

#define DE 2
#define RE 3

SoftwareSerial RS485Serial(10, 11); // TX, RX to Uno
int joyRight = A0;
int joyLeft = A1;
int joyNeutralRight = 512;
int joyNeutralLeft = 512;
const int deadzone = 200;

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
  joyNeutralRight = analogRead(joyRight); // calibrate neutral
  joyNeutralLeft = analogRead(joyLeft);
}

void loop() {
  int joyValRight = analogRead(joyRight);
  int joyValLeft = analogRead(joyLeft);
  char directionR = 0;
  char directionL = 0;
  //assess directionality of right joystick
  if (joyValRight < joyNeutralRight - deadzone) directionR = 'RF';
  else if (joyValRight > joyNeutralRight + deadzone) directionR = 'RB';
  //assess directionality of left joystick
  if (joyValLeft < joyNeutralLeft - deadzone) directionL = 'LF';
  else if (joyValLeft > joyNeutralLeft + deadzone) directionL = 'LB';
  //Serial.println(direction);

  if (directionR != 0 || directionL != 0) {
    Serial.print("Right: ");
    Serial.print(joyValRight);
    Serial.print(" , Left:");
    Serial.println(joyValLeft);
    
    uint8_t packet[3];
    packet[0] = directionR;
    packet[1] = directionL;
    packet[2] = crc8(&packet[0], 1);

    digitalWrite(DE, HIGH);
    digitalWrite(RE, HIGH);
    RS485Serial.write(packet, 3);
    RS485Serial.flush();
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
  }

  delay(50);
}
