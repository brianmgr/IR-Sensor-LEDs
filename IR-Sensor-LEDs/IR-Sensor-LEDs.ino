// ATTiny85 Output
int redPin = 1;
int grnPin = 3;
int bluPin = 4;
int irSensor = A1;

// Color arrays
int rustBrown[3] = {72, 5, 0};
int pink[3] = {100, 10, 45};
int kindaPurple[3] = {96, 10, 100};
int lightBlue[3] = {0, 30, 100};
int black[3]  = { 0, 0, 0 };

// Set initial color
int redVal = black[0];
int grnVal = black[1]; 
int bluVal = black[2];

// Define zone coordinates in IR sensor output
int zone1 = 470;  // Human is closest to the IR sensor
int zone2 = 400;  // Human is one quarter to half way to the max distance from the IR sensor
int zone3 = 360;  // Human is half way to three quarters the max distance from the IR sensor
int zone4 = 320; // Human is the max distance away from the IR sensor before not being seen

// 1.8ms crossFade delay
int wait = 1.8;
int distance = 0;

// Power saver stuff
int inZoneCount = 0;
int currentZone = 0;
int lastZone = 0;

// Low Power mode stuff
int lowPowerWait = 15000;
int lowPowerCount = 0;
int lowPowerStartZone = 0; // Zone that low Power Mode started in
int lowPowerMode = 0;

// Sleep stuff
int sleepWait = 600000;
int sleepMode = 0;


void lowPower()
{
  // Set lowPowerStartZone
  lowPowerStartZone = currentZone;
  lowPowerMode = 1;
  
  // Stay in zone 4 color
  crossFade(lightBlue);

  //check every 15 seconds to see if zone has changed again
  while((lowPowerCount < 40) && (lowPowerMode == 1))
  {
    delay(lowPowerWait);
    
    lowPowerCount += 1;

    getZone();

    //break out of loop if different zone is seen
    if (currentZone != lowPowerStartZone)
    {
      lowPowerMode = 0;
      lowPowerCount = 0;
    }

   // Turn off lowPowerMode and go into sleep mode if 80 cycles have passed and no different zone was seen
   if ((lowPowerCount >= 80) && (lowPowerMode == 1))
   {
      lowPowerMode = 0;
      lowPowerCount = 0;
      sleep();
   } 
  } 
}

void sleep()
{

  sleepMode = 1;

  // Turn off LEDs
  crossFade(black);
  
  // Check distance every 10 minutes
  while (sleepMode == 1)
  {
    delay(sleepWait);
    getZone();
    if (currentZone != lowPowerStartZone)
    {
      sleepMode = 0;
      lowPowerCount = 0;
    } 
  }  
}

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
}

void loop()
{

  // Zone History
  lastZone = currentZone;

  // Check IR sensor distance to human
  getZone();

  // Zone Counter
  if (lastZone == currentZone)
  {
    inZoneCount ++;
  } else if (lastZone != currentZone)
  {
    inZoneCount = 0;
  }

  // Crossfade to color of zone
  if (currentZone == 1){
    crossFade(rustBrown);
    delay(0);
  }
  else if (currentZone == 2){
    crossFade(pink);
  }
  else if (currentZone == 3){
    crossFade(kindaPurple);
  }
  else if (currentZone == 4){
    crossFade(lightBlue);
  }

  // Go to low power mode if ~10 minutes has passed in the same zone
  if (inZoneCount >= 10)
  {
    inZoneCount = 0;
    lowPower();
  }
}

void getZone()
{
  // Check IR sensor distance to human
  distance = analogRead(irSensor);

  if (distance >= zone1){
    currentZone = 1;
  }
  else if (distance < zone1 && distance > zone2){
    currentZone = 2;  
  }
  else if (distance < zone2 && distance > zone3){
    currentZone = 3;
  }
  else if (distance < zone3){
    currentZone = 4;
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
  }
  // Update current values for next loop
  prevR = redVal; 
  prevG = grnVal; 
  prevB = bluVal;
}
