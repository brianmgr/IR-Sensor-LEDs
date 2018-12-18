//Mega2560
// Instantiate LED pins and IR sensor pin
int redLed = 2;
int greenLed = 3;
int blueLed = 4;

// Color arrays
int rustBrown[3] = {0, 114, 225};
int naranja[3] = {40, 74, 225};
int kindaPurple[3] = {35, 115, 0};
int lightBlue[3] = {95, 20, 0};
int black[3] = {0,0,0};

void lightUp(int color[3])
{ 
  analogWrite(redLed, color[0]);
  analogWrite(greenLed, color[1]);
  analogWrite(blueLed, color[2]);
}

// Run setup only once, on boot or reset
void setup()
{
  pinMode(redLed, OUTPUT);   // Instatiate LED pin
  pinMode(greenLed, OUTPUT); // Instatiate LED pin
  pinMode(blueLed, OUTPUT);  // Instatiate LED pin
}

// Loop forever after setup
void loop()
{

  delay(1000);
  lightUp(lightBlue);

  delay(1000);
  lightUp(kindaPurple);

  delay(1000);
  lightUp(naranja);

  delay(1000);
  lightUp(rustBrown);
  
}
