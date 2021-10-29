const int TRIG = 10;
const int ECHO = 11;

const int STOP = 7;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(STOP, OUTPUT);

  analogWrite(TRIG, 128);
}

void loop() {
  // put your main code here, to run repeatedly:

  int pulse = pulseIn(ECHO, HIGH, 15000);
  int stop_ = 0;

  if (pulse <= 500 && pulse > 0)
  {
    stop_ = HIGH;
    digitalWrite(STOP, stop_);
  }
  else
  {
    stop_ = LOW;
    digitalWrite(STOP, stop_);
  }

  Serial.println(stop_); 
}
