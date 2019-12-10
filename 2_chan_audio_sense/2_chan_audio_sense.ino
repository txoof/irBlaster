// Known good board version 1.6.21 prior to 10 December 2019
// avrdude Version 6.3-20190619

#include <movingAvg.h>    // moving average library
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <elapsedMillis.h>    //measure elapsed time

// ====REMOTE CODES====
//define the IR Sender
IRsendRaw mySender;

//Raw IR codes read with ../rawRead/rawRead.ino
#define RAW_DATA_LEN 68
/* 
store the remote codes in PROGMEM; remote codes in regular memory causes
all sorts of weird runtime errors as the arduino runs out of memory
*/
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
  
//  {8550, 4306, 530, 1606, 522, 574, 506, 1606, //source AUX 
//  526, 570, 510, 566, 502, 1630, 510, 566, 
//  502, 1630, 510, 566, 502, 1634, 506, 566, 
//  502, 1634, 506, 1602, 526, 574, 506, 1630, 
//  498, 574, 506, 1630, 498, 1610, 530, 1606, 
//  530, 1606, 526, 570, 538, 538, 534, 1598, 
//  538, 538, 534, 538, 530, 542, 538, 538, 
//  534, 538, 530, 1606, 534, 1602, 534, 538, 
//  534, 1602, 534, 1000}};
  
  {8550, 4306, 530, 1606, 534, 566, 502, 1606,  //source CDR
  534, 566, 502, 570, 510, 1602, 526, 570,
  510, 1602, 534, 566, 502, 1606, 534, 566,
  506, 1602, 530, 1606, 534, 566, 502, 1610,
  530, 570, 498, 574, 506, 566, 502, 570,
  510, 1602, 526, 570, 510, 566, 502, 570,
  510, 1602, 526, 1610, 526, 1606, 534, 1602,
  534, 566, 502, 1610, 530, 1602, 534, 1602,
  526, 574, 506, 1000}};


//const uint16_t powerOnOff[RAW_DATA_LEN] = {
//  8550, 4306, 530, 1606, 530, 566, 502, 1610,
//  530, 566, 502, 574, 506, 1630, 506, 566,
//  506, 1630, 506, 566, 502, 1610, 530, 566,
//  502, 1634, 506, 1606, 530, 570, 498, 1634,
//  506, 570, 510, 562, 506, 566, 502, 1634,
//  506, 1606, 530, 1610, 530, 562, 538, 538,
//  530, 542, 538, 1598, 538, 1570, 558, 542,
//  538, 538, 530, 542, 538, 1598, 530, 1578,
//  558, 1578, 562, 1000
//};
//
//const uint16_t sourceCD[RAW_DATA_LEN] = {
//  8546, 4310, 558, 1578, 562, 538, 498, 1638,
//  530, 542, 506, 570, 502, 1634, 502, 570,
//  498, 1638, 534, 538, 498, 1638, 530, 542,
//  506, 1606, 554, 1582, 558, 538, 510, 1602,
//  554, 546, 506, 566, 502, 570, 510, 1602,
//  554, 1582, 558, 538, 510, 566, 502, 1606,
//  554, 546, 502, 1610, 558, 1574, 554, 546,
//  502, 570, 510, 1602, 526, 1610, 526, 574,
//  506, 1602, 526, 1000
//};
//
////uint16_t sourceAUX[RAW_DATA_LEN] = {
////  8550, 4310, 526, 1634, 506, 566, 502, 1634,
////  506, 566, 502, 574, 506, 1626, 510, 566,
////  506, 1630, 506, 566, 502, 1634, 506, 566,
////  502, 1634, 506, 1606, 530, 566, 506, 1630,
////  506, 566, 502, 1634, 506, 1630, 506, 1630,
////  510, 1602, 526, 570, 510, 566, 502, 1634,
////  502, 570, 502, 570, 510, 566, 502, 570,
////  510, 562, 506, 1630, 506, 1630, 510, 562,
////  506, 1630, 510, 1000
////};
//
//const uint16_t sourceCDR[RAW_DATA_LEN] = {
//  8550, 4306, 530, 1606, 534, 566, 502, 1606,
//  534, 566, 502, 570, 510, 1602, 526, 570,
//  510, 1602, 534, 566, 502, 1606, 534, 566,
//  506, 1602, 530, 1606, 534, 566, 502, 1610,
//  530, 570, 498, 574, 506, 566, 502, 570,
//  510, 1602, 526, 570, 510, 566, 502, 570,
//  510, 1602, 526, 1610, 526, 1606, 534, 1602,
//  534, 566, 502, 1610, 530, 1602, 534, 1602,
//  526, 574, 506, 1000
//};
//
////Set up array pointers to codes
//const uint16_t *sources[3] = {powerOnOff, sourceCD, sourceCDR};//, sourceAUX};


// ====PIN ASSIGNMENTS====
const int audioPin1 = A1;     //Channel 1
const int audioPin2 = A6;     //Channel 2
const int debugPin = A0;      //when debug jumper is attached, pin is LOW (false); when off pin is HIGH (true)
const int chOne = A4;
const int chTwo = A3;
const int irRec = A5;
const int statusLightPin = 13;   //active channel indicator light (onboard LED)


//  ====CHANNEL MEASUREMENTS====
# define CHANNELS 3     //number of channels to sample from
int audioChannels[CHANNELS] = {audioPin1, audioPin1, audioPin2};      //element 0 will be ignored
int channelValues[CHANNELS] = {0, 0, 0};
int channelStatus[CHANNELS] = {chOne, chOne, chTwo};      //element 0 will be ignored
const int SAMPLES = 300; //this is the maximum; larger SAMPLES value causes memory issues
movingAvg audioAverages[CHANNELS] = {movingAvg(SAMPLES), movingAvg(SAMPLES), movingAvg(SAMPLES)};


//  ====CHANNEL VARIABLES====
int currentChannel = 0;     //channel that is currently active (0 is off)
int previousChannel = 0;    //channel that was active before change (0 is off)
/*
 * Tested values:
 * 20 - too high, quite songs cut out
 * 15 - good compromise
 */
const int audioThreshold = 15;      //minimum value for an "active" channel


//  ====TIMERS===
int counter = 0;
const int heartBeat = 500;
int channelReleaseTimeOut = 60000;      //time to wait before releasing an inactive timer (60 seconds)
long powerTimeOut = 300000;       //time to wait before turnning off (5 min)
int powerOnDelay = 200;           //wait for the receiver to become active 
elapsedMillis channelReleaseTimer = 0;
elapsedMillis powerTimer = 0;

// ====CONTROL VARIABLES====
bool debugMode = false;
bool statusLight = false;


void debug(String message="", int val=0) {  
  if (debugMode) {
    Serial.print(message);
    Serial.println(val);
  }
} //END debug()


void sendCode(int myChannel) {
  uint16_t myCode[RAW_DATA_LEN];
  for (int i = 0; i < RAW_DATA_LEN; i++) {
    myCode[i] = pgm_read_word_near(sources[myChannel] + i); 
  }
    
  for (int i=0; i < 20; i++) {
    mySender.send(myCode, RAW_DATA_LEN, 36);
    delay(2);
  }
} //END sendCode()

int findActiveChannel() {   //returns first active channel in the array or 0 if none are active
  int myChannel = 0;
  for (int i = 0; i < CHANNELS; i++) {
    if (channelValues[i] >= audioThreshold) {
      myChannel = i;
      break;
    }
  }
  return myChannel;
} //END findActiveChannel()


void flashStatus(int number=5, int len=100) {
  for (int i=0; i < number; i++) {
    digitalWrite(statusLightPin, true);
    delay(len);
    digitalWrite(statusLightPin, false);
    delay(len/2);
  }
  digitalWrite(statusLightPin, statusLight);
}


void setup() {
  delay(1000); //delay in case of runaway loop - allow programmer time to interrupt
  debugMode = false;
  //  ====PIN SETUP====
  pinMode(statusLightPin, OUTPUT);
  pinMode(debugPin, INPUT_PULLUP); //set pin to high impeadance mode (HIGH when not connected)
  pinMode(audioPin1, INPUT);
  pinMode(audioPin2, INPUT);
  pinMode(chOne, OUTPUT);
  pinMode(chTwo, OUTPUT);


  debugMode = !digitalRead(debugPin);     //debug pin pulls high when not connected

  flashStatus();

  Serial.begin(9600);
  delay(2000);
  
  if (debugMode == true) { 

    statusLight = true;       //set default state for status light - always on when active in debug mode
    debug(F("starting up in debug mode "), -1);
    powerTimeOut = 10000;
    channelReleaseTimeOut = 10000;
    
    debug(F("decreasing powerTimeOut to: "), powerTimeOut);
    debug(F("decreasing channelReleaseTimeOut to: "), channelReleaseTimeOut);
    debug(F("Version 10 Dec 2019"), -1);
    
//    debug(F("Free mem: "), freeMemory);
    
  }

  // ====INIT VARIABLES====
  for (int i = 0; i < CHANNELS; i++) { //init and reset the moving averages
    audioAverages[i].begin();
    audioAverages[i].reset();
  }

}


void loop() {
  debugMode = !digitalRead(debugPin);
//  const String sources_str[3] = {"", "CD", "CDR"};
  int activeChannel = 0;    //set the default active channel 
  for (int i=0; i < CHANNELS; i++) {      //sample channels
    int audioValue = 0;
    bool chStat = false;
    if (i > 0) {      //only sample channels > 0
      audioValue = analogRead(audioChannels[i]) - 512;      //voltage divider on amp circuit shifts all values +512 (2.5V)
      audioValue = abs(audioValue);     //abs() funciton is actually a macro; needs to be on own line
      channelValues[i] = audioAverages[i].reading(audioValue);    //update and store the moving average for each channel
      
      if (channelValues[i] >= audioThreshold and debugMode) {
        chStat = true;
      } 

      digitalWrite(channelStatus[i], chStat);
    }
    
    if (counter >= heartBeat and debugMode) {
      debug(F("channel: "), i);
      debug(F("    audioValue: "), audioValue);
      debug(F("           avg: "), channelValues[i]);
    } 
  }     //end sample channels
  
  if (counter >= heartBeat and debugMode) {
    debug(F("======================"), -1);
  }


  if (channelValues[currentChannel] >= audioThreshold) {      //reset the channel release timer if the current channel is active
    channelReleaseTimer = 0;
    powerTimer = 0; 
  } else {
    activeChannel = findActiveChannel();

    //DEBUGGING - show countdown to channel release
    if(channelReleaseTimer < channelReleaseTimeOut and counter >= 500) {
      debug(F("Channel became inactive: "), currentChannel);
      debug(F("\treleasing in: "), channelReleaseTimeOut - channelReleaseTimer);
    } 
    //END DEBUGGING

    if (activeChannel != currentChannel and channelReleaseTimer >=channelReleaseTimeOut) {      //change channel 
      debug(F("Changing channel from: "), currentChannel);
      debug(F("Changing channel to: "), activeChannel);
//      debug(sources_str[activeChannel], -1);
      
      currentChannel = activeChannel;
      powerTimer = 0; 
    }
  }
  //END else find active channel

  if (currentChannel > 0) {     //reset the power timeout if there is an active channel
    powerTimer = 0;
  }

  if (previousChannel != currentChannel) {      //check for a channel change and send appropriate codes
    if (previousChannel == 0) {     //state change from off to on
      if (debugMode) {      //in debug mode turn the status light on
        statusLight = true;
      }
      
      flashStatus(2, 500);      //2 slow pulses to indicate power on
      
      debug(F("power state change -> ON "), -1);
//      debug(F("Free mem: "), freeMemory());
      sendCode(0);
//      debug(F("Free mem: "), freeMemory());
      debug(F("\tdelaying for reciver to power up: "), powerOnDelay);
      delay(powerOnDelay);
      previousChannel = currentChannel;
    }

    if (currentChannel < 1 and powerTimer >= powerTimeOut) {
      if (debugMode) {
        statusLight = false;
      }
       
      flashStatus(3, 500);      //3 slow pulses to indicate power off
      
      debug(F("power state change -> OFF "), -1);
//      debug(F("Free mem: "), freeMemory());
      sendCode(0);     
//      debug(F("Free mem: "), freeMemory());
      previousChannel = currentChannel;
    }
    
    if (currentChannel > 0) {
//      debug(F("Send channel change: "), currentChannel);
//      debug(F("Free mem: "), freeMemory());
      flashStatus(3, 100);      //3 fast pulses to indicate channel change
      sendCode(currentChannel);
      for (int i=0; i < 3; i++) {     //repeat 3 times
        flashStatus(currentChannel, 500);     //flash the current channel 
        delay(1500);      //delay 1.5 seconds between each flash
      }
//      debug(F("Free mem: "), freeMemory());
      previousChannel = currentChannel;
    }

  }

  
  if (counter >= heartBeat) {
    debug(F("\nheartBeat "), -0);
    counter = 0;
  }

  counter++;
  delay(2);

}
