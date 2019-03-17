#include <elapsedMillis.h>    //measure elapsed time
#include <movingAvg.h>    // moving average library
#include <advancedSerial.h>


const int audioPin0 = A0;     //Channel 0
const int audioPin1 = A1;     //Channel 1
const int statusLight = 10;   //active channel indicator light
const int audioThreshold = 5; //minimum level to be considered "active"
const int channelRelease = 5000;     //amount of time to wait before releasing an inactive channel


#define CHANS 2    //define the number of channels to use
int audioChannels[CHANS] = {audioPin0, audioPin1};
int channelValues[CHANS]; 

const int SAMPLES = 100;
movingAvg audioAverages[CHANS] = {movingAvg(SAMPLES), movingAvg(SAMPLES)};    //array of moving average objects for smoothing analog input


bool channelIsActive = false;   //true when one or more channels are active

int currentChannel = -1;   //channel that is actively playing over speakers
int prevChannel = -1;   

elapsedMillis channelReleaseTimer;

int findActiveChannel(){    //returns first active channel in the array
  int myChannel = -1;
  for (int i=0; i < CHANS; i++) {
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
  
  for (int i=0; i < CHANS; i++) { //init the moving average library THIS IS CRUCIAL! DO NOT SKIP THIS STEP!
    audioAverages[i].begin();
  }

  aSerial.setPrinter(Serial);
  aSerial.setFilter(Level::vvv);
  /* Uncomment the following line to disable the output. By defalut the ouput is on. */
  // aSerial.off();
}

void loop() {

  channelIsActive = false;
  for (int i=0; i < CHANS; i++) {   //begin sampling
    int audioValue = analogRead(i) - 512;   //voltage divider shifts all values + ~2.5v
    audioValue = abs(audioValue);   //abs is a macro use on own line
    channelValues[i] = audioAverages[i].reading(audioValue);    //store and update the moving average

//    aSerial.vvvv().p("Channel ").p(i).p(" value: ").pln(audioValue);
//    aSerial.vvvv().p("\t avg: ").pln(channelValues[i]);

    if (channelValues[i] >= audioThreshold) {  
      channelIsActive = true;
    }
  }   //end sampling



  if (channelIsActive) {    //begin analysis if there is an active channel
//    aSerial.vvvv().pln("a channel is active");

    int activeChannel = findActiveChannel();
    
    if (currentChannel < 0) {   //if nothing is active, find the first active channel and use it 
      currentChannel = activeChannel;
//      aSerial.vvvv().p("Inactive -> Active; Channel ").pln(currentChannel);
    } 
    
    //currentChannel is still active and the audio threshold is high enough, reset the release timer
    if (activeChannel == currentChannel and channelValues[activeChannel] >= audioThreshold) {   
//      aSerial.vvvv().pln("Resetting channel release timer (activeChannel == currentChannel)");
      channelReleaseTimer = 0;
    }

     //activeChannel has changed, check to see if the release timer has expired
    if (activeChannel != currentChannel) {
      if (channelReleaseTimer >= channelRelease) {
        currentChannel = activeChannel;
      } else {
        aSerial.vv().p("Channel ").p(currentChannel).p(" inactive; releasing in ").pln(channelRelease - channelReleaseTimer);
      }
    }

  // if no channel active, attempt to find an active channel or set to -1 (not active)
  } else {
    if (channelReleaseTimer >= channelRelease) {
      currentChannel = findActiveChannel();
//        aSerial.vvv().p("Active -> Inactive; Channel ").pln(currentChannel);
    } else {
      //log the inactivity countdown
      aSerial.vv().p("Channel ").p(currentChannel).p(" inactive; releasing in ").pln(channelRelease - channelReleaseTimer);
    }
  }

  //action - if channel change is detected, take appropriate action
  if (currentChannel != prevChannel) {
    if (currentChannel > -1) {
      digitalWrite(statusLight, channelIsActive);
    }
//    } else {
//      digitalWrite(statusLight, false);
//    }
    
    aSerial.v().p("Channel changed from: ").p(prevChannel).p(" to: ").pln(currentChannel);
    prevChannel = currentChannel;
  }


  delay(2);
}
