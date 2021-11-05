const int TRIG = 10;
const int ECHO = 11;

const int STOP = 7;

int stop_ = LOW;

void setup() {
  // put your setup code here, to run once:


  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(STOP, OUTPUT);

  analogWrite(TRIG, 128);

  attachInterrupt(digitalPinToInterrupt(2), IR2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), IR3, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:

  int pulse = pulseIn(ECHO, HIGH, 15000);
  
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
}

void IR2() {
  if (!stop_)
  {
    digitalWrite(5, digitalRead(2));
    digitalWrite(6, digitalRead(3));
  }
  else
    analogWrite(5, 0);
}

void IR3() {
  if (!stop_)
  {
    digitalWrite(6, digitalRead(3));
    digitalWrite(5, digitalRead(2));
  }
  else
    analogWrite(6, 0);
}
