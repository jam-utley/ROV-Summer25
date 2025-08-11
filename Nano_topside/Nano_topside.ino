

//VRx not used on joysticks

int joyL;
int joyR;

void setup() {
  Serial.begin(9600);
}

void loop() {
  joyL = analogRead(A1); //connect Vry to A0 on nano -reading left joystick
  joyR = analogRead(A0);

    // Send as two bytes (MSB first)
  //Serial.write(highByte(joyL));  // high byte
  Serial.println("");
  Serial.print("Left: ");
  Serial.print(joyL);
  Serial.print(", Right: ");
  Serial.print(joyR);

  //Serial.write(lowByte(joyL));   // low byte

  // int joyR = analogRead(A1); //connecty Vry to A1 on nano -reading right joystick

  //   // Send as two bytes (MSB first)
  // Serial.write(highByte(joyR));  // high byte
  // Serial.write(lowByte(joyR));   // low byte


  delay(500);
  
}