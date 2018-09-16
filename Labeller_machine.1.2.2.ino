#include <Servo.h>
#include <LiquidCrystal.h>
#include <LCDKeypad.h>

/*  
 *   Labeling machine: code is work in progress
 *   Digital output pwm to speed controller for roller winder
 *   Infared sensor for label timing,need to stop label primed for next label.. no smoothing added yet, smoothing may effect timing and need adjustment if added later
 *   Prime button to run winder to start paper on spool. Using digital input with no smoothing, hopefully not too jumpy
 *   
 *  code wrote 3/29/18 and Updated 9/15/18 
 *  cleaned up the code a little, added small menus
 *  added ability to manually adjust winder speed and clean up some functions
 */ 
byte Winderspeed = 180 ;  //0=Full reverse||| 90= Stop ||| 180 Full forward  Need to adjust this as roller diameter increases and pulls faster to prevent jamming
byte WinderPin = 11 ;  // Digital pin for servo control
byte PrimeButton = 12 ;  // button wired to ground to prime roller
byte SensorPin= 3 ;     // pin for infarad sensor for windup roller
//byte RollerServoVcc = 2; // servo control pin for roller motor
byte RollerServoVCC = 0 ; // RollerServo VCC
byte RollerServoGND = 1 ; // gnd
byte RollerPin = 2 ; // Need to see wtf I was doing as pin 2 is used above, is one of these analog?? I don't remember.. I commented out the above use of pin 2
int LabelSense = 650 ;  // sensitivity of infarad sensor.. may need  to average some readings if too jumpy
Servo WinderServo;  // create servo WinderServo to control speed controller for Label winder motor
Servo RollerMotor ;
LCDKeypad lcd;
int block_delay(150);
int repeat_delay(100);
byte Mode = 0 ;
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
  pinMode(RollerServoVCC,OUTPUT);  // VCC for Roller servo control
  pinMode(RollerServoGND,OUTPUT);  // GND for Roller servo control
  digitalWrite(RollerServoGND,LOW ); //GND for roller servo control
  digitalWrite(RollerServoVCC,HIGH ); //VCC for Roller servo control
 // pinMode(RollerServoVcc,OUTPUT);  // VCC for roller height sensor
 // digitalWrite(RollerServoVcc,HIGH);  // 
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH); // VCC pin for roller motor lifted sensor
  pinMode(A4, INPUT);
  lcd.print("Labeler Control");
  delay(500);
  lcd.clear();
  lcd.print("Up=Prime  D=Empty");
  lcd.setCursor(0,1);
  lcd.print("L/R=Roll Labels");

}

void loop() {
 // put your main code here, to run repeatedly:
  
    switch (lcd.buttonBlocking(block_delay,repeat_delay)) {
    case KEYPAD_LEFT:
      lcd.clear();
      lcd.print("Roll Labels   ");
      lcd.setCursor(0,1);
     lcd.print("U/D=Adjust Speed");
      Mode = 4 ;
          break;
    case KEYPAD_RIGHT:
      lcd.clear();
      lcd.print("Roll Labels     ");
       lcd.setCursor(0,1);
     lcd.print("U/D=Adjust Speed");
      Mode = 4 ;
         break; 
    case KEYPAD_DOWN:
    lcd.clear();
    if (Mode == 4) {
      lcd.clear() ;
      lcd.print("Slow Winder");
      Winderspeed = Winderspeed - 5 ;
      lcd.setCursor(0,1);
      lcd.print("Label speed:");
      lcd.print(Winderspeed,DEC);
      delay(300);
      break ;
        }
      lcd.print("Empty Spool  ");
       Mode = 1 ;
       break;
       
    case KEYPAD_UP:
     if (Mode == 4) {
      lcd.clear() ;
      lcd.print("Speed up Winder");
      Winderspeed = Winderspeed + 5 ;
      lcd.setCursor(0,1);
      lcd.print("Label speed:");
      lcd.print(Winderspeed,DEC);
      delay(300);
      break ;
        }
     lcd.clear();
      lcd.print("Prime  ");
         Mode = 2 ;
         break;
         
    case KEYPAD_SELECT:
      lcd.clear();
      lcd.print("Select Button");
      delay(500);
      break;
    default:
      if (Mode == 1) EmptySpool();
      if (Mode == 2) PrimeMode();
      if (Mode == 4 && analogRead(4) > 900) TimingTest(); // Timing test mode if roller is lifted and in mnode 4
         
      break;
  }

  if (Mode == 1) EmptySpool();
  if (Mode == 2) PrimeMode();
  if (Mode == 4 && analogRead(4) > 900) TimingTest(); // Timing test mode if roller is lifted and in mnode 4
     
}

void EmptySpool() {
  
  if(  digitalRead(PrimeButton) == 0 ) { // if prime buttin is pressed then run winder backwards
  WinderServo.write(0) ;  // unwind labels
}
else WinderServo.write(90); // unless prime button is pressed or label is not seen then stop

   }


void PrimeMode() {
  if(  digitalRead(PrimeButton) == 0 ) { // if prime buttin is pressed then run winder
  WinderServo.write(Winderspeed) ;  // wind up labels
}
else WinderServo.write(90); // unless prime button is pressed or label is not seen then stop

   } // end of priming mode


void TimingTest() {  // Roller bar is already raise waiting for both motors to start before getting here

  RollerMotor.write(180);  // turn on motor for roller
  delay(100); // wait 100ms for the roller motor to start
  StartTime = millis();  // start timer
  WinderServo.write(Winderspeed);  // start label feed roller
  delay(250);  // wait 250 ms to make sure label passes over sensor so it doesn't stop as soon as it starts
  while (analogRead(SensorPin) > LabelSense) {  // keep feeding label until gap is seen
  WinderServo.write(Winderspeed);
  }
  WinderServo.write(90); // stop label motor
  EndTime = millis() ;
  delay(500); // keep spinning roller to make sure label is fully wrapped
  RollerMotor.write(90); // stop roller
  LabelTime = EndTime - StartTime ;
  if (LabelTime < 520 && Winderspeed > 100) Winderspeed = Winderspeed - 5 ;  // slow down winder if label was going too fast
  if (LabelTime > 580 && Winderspeed < 176) Winderspeed = Winderspeed + 5 ;  // speed up winder if label was going too slow     
 
  lcd.clear();
  lcd.print("Label Time:");
  lcd.print(LabelTime,DEC);
  lcd.setCursor(0,1);
  lcd.print("Label speed:");
  lcd.print(Winderspeed,DEC);
  delay(2000); // lowered to 2 seconds, raise to 3 if roller isn't lowered quick enough and this function gets triggered again
  }



