  #include <Servo.h>
#include <LiquidCrystal.h>
#include <LCDKeypad.h>

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
byte PrimeButton = 12 ;  // button wired to ground to prime roller
byte SensorPin= 3 ;     // pin for infarad sensor for windup roller
//byte RollerSensor = 4 ;
byte RollerServoVcc = 2; // servo control pin for roller motor
byte RollerServoVCC = 0 ; // RollerServo VCC
byte RollerServoGND = 1 ; // gnd
byte RollerPin = 2 ;
int LabelSense = 650 ;  // sensitivity of infarad sensor.. may need  to average some readings if too jumpy
Servo WinderServo;  // create servo WinderServo to control speed controller for Label winder motor
Servo RollerMotor ;
LCDKeypad lcd;
int block_delay(150);
int repeat_delay(100);
byte Mode = 0 ;
  unsigned long startMillis;  //some global variables available anywhere in the program
  unsigned long currentMillis;
  const unsigned long period = 600;  //how long to wait running roller until label is fed
  unsigned long StartTime ;
  unsigned long EndTime ;
  unsigned long LabelTime;

void setup() {
  lcd.begin(16,2);
  WinderServo.attach(WinderPin);  // attaches the servo on pin 9 to the servo object
  RollerMotor.attach(RollerPin);
  pinMode(PrimeButton, INPUT_PULLUP);   // sets Prime button input pin
  pinMode(SensorPin, INPUT) ;   // set up input pin for infarad sensor for label roller
  pinMode(13,OUTPUT); // trying to use icsp pins for easy connection for prime button..
  digitalWrite(13,LOW); //12 and 13 are next to each other.. tying pin 13 to gnd for prime switch and RollerServoGND
  pinMode(A1,OUTPUT);  // VCC for label sensor
  pinMode(A2,OUTPUT);  // GND for label sensor
  digitalWrite(A2,LOW ); // A2 is GND for sensor
  digitalWrite(A1,HIGH);  // A1 is VCC for sensor
  pinMode(RollerServoVCC,OUTPUT);  // VCC for label sensor
  pinMode(RollerServoGND,OUTPUT);  // GND for label sensor
  pinMode(RollerServoVcc,OUTPUT);  // VCC for roller height sensor
  digitalWrite(RollerServoVcc,HIGH);  // D10 is GND for RollerServoVcc
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH); // VCC pin for roller motor lifted sensor
  pinMode(A4, INPUT);
  digitalWrite(RollerServoGND,LOW ); //GND for roller servo control
  digitalWrite(RollerServoVCC,HIGH );
 
  
  
  lcd.clear();
  
  lcd.print("Labeler Control");
  
  delay(500);
  lcd.clear();
  startMillis = millis();  //initial start time of raised roller
}

void loop() {
 // put your main code here, to run repeatedly:
 lcd.setCursor(8,0);
 
  
 
   if (Mode == 0 ) {    
    switch (lcd.buttonBlocking(block_delay,repeat_delay)) {
    case KEYPAD_LEFT:
      lcd.clear();
      lcd.print("Timing Test   ");
      Mode = 4 ;
          break;
    case KEYPAD_RIGHT:
      lcd.clear();
      lcd.print("Auto Mode     ");
      Mode = 2 ;
         break; 
    case KEYPAD_DOWN:
    lcd.clear();
      lcd.print("Empty Spool  ");
     Mode = 5 ;
         break;
    case KEYPAD_UP:
     lcd.clear();
      lcd.print("Prime  ");
         Mode = 3 ;
         break;
    case KEYPAD_SELECT:
      lcd.clear();
      lcd.print("Manual Mode");
      Mode = 1 ;
      delay(500);
     // RunManual();
      break;
    default:
      lcd.print("NONE    ");
      Mode = 0 ;
      break;
  }
   }

  
  if (Mode == 1 ) { // manual mode, trying to make sure loop cycles for timer operation  
  if( analogRead(SensorPin) > LabelSense || digitalRead(PrimeButton) == 0 ) { // of not seeing label or prime buttin is pressed then run winder
  WinderServo.write(Winderspeed) ;  // wind up labels
}
else WinderServo.write(90); // unless prime button is pressed or label is not seen then stop

 if( analogRead(4) > 900) { // roller is lifted
  RollerMotor.write(180);
 }
 else RollerMotor.write(90); // stop if lowered 
  } // end of manual mode

//////////////////////////////////////////////////////////////////////////////////////////////////
if (Mode ==2) {

startMillis = millis();
  if (analogRead(4) > 900) {  // wait til roller is lifted
  RollerMotor.write(180);  // turn on motor for roller
  delay(500);     // wait a second to make sure roller is squishing crayon
  WinderServo.write(Winderspeed);  // start label feed roller
  delay(200); // wait a bit before checking..
  while (analogRead(SensorPin) > LabelSense) {  // keep feeding label as needed
    WinderServo.write(Winderspeed);
  }
  WinderServo.write(90); // stop label motor
  delay(500);
  RollerMotor.write(90); // stop roller
  delay(3000);
  

}
}
 if (Mode == 3 ) { // prime mode, only run label winder when button is pushed
  if(  digitalRead(PrimeButton) == 0 ) { // of not seeing label or prime buttin is pressed then run winder
  WinderServo.write(Winderspeed) ;  // wind up labels
}
else WinderServo.write(90); // unless prime button is pressed or label is not seen then stop

   } // end of priming mode



if (Mode == 4 ) { // prime mode, only run label winder when button is pushed
  if (analogRead(4) > 900) { // prime buttin is pressed then run timer
    
 TimingTest() ;
}

   } // end of button code for timingtest


   if (Mode == 5 ) { // prime mode, only run label winder when button is pushed
  if(  digitalRead(PrimeButton) == 0 ) { // of not seeing label or prime buttin is pressed then run winder
  WinderServo.write(0) ;  // wind up labels
}
else WinderServo.write(90); // unless prime button is pressed or label is not seen then stop

   } // end of priming mode



}






  




void RunManual()
{
   lcd.clear();
   lcd.print("never get here  ");
  while(1 == 1) {
  //delay(500);
  //   if (digitalRead(PrimeButton)== 0) {
  if( analogRead(SensorPin) > LabelSense || digitalRead(PrimeButton) == 0 ) { // of not seeing label or prime buttin is pressed then run winder
  WinderServo.write(Winderspeed) ;  // wind up labels
}
else WinderServo.write(90); // unless prime button is pressed or label is not seen then stop

 if( analogRead(4) > 900) { // roller is lifted
  RollerMotor.write(180);
 }
 else RollerMotor.write(90); // stop if lowered 

  }
}

void RunAuto()
{
   lcd.clear();
   lcd.print("never get here   ");
  while(1 == 1) {
  //delay(500);
  //   if (digitalRead(PrimeButton)== 0) {
  if( analogRead(SensorPin) > LabelSense || digitalRead(PrimeButton) == 0 ) { // of not seeing label or prime buttin is pressed then run winder
  WinderServo.write(Winderspeed) ;  // wind up labels
}
else WinderServo.write(90); // unless prime button is pressed or label is not seen then stop


 if( analogRead(4) > 900) { // roller is lifted
  RollerMotor.write(180);
 }
 else RollerMotor.write(90); // stop if lowered 

  }
}

