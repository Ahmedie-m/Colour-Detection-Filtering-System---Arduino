#include <Wire.h>
#include "Servo.h"
#include "Adafruit_TCS34725.h"

const int openPos = 90;
const int ripePos =  90;
const int rawPos =  180;
const int initPos =  0;
int ripeTotal = 0;
int rawTotal = 0;
int notDetectedCheck = 0;

// initialize
Servo orangeFilter;
Servo orangeFate;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

float detectOrange(float red, float green, float blue){
  if(( red >  130 )  &&  ( green <  100 )  &&  ( blue <  60 )){
    notDetectedCheck = 0;
    ripeTotal++;
    return 1; // ripe
  }
  if(( red < 100 )  &&  ( green >  100 )  &&  ( blue <  100 )){
    notDetectedCheck = 0;
    rawTotal++;
    return 0; // raw
  }
  else{
    notDetectedCheck++;
    return -1; // needs another check
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor!");
  } else {
    Serial.println("TCS34725 NOT Found! Check your connections...");
    while(1); // halt!
  }

  // Servo 1 is connected to PWM pin 3
  orangeFilter.attach(3);
  // drive servo in basic
  orangeFilter.write(0);
  // Servo 2 is connected to PWM pin 5
  orangeFate.attach(5);
  // drive servo in basic
  orangeFate.write(0);
  delay(1000);
}

void loop() {
  
  float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED

  delay(60);  // takes 60ms to read

  tcs.getRGB(&red, &green, &blue);

  tcs.setInterrupt(true);  // turn off LED

  ledControl();
  
  Serial.print("\tR:\t"); Serial.print(int(red));
  Serial.print("\tG:\t"); Serial.print(int(green));
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.println("");

  int orangeStatus = detectOrange(red, green, blue);

   switch(orangeStatus){
    case 1:
      Serial.print("\tOUTPUT: ORANGE (RIPE)\t");
      orangeSlider(orangeStatus);
      Serial.println("");
    break;
    case 0:
      Serial.print("\tOUTPUT: GREEN (RAW)\t");
      orangeSlider(orangeStatus);
      Serial.println("");
    break;
    default:
      Serial.print("\tOUTPUT: NOT DETECTED\t");
      Serial.print("Check #");
      Serial.print(notDetectedCheck);
      Serial.println("");
      if(notDetectedCheck >= 3) {
        notDetectedCheck = 0;
        newOrange();
        delay(4000);
        Serial.println("");
      }
      delay(1500);
    break;
  }
}

int ledControl(){
  if(Serial.available() > 0 ){
    int com = Serial.read();
    Serial.println(com - 48); // Arduino uses ACSII numbering, we need to minus 48
    if (com == 0) {
      tcs.setInterrupt(true);  // turn off LED
    }
    if (com == 1) {
      tcs.setInterrupt(false);  // turn on LED
    }
  }
}

int newOrange(){
  Serial.print("MACHINE: Letting in a new orange...");
  Serial.println("");
  orangeFilter.write(openPos);  // No detection rotate 90 to let an Orange in (after 3 checks);
  delay(1000);
  orangeFilter.write(initPos);
}

int orangeSlider(int orangeStatus){
  if(orangeStatus == 1){
    Serial.print("MACHINE: Moving to ripe box...");
    Serial.println("");
    orangeFate.write(ripePos);    // Rotate 90 to let the Orange in Ripe Box
    delay(5000);                 // Wait 5 sec
    Serial.print("MACHINE: Moving back to initial position...");
    Serial.println("");
    orangeFate.write(initPos);  // Go to initial Position
  }
  if(orangeStatus == 0){
    Serial.print("MACHINE: Moving to raw box...");
    Serial.println("");
    orangeFate.write(rawPos);    // Rotate 180 to let the Orange in Raw Box
    delay(5000);                // Wait 5 sec
    Serial.print("MACHINE: Moving back to initial position...");
    Serial.println("");
    orangeFate.write(initPos); // Go to initial Position
  }
  delay(1000);
  newOrange();
}
