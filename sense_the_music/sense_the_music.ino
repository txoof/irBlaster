#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <elapsedMillis.h>    //measure elapsed time

//define the IR Sender
IRsendRaw mySender;

const int analogInPin = A0;   //Analog input pin - reads from amplifyer circuit
const int statusLight = 10;   //Digital output pin for indicator light

int sensorValueA = 0;         //Sensor reading now
int sensorValueB = 0;         //Previous sensor reading
int sensorDeltaA = 0;          //Change in sensor reading
int sensorDeltaB = 0;             //Record the last delta - useful for prefenting accidental triggering on start
int sensorThreshold = 50;    //Threshold in delta to trigger action - increase to make less sensitive to line noise
int stateChangeCount = 0;
long delayTime = 10000;         //Delay for no action before timing out and shutting off

bool powerStateA = false;
bool powerStateB = false;

elapsedMillis heartBeatTime;  //Timer for measuring hearbeat time
elapsedMillis turnOverTime;   //Timer for measuring the turnover between changes in sensor reading

//Define a data type - RAW_DATA_LEN
#define RAW_DATA_LEN 68
//power on signal
uint16_t powerOn[RAW_DATA_LEN]={
  8978, 4494, 522, 582, 550, 582, 570, 562, 
  518, 586, 546, 586, 578, 554, 526, 578, 
  554, 578, 574, 1662, 550, 1714, 518, 1718, 
  578, 1658, 554, 578, 574, 1662, 550, 1714, 
  522, 1714, 578, 554, 518, 1718, 578, 1658, 
  554, 1710, 522, 1714, 550, 582, 518, 586, 
  546, 586, 578, 1658, 554, 578, 542, 590, 
  522, 582, 550, 582, 550, 1686, 550, 1714, 
  526, 1710, 542, 1000};

void setup() {
  pinMode(statusLight, OUTPUT);
  Serial.begin(9600);
  delay(2000); while(!Serial); //delay for serial to come up
  // put your setup code here, to run once:
  
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValueA = analogRead(analogInPin);
  sensorDeltaA = abs(sensorValueA - sensorValueB);

  if (heartBeatTime % 100 and sensorDeltaA >= sensorThreshold) {
    Serial.print("Threshold: ");
    Serial.print(sensorThreshold);
    Serial.print("\t sensorDelta = ");
    Serial.println(sensorDeltaA);
  } else {
    if (heartBeatTime % 500 == 0) {
      Serial.print("Switching off in: ");
      Serial.println((delayTime - turnOverTime)/1000);
      Serial.print("sensorDelta = ");
      Serial.println(sensorDeltaA);
    }
  }

  //check if the data is over the threshold and switch on
  if ((sensorDeltaA >= sensorThreshold) and (sensorDeltaB != 0)) {
    turnOverTime = 0;
    powerStateA = true;
  } 
  
  sensorValueB = sensorValueA;
  sensorDeltaB = sensorDeltaA;

  //check if no sensor data beyond the threshold has been returned within the delay window, switch off
  if (turnOverTime >= delayTime) {
    turnOverTime = 0;
    powerStateA = false;    
  }

  //if the powerstate changes act acordingly
  if (powerStateA != powerStateB) {
    digitalWrite(statusLight, powerStateA);    
    powerStateB = powerStateA;
    Serial.print("switch power to: ");
    Serial.println(powerStateA);
    mySender.send(powerOn, RAW_DATA_LEN, 36); //send the ir code for switching power state
  }    
  

  
//  if (heartBeatTime > 1000) {
//    Serial.println("hearbeat");
//    heartBeatTime = 0;
//  }

  delay(2);
  
}
