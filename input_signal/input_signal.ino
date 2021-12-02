volatile long unsigned tchannel_2[4] = {0, 1500, 1500, 0}; // 1500uS is the neutral position, volatile is necessary for the interrupt
volatile long unsigned tchannel_3[4] = {0, 1500, 1500, 0}; 
volatile long unsigned tchannel_6[3] = {0, 1000, 1000}; // 1000 motor off 2000 motor on 
long unsigned prev2 = 1500, prev3 = 1500;

float weight = 0.99;
int defaultval;
#define forwardCap 1850  // forward speed cap
#define backwardCap 1150 // backward speed cap
#define deadzone 25

void setup() {
  
  /*Relay For Ignition*/
  pinMode(12, OUTPUT);
  pinMode(5, INPUT); 
  pinMode(6, INPUT);
  
  /*Motor Driver*/
  pinMode(11, OUTPUT); // 490Hz 
  pinMode(9, OUTPUT);  // 490HZ
  
  /*OUTPUT & Serial*/  
  Serial.begin(9600);

  /*Default Value for Motors */
  defaultval  = map(1500, 1000, 2000, 128, 255);
  
  /*Receiver Channels*/      
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), IR2, CHANGE); // Read both rising and falling edge 
  attachInterrupt(digitalPinToInterrupt(3), IR3, CHANGE); // Only Pin 2 & 3 Are Supported For This Hardware Interrupt On Nano 
}

bool c6HIGH = false; // is c6 initialized

void loop() {
  
 /* Measuring Channel 6 Relay Signal */
  if(digitalRead(5) && !c6HIGH){
    tchannel_6[0] = micros();
    c6HIGH = true;
  }
  else if(!digitalRead(5) && c6HIGH){
    tchannel_6[1] = micros();
    tchannel_6[2] = tchannel_6[1] - tchannel_6[0];
    c6HIGH = false;
  }

  /*Dead Zones For Relay Switch */
  if(tchannel_6[2] > 1650){ 
    closeRelay(); // Deadzone - David Jochems 
  }
  else if(tchannel_6[2] < 1350) {
    openRelay(); // Deadzone - David Jochems 
  }

/*Smoothing Operation On Output*/
  if (tchannel_2[3])
  {
//    if (tchannel_2[1] - tchannel_2[0] <= forwardCap && tchannel_2[1] - tchannel_2[0] >= backwardCap)
//    {
      if (abs(tchannel_2[1] - tchannel_2[0] - 1500) >= abs(tchannel_3[2] - 1500)) // accelerating
        tchannel_2[2] = tchannel_2[2] * weight + (tchannel_2[1] - tchannel_2[0]) * (1 - weight);
      else // deccelerating
        tchannel_2[2] = tchannel_2[1] - tchannel_2[0];
//    }
  }
  if (tchannel_3[3])
  {
//    if (tchannel_3[1] - tchannel_3[0] <= forwardCap && tchannel_3[1] - tchannel_3[0] >= backwardCap)
//    {
      if (abs(tchannel_3[1] - tchannel_3[0] - 1500) >= abs(tchannel_3[2] - 1500))
        tchannel_3[2] = tchannel_3[2] * weight + (tchannel_3[1] - tchannel_3[0]) * (1 - weight);
      else
        tchannel_3[2] = tchannel_3[1] - tchannel_3[0];
//    }
  }

  /*Decrease Processor Performance*/
  Serial.print(tchannel_2[2]);
  Serial.println(tchannel_3[2]);
  
  /* rounding - if sticks are close to each other, make them the same value (the avg)*/
//  if (tchannel_2[2] - tchannel_3[2] < 100 && tchannel_2[2] - tchannel_3[2] > -100)
//  {
//    int avg = (tchannel_2[2] + tchannel_3[2]) / 2;
//    tchannel_2[2] = avg;
//    tchannel_3[2] = avg;
//  }

  /*correcting out-of-bounds values*/
  if (tchannel_2[2] > forwardCap) tchannel_2[2] = prev2;
  if (tchannel_2[2] < backwardCap) tchannel_2[2] = prev2;
  if (tchannel_3[2] > forwardCap) tchannel_3[2] = prev3;
  if (tchannel_3[2] < backwardCap) tchannel_3[2] = prev3;

  prev2 = tchannel_2[2];
  prev3 = tchannel_3[2];

  int rightMotor, leftMotor;
  /* Driving Functionality*/ 
  if (tchannel_2[2] > 1500 + deadzone)
    rightMotor = map(tchannel_2[2], 1500 + deadzone, 2000, 192, 128);
  else if (tchannel_2[2] < 1500 - deadzone)
    rightMotor = map(tchannel_2[2], 1000, 1500 - deadzone, 255, 192);
  else
    rightMotor = map(1, 0, 2, 255, 128);

  if (tchannel_3[2] > 1500 + deadzone)
    leftMotor = map(tchannel_3[2], 1500 + deadzone, 2000, 192, 255);
  else if (tchannel_3[2] < 1500 - deadzone)
    leftMotor = map(tchannel_3[2], 1000, 1500 - deadzone, 128, 192);
  else
    leftMotor = map(1, 0, 2, 128, 255);
    
  bool Stop = digitalRead(6);

  /*Drive Motors And Prevent Forward Collision*/
  if(!Stop || (leftMotor < defaultval && rightMotor < defaultval) ) // Obstacle Stop 
  {  
    analogWrite(9, rightMotor); // Apply mapped duty cycle 
    analogWrite(11, leftMotor);
  }
  else{
    analogWrite(9, defaultval);
    analogWrite(11, defaultval);
  }
}


void IR2() {
  if (digitalRead(2) == HIGH){
    tchannel_2[0] = micros();
    tchannel_2[3] = 0; // not ready for calculation 
  }
  else{
    tchannel_2[1] = micros();
    tchannel_2[3] = 1; // ready for calculation 
  }
}

void IR3() {
  if (digitalRead(3) == HIGH){
    tchannel_3[0] = micros();
    tchannel_3[3] = 0; // not ready for calculation 
  }
  else{
    tchannel_3[1] = micros();
    tchannel_3[3] = 1; // ready for calculation 
  }
}

volatile long unsigned safe_range( volatile long unsigned a[]) {   
  if(a[1] - a[0] > forwardCap || a[1] - a[0] < backwardCap) // Overflow had nothing to do with wrong value being returned - Braydon 
      return a[2];                            // Reuse previous value to drive motor 
  return a[1] - a[0];                         // No problem with the difference, return it 
}

void closeRelay(){
  digitalWrite(12, LOW); 
}

void openRelay(){
  digitalWrite(12, HIGH);
}
