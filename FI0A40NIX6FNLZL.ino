
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <Bridge.h>
#include <BlynkSimpleYun.h>
#include <Servo.h>
#include <SimpleTimer.h>
#include <MotionGenerator.h>

// Insert Auth Token from the Blynk App.
char auth[] = "Paste here your token";

const int servoPin1 = 3;      // pin servo 1
const int servoPin2 = 5;      // pin servo 2
const int servoPin3 = 6;      // pin servo 3
const int servoPin4 = 9;      // pin servo 4
const int servoPin5 = 10;     // pin servo 5

const int blueLedPin = 7;     // pin BLUE led
const int redLedPin = 8;      // pin RED led

boolean autoMode = false;    // automatic mode
int s1=100, s2=90, s3=70, s4=50, s5=120;   // inital Robot pose
float y1=s1, y2=s2, y3=s3, y4=s4, y5=s5;

const int learnSize = 12;         // save up to 12 points
int s1Learn[learnSize], s2Learn[learnSize];
int s3Learn[learnSize], s4Learn[learnSize], s5Learn[learnSize];

int kPoints = 0;
int i = 0;    

Servo servo1, servo2, servo3, servo4, servo5;
SimpleTimer timer;

/*
 * Initialization
 *
 * @param int aVelMax maximum velocity (units/s)
 * @param int aAccMax maximum acceleration (units/s^2)
 * @param int aInitPos initial position (units)
 *
 */
MotionGenerator *servo1Profile = new MotionGenerator(150, 300, 0);
MotionGenerator *servo2Profile = new MotionGenerator(150, 300, 0);
MotionGenerator *servo3Profile = new MotionGenerator(150, 500, 0);
MotionGenerator *servo4Profile = new MotionGenerator(150, 500, 0);

//***************************************************************************
void setup()
{
  Serial.begin(9600);
  Serial.println('a');

  // Atach servos
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  servo5.attach(servoPin5);

  // Set Led pins
  pinMode(blueLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  // Start Blynk connection
  Blynk.begin(auth);
  // Or specify server using one of those commands:
  //Blynk.begin(auth, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, server_ip, port);

  while (Blynk.connect() == false) {
  // Wait until connected
  }

  // Turn BLUE led on
  digitalWrite(blueLedPin, HIGH);

  // Initialize Robot
  servo1.write(s1);
  servo2.write(s2);
  servo3.write(s3);
  servo4.write(s4);
  servo5.write(s5);

  timer.setInterval(10L, filterInput);
  timer.setInterval(225L, autoModeFcn);

}
//***************************************************************************

// This function will be called every time Joystick Widget
// in Blynk app writes values to the Virtual Pin
BLYNK_WRITE(V1)
{
  // Read joystick values
  if (!autoMode) {
    s1 = param[0].asInt();
    s2 = param[1].asInt();
  }
}

BLYNK_WRITE(V2)
{
  // Read joystick values
  if (!autoMode) {  
    s4 = 180 - param[0].asInt();
    s3 = 180 - param[1].asInt();
  }
}

BLYNK_WRITE(V3)
{  
  if (!autoMode) {  
    s5 = param.asInt(); // assigning incoming value
  }
}

// This function will be called every time the AUTO button is pressed
BLYNK_WRITE(V4)
{  
  autoMode = param.asInt(); // assigning incoming value

  // Turn on RED led if autoMode is true
  digitalWrite(redLedPin, autoMode);
}

// This function will be called every time the LEARN button is pressed
BLYNK_WRITE(V5)
{  

  if (!autoMode)
  {
    s1Learn[kPoints] = y1;
    s2Learn[kPoints] = y2;
    s3Learn[kPoints] = y3;
    s4Learn[kPoints] = y4;
    s5Learn[kPoints] = y5;
    kPoints++;
    Blynk.virtualWrite(V6, kPoints);  // show number of points in the app
  }
  
}

// This function will be called every time the RESET button is pressed
BLYNK_WRITE(V0)
{ 
  autoMode = false;
  kPoints = 0;    // Reset the counter

  Blynk.virtualWrite(V4, LOW);
  Blynk.virtualWrite(V6, kPoints);  
}

//***************************************************************************
// Automatic mode function
void autoModeFcn()
{
  if (autoMode)
  { 
    Blynk.run(); // Run Blynk
       
    digitalWrite(blueLedPin, LOW);          // Turn off BLUE led
    digitalWrite(redLedPin, !digitalRead(redLedPin));   // Toggle RED led if autoMode is true    

    int err;
    s1 = s1Learn[i];
    s2 = s2Learn[i];
    s3 = s3Learn[i];
    s4 = s4Learn[i];
    s5 = s5Learn[i]; 
      
    err = abs(s1-y1) + abs(s2-y2) + abs(s3-y3) + abs(s4-y4) + abs(s5-y5);
   
    if (err < 5)  // if error is arbitrarly small move to next point
    {         

      i++;    // increment index
      if (i >= kPoints) {
        i = 0;    // reset index
      } 
    }    
  }
  
  else
  {   
    i = 0;
    digitalWrite(redLedPin, LOW);     // Turn off RED led
    digitalWrite(blueLedPin, HIGH);   // Turn on BLUE led
  }

}

//***************************************************************************
// Filter the input
void filterInput()
{

    // Servo 3
    y1 = servo1Profile->update(s1); servo1.write(y1); 
    y2 = servo2Profile->update(s2); servo2.write(y2);
    y3 = servo3Profile->update(s3); servo3.write(y3);
    y4 = servo4Profile->update(s4); servo4.write(y4);
    y5 = s5;                        servo5.write(y5);

    // Next, Read/Write on serial with Matlab to do forward/inverse kinematics ;)
    // Send data only when you receive data
    if (Serial.available() > 0)
    {
      int val = Serial.read();  // read the incoming byte
      if (val == 'R')
      {
        // Write to serial
        Serial.print(y1); Serial.print(" ");
        Serial.print(y2); Serial.print(" ");
        Serial.print(y3); Serial.print(" ");
        Serial.print(y4); Serial.print(" ");
        Serial.println(y5);
      } 
    }
    
    // Check if profile is finished
    // if (trapezoidalProfile->getFinished()) {};

    // Reset internal state
    // trapezoidalProfile->reset();
}


//***************************************************************************
void loop()
{    
  timer.run();      // Initiates SimpleTimer
  if (!autoMode) {  // to reduce number of calls to Blynk in automatic mode
    Blynk.run();    // Initiates Blynk
  }
}
//***************************************************************************
