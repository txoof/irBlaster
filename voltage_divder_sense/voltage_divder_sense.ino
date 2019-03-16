const int audioPin0 = A0;
long audioValue0 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  audioValue0 = abs(analogRead(audioPin0) - 512);
 
  Serial.println(audioValue0);
  
  delay(100);
}
