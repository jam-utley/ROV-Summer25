

//VRx not used on joysticks

void setup() {
  Serial.begin(9600);
}

void loop() {
  int joyL = analogRead(A0); //connect Vry to A0 on nano -reading left joystick

    // Send as two bytes (MSB first)
  Serial.write(highByte(joyL));  // high byte
  Serial.write(lowByte(joyL));   // low byte

  int joyR = analogRead(A1); //connecty Vry to A1 on nano -reading right joystick

    // Send as two bytes (MSB first)
  Serial.write(highByte(joyR));  // high byte
  Serial.write(lowByte(joyR));   // low byte


  delay(50);
  
}