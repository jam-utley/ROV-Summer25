#include <Wire.h>
#include <LiquidCrystal.h>

// === Joystick Pins ===
#define joyL_pin A0
#define joyR_pin A1

// === RS485 control pins === //DE and RE tied together
#define RS485_Dir D8


// LCD setup
const int rs = 13, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


int joyL = 512;
int joyR = 512;
int prevJoyL = 512;
int prevJoyR = 512;

const int changeThreshold = 4; // minimal change before sending

void setup() {
  pinMode(RS485_Dir, OUTPUT);
  digitalWrite(RS485_Dir, LOW); // start in receive mode


  Serial.begin(9600);
  lcd.begin(16,2);

  //Reading starting joystick values for aid in calibration
  joyL = analogRead(joyL_pin);
  joyR = analogRead(joyR_pin);

  prevJoyL = joyL;
  prevJoyR = joyR;

  sendJoystick(joyL, joyR);

 
}

void loop() {
  // === Read joysticks ===
  joyL = analogRead(joyL_pin);
  joyR = analogRead(joyR_pin);

  // === Send only if movement exceeds threshold ===
  if (abs(joyL - prevJoyL) >= changeThreshold || abs(joyR - prevJoyR) >= changeThreshold) {
    sendJoystick(joyL, joyR);
    prevJoyL = joyL;
    prevJoyR = joyR;
  }

  // === Receive humidity/temp from Uno ===
  if (Serial.available() >= 5) { // header + 2 bytes hum + 2 bytes temp
    if (Serial.read() == 0xFF) { // packet sync
      int hum = (Serial.read() << 8) | Serial.read();
      int temp = (Serial.read() << 8) | Serial.read();
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("H:"); lcd.print(hum * 0.1, 1); lcd.print("% ");
      lcd.setCursor(0, 1);
      lcd.print("T:"); lcd.print(temp * 0.1, 1); lcd.print("C ");
    }
  }

  delay(20); // ~50 updates/sec
}

// Send joystick data to Uno 
void sendJoystick(int joyL, int joyR) {
  // Switch RS485 to transmit
  digitalWrite(RS485_Dir, HIGH);

  Serial.write(0xAA);                 // packet header
  Serial.write(highByte(joyL));       // joyL high byte
  Serial.write(lowByte(joyL));        // joyL low byte
  Serial.write(highByte(joyR));       // joyR high byte
  Serial.write(lowByte(joyR));        // joyR low byte

  Serial.flush(); // wait until data is sent

  // Back to receive mode
  digitalWrite(RS485_Dir, LOW);
}
