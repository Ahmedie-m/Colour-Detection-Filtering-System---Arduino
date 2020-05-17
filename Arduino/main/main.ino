// necessary libraries
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_TCS34725.h>
#include <Process.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

// initializing library variables
BridgeServer server;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// initial Positions for all
const int initPos =  0;

// positions for new orange
const int openPos = 90;

// positions for outcome boxes
const int unknownPos = 0;
const int ripePos =  90;
const int rawPos =  180;

// ThingSpeak parameters
int ripeTotal = 0;
int rawTotal = 0;
String postApiKey = "EIVSNOCRGC5SOS5Q";

// counters
int noColorDetectedCheck = 0;
int noFruitDetectedCheck = 0;

// pins
int ultrasonicPins[2][2] {
  {7, 8}, // Fruit Detection // trig, echo
  {9, 10}, // Slider Detection // trig, echo
};
int orangeFilterPin = 5;
int orangeFatePin = 11;

// initialize
Servo orangeFilter;
Servo orangeFate;

// on/off boolean
boolean isMachineOn = true;
// checks if an orange is in the detection room
boolean isOrangeInDetection = false;

void setup() {
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
  Serial.begin(9600);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor!");
  } else {
    Serial.println("TCS34725 NOT Found! Check your connections...");
    while(1); // halt!
  }

  // Servo 1 is connected to PWM pin 5
  orangeFilter.attach(orangeFilterPin);
  // drive servo in basic
  orangeFilter.write(0);
  // Servo 2 is connected to PWM pin 11
  orangeFate.attach(orangeFatePin);
  // drive servo in basic
  orangeFate.write(0);
  delay(1000);
}

void loop() {
  int fruitDetection = 8;
  tcs.setInterrupt(true); // turn off LED when not used
  BridgeClient client = server.accept();

  if (client) {
    process(client);
  }
  
  if (isMachineOn) {
    if (!isOrangeInDetection) {
      while (fruitDetection >= 4) {
        fruitDetection = ultrasonicDetect(1);
        noFruitDetectedCheck++;
        Serial.print("MACHINE: No fruit in detection area - Check #");
        Serial.println(noFruitDetectedCheck);
        delay(500);
        if(noFruitDetectedCheck >= 3) {
          noFruitDetectedCheck = 0;
          newOrange();
        }
      }
      noFruitDetectedCheck = 0;
      isOrangeInDetection = true;
    }
    
    if (isOrangeInDetection) {
      float red, green, blue;
      
      tcs.setInterrupt(false);  // turn on LED
    
      delay(60);  // takes 60ms to read
    
      tcs.getRGB(&red, &green, &blue);
    
      tcs.setInterrupt(true);  // turn off LED
      
      Serial.print("\tR:\t"); Serial.print(int(red));
      Serial.print("\tG:\t"); Serial.print(int(green));
      Serial.print("\tB:\t"); Serial.print(int(blue));
      Serial.println("");
  
      int orangeStatus = detectOrange(red, green, blue);
    
       switch(orangeStatus){
        case 1:
          Serial.println("\tOUTPUT: ORANGE (RIPE)\t");
          orangeSlider(orangeStatus);
        break;
        case 0:
          Serial.println("\tOUTPUT: GREEN (RAW)\t");
          orangeSlider(orangeStatus);
        break;
        default:
          Serial.print("\tOUTPUT: NOT DETECTED\t");
          Serial.print("Check #");
          Serial.println(noColorDetectedCheck);
          if(noColorDetectedCheck >= 3) {
            noColorDetectedCheck = 0;
            orangeSlider(orangeStatus);
          }
          delay(1500);
        break;
      }
    }
  }
  else {
    Serial.println("Machine turned off");
    delay(300);
  }
}

float detectOrange(float red, float green, float blue){
  if(( red >  130 )  &&  ( green <  100 )  &&  ( blue <  60 )){
    noColorDetectedCheck = 0;
    ripeTotal++;
    // postToThingSpeak(ripeTotal, 1);
    updateArduinoDatabase("ripe", red, green, blue);
    return 1; // ripe
  }
  if(( red < 100 )  &&  ( green >  100 )  &&  ( blue <  100 )){
    noColorDetectedCheck = 0;
    rawTotal++;
    // postToThingSpeak(rawTotal, 2);
    updateArduinoDatabase("raw", red, green, blue);
    return 0; // raw
  }
  else{
    noColorDetectedCheck++;
    return -1; // needs another check
  }
}

void process(BridgeClient client) {
  // If there is a request, then read the URL command
  String command = client.readStringUntil('/'); //Read in the string up to the first forward dash "/".
  command.trim();        //kill whitespace
  if (command == "statusfilter") {
    isMachineOn = client.parseInt();
    if (isMachineOn == 1) {
      isMachineOn = true;
    }
    if (isMachineOn == 0) {
      isMachineOn = false;
    }
  }
  
  // Close connection and free resources.
  client.stop();
  client.flush();
}

int newOrange(){
  isOrangeInDetection = false;
  Serial.println("MACHINE: Letting in a new orange...");
  orangeFilter.write(openPos);  // No detection rotate 90 to let an Orange in (after 3 checks);
  delay(1000);
  orangeFilter.write(initPos);
  delay(200); // give time for the sensor to pick up the orange in the detection room
}

int orangeSlider(int orangeStatus){
  int sliderLocation; 
  if(orangeStatus == 1){
    Serial.println("MACHINE: Moving to ripe box...");
    orangeFate.write(ripePos);    // Rotate 90 to let the Orange in Ripe Box
    while (sliderLocation > 55) {
      sliderLocation = ultrasonicDetect(1);
      Serial.print("The slider is ");
      Serial.println(sliderLocation);
    }
    Serial.println("MACHINE: Moving back to initial position...");
    orangeFate.write(initPos);  // Go to initial Position
  }
  if(orangeStatus == -1){
    Serial.println("MACHINE: Moving to unknown box...");
    orangeFate.write(unknownPos);    // Rotate 180 to let the Orange in Raw Box
    while (sliderLocation > 35) {
      sliderLocation = ultrasonicDetect(1);
      Serial.print("The slider is ");
      Serial.println(sliderLocation);
    }              
    Serial.println("MACHINE: Moving back to initial position...");
    orangeFate.write(initPos); // Go to initial Position
  }
  if(orangeStatus == 0){
    Serial.println("MACHINE: Moving to raw box...");
    orangeFate.write(rawPos);    // Rotate 180 to let the Orange in Raw Box
    while (sliderLocation > 15) {
      sliderLocation = ultrasonicDetect(1);
      Serial.print("The slider is ");
      Serial.println(sliderLocation);
    }              
    Serial.println("MACHINE: Moving back to initial position...");
    orangeFate.write(initPos); // Go to initial Position
  }
  delay(1000);
  newOrange();
}

/*
void postToThingSpeak(int value, int chartNum) {
  Process p;
  String cmd = "curl -d 'key=";
  cmd = cmd + postApiKey;
  cmd = cmd + "&field";
  cmd = cmd + chartNum;
  cmd = cmd + "=";
  cmd = cmd + value;
  cmd = cmd + "' -k http://api.thingspeak.com/update";
  Serial.println("MACHINE: Statistics are now being sent to the server...");
  p.runShellCommand(cmd);

  // do nothing until the process finishes, so you get the whole output:
  while(p.running());
  delay(5000); // we need to wait 15 seconds with the free plan before each request
}*/

void updateArduinoDatabase(String type, int red, int green, int blue) {
  Process p;              
  p.begin("/mnt/sda1/sensor.php");      
  p.addParameter(String(type));
  p.addParameter(String(red));
  p.addParameter(String(green));
  p.addParameter(String(blue));
  p.run();
}

int ultrasonicDetect(int pins) {
  long duration;
  int trigPin, echoPin, cm;

  switch(pins){
    case 1:
    trigPin = ultrasonicPins[1][0];
    echoPin = ultrasonicPins[1][1];
    break;
    case 0:
    trigPin = ultrasonicPins[0][0];
    echoPin = ultrasonicPins[0][1];
    break;
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration / 2) / 29.1;     // Divide by 29.1 or multiply by 0.0343

  delay(500);

  return cm;
}

void addLogTitle (String) {


}
