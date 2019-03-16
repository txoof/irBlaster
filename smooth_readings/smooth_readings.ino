#include <movingAvg.h>

//Analog pins for measuring the audio singnal
const int audioPin0 = A0;        //Channel 0
const int audioPin1 = A1;        //Channel 1

#define CHANS 2
int audioChannels[CHANS] = {audioPin0, audioPin1};
int audioValues[CHANS];

movingAvg audioAverages[CHANS] = {movingAvg(10), movingAvg(10)};
//movingAvg audio0(10);
//movingAvg audio1(10);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for serial to come online
  for (int i=0; i < CHANS; i++) {
    audioAverages[i].begin();
//    audio0.begin();
//    audio1.begin();
  }

}

void loop() {
  for (int i=0; i < CHANS; i++) {
    int audioValue = analogRead(i) - 512;
    audioValue = abs(audioValue);
    audioValues[i] = audioAverages[i].reading(audioValue);
//    audioValues[i] = audio0.reading(audioValue);

    Serial.print("Channel: ");
    Serial.println(i);
//    Serial.println(audioAverages[i]);
    Serial.println(audioValues[i]);
  }
  Serial.println("**********");
}
