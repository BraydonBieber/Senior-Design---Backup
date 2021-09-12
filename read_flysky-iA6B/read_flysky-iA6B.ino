/*
 * Author(s): Erick Oduniyi
 * Description: Reading PWM output from Flysky-iA6B reciever channels
 * Adapted from https://dronebotworkshop.com/radio-control-arduino-car/
 * See @input_signal.ino file for the corresponding handheld transmitter channels
 */


 
// Map Flysky-iA6B channels to Arduino-Nano digital pins (PWM)
// Ex: #define rcChannel digitalPin
#define CH1 3
#define CH2 5
#define CH3 6
#define CH4 9
#define CH5 10
#define CH6 11

// Represent control stick reciever values as integers
// Values range: Min = 1000, Neutral = 1500, Max = 2000
unsigned long int ch1Value;
unsigned long int ch2Value;
unsigned long int ch3Value;
unsigned long int ch4Value;
unsigned long int ch5Value;
unsigned long int ch6Value;

// Set timeout to length of the signal period (i.e., 20 ms)
const unsigned long int timeOut = 20000; 

// Initialize serial monitor and pins as input
void setup() {
  Serial.begin(9600);
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);
}

// Main program to be repeated indefinitely
void loop() {
  ch1Value = pulseIn(CH1, HIGH, timeOut);
  ch2Value = pulseIn(CH2, HIGH, timeOut);
  ch3Value = pulseIn(CH3, HIGH, timeOut);
  ch4Value = pulseIn(CH4, HIGH, timeOut);
  ch5Value = pulseIn(CH5, HIGH, timeOut);
  ch6Value = pulseIn(CH6, HIGH, timeOut);
  
  // Print the values of the first four channels to serial monitor
  char buffer[100];
  sprintf(buffer, "channel 1: %u, channel 2: %u, channel 3: %u, channel 4: %u", ch1Value, ch2Value, ch3Value, ch4Value);
  Serial.println(buffer);

  // Wait for 1 second
  delay(1000);
}
