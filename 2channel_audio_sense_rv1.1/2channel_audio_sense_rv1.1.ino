#include <elapsedMillis.h>    //measure elapsed time
#include <movingAvg.h>    // moving average library
#include <advancedSerial.h>

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


#define CHANS 2    //define the number of channels to use
int audioChannels[CHANS] = {audioPin0, audioPin1};
int channelValues[CHANS];

const int SAMPLES = 300;
movingAvg audioAverages[CHANS] = {movingAvg(SAMPLES), movingAvg(SAMPLES)};    //array of moving average objects for smoothing analog input


bool channelIsActive = false;   //true when one or more channels are active

int currentChannel = -1;   //channel that is actively playing over speakers
int prevChannel = -1;

elapsedMillis channelReleaseTimer;
//elapsedMillis powerTimer;

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

  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for serial to come online

  channelReleaseTimer = 0;    //set the release timer to 0

  for (int i = 0; i < CHANS; i++) { //init the moving average library THIS IS CRUCIAL! DO NOT SKIP THIS STEP!
    audioAverages[i].begin();
    audioAverages[i].reset();
  }

  aSerial.setPrinter(Serial);
  aSerial.setFilter(Level::vvv);
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
//    if (activeChannel == currentChannel and channelValues[activeChannel] >= audioThreshold) {
    if (channelValues[currentChannel] >= audioThreshold) {   //if the current channel is still active, ignore other channels
      channelReleaseTimer = 0;
    }

    //activeChannel has changed, check to see if the release timer has expired
    if (activeChannel != currentChannel) {
      if (channelReleaseTimer >= channelRelease) {
        aSerial.vv().p("Capturing channel: ").p(currentChannel).p(" due to inactivivity and switching to channel: ").pln(activeChannel);
        currentChannel = activeChannel;
      } else {
        // this is not quite right - the debug message displays when it is not needed, but it's ok
        aSerial.vv().p("Releasing inactive channel: ").p(currentChannel).p(" in ").pln(channelRelease - channelReleaseTimer);
      }
    } 
    
  } else {    //no channel active, attempt to find an active channel or set to -1 (not active)
    if (channelReleaseTimer >= channelRelease) {
      currentChannel = findActiveChannel();   //this will return -1 if no channel is active
    } else {
      //log the inactivity countdown
      //figure out how to use the power time out here
      aSerial.vv().p("No channel active; releasing in ").pln(channelRelease - channelReleaseTimer);
    }
  }

  //action - if channel change is detected, take appropriate action
  if (currentChannel != prevChannel) {
    digitalWrite(statusLight, channelIsActive);
    aSerial.v().p("Channel changed from: ").p(prevChannel).p(" to: ").pln(currentChannel);
    aSerial.v().pln("Sendinging power-on signal");
    prevChannel = currentChannel;

    //current channel is not in off state - send appropriate channel signal
    if (currentChannel > -1) {
      aSerial.vv().p("Switching to source: ").pln(sourcesSTR[currentChannel]);
    }   //end of switch source
  }


  delay(2);
}
