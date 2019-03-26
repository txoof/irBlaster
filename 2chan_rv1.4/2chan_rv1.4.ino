#include <movingAvg.h>    // moving average library
#include <elapsedMillis.h>    //measure elapsed time
#include <advancedSerial.h>   //advanced serial output for better debugging
#include <IRLibSendBase.h>    //We need the base code
#include <IRLib_HashRaw.h>    //Only use raw sender

// ====REMOTE CODES====
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
uint16_t sources[4] = {powerOnOff, sourceCD, sourceCDR, sourceAUX};

//const uint16_t sources[3][RAW_DATA_LEN] = {
////  {8550, 4306, 530, 1606, 530, 566, 502, 1610, //POWER ON/OFF
////  530, 566, 502, 574, 506, 1630, 506, 566,
////  506, 1630, 506, 566, 502, 1610, 530, 566,
////  502, 1634, 506, 1606, 530, 570, 498, 1634,
////  506, 570, 510, 562, 506, 566, 502, 1634,
////  506, 1606, 530, 1610, 530, 562, 538, 538,
////  530, 542, 538, 1598, 538, 1570, 558, 542,
////  538, 538, 530, 542, 538, 1598, 530, 1578,
////  558, 1578, 562, 1000}, 
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
//  
//} ;


// ====PIN ASSIGNMENTS====
const int audioPin1 = A1;     //Channel 1
const int audioPin2 = A2;
const int debugPin = A3;     //when low, run in debug mode
const int statusLight = 10;   //active channel indicator light


//  ====CHANNEL MEASUREMENTS====
# define CHANNELS 3     //number of channels to sample from
int audioChannels[CHANNELS] = {audioPin1, audioPin1, audioPin2};      //element 0 will be ignored
int channelValues[CHANNELS] = {0, 0, 0};
const int SAMPLES = 300;
movingAvg audioAverages[CHANNELS] = {movingAvg(SAMPLES), movingAvg(SAMPLES), movingAvg(SAMPLES)};


//  ====CHANNEL VARIABLES====
int currentChannel = 0;     //channel that is currently active (0 is off)
int previousChannel = 0;    //channel that was active before change (0 is off)
const int audioThreshold = 10;      //minimum value for an "active" channel

//  ====TIMERS===
long channelReleaseTimeOut = 5000;
long powerTimeOut = 10000;
int counter = 0;
int relTime = 0;      //variable for holding release time remaining
int powerDelay = 10000;     //ms delay to wait 
const int heartBeat = 500;
elapsedMillis channelReleaseTimer = 0;
elapsedMillis powerTimer = 0;



//   ====FUNCTIONS====
void flashStatus(int repeat=10, int wait=25) {      //flash status light
  for (int i=0; i < repeat; i++) {
    digitalWrite(statusLight, true);
    delay(wait);
    digitalWrite(statusLight, false);
    delay(wait/2);
  }
}

int findActiveChannel() {   //returns first active channel in the array or -1 if none are active
  int myChannel = 0;
  for (int i = 0; i < CHANNELS; i++) {
    if (channelValues[i] >= audioThreshold) {
      myChannel = i;
      aSerial.vvvv().p("CAUGHT CHANNEL: ").pln(i);
      break;
    }
  }
  return myChannel;
}

//broadcast an IR signal 
//void sendCode(uint16_t code) {
//void sendCode() {
//  int repeat = 20;
//  for (int i=0; i < repeat; i++) {
////    mySender.send(powerOnOff, RAW_DATA_LEN, 36);
//    Serial.println(i);  
//    delay(5);
//  }
//  aSerial.vv().pln("   sent code");
//}

void sendCode(int mySource) {
  for (int i=0; i < 20; i++) {
    Serial.println(i);
    mySender.send(powerOnOff,RAW_DATA_LEN,36);    
  }
}

void setup() {
  delay(1000); //delay in case of runaway loop - allow programmer time to interrupt
  flashStatus(5, 100);
  //  ====PIN SETUP====
  pinMode(statusLight, OUTPUT);
  pinMode(debugPin, INPUT);
  pinMode(audioPin1, INPUT);
  pinMode(audioPin2, INPUT);
  
  bool debugMode = false;

  if (!digitalRead(debugPin)) {
    debugMode = true;
  }

  if (debugMode) {
    Serial.begin(9600);
    delay(2000); //delay for serial to come online
  
    aSerial.setPrinter(Serial);
    aSerial.setFilter(Level::vvv);
  
    aSerial.pln("sketch starting");
  } //end if debug mode

  for (int i=0; i < CHANNELS; i++) { //init and reset the moving averages
    audioAverages[i].begin();
    audioAverages[i].reset();
  }

  powerTimer = powerTimeOut +1;        //ensure that powerstate can immediately be changed
  channelReleaseTimer = channelReleaseTimeOut + 1;      //ensure that channel can be released immediately on startup


}

void loop() {
//  aSerial.v().pln(powerTimer);
//  sendCode(sources[0]);
  int audioValue = 0;
  int activeChannel = 0;

 
  
  for (int i=0; i < CHANNELS; i++) { //sample channels
    if (i > 0) {
      int audioValue = analogRead(i) - 512; //voltage divider on amp circut shifts all values + ~2.5V
      audioValue = abs(audioValue);     //for some reason abs() is a macro and needs to be on its own line
      channelValues[i] = audioAverages[i].reading(audioValue);    //update and store the moving average for each channel
    }

    if (counter >= heartBeat) {
      aSerial.vvvv().p("Channel ").pln(i);
      aSerial.vvvv().p("\t value: ").pln(audioValue);
      aSerial.vvvv().p("\t avg: ").pln(channelValues[i]);
      aSerial.vvvv().pln();
    }
  }
  if (counter >= heartBeat) {
    aSerial.vvvv().pln("======================");
  }


  if (channelValues[currentChannel] >= audioThreshold) {     //reset the channel release timer if the channel is active
    channelReleaseTimer = 0;
    powerTimer = 0;
  } else {      //else find the first active channel
    activeChannel = findActiveChannel(); 
    
    // DEBUGGING - show countdown to channel release
    if (channelReleaseTimer < channelReleaseTimeOut and counter >= 500) {
      aSerial.vvv().p("channel: ").p(currentChannel).pln(" inactive");
      relTime = (channelReleaseTimeOut - channelReleaseTimer);
      aSerial.vvv().p("  releasing in: ").p(channelReleaseTimeOut - channelReleaseTimer).pln("ms");  
    } //END DEBUGGING
    
    if (activeChannel != currentChannel and channelReleaseTimer >= channelReleaseTimeOut) {     //change the channel
      aSerial.v().p("Changing channel from: ").p(currentChannel).p(" to: ").pln(activeChannel);
      currentChannel = activeChannel;
      channelReleaseTimer = 0;
      powerTimer = 0;
    }
  }       //END else find active channel

  if (currentChannel > 0) {      //reset the power timeout 
    powerTimer = 0;
  }  

  if (previousChannel != currentChannel) { //check for a channel change and send codes
    aSerial.vvv().pln("channel change detected");
    if (previousChannel == 0) {
      digitalWrite(statusLight, true);
      aSerial.vv().pln("Power state change - send power on code");
//      sendCode(0);     //send power on/off code ///WTF this breaks the sketch completely?
//      for (int i=0; i < 20; i++) {
//        Serial.println(i);
//        mySender.send(powerOnOff,RAW_DATA_LEN,36);    
//        delay(5);
//      }
      aSerial.vv().pln("delay for time to allow receiver to power up");
      delay(powerDelay);
      
    }

    if (currentChannel < 1 and powerTimer >= powerTimeOut) {
      digitalWrite(statusLight, false);
      aSerial.vv().pln("Power state change - send power off code");
//      sendCode(0);     //send power on/off code

      previousChannel = currentChannel;
    }

    if (currentChannel > 0) {
      aSerial.vv().p("Send code for switch to channel: ").pln(currentChannel);
//      sendCode(sources[currentChannel]);      //send code for this channel    
      previousChannel = currentChannel;     //set these equal to prevent power flip-flopping

    }
  } //END check for channel change

  if (counter >= heartBeat) {
    aSerial.vvv().pln("heartbeat\n");
    counter = 0;
  }
  counter = counter + 1;
 
  delay(2);
} 
