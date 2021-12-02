const int TRIG = 9;
const int ECHO = 2;
const int STOP = 6;

int stopDistance = 75;  // measured in cm, kind of

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(STOP, OUTPUT);
  analogWrite(TRIG, 128);
  Serial.begin(9800);
 // attachInterrupt(digitalPinToInterrupt(2), IR2, CHANGE); we will just use pulsein because we are integrating one sensor only 
}

void loop() {
  float pulse = pulseIn(ECHO, HIGH, 15000)*170/10000; // Convert pulse width to distance in cm
  if(pulse < stopDistance && pulse != 0) // 100 cm is 1m, also ignore timeout values of 0 since the signal echo did not return 
    digitalWrite(STOP, HIGH);
  else
    digitalWrite(STOP, LOW);
}
