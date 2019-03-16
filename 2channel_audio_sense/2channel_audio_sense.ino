#include <elapsedMillis.h>    //measure elapsed time
#include <movingAvg.h>        //moving average library


//pioneer reciever codes

// power on/off
//#define RAW_DATA_LEN 68
//uint16_t rawData[RAW_DATA_LEN]={
//  8550, 4306, 530, 1606, 530, 566, 502, 1610, 
//  530, 566, 502, 574, 506, 1630, 506, 566, 
//  506, 1630, 506, 566, 502, 1610, 530, 566, 
//  502, 1634, 506, 1606, 530, 570, 498, 1634, 
//  506, 570, 510, 562, 506, 566, 502, 1634, 
//  506, 1606, 530, 1610, 530, 562, 538, 538, 
//  530, 542, 538, 1598, 538, 1570, 558, 542, 
//  538, 538, 530, 542, 538, 1598, 530, 1578, 
//  558, 1578, 562, 1000};


//source CD
//#define RAW_DATA_LEN 68
//uint16_t rawData[RAW_DATA_LEN]={
//  8546, 4310, 558, 1578, 562, 538, 498, 1638, 
//  530, 542, 506, 570, 502, 1634, 502, 570, 
//  498, 1638, 534, 538, 498, 1638, 530, 542, 
//  506, 1606, 554, 1582, 558, 538, 510, 1602, 
//  554, 546, 506, 566, 502, 570, 510, 1602, 
//  554, 1582, 558, 538, 510, 566, 502, 1606, 
//  554, 546, 502, 1610, 558, 1574, 554, 546, 
//  502, 570, 510, 1602, 526, 1610, 526, 574, 
//  506, 1602, 526, 1000};

//source AUX
//#define RAW_DATA_LEN 68
//uint16_t rawData[RAW_DATA_LEN]={
//  8550, 4310, 526, 1634, 506, 566, 502, 1634, 
//  506, 566, 502, 574, 506, 1626, 510, 566, 
//  506, 1630, 506, 566, 502, 1634, 506, 566, 
//  502, 1634, 506, 1606, 530, 566, 506, 1630, 
//  506, 566, 502, 1634, 506, 1630, 506, 1630, 
//  510, 1602, 526, 570, 510, 566, 502, 1634, 
//  502, 570, 502, 570, 510, 566, 502, 570, 
//  510, 562, 506, 1630, 506, 1630, 510, 562, 
//  506, 1630, 510, 1000};


//source CDR
//#define RAW_DATA_LEN 68
//uint16_t rawData[RAW_DATA_LEN]={
//  8550, 4306, 530, 1606, 534, 566, 502, 1606, 
//  534, 566, 502, 570, 510, 1602, 526, 570, 
//  510, 1602, 534, 566, 502, 1606, 534, 566, 
//  506, 1602, 530, 1606, 534, 566, 502, 1610, 
//  530, 570, 498, 574, 506, 566, 502, 570, 
//  510, 1602, 526, 570, 510, 566, 502, 570, 
//  510, 1602, 526, 1610, 526, 1606, 534, 1602, 
//  534, 566, 502, 1610, 530, 1602, 534, 1602, 
//  526, 574, 506, 1000};





//Analog pins for measuring the audio singnal
const int audioPin0 = A0;        //Channel 0
const int audioPin1 = A1;        //Channel 1
const int statusLight = 10;
const long delayTime = 5000; //timeout delay before switching off
const int channelHoldTime = 2000; //time to wait before switching audio sources


#define CHANS 2
int audioChannels[CHANS] = {audioPin0, audioPin1}; //physical pins
int channelValues[CHANS];

movingAvg audioAverages[CHANS] = {movingAvg(10), movingAvg(10)};


int audioThreshold = 10;       //minimum channel value to indicate activity
int activeChannel = -1;        //currently active channel
int currentChannel = -1;       //current audio source
int prevChannel = -1;

bool currentPowerState = false;     //power state
bool prevPowerState = false;

elapsedMillis shutDownTimer;   //Timer for measuring period of inactivity before shutting down
elapsedMillis lastChannelChange;
elapsedMillis heartBeat;

void setup() {
  // put your setup code here, to run once:
  pinMode(statusLight, OUTPUT);
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for serial to come online
  for (int i=0; i < CHANS; i++) { //init the moving average library
    audioAverages[i].begin();
  }

  
}

void loop() {
  //measure each audio channel and record the level
  for (int i=0; i < CHANS; i++) {
    int audioValue = analogRead(i) - 512;
    audioValue = abs(audioValue);
    channelValues[i] = audioAverages[i].reading(audioValue);
  }
  
  //find the first active channel - ignore all other active channels if multiple sources are broadcasting
  activeChannel = -1; //set active channel to -1 to start
  for (int i = 0; i < CHANS; i++) {
    if (channelValues[i] > audioThreshold) {
      activeChannel = i; //record the active channel and stop looking
      shutDownTimer = 0; //keep the turn over time at 0 as long as there is an active channel
      currentPowerState = true;
      break;
    }
  }

  // if there is an active channel attempt to switch to that channel
  if (activeChannel > -1 and currentChannel > -1) {
    // check to see if a channel change is needed
    if (activeChannel != currentChannel) {
      // if there's no current activity and the hold time is expired change channels
      // this prevents flip flopping rappidly between two sources if an extra source is turned on
      if (channelValues[currentChannel] < audioThreshold and lastChannelChange >= channelHoldTime) {
        currentChannel = activeChannel; //switch channels
        lastChannelChange = 0; //reset the timer                  
      }
    }
  }

  //if there is no current channel (<0), switch to active channel
  if (activeChannel > -1 and currentChannel < 0) {
    currentChannel = activeChannel;
    lastChannelChange = 0;
  }

  //if there has been no activity for at least channelHoldTime switch to inactive
  if ((activeChannel < 0) and (lastChannelChange >= channelHoldTime) and (activeChannel != currentChannel)) {
    currentChannel = activeChannel;
    lastChannelChange = 0;
  }
  
  if (shutDownTimer >= delayTime and currentPowerState) {
    shutDownTimer = 0;
    currentPowerState = false;
  }

  //deal with powerstate changes
  if (currentPowerState != prevPowerState) {
    digitalWrite(statusLight, currentPowerState);  
    prevPowerState = currentPowerState;
    // send ir command for power on/off
  }

  // send appropriate channel ir command
  if (currentChannel != prevChannel) {
    if (currentChannel > -1) {
      Serial.print("switching to channel: ");
      Serial.println(currentChannel); 
    }
    prevChannel = currentChannel;
  }


  if (heartBeat > 1000 and !currentPowerState) {
    Serial.println("heartbeat");
    heartBeat = 0;

  }

  delay(2);
}
