
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>
#include <DHT.h>


// NeoPixel setup
#define PIXEL_PIN       A2 
#define NUMPIXELS       12
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);



// Motor setup
const int motorLfwd = 7;
const int motorLbck = 8;
const int motorLen  = 11; //left motor enable pin 
const int motorRfwd = 9;
const int motorRbck = 12;
const int motorRen = 10; //right motor enable pin

// Control variables
int joyL = 0; 
int joyR = 0;
int joyLneutral = 0;
int joyRneutral = 0;
const int deadzone = 20;

void setup() {
  pinMode(motorLfwd, OUTPUT);
  pinMode(motorLbck, OUTPUT);
  pinMode(motorLen, OUTPUT);

  pinMode(motorRfwd, OUTPUT);
  pinMode(motorRbck, OUTPUT);
  pinMode(motorRen, OUTPUT);

  Serial.begin(9600);

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

  pixels.begin();
  pixels.setBrightness(20);



}

void loop() {
  // === Receive RS485 joystick data ===
   if (Serial.available() >= 4) {
    joyL = Serial.read() << 8;  // high byte
    joyL |= Serial.read();      // low byte
    joyR = Serial.read() << 8;  // high byte
    joyR |= Serial.read();      // low byte
  }

  // === Motor control (LEFT) ===
  int motorLspeed = 0;
  if ((joyL - joyLneutral) < -deadzone) { // Forward
    digitalWrite(motorLfwd, HIGH);
    digitalWrite(motorLbck, LOW);
    motorLspeed = map(constrain(joyL, 0, joyLneutral - deadzone), 0, joyLneutral - deadzone, 255, 0);
    for (int i = 0; i < 6; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green
    }
  } else if ((joyL - joyLneutral) > deadzone) { // Reverse
    digitalWrite(motorLfwd, LOW);
    digitalWrite(motorLbck, HIGH);
    motorLspeed = map(constrain(joyL, joyLneutral + deadzone, 1023), joyLneutral + deadzone, 1023, 0, 255);
    for (int i = 0; i < 6; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red
    }
  } else {
    digitalWrite(motorLfwd, LOW);
    digitalWrite(motorLbck, LOW);
    motorLspeed = 0;
    for (int i = 0; i < 6; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Off
    }
  }

  // === Motor control (RIGHT) ===
  int motorRspeed = 0;
  if ((joyR - joyRneutral) < -deadzone) { // Forward
    digitalWrite(motorRfwd, HIGH);
    digitalWrite(motorRbck, LOW);
    motorRspeed = map(constrain(joyR, 0, joyRneutral - deadzone), 0, joyRneutral - deadzone, 255, 0);
    for (int i = 6; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green
    }
  } else if ((joyR - joyRneutral) > deadzone) { // Reverse
    digitalWrite(motorRfwd, LOW);
    digitalWrite(motorRbck, HIGH);
    motorRspeed = map(constrain(joyR, joyRneutral + deadzone, 1023), joyRneutral + deadzone, 1023, 0, 255);
    for (int i = 6; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red
    }
  } else {
    digitalWrite(motorRfwd, LOW);
    digitalWrite(motorRbck, LOW);
    motorRspeed = 0;
    for (int i = 6; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Off
    }
  }

  analogWrite(motorLen, motorLspeed);
  analogWrite(motorRen, motorRspeed);
  pixels.show();

  //delay(50);


  delay(1000);
}

