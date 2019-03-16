 /*
 Analog input, analog output, serial output

 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.

 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground

 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

// These constants won't change.  They're used to give names
// to the pins used:

#include <elapsedMillis.h>

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
int oldSensorValue = 0;     // old sensor value

int minimum = 1023; 
int maximum = 0;
elapsedMillis heartBeatTime;
elapsedMillis lightTime;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  // change the analog out value:
  //analogWrite(analogOutPin, outputValue);

  if (abs(sensorValue - oldSensorValue) >= 250 ) {
    // print the results to the serial monitor:
    analogWrite(analogOutPin, 500);
    lightTime = 0;
    Serial.print("sensor = ");
    Serial.print(sensorValue);
    Serial.print("\t delta = ");
    Serial.println(abs(sensorValue - oldSensorValue));
    Serial.print("maximum = ");
    Serial.print(maximum);
    Serial.print("\t minimum = ");
    Serial.println(minimum);
  }


  if (sensorValue > maximum) {
    maximum = sensorValue;
  }

  if (sensorValue < minimum) {
    minimum = sensorValue;
  }

  if (lightTime > 250) {
    analogWrite(analogOutPin, 0);
  }

  if (heartBeatTime > 1000) {
    Serial.println("heartbeat");
    heartBeatTime = 0;
  }

  oldSensorValue = sensorValue;

  

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
}
