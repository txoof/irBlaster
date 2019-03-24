#include <elapsedMillis.h>    //measure elapsed time
#include <advancedSerial.h>   //advanced serial output for better debugging
#include <movingAvg.h>    // moving average library
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender

//define the IR Sender
IRsendRaw mySender;
#define RAW_DATA_LEN 68


uint16_t powerOnOff[RAW_DATA_LEN] = {
  8550, 4306, 530, 1606, 530, 566, 502, 1610,
  530, 566, 502, 574, 506, 1630, 506, 566,
  506, 1630, 506, 566, 502, 1610, 530, 566,
  502, 1634, 506, 1606, 530, 570, 498, 1634,
  506, 570, 510, 562, 506, 566, 502, 1634,
  506, 1606, 530, 1610, 530, 562, 538, 538,
  530, 542, 538, 1598, 538, 1570, 558, 542,
  538, 538, 530, 542, 538, 1598, 530, 1578,
  558, 1578, 562, 1000};

uint16_t sourceCD[RAW_DATA_LEN] = {
  8546, 4310, 558, 1578, 562, 538, 498, 1638,
  530, 542, 506, 570, 502, 1634, 502, 570,
  498, 1638, 534, 538, 498, 1638, 530, 542,
  506, 1606, 554, 1582, 558, 538, 510, 1602,
  554, 546, 506, 566, 502, 570, 510, 1602,
  554, 1582, 558, 538, 510, 566, 502, 1606,
  554, 546, 502, 1610, 558, 1574, 554, 546,
  502, 570, 510, 1602, 526, 1610, 526, 574,
  506, 1602, 526, 1000};

uint16_t sourceAUX[RAW_DATA_LEN] = {
  8550, 4310, 526, 1634, 506, 566, 502, 1634,
  506, 566, 502, 574, 506, 1626, 510, 566,
  506, 1630, 506, 566, 502, 1634, 506, 566,
  502, 1634, 506, 1606, 530, 566, 506, 1630,
  506, 566, 502, 1634, 506, 1630, 506, 1630,
  510, 1602, 526, 570, 510, 566, 502, 1634,
  502, 570, 502, 570, 510, 566, 502, 570,
  510, 562, 506, 1630, 506, 1630, 510, 562,
  506, 1630, 510, 1000};

uint16_t sourceCDR[RAW_DATA_LEN] = {
  8550, 4306, 530, 1606, 534, 566, 502, 1606,
  534, 566, 502, 570, 510, 1602, 526, 570,
  510, 1602, 534, 566, 502, 1606, 534, 566,
  506, 1602, 530, 1606, 534, 566, 502, 1610,
  530, 570, 498, 574, 506, 566, 502, 570,
  510, 1602, 526, 570, 510, 566, 502, 570,
  510, 1602, 526, 1610, 526, 1606, 534, 1602,
  534, 566, 502, 1610, 530, 1602, 534, 1602,
  526, 574, 506, 1000};

//Set up array of codes
uint16_t *sources[3] = {sourceCD, sourceCDR, sourceAUX};


// ====PIN ASSIGNMENTS====
const int audioPin0 = A0;     //Channel 0
const int audioPin1 = A1;     //Channel 1
const int debugPin = A2;      //when low, run in debug mode
const int statusLight = 10;   //active channel indicator light
const int audioThreshold = 8; //minimum level to be considered "active"



// ====VARIABLES====
// ===OPPERATIONS VARS===
bool debugMode = false;     // set the debugging state

// ===TIMER VARS===
long channelRelease = 10000;      // amount of time to wait before releasing inactive channel
long powerTimeout = 480000;        // amount of time to wait before powering off
const int powerOnDelay = 10000;
elapsedMillis channelReleaseTimer;      // timer for measuring elapsed time of inactive channel
elapsedMillis powerTimer;       // timer for measuring elapsed time for power off
elapsedMillis heartBeat;    

// ===CHANNEL MEASUREMENTS===
#define CHANS 2       //number of channels to sample from 
int audioChannels[CHANS] = {audioPin0, audioPin1};
int channelValues[CHANS];
int SAMPLES = 300;      // number of samples to keep in the rolling average
movingAvg audioAverages[CHANS] = {movingAvg(SAMPLES), movingAvg(SAMPLES)};    //array of moving average objects for smoothing analog input
bool channelIsActive = false;
int currentChannel = -1;
int prevChannel = -1;

int findActiveChannel() {   //returns first active channel in the array or -1 if none are active
  int myChannel = -1;
  for (int i = 0; i < CHANS; i++) {
    if (channelValues[i] >= audioThreshold) {
      myChannel = i;
      break;
    }
  }
  return myChannel;
}

void flashStatus(int repeat=10, int wait=25) {
  for (int i=0; i < repeat; i++) {
    digitalWrite(statusLight, true);
    delay(wait);
    digitalWrite(statusLight, false);
    delay(wait/2);
  }
}

//broadcast an IR signal 
void sendCode(uint16_t *code) {
  int repeat = 20;
  for (int i=0; i < repeat; i++) {
    mySender.send(code, RAW_DATA_LEN, 36);
    delay(5);
  }
  aSerial.vv().pln("sent code");
}


void setup() {
  // ====PIN SETUP===
  pinMode(statusLight, OUTPUT);
  pinMode(debugPin, INPUT);
  pinMode(audioPin0, INPUT);
  pinMode(audioPin1, INPUT);
  
  flashStatus(10, 25);      // flash status light 10, 25 on 25/2 off

  if (!digitalRead(debugPin)) {
    debugMode = true;
  }

  if (debugMode) {      //enable debugging routines
    Serial.begin(9600);
    delay(2000); //delay for serial to come online

    aSerial.setPrinter(Serial);
    aSerial.setFilter(Level::vvvv);

    // Set the timeout to shorter times
    channelRelease = 5000;
    powerTimeout = 6000;

    aSerial.v().pln("Audio sensing start in debug mode");
    aSerial.v().pln("Version 0.1.1");
    aSerial.v().pln("Adjusting timeout values:");
    aSerial.v().p("  channelRelease: ").pln(channelRelease);
    aSerial.v().p("  powerTimeout: ").pln(powerTimeout);
  } else {
    aSerial.off();      // turn of the serial logging all together
  }

  // ====VARIABLE REASSIGNMENT====
  channelReleaseTimer = 0;
  powerTimer = 0;

  for (int i = 0; i < CHANS; i++) { //init the moving average library
    audioAverages[i].begin();
    audioAverages[i].reset();
  }
  
}

void loop() {
  channelIsActive = false;
  for (int i = 0; i < CHANS; i++) { //begin sampling
    int audioValue = analogRead(i) - 512;   //voltage divider shifts all values + ~2.5v
    audioValue = abs(audioValue);   //abs is a macro use on own line
    channelValues[i] = audioAverages[i].reading(audioValue);    //store and update the moving average

    if (channelValues[i] >= audioThreshold) {
      channelIsActive = true;
    }
    
    if (heartBeat >= 500) {
          aSerial.vvvv().p("Channel ").pln(i);
          aSerial.vvvv().p("\t value: ").pln(audioValue);
          aSerial.vvvv().p("\t avg: ").pln(channelValues[i]);
          aSerial.vvvv().pln();
    }  
  } // end sampling

  if (channelIsActive) { // begin analysis of active channel
    int activeChannel = findActiveChannel();


    // active does not match current and measured audio is lower than threshold, and the timer 
    // has expired, change the channel
    if (activeChannel != currentChannel and channelValues[currentChannel] < audioThreshold 
      and channelReleaseTimer >= channelRelease) {

      currentChannel = activeChannel;
    }
  } // end channel is active

  if (currentChannel != prevChannel) {
    prevChannel = currentChannel;
    powerTimer = 0;
    channelReleaseTimer = 0;
  }
  
  
  if (heartBeat >= 500) {
      heartBeat = 0;
  }

  delay(2);
}
