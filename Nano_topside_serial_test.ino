#include <SoftwareSerial.h>

#define DE 2
#define RE 3

SoftwareSerial RS485Serial(10, 11); // TX, RX to Uno

const int joyRight = A0;
const int joyLeft = A1;
int joyLNeutral = 512;
int joyRNeutral = 512;
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

  Serial.begin(2400);
  RS485Serial.begin(2400);

  delay(1000);
  joyLNeutral = analogRead(joyLeft);
  joyRNeutral = analogRead(joyRight);
}

void loop() {
  int joyLVal = analogRead(joyLeft);
  int joyRVal = analogRead(joyRight);
  // Serial.print(joyLVal);
  // Serial.print("  ");
  // Serial.println(joyRVal);

  char cmdL = 0;
  char cmdR = 0;

  // Left joystick
  if (joyLVal < joyLNeutral - deadzone) cmdL = 255; //makes motor go ful speed
  else if (joyLVal > joyLNeutral + deadzone) cmdL = 0; //makes motor stop
  //Serial.println(cmdL);

  // Right joystick
  if (joyRVal < joyRNeutral - deadzone) cmdR = 255;
  else if (joyRVal > joyRNeutral + deadzone) cmdR = 0;

  // Only send if at least one joystick is outside deadzone
  if (cmdL != 0 || cmdR != 0) {
    uint8_t packet[3];

    // Use 'L'/'R' as first byte and joystick direction as second
    // If a joystick is neutral, send 0
    packet[0] = cmdL ? cmdL : 0;
    packet[1] = cmdR ? cmdR : 0;
    packet[2] = crc8(packet, 2);

    digitalWrite(DE, HIGH);
    digitalWrite(RE, HIGH);
    RS485Serial.write(packet, 3);
    //RS485Serial.flush();
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    Serial.print("Sent Left: ");
    Serial.print(packet[0]);
    Serial.print(" Right: ");
    Serial.println(packet[1]);
  }
  else{
    uint8_t packet[3];

    // Use 'L'/'R' as first byte and joystick direction as second
    // If a joystick is neutral, send 0
    packet[0] = 0;
    packet[1] = 0;
    packet[2] = crc8(packet, 2);

    digitalWrite(DE, HIGH);
    digitalWrite(RE, HIGH);
    RS485Serial.write(packet, 3);
    RS485Serial.flush();
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    Serial.print("Sent Left: ");
    Serial.print(packet[0]);
    Serial.print(" Right: ");
    Serial.println(packet[1]);
  }

  delay(10);
}