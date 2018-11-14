// Turn RGB LEDs on and off depending on the distance to a human, as determined by an IR sensor.
// By Brian Mgrdichian
// Some code borrowed from https://www.arduino.cc/en/Tutorial/ColorCrossfader

// Instantiate LED pins and IR sensor pin
int redLed = 0;
int greenLed = 1;
int blueLed = 2;
int irSensor = 3;

// Color arrays
int rustBrown[3] = {255, 140, 20};    // For use in Zone 0
int kindaPurple[3] = {220, 140, 255}; // For use in Zone 1
int lightBlue[3] = {180, 255, 255};   // For use in Zone 2
int barelyThere[3] = {30, 30, 30};    // For use in Zone 3
int black[3] = {0, 0, 0};             // Black

// Set initial LED colors
int redVal = black[0];
int grnVal = black[1];
int bluVal = black[2];

// Initialize color variables
int prevR = redVal;
int prevG = grnVal;
int prevB = bluVal;

// 10ms internal crossFade delay; increase for slower fades
int wait = 10;

// Define zone coordinates in IR sensor output
int zone0 = 256;  // Human is closest to the IR sensor
int zone1 = 512;  // Human is one quarter to half way to the max distance from the IR sensor
int zone2 = 768;  // Human is half way to three quarters the max distance from the IR sensor
int zone3 = 1024; // Human is the max distance away from the IR sensor before not being seen

// Time keeping for battery savings
int movementTimer = 0; // Time tracking iterator for movement sensed by IR.
int lastMovement = 0;  // Tracking of IR distance over time.
int wait = 10;         // Amount of time in milliseconds to delay between each analog write to the LEDs.
int timerEnd = 180000; // Amount of time in milliseconds before LEDs shut off (180000 = 3 minutes).

// Turns all LEDs off
void ledsOff()
{
   analogWrite(redLed, 0);
   analogWrite(greenLed, 0);
   analogWrite(blueLed, 0);
}

// IR Sensor error indicator. Blink red LEDs if the IR sensor isn't responding
void errorFound()
{
   analogWrite(redLed, 255);
   delay(250);
   ledsOff();
   delay(250);
}

// Calculate the steps to move from one value to the next for a given LED
int calculateStep(int prevValue, int endValue)
{
   int step = endValue - prevValue; // What's the overall gap?
   if (step)
   {                      // If its non-zero,
      step = 1020 / step; //   divide by 1020
   }
   return step;
}

// Calculate and safely step increments to each LED
int calculateVal(int step, int val, int i)
{
   if ((step) && i % step == 0)
   { // If step is non-zero and its time to change a value,
      if (step > 0)
      { //   increment the value if step is positive...
         val += 1;
      }
      else if (step < 0)
      { //   ...or decrement it if step is negative
         val -= 1;
      }
   }
   // Defensive driving: make sure val stays in the range 0-255
   if (val > 255)
   {
      val = 255;
   }
   else if (val < 0)
   {
      val = 0;
   }
   return val;
}

/* crossFade() converts the percentage colors to a 
*  0-255 range, then loops 1020 times, checking to see if  
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/

void crossFade(int color[3])
{
   // Convert to 0-255
   int R = (color[0] * 255) / 100;
   int G = (color[1] * 255) / 100;
   int B = (color[2] * 255) / 100;

   int stepR = calculateStep(prevR, R);
   int stepG = calculateStep(prevG, G);
   int stepB = calculateStep(prevB, B);

   for (int i = 0; i <= 1020; i++)
   {
      redVal = calculateVal(stepR, redVal, i);
      grnVal = calculateVal(stepG, grnVal, i);
      bluVal = calculateVal(stepB, bluVal, i);

      analogWrite(redLed, redVal); // Write current values to LED pins
      analogWrite(greenLed, grnVal);
      analogWrite(blueLed, bluVal);

      delay(wait); // Pause for 'wait' milliseconds before resuming the loop
   }
   // Update current values for next loop
   prevR = redVal;
   prevG = grnVal;
   prevB = bluVal;
}

// Run setup only once, on boot or reset
void setup()
{
   pinMode(redLed, OUTPUT);   // Instatiate LED pin
   pinMode(greenLed, OUTPUT); // Instatiate LED pin
   pinMode(blueLed, OUTPUT);  // Instatiate LED pin
   pinMode(irSensor, INPUT);  // Instatiate IR Sensor
   ledsOff();                 // Turn all LEDs off
}

// Loop forever after setup
void loop()
{

   // Check IR sensor distance to human
   int distance = analogRead(irSensor);

   // If sensor isn't working, blink red LEDs
   if (!distance)
   {
      errorFound();
   }

   //If there's no movement for 3 minutes, fade LEDs out and turn them off
   if (movementTimer >= timerEnd)
   {
      crossFade(black);
      ledsOff();
   }

   // Increment movement iterators to track humans
   if ((distance) && (distance = lastMovement))
   {
      movementTimer += wait; // Add the delay time to the overall timer
   }
   else if ((distance) && (distance != lastMovement))
   {
      movementTimer = 0;
   }

   // If sensor is working, read its value, tie to a zone, cross fade to its respective color.
   else if ((distance) && ((distance <= zone0)))
   {
      crossFade(rustBrown);
   }
   else if ((distance) && ((distance > zone0 && distance <= zone1)))
   {
      crossFade(kindaPurple);
   }
   else if ((distance) && ((distance > zone1 && distance <= zone2)))
   {
      crossFade(lightBlue);
   }
   else if ((distance) && ((distance > zone2 && distance <= zone3)))
   {
      crossFade(barelyThere);
   }
}
