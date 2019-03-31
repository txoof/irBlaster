#include <movingAvg.h>    // moving average library
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender
#include <elapsedMillis.h>    //measure elapsed time

// ====REMOTE CODES====
//define the IR Sender
IRsendRaw mySender;
#define RAW_DATA_LEN 68
const uint16_t powerOnOff[RAW_DATA_LEN] = {
  8550, 4306, 530, 1606, 530, 566, 502, 1610,
  530, 566, 502, 574, 506, 1630, 506, 566,
  506, 1630, 506, 566, 502, 1610, 530, 566,
  502, 1634, 506, 1606, 530, 570, 498, 1634,
  506, 570, 510, 562, 506, 566, 502, 1634,
  506, 1606, 530, 1610, 530, 562, 538, 538,
  530, 542, 538, 1598, 538, 1570, 558, 542,
  538, 538, 530, 542, 538, 1598, 530, 1578,
  558, 1578, 562, 1000
};

const uint16_t sourceCD[RAW_DATA_LEN] = {
  8546, 4310, 558, 1578, 562, 538, 498, 1638,
  530, 542, 506, 570, 502, 1634, 502, 570,
  498, 1638, 534, 538, 498, 1638, 530, 542,
  506, 1606, 554, 1582, 558, 538, 510, 1602,
  554, 546, 506, 566, 502, 570, 510, 1602,
  554, 1582, 558, 538, 510, 566, 502, 1606,
  554, 546, 502, 1610, 558, 1574, 554, 546,
  502, 570, 510, 1602, 526, 1610, 526, 574,
  506, 1602, 526, 1000
};

//uint16_t sourceAUX[RAW_DATA_LEN] = {
//  8550, 4310, 526, 1634, 506, 566, 502, 1634,
//  506, 566, 502, 574, 506, 1626, 510, 566,
//  506, 1630, 506, 566, 502, 1634, 506, 566,
//  502, 1634, 506, 1606, 530, 566, 506, 1630,
//  506, 566, 502, 1634, 506, 1630, 506, 1630,
//  510, 1602, 526, 570, 510, 566, 502, 1634,
//  502, 570, 502, 570, 510, 566, 502, 570,
//  510, 562, 506, 1630, 506, 1630, 510, 562,
//  506, 1630, 510, 1000
//};

const uint16_t sourceCDR[RAW_DATA_LEN] = {
  8550, 4306, 530, 1606, 534, 566, 502, 1606,
  534, 566, 502, 570, 510, 1602, 526, 570,
  510, 1602, 534, 566, 502, 1606, 534, 566,
  506, 1602, 530, 1606, 534, 566, 502, 1610,
  530, 570, 498, 574, 506, 566, 502, 570,
  510, 1602, 526, 570, 510, 566, 502, 570,
  510, 1602, 526, 1610, 526, 1606, 534, 1602,
  534, 566, 502, 1610, 530, 1602, 534, 1602,
  526, 574, 506, 1000
};

//Set up array of codes
const uint16_t *sources[3] = {powerOnOff, sourceCD, sourceCDR};//, sourceAUX};


//This causes some sort of horrible crash - maybe an overflow of the uint16 data type?
//#define RAW_DATA_LEN 68
//uint16_t sources[4][RAW_DATA_LEN] = {
//  {8550, 4306, 530, 1606, 530, 566, 502, 1610, //POWER ON/OFF
//  530, 566, 502, 574, 506, 1630, 506, 566,
//  506, 1630, 506, 566, 502, 1610, 530, 566,
//  502, 1634, 506, 1606, 530, 570, 498, 1634,
//  506, 570, 510, 562, 506, 566, 502, 1634,
//  506, 1606, 530, 1610, 530, 562, 538, 538,
//  530, 542, 538, 1598, 538, 1570, 558, 542,
//  538, 538, 530, 542, 538, 1598, 530, 1578,
//  558, 1578, 562, 1000},
//
//  {8546, 4310, 558, 1578, 562, 538, 498, 1638, //sourceCD
//  530, 542, 506, 570, 502, 1634, 502, 570,
//  498, 1638, 534, 538, 498, 1638, 530, 542,
//  506, 1606, 554, 1582, 558, 538, 510, 1602,
//  554, 546, 506, 566, 502, 570, 510, 1602,
//  554, 1582, 558, 538, 510, 566, 502, 1606,
//  554, 546, 502, 1610, 558, 1574, 554, 546,
//  502, 570, 510, 1602, 526, 1610, 526, 574,
//  506, 1602, 526, 1000},
//
//
//  {8550, 4306, 530, 1606, 534, 566, 502, 1606,  //sourceCDR
//  534, 566, 502, 570, 510, 1602, 526, 570,
//  510, 1602, 534, 566, 502, 1606, 534, 566,
//  506, 1602, 530, 1606, 534, 566, 502, 1610,
//  530, 570, 498, 574, 506, 566, 502, 570,
//  510, 1602, 526, 570, 510, 566, 502, 570,
//  510, 1602, 526, 1610, 526, 1606, 534, 1602,
//  534, 566, 502, 1610, 530, 1602, 534, 1602,
//  526, 574, 506, 1000},
//
//  {8550, 4310, 526, 1634, 506, 566, 502, 1634,  //sourceAUX
//  506, 566, 502, 574, 506, 1626, 510, 566,
//  506, 1630, 506, 566, 502, 1634, 506, 566,
//  502, 1634, 506, 1606, 530, 566, 506, 1630,
//  506, 566, 502, 1634, 506, 1630, 506, 1630,
//  510, 1602, 526, 570, 510, 566, 502, 1634,
//  502, 570, 502, 570, 510, 566, 502, 570,
//  510, 562, 506, 1630, 506, 1630, 510, 562,
//  506, 1630, 510, 1000}
//} ;


// ====PIN ASSIGNMENTS====
const int audioPin1 = A1;     //Channel 1
const int audioPin2 = A2;
const int debugPin = A3;     //when low, run in debug mode
const int statusLightPin = 10;   //active channel indicator light


//  ====CHANNEL MEASUREMENTS====
# define CHANNELS 3     //number of channels to sample from
int audioChannels[CHANNELS] = {audioPin1, audioPin1, audioPin2};      //element 0 will be ignored
int channelValues[CHANNELS] = {0, 0, 0};
const int SAMPLES = 300;
movingAvg audioAverages[CHANNELS] = {movingAvg(SAMPLES), movingAvg(SAMPLES), movingAvg(SAMPLES)};

//  ====CHANNEL VARIABLES====
int currentChannel = 0;     //channel that is currently active (0 is off)
int previousChannel = 0;    //channel that was active before change (0 is off)
const int audioThreshold = 15;      //minimum value for an "active" channel

//  ====TIMERS===
int counter = 0;
const int heartBeat = 500;
int channelReleaseTimeOut = 5000;      //time to wait before releasing an inactive timer
int powerTimeOut = 10000;       //time to wait before turnning off
int powerOnDelay = 7000;
elapsedMillis channelReleaseTimer = 0;
elapsedMillis powerTimer = 0;

// ====CONTROL VARIABLES====
bool debugMode = false;
bool statusLight = false;

void sendCode(int myChannel) {
//  debug("sending code", -1);
  for (int i=0; i < 20; i++) {
//    mySender.send(sources[myChannel], RAW_DATA_LEN, 36);
    mySender.send(powerOnOff, RAW_DATA_LEN, 36);
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

void debug(String message="", int val=0) {
  
  if (debugMode) {
    Serial.print(message);
    Serial.println(val);
  }
} //END debug()


#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


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
  pinMode(debugPin, INPUT);
  pinMode(audioPin1, INPUT);
  pinMode(audioPin2, INPUT);

  if (digitalRead(debugPin)) {
    debugMode = true;
  }

  flashStatus();
  
  if (debugMode) {
    Serial.begin(9600);
    delay(2000);
    Serial.print(F("debug mode: "));
    Serial.println(digitalRead(debugPin));
    statusLight = true;
    debug(F("starting up "), -1);
    debug(F("Free mem: "), freeMemory);
    
  }

  // ====INIT VARIABLES====
  for (int i = 0; i < CHANNELS; i++) { //init and reset the moving averages
    audioAverages[i].begin();
    audioAverages[i].reset();
  }

}


void loop() {
  int activeChannel = 0;
  for (int i=0; i < CHANNELS; i++) {      //sample channels
    int audioValue = 0;
    if (i > 0) {      //only sample channels > 0
      audioValue = analogRead(audioChannels[i]) - 512;      //voltage divider on amp circuit shifts all values +512 (2.5V)
      audioValue = abs(audioValue);     //abs() funciton is actually a macro; needs to be on own line
      channelValues[i] = audioAverages[i].reading(audioValue);    //update and store the moving average for each channel
    }
    
//    if (counter >= heartBeat) {
//      debug("channel: ", i);
//      debug("    audioValue: ", audioValue);
//      debug("           avg: ", channelValues[i]);
//    } 
  }     //end sample channels
  
//  if (counter >= heartBeat) {
//    debug("======================", -1);
//  }


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
      debug(F("Free mem: "), freeMemory());
      sendCode(0);
      debug(F("Free mem: "), freeMemory());
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
      debug(F("Free mem: "), freeMemory());
      sendCode(0);     
      debug(F("Free mem: "), freeMemory());
      previousChannel = currentChannel;
    }
    
    if (currentChannel > 0) {
      debug(F("Send channel change: "), currentChannel);
      debug(F("Free mem: "), freeMemory());
      flashStatus(3, 100);      //3 fast pulses to indicate channel change
      sendCode(currentChannel);
      for (int i=0; i < 3; i++) {     //repeat 3 times
        flashStatus(currentChannel, 500);     //flash the current channel 
        delay(800);
      }
      debug(F("Free mem: "), freeMemory());
      previousChannel = currentChannel;
    }

  }

  
  if (counter >= heartBeat) {
//    sendCode(0);
//    Serial.println(F("HeartBeat...."));
    debug(F("\nheartBeat "), -1);
    counter = 0;
  }

  counter++;
  delay(2);
  
  

}
