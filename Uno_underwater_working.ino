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

  // Non-blocking variables
  uint8_t packet[3];
  int index = 0;
  int lastL = -1, lastR = -1;

   // === Calibrate joystick neutral positions ===
  //delay(100); // give time for signal to stabilize along the tether
//   if (Serial.available() >= 4) {
//     joyL = Serial.read() << 8;
//     joyL |= Serial.read();
//     joyR = Serial.read() << 8;
//     joyR |= Serial.read();

//     joyLneutral = joyL;
//     joyRneutral = joyR;
// } else {
//   // fallback if nothing is received
//   joyLneutral = 512;
//   joyRneutral = 512;
// }
// //delay(100);
}

// void loop() {
//   if (RS485Serial.available() >= 0) {
//     uint8_t packet[3];
//     RS485Serial.setTimeout(20);
//     RS485Serial.readBytes(packet, 3);
//     //for (int i = 0; i < 3; i++) {
//      // packet[i] = RS485Serial.read();

//    // }

//     // Check CRC over first 2 bytes
//     if (crc8(packet, 2) == packet[2]) {

//       // LEFT MOTOR
//       if (packet[0] == 255) {           // Forward
//         digitalWrite(motorLfwd, HIGH);
//         digitalWrite(motorLbck, LOW);
//         analogWrite(motorLen, packet[0]);  // full speed
//         Serial.println("left");
//       } else if  (packet[0] == 0) {                          // Neutral
//         digitalWrite(motorLfwd, LOW);
//         digitalWrite(motorLbck, LOW);
//         analogWrite(motorLen, 0);  // no speed
//         Serial.println("left neutral");
//       }

//       // RIGHT MOTOR
//       if (packet[1] == 255) {           // Forward
//         digitalWrite(motorRfwd, HIGH);
//         digitalWrite(motorRbck, LOW);
//         analogWrite(motorRen, packet[1]);  // full speed
//         Serial.println("testing");
//       } else if (packet[1] == 0){                          // Neutral
//         digitalWrite(motorRfwd, LOW);
//         digitalWrite(motorRbck, LOW);
//         analogWrite(motorRen, 0);  // no speed
//         Serial.println("right neutral"); 
//       }

//       // Debug print
//       Serial.print("Left: "); Serial.print(packet[0]);
//       Serial.print("  Right: "); Serial.println(packet[1]);

//     } else {
//       Serial.println("CRC error");
//     }
//   }
// }

enum RS485State {
    WAIT_BYTE_0,
    WAIT_BYTE_1,
    WAIT_CRC
};

RS485State state = WAIT_BYTE_0;
uint8_t packet[3];
int lastL = -1, lastR = -1;
unsigned long lastByteTime = 0;       // timestamp of last byte
const unsigned long PACKET_TIMEOUT = 50; // ms, adjust as needed

void loop() {
    unsigned long currentTime = millis();

    // Reset FSM if packet is taking too long
    if (state != WAIT_BYTE_0 && (currentTime - lastByteTime > PACKET_TIMEOUT)) {
        state = WAIT_BYTE_0;
    }

    while (RS485Serial.available()) {
        uint8_t incoming = RS485Serial.read();
        lastByteTime = currentTime; // update timestamp on every received byte

        switch (state) {
            case WAIT_BYTE_0:
                packet[0] = incoming;
                state = WAIT_BYTE_1;
                break;

            case WAIT_BYTE_1:
                packet[1] = incoming;
                state = WAIT_CRC;
                break;

            case WAIT_CRC:
                packet[2] = incoming;

                // Process packet if CRC matches
                if (crc8(packet, 2) == packet[2]) {

                    // LEFT MOTOR
                    if (packet[0] == 255) {
                        digitalWrite(motorLfwd, HIGH);
                        digitalWrite(motorLbck, LOW);
                        analogWrite(motorLen, 255);
                    } else if (packet[0] == 0) {
                        digitalWrite(motorLfwd, LOW);
                        digitalWrite(motorLbck, LOW);
                        analogWrite(motorLen, 0);
                    }

                    // RIGHT MOTOR
                    if (packet[1] == 255) {
                        digitalWrite(motorRfwd, HIGH);
                        digitalWrite(motorRbck, LOW);
                        analogWrite(motorRen, 255);
                    } else if (packet[1] == 0) {
                        digitalWrite(motorRfwd, LOW);
                        digitalWrite(motorRbck, LOW);
                        analogWrite(motorRen, 0);
                    }

                    // Debug print only if values changed
                     if (packet[0] != lastL || packet[1] != lastR) {
                        Serial.print("Left: "); Serial.print(packet[0]);
                        Serial.print("  Right: "); Serial.println(packet[1]);
                        lastL = packet[0];
                        lastR = packet[1];
                    }

                } else {
                    Serial.println("CRC error");
                }

                state = WAIT_BYTE_0; // reset FSM for next packet
                break;
        }
    }
}


