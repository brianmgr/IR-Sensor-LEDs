// Mega2560 Output
int redPin = 2;   // Red LED,   connected to digital pin 9
int grnPin = 3;  // Green LED, connected to digital pin 10
int bluPin = 4;  // Blue LED,  connected to digital pin 11
int irSensor = A0;

//// ATTiny85 Output
//int redLed = 1;
//int greenLed = 3;
//int blueLed = 4;
//int irSensor = A1;

// Color arrays
int rustBrown[3] = {72, 25, 5};
int naranja[3] = {100, 50, 0};
int kindaPurple[3] = {96, 10, 100};
int lightBlue[3] = {0, 30, 100};
int black[3]  = { 0, 0, 0 };

// Set initial color
int redVal = black[0];
int grnVal = black[1]; 
int bluVal = black[2];

// Define zone coordinates in IR sensor output
int zone3 = 480;  // Human is closest to the IR sensor
int zone2 = 400;  // Human is one quarter to half way to the max distance from the IR sensor
int zone1 = 380;  // Human is half way to three quarters the max distance from the IR sensor
int zone0 = 300; // Human is the max distance away from the IR sensor before not being seen

int wait = 1;      // 10ms internal crossFade delay; increase for slower fades
int hold = 0;       // Optional hold when a color is complete, before the next crossFade
int DEBUG = 0;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 100; // How often should DEBUG report?

// Initialize color variables
int prevR = redVal;
int prevG = grnVal;
int prevB = bluVal;

// Set up the LED outputs
void setup()
{
  pinMode(redPin, OUTPUT);   // sets the pins as output
  pinMode(grnPin, OUTPUT);   
  pinMode(bluPin, OUTPUT); 

  if (DEBUG) {           // If we want to see values for debugging...
    Serial.begin(9600);  // ...set up the serial ouput 
  }
}

// Main program: list the order of crossfades
void loop()
{
  delay (10);
  
  // Check IR sensor distance to human
  int distance = analogRead(irSensor);

  if (DEBUG) {
    Serial.println(distance);
  }
  
  // If sensor is working, read its value, tie to a zone, cross fade to its respective color.
  if (distance <= zone0){
    crossFade(rustBrown);
  }
  else if (distance > zone0 && distance <= zone1){
    crossFade(naranja);
  }
  else if (distance > zone1 && distance <= zone2){
    crossFade(kindaPurple);
  }
  else if (distance > zone2){
    crossFade(lightBlue);
  }

}

int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  return step;
}

int calculateVal(int step, int val, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;           
    } 
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    } 
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

void crossFade(int color[3]) {
  // Convert to 0-255
  int R = (color[0] * 255) / 100;
  int G = (color[1] * 255) / 100;
  int B = (color[2] * 255) / 100;

  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G); 
  int stepB = calculateStep(prevB, B);

  for (int i = 0; i <= 1020; i++) {
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    analogWrite(redPin, (255 - redVal));   // Write current values to LED pins
    analogWrite(grnPin, (255 - grnVal));      
    analogWrite(bluPin, (255 - bluVal)); 

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop

    if (DEBUG) { // If we want serial output, print it at the 
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(redVal);
        Serial.print(" / ");
        Serial.print(grnVal);
        Serial.print(" / ");  
        Serial.println(bluVal); 
      } 
      DEBUG += 1;
    }
  }
  // Update current values for next loop
  prevR = redVal; 
  prevG = grnVal; 
  prevB = bluVal;
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}
