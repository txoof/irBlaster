/* rawSend.ino Example sketch for IRLib2
 *  Illustrates how to send a code Using raw timings which were captured
 *  from the "rawRecv.ino" sample sketch.  Load that sketch and
 *  capture the values. They will print in the serial monitor. Then you
 *  cut and paste that output into the appropriate section below.
 */
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender

IRsendRaw mySender;

void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  Serial.println(F("Every time you press a key is a serial monitor we will send."));
}
/* Cut and paste the output from "rawRecv.ino" below here. It will 
 * consist of a #define RAW_DATA_LEN statement and an array definition
 * beginning with "uint16_t rawData[RAW_DATA_LEN]= {…" and concludes
 * with "…,1000};"
 */
#define RAW_DATA_LEN 68
uint16_t rawData[RAW_DATA_LEN]={
  8978, 4494, 522, 582, 550, 582, 570, 562, 
  518, 586, 546, 586, 578, 554, 526, 578, 
  554, 578, 574, 1662, 550, 1714, 518, 1718, 
  578, 1658, 554, 578, 574, 1662, 550, 1714, 
  522, 1714, 578, 554, 518, 1718, 578, 1658, 
  554, 1710, 522, 1714, 550, 582, 518, 586, 
  546, 586, 578, 1658, 554, 578, 542, 590, 
  522, 582, 550, 582, 550, 1686, 550, 1714, 
  526, 1710, 542, 1000};



/*
 * Cut-and-paste into the area above.
 */
   
void loop() {
  if (Serial.read() != -1) {
    //send a code every time a character is received from the 
    // serial port. You could modify this sketch to send when you
    // push a button connected to an digital input pin.
    mySender.send(rawData,RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
    Serial.println(F("Sent signal."));
  }
}
