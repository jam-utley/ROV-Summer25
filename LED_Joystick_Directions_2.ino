#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        6
#define NUMPIXELS 16

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

#define VRX_PIN  A1 // Arduino pin connected to VRX pin
#define VRY_PIN  A0 // Arduino pin connected to VRY pin

//int xValue = analogRead(A1);
//int yValue = analogRead(A0);
//Init joystick readings
int xValue; // To store value of the X axis
int yValue; // To store value of the Y axis
double DirVec = 0; //Stores direction vector. Initialized at 0
int originX; //Stores the origin location x and y values
int originY;
int rangeHigh = 2; //Parameters for tuning joystick sensativity
int rangeLow = -2;
double theta = 0; 
double xTrue;
double yTrue;

//150 30 6
int Color_Red = 0;     //Colors for
int Color_Blue = 150;
int Color_Green = 150;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  pixels.setBrightness(20);

  Serial.begin(9600) ;

  originX = analogRead(VRX_PIN); //The initial reading of X and Y. Used for recentering the 2d plane.
  originY = analogRead(VRY_PIN);
}

void loop() {
  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  xTrue = xValue - originX;
  yTrue = yValue - originY;
  yTrue = yTrue * (-1);  //You may be wondering why this line is here. 
  //In truth it is because the sensor's y axis has negative going up instead of down
  //This adjustment readjusts the value of yTrue to better align it
  //with more standard cartesian systems

  DirVec = sqrt(abs(sq(xTrue) - sq(yTrue)));     //Computes the magnitude of the vector.
  theta = atan((yTrue)/(xTrue));    
  //Computes the angle from the x axis. For best results, hold joystick with terminals pointed towards x =-inf
  
  
  print_Values();   //Provides serial output.

  if (xTrue > rangeHigh && yTrue > rangeLow && yTrue < rangeHigh){
    //0 Pi
    pixels.setPixelColor(8, pixels.Color(150, 0, 0));
  }
  if (xTrue > rangeHigh && yTrue > rangeHigh){
    //Quadrant I
    if (theta < 0.52){     //0.52 is 1.57/3 or 2pi/3
      pixels.setPixelColor(9, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta > 0.52 && theta < 1.05){
      pixels.setPixelColor(10, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta > 1.05){
      pixels.setPixelColor(11, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
  }
  if (yTrue > rangeHigh && xTrue > rangeLow && xTrue < rangeHigh){
    // pi/2
    pixels.setPixelColor(12, pixels.Color(150, 0, 0));
  }
  if (yTrue > rangeHigh && rangeLow > xTrue){
    //Quadrant II
    if (theta < -1.05){     //0.52 is 1.57/3 or 2pi/3
      pixels.setPixelColor(13, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta < -0.52 && theta > -1.05){
      pixels.setPixelColor(14, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta > -0.52){
      pixels.setPixelColor(15, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
  }
  if (xTrue < rangeLow && yTrue > rangeLow && yTrue < rangeHigh){
    // pi
    pixels.setPixelColor(0, pixels.Color(150, 0, 0));
  }
  if (xTrue < rangeLow && yTrue < rangeLow){
    //Quadrant III
    if (theta < 0.52){     //0.52 is 1.57/3 or 2pi/3
      pixels.setPixelColor(1, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta > 0.52 && theta < 1.05){
      pixels.setPixelColor(2, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta > 1.05){
      pixels.setPixelColor(3, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
  }
  if (yTrue < rangeLow && xTrue > rangeLow && xTrue < rangeHigh){
    // 3pi/2
    pixels.setPixelColor(4, pixels.Color(150, 0, 0));
  }
  if (xTrue > rangeHigh && rangeLow > yTrue){
    //Quadrant IV
    if (theta < -1.05){     //0.52 is 1.57/3 or 2pi/3
      pixels.setPixelColor(5, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta < -0.52 && theta > -1.05){
      pixels.setPixelColor(6, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
    if (theta > -0.52){
      pixels.setPixelColor(7, pixels.Color(Color_Red, Color_Green, Color_Blue));
    }
  }
  


  pixels.show();
  delay(100);
  pixels.clear();

}


void print_Values(){
  Serial.print("x = ");
  Serial.print(xValue);
  Serial.print(", y = ");
  Serial.print(yValue);
  Serial.print(", xTrue = ");
  Serial.print(xTrue);
  Serial.print(", yTrue = ");
  Serial.print(yTrue);
  Serial.print(", V = ");
  Serial.print(DirVec);
  Serial.print(", Theta = ");
  Serial.println(theta);
}