#include <elapsedMillis.h>    //measure elapsed time
#include <movingAvg.h>    // moving average library
#include <advancedSerial.h>   //advanced serial output for better debugging
//#include

// power on/off
#define RAW_DATA_LEN 68

const uint16_t powerOff[RAW_DATA_LEN] = {
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

const uint16_t sourceAUX[RAW_DATA_LEN] = {
  8550, 4310, 526, 1634, 506, 566, 502, 1634,
  506, 566, 502, 574, 506, 1626, 510, 566,
  506, 1630, 506, 566, 502, 1634, 506, 566,
  502, 1634, 506, 1606, 530, 566, 506, 1630,
  506, 566, 502, 1634, 506, 1630, 506, 1630,
  510, 1602, 526, 570, 510, 566, 502, 1634,
  502, 570, 502, 570, 510, 566, 502, 570,
  510, 562, 506, 1630, 506, 1630, 510, 562,
  506, 1630, 510, 1000
};

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


//uint16_t* sources[] = {sourceCD, sourceAUX,  sourceCDR};
const uint16_t *sources[3] = {sourceCD, sourceAUX, sourceCDR};
//dummy code place holder for array of remote instructions
String sourcesSTR[] = {"sourceCD", "sourceAUX", "sourceCDR"};


const int audioPin0 = A0;     //Channel 0
const int audioPin1 = A1;     //Channel 1
const int statusLight = 10;   //active channel indicator light
const int audioThreshold = 5; //minimum level to be considered "active"
const int channelRelease = 5000;     //amount of time to wait before releasing an inactive channel
const long powerTimeout = 10000;    //amount of time to wait before powering off
elapsedMillis powerTimer;


#define CHANS 2    //define the number of channels to use
int audioChannels[CHANS] = {audioPin0, audioPin1};
int channelValues[CHANS];

const int SAMPLES = 300;
movingAvg audioAverages[CHANS] = {movingAvg(SAMPLES), movingAvg(SAMPLES)};    //array of moving average objects for smoothing analog input


bool channelIsActive = false;   //true when one or more channels are active

int currentChannel = -1;   //channel that is actively playing over speakers
int prevChannel = -1;

elapsedMillis channelReleaseTimer;

elapsedMillis heartBeat;

int findActiveChannel() {   //returns first active channel in the array
  int myChannel = -1;
  for (int i = 0; i < CHANS; i++) {
    if (channelValues[i] >= audioThreshold) {
      myChannel = i;
    }
  }
  return myChannel;
}




void setup() {
  // put your setup code here, to run once:
  pinMode(statusLight, OUTPUT);

  // set all the timers to 0 to start
  powerTimer = 0;
  heartBeat = 0;
  channelReleaseTimer = 0;

  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for serial to come online

  channelReleaseTimer = 0;    //set the release timer to 0

  for (int i = 0; i < CHANS; i++) { //init the moving average library THIS IS CRUCIAL! DO NOT SKIP THIS STEP!
    audioAverages[i].begin();
    audioAverages[i].reset();
  }

  aSerial.setPrinter(Serial);
  aSerial.setFilter(Level::vvv);
  aSerial.pln("Audio sensing start");
  /* Uncomment the following line to disable the output. By defalut the ouput is on. */
  // aSerial.off();
}

void loop() {

  channelIsActive = false;
  for (int i = 0; i < CHANS; i++) { //begin sampling
    int audioValue = analogRead(i) - 512;   //voltage divider shifts all values + ~2.5v
    audioValue = abs(audioValue);   //abs is a macro use on own line
    channelValues[i] = audioAverages[i].reading(audioValue);    //store and update the moving average

    //    aSerial.vvvv().p("Channel ").p(i).p(" value: ").pln(audioValue);
    //    aSerial.vvvv().p("\t avg: ").pln(channelValues[i]);

    if (channelValues[i] >= audioThreshold) {
      channelIsActive = true;
    }
  }   //end sampling


  // analize active channels
  if (channelIsActive) {    //begin analysis if there is an active channel
    int activeChannel = findActiveChannel();

    //current channel is inactive state immediately switch on
    if (currentChannel < 0) {
      currentChannel = activeChannel;
    }

    //currentChannel is still active and the audio threshold is high enough, reset the release timer
    if (channelValues[currentChannel] >= audioThreshold) {   //if the current channel is still active, ignore other channels
      channelReleaseTimer = 0;
      powerTimer = 0;
    }

    //activeChannel has changed, check to see if the release timer has expired
    if (activeChannel != currentChannel and channelReleaseTimer > 100) {    //delay 100ms before attempting to capture an inactive channel
      if (channelReleaseTimer >= channelRelease) {    //if the timer has expired, capture the inactive channel
        aSerial.vv().p("Capturing channel: ").p(currentChannel).p(" due to inactivivity and switching to channel: ").pln(activeChannel);
        currentChannel = activeChannel;
      } else {
        if (heartBeat >= 500) {
          aSerial.vv().p("Releasing inactive channel: ").p(currentChannel).p(" in ").p(channelRelease - channelReleaseTimer).pln(" ms");
        }
      }
    } 
    
  } else {    //no channel active - set active channel to < 0
//    if (channelReleaseTimer >= channelRelease) {
    if (powerTimer >= powerTimeout) {
      currentChannel = findActiveChannel();   //this will return -1 if no channel is active1
    }
  }
  

  if (currentChannel != prevChannel) {    //if a channel change happened send appropriate signals
    
    if (currentChannel > -1) {    //change the channel if the change was to an active source
      aSerial.v().p("Channel changed from: ").p(prevChannel).p(" to: ").pln(currentChannel);
      // change channels here 
    } else {
      aSerial.v().p("Input sources became inactive and changed from: ").p(prevChannel).p(" to: ").pln(currentChannel);
    }

    if ((currentChannel > -1 and prevChannel < 0) or (currentChannel < 0 and prevChannel > -1)) {
      aSerial.v().p("Setting power-on state to: ").pln(channelIsActive);
      digitalWrite(statusLight, channelIsActive);
    }

    prevChannel = currentChannel;   //record the channel change
  }


  

  if (heartBeat >= 500) {
    
    heartBeat = 0;    //reset heartbeat
  }

  delay(2);
}
