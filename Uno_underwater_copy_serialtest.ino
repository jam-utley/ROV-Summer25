#include <SoftwareSerial.h>

#define DE 2
#define RE 3

SoftwareSerial RS485Serial(10, 11); // RX from Nano

// Motor setup
const int motorLfwd = 7;
const int motorLbck = 8;
const int motorLen  = 4; //left motor enable pin 
const int motorRfwd = 9;
const int motorRbck = 12;
const int motorRen = 5; //right motor enable pin

// Control variables
int joyL = 0; 
int joyR = 0;
int joyLneutral = 0;
int joyRneutral = 0;
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

  pinMode(motorLfwd, OUTPUT);
  pinMode(motorLbck, OUTPUT);
  pinMode(motorLen, OUTPUT);

  pinMode(motorRfwd, OUTPUT);
  pinMode(motorRbck, OUTPUT);
  pinMode(motorRen, OUTPUT);

  Serial.begin(2400);      // print Forward/Backward
  RS485Serial.begin(2400); // receive from Nano

   // === Calibrate joystick neutral positions ===
  delay(1000); // give time for signal to stabilize along the tether
  if (Serial.available() >= 4) {
    joyL = Serial.read() << 8;
    joyL |= Serial.read();
    joyR = Serial.read() << 8;
    joyR |= Serial.read();

    joyLneutral = joyL;
    joyRneutral = joyR;
} else {
  // fallback if nothing is received
  joyLneutral = 512;
  joyRneutral = 512;
}
delay(2000);
}

void loop() {
  if (RS485Serial.available() >= 3) {
    uint8_t packet[3];
    RS485Serial.readBytes(packet, 3);

    // Check CRC over first 2 bytes
    if (crc8(packet, 2) == packet[2]) {

      // LEFT MOTOR
      if (packet[0] == 'L') {           // Forward
        digitalWrite(motorLfwd, HIGH);
        digitalWrite(motorLbck, LOW);
      } else if (packet[0] == 'B') {    // Backward
        digitalWrite(motorLfwd, LOW);
        digitalWrite(motorLbck, HIGH);
      } else {                          // Neutral
        digitalWrite(motorLfwd, LOW);
        digitalWrite(motorLbck, LOW);
      }

      // RIGHT MOTOR
      if (packet[1] == 'F') {           // Forward
        digitalWrite(motorRfwd, HIGH);
        digitalWrite(motorRbck, LOW);
      } else if (packet[1] == 'B') {    // Backward
        digitalWrite(motorRfwd, LOW);
        digitalWrite(motorRbck, HIGH);
      } else {                          // Neutral
        digitalWrite(motorRfwd, LOW);
        digitalWrite(motorRbck, LOW);
      }

      // Debug print
      Serial.print("Left: "); Serial.print(packet[0]);
      Serial.print("  Right: "); Serial.println(packet[1]);

    } else {
      Serial.println("CRC error");
    }
  }
}


