/*
 * Author(s): Braydon Bieber
 * Description: Interupts - Measure Pulse Width, Two Channels
 */


/*
 * Name - Channel
 * ---------------------
 * Right Analog Horiz        - 1
 * Right Analog Vert         - 2
 * Left Analog Vert          - 3
 * Left Analog Horiz         - 4
 * VRA Knob Failsafe/Unused  - 5
 * VRB Knob Failsafe/Unused  - 6
 */

volatile long unsigned tchannel_2[3] = {0, 1500, 1500}; // 1500uS is the neutral position, volatile is necessary for the interrupt
volatile long unsigned tchannel_3[3] = {0, 1500, 1500}; 
volatile long unsigned tchannel_6[3] = {0, 1000, 1000}; // 1000 motor off 2000 motor on 

float prevWeight = 0.95;

void setup() {
  
  /*Relay For Ignition*/
  pinMode(12, OUTPUT);
  pinMode(5, INPUT); 
  
  /*Motor Driver*/
  pinMode(11, OUTPUT); // 490Hz 
  pinMode(9, OUTPUT);  // 490HZ
  
  /*OUTPUT & Serial*/  
  Serial.begin(9600);

  /*Receiver Channels*/      
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), IR2, CHANGE); // Read both rising and falling edge 
  attachInterrupt(digitalPinToInterrupt(3), IR3, CHANGE); // Only Pin 2 & 3 Are Supported For This Hardware Interrupt On Nano 
}

bool c6HIGH = false;

void loop() {
  
 /* Measuring Relay Signal */
  if(digitalRead(5) && !c6HIGH){
    tchannel_6[0] = micros();
    c6HIGH = true;
  }
  else if(!digitalRead(5) && c6HIGH){
    tchannel_6[1] = micros();
    tchannel_6[2] = safe_range(tchannel_6);
    c6HIGH = false;
  }
  if(tchannel_6[2] > 1650){ 
    closeRelay(); // Deadzone - David Jochems 
  }
  else if(tchannel_6[2] < 1350) {
    openRelay(); // Deadzone - David Jochems 
  }

  // correcting out-of-bounds values
  if (tchannel_2[2] > 2000) tchannel_2[2] = 2000;
  if (tchannel_2[2] < 1000) tchannel_2[2] = 1000;
  if (tchannel_3[2] > 2000) tchannel_3[2] = 2000;
  if (tchannel_3[2] < 1000) tchannel_3[2] = 1000;
      
  // rounding - if sticks are close to each other, make them the same value (the avg)
  if (tchannel_2[2] - tchannel_3[2] < 25 && tchannel_2[2] - tchannel_3[2] > -25)
  {
    int avg = (tchannel_2[2] + tchannel_3[2]) / 2;
    tchannel_2[2] = avg;
    tchannel_3[2] = avg;
  }
  
  /* Driving Functionality*/ 
  int rightMotor = map(tchannel_2[2], 1000, 2000, 128, 255); // Mapping - 1000 to 2000 to 128 to 255 - David and Braydon 
  int leftMotor  = map(tchannel_3[2], 1000, 2000, 128, 255);  
  analogWrite(9, rightMotor); // Apply mapped duty cycle 
  analogWrite(11, leftMotor);
}


void IR2() {
  measure(2, tchannel_2);
}

void IR3() {
  measure(3, tchannel_3);
}

void measure(int pin, volatile long unsigned times[]) {  
  if(digitalRead(pin)==HIGH)
    times[0] = micros();
  else{
    times[1] = micros();
    times[2] = times[2] * prevWeight + safe_range(times) * (1 - prevWeight); // smoothing using weighted avg to prevent quick speed changes 
  }
}

volatile long unsigned safe_range( volatile long unsigned a[]) {   
  if(a[1] - a[0] > 2100 || a[1] - a[0] < 900) // Overflow had nothing to do with wrong value being returned - Braydon 
      return a[2];                            // Reuse previous value to drive motor 
  return a[1] - a[0];                         // No problem with the difference, return it 
}

void closeRelay(){
  digitalWrite(12, LOW); 
}

void openRelay(){
  digitalWrite(12, HIGH);
}
