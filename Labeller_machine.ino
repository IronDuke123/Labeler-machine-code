#include <Servo.h>

/*  
 *   Labeling machine: code is work in progress
 *   Digital output pwm to speed controller for roller winder
 *   Infared sensor for label timing,need to stop label primed for next label.. no smoothing added yet, smoothing may effect timing and need adjustment if added later
 *   Prime button to run winder to start paper on spool. Using digital input with no smoothing, hopefully not too jumpy
 *   
 *  code wrote 3/29/18 
 */ 
byte Winderspeed = 180 ;  //0=Full reverse||| 90= Stop ||| 180 Full forward
byte WinderPin = 11 ;  // Digital pin for servo control
byte PrimeButton = 2 ;  // button wired to ground to prime roller
byte SensorPin= 3 ;     // pin for infarad sensor
int LabelSense = 500 ;  // sensitivity of infarad sensor.. may need  to average some readings if too jumpy
Servo WinderServo;  // create servo WinderServo to control speed controller for Label winder motor


void setup() {
  
  WinderServo.attach(WinderPin);  // attaches the servo on pin 9 to the servo object
  pinMode(PrimeButton, INPUT_PULLUP);   // sets Prime button input pin
  pinMode(SensorPin, INPUT) ;   // set up input pin for infarad sensor
}

void loop() {
  // put your main code here, to run repeatedly:
if( analogRead(SensorPin) < LabelSense || digitalRead(PrimeButton) == LOW ) {
  WinderServo.write(Winderspeed) ;
}
else WinderServo.write(90);


}
