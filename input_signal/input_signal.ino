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

volatile long unsigned tchannel_2[3] ={0, 1500, 1500};  // 1500uS is the neutral position, volatile is necessary for the interrupt
volatile long unsigned tchannel_3[3] ={0, 1500, 1500}; 

void setup() {
  
  /* Temporary Receiver Power Supply*/
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);

  /*OUTPUT & Serial*/  
  Serial.begin(9600);

  /*Receiver Channels*/      
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), IR2, CHANGE); // Read both rising and falling edge 
  attachInterrupt(digitalPinToInterrupt(3), IR3, CHANGE); // Only Pin 2 & 3 Are Supported For This Hardware Interrupt On Nano 
}

void loop() {
  char buffer[100];
  sprintf(buffer, "channel 2: %u  channel 3: %u", (long unsigned)tchannel_2[2]);
  Serial.print(buffer);
  Serial.println((long unsigned)tchannel_3[2]);
}


void IR2() {
  measure(2, tchannel_2);
  measure(3, tchannel_3);
}

void IR3() {
  measure(3, tchannel_3);
}

void measure(int pin, volatile long unsigned times[]) {  
  if(digitalRead(pin)==HIGH)
    times[0] = micros();
  else{
    times[1] = micros();
    times[2] = safe_range(times);
  }
}

// handles the return value's eventual overflow from micros() 
volatile long unsigned safe_range( volatile long unsigned a[]) {   
   
  if(a[1] - a[0] > 3000) // condition will be true when micros has overflowed
      return a[0] - a[1];
      
  return a[1] - a[0];
}
