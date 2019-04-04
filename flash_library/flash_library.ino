#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender

IRsendRaw mySender;

  
#define RAW_DATA_LEN 68  
const uint16_t sources[3][RAW_DATA_LEN] PROGMEM =
  {{8550, 4306, 530, 1606, 530, 566, 502, 1610, //power on/off
  530, 566, 502, 574, 506, 1630, 506, 566,
  506, 1630, 506, 566, 502, 1610, 530, 566,
  502, 1634, 506, 1606, 530, 570, 498, 1634,
  506, 570, 510, 562, 506, 566, 502, 1634,
  506, 1606, 530, 1610, 530, 562, 538, 538,
  530, 542, 538, 1598, 538, 1570, 558, 542,
  538, 538, 530, 542, 538, 1598, 530, 1578,
  558, 1578, 562, 1000},

  {8546, 4310, 558, 1578, 562, 538, 498, 1638,  //source CD
  530, 542, 506, 570, 502, 1634, 502, 570,
  498, 1638, 534, 538, 498, 1638, 530, 542,
  506, 1606, 554, 1582, 558, 538, 510, 1602,
  554, 546, 506, 566, 502, 570, 510, 1602,
  554, 1582, 558, 538, 510, 566, 502, 1606,
  554, 546, 502, 1610, 558, 1574, 554, 546,
  502, 570, 510, 1602, 526, 1610, 526, 574,
  506, 1602, 526, 1000},

  {8550, 4306, 530, 1606, 534, 566, 502, 1606,  //source CDR
  534, 566, 502, 570, 510, 1602, 526, 570,
  510, 1602, 534, 566, 502, 1606, 534, 566,
  506, 1602, 530, 1606, 534, 566, 502, 1610,
  530, 570, 498, 574, 506, 566, 502, 570,
  510, 1602, 526, 570, 510, 566, 502, 570,
  510, 1602, 526, 1610, 526, 1606, 534, 1602,
  534, 566, 502, 1610, 530, 1602, 534, 1602,
  526, 574, 506, 1000}};
  




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) {
    ;
  }



}

void loop() {
    if (Serial.read() != -1) {
      //send a code every time a character is received from the 
      // serial port. You could modify this sketch to send when you
      // push a button connected to an digital input pin.
      mySender.send(sources[0],RAW_DATA_LEN,36);//Pass the buffer,length, optionally frequency
      Serial.println(F("Sent signal."));
    }

}
