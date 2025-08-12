#include <Adafruit_NeoPixel.h>
#include <DHT.h>

// === RS485 Setup ===
// Change pins if needed
#define RS485_Dir 2

// === NeoPixel setup ===
#define PIXEL_PIN  A2
#define NUMPIXELS  12
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// === Motor setup ===
const int motorLfwd = 7;
const int motorLbck = 8;
const int motorLen  = 11; // left motor enable pin (PWM)
const int motorRfwd = 9;
const int motorRbck = 12;
const int motorRen  = 10; // right motor enable pin (PWM)

// === Control variables ===
int joyL = 0;
int joyR = 0;
int joyLneutral = 512;
int joyRneutral = 512;
const int deadzone = 20;

// === DHT11 Setup ===
#define DHTPIN A3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === Timers ===
unsigned long lastSensorSend = 0;

void setup() {
  // Motor pins
  pinMode(motorLfwd, OUTPUT);
  pinMode(motorLbck, OUTPUT);
  pinMode(motorLen, OUTPUT);
  pinMode(motorRfwd, OUTPUT);
  pinMode(motorRbck, OUTPUT);
  pinMode(motorRen, OUTPUT);

  // RS485 control pins
  pinMode(RS485_Dir, OUTPUT);
  digitalWrite(RS485_Dir, LOW); // start in receive mode

  Serial.begin(9600);
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(20);

  dht.begin();

  // === Calibrate joystick neutral ===
  bool calibrated = false;
  while (!calibrated) {
    if (Serial.available() >= 5) {
      if (Serial.read() == 0xAA) { // joystick packet header
        joyL = (Serial.read() << 8) | Serial.read();
        joyR = (Serial.read() << 8) | Serial.read();
        joyLneutral = joyL;
        joyRneutral = joyR;
        calibrated = true;
      }
    }
  }
}

void loop() {
  // === Receive joystick packet from Nano ===
  if (Serial.available() >= 5) {
    if (Serial.read() == 0xAA) {
      joyL = (Serial.read() << 8) | Serial.read();
      joyR = (Serial.read() << 8) | Serial.read();
    }
  }

  // === Motor control (LEFT) ===
  int motorLspeed = 0;
  if ((joyL - joyLneutral) < -deadzone) { // Forward
    digitalWrite(motorLfwd, HIGH);
    digitalWrite(motorLbck, LOW);
    motorLspeed = map(constrain(joyL, 0, joyLneutral - deadzone), 0, joyLneutral - deadzone, 255, 0);
    for (int i = 0; i < 6; i++) pixels.setPixelColor(i, pixels.Color(0, 255, 0));
  } else if ((joyL - joyLneutral) > deadzone) { // Reverse
    digitalWrite(motorLfwd, LOW);
    digitalWrite(motorLbck, HIGH);
    motorLspeed = map(constrain(joyL, joyLneutral + deadzone, 1023), joyLneutral + deadzone, 1023, 0, 255);
    for (int i = 0; i < 6; i++) pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  } else { // Stop
    digitalWrite(motorLfwd, LOW);
    digitalWrite(motorLbck, LOW);
    motorLspeed = 0;
    for (int i = 0; i < 6; i++) pixels.setPixelColor(i, 0);
  }

  // === Motor control (RIGHT) ===
  int motorRspeed = 0;
  if ((joyR - joyRneutral) < -deadzone) { // Forward
    digitalWrite(motorRfwd, HIGH);
    digitalWrite(motorRbck, LOW);
    motorRspeed = map(constrain(joyR, 0, joyRneutral - deadzone), 0, joyRneutral - deadzone, 255, 0);
    for (int i = 6; i < NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(0, 255, 0));
  } else if ((joyR - joyRneutral) > deadzone) { // Reverse
    digitalWrite(motorRfwd, LOW);
    digitalWrite(motorRbck, HIGH);
    motorRspeed = map(constrain(joyR, joyRneutral + deadzone, 1023), joyRneutral + deadzone, 1023, 0, 255);
    for (int i = 6; i < NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  } else { // Stop
    digitalWrite(motorRfwd, LOW);
    digitalWrite(motorRbck, LOW);
    motorRspeed = 0;
    for (int i = 6; i < NUMPIXELS; i++) pixels.setPixelColor(i, 0);
  }

  analogWrite(motorLen, motorLspeed);
  analogWrite(motorRen, motorRspeed);
  pixels.show();

  // === Send humidity/temp to Nano every 1s ===
  if (millis() - lastSensorSend >= 1000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int humTenths = (int)(h * 10);
    int tempTenths = (int)(t * 10);
    sendHumidityTemp(humTenths, tempTenths);
    lastSensorSend = millis();
  }

  delay(20); // ~50 updates per second
}

// === Send humidity/temp to Nano ===
void sendHumidityTemp(int humTenths, int tempTenths) {
  // Switch RS485 to transmit
  digitalWrite(RS485_Dir, HIGH);

  Serial.write(0xFF);                 // packet header
  Serial.write(highByte(humTenths));  // humidity high byte
  Serial.write(lowByte(humTenths));   // humidity low byte
  Serial.write(highByte(tempTenths)); // temp high byte
  Serial.write(lowByte(tempTenths));  // temp low byte

  Serial.flush(); // wait until data is sent

  // Back to receive mode
  digitalWrite(RS485_Dir, LOW);
}
