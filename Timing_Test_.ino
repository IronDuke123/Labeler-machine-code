void TimingTest() {

  lcd.clear();
  lcd.print("got here    ");
  delay(100);
  RollerMotor.write(180);  // turn on motor for roller
  StartTime = millis();  // start timer
// if (analogRead(4) > 900) {  // wait til roller is lifted
  
 // delay(500);     // wait a second to make sure roller is squishing crayon
  WinderServo.write(Winderspeed);  // start label feed roller
  delay(300);
  while (analogRead(SensorPin) > LabelSense) {  // keep feeding label as needed
  WinderServo.write(Winderspeed);
  }
  WinderServo.write(90); // stop label motor
  EndTime = millis() ;
  delay(500);
  RollerMotor.write(90); // stop roller
  LabelTime = EndTime - StartTime ;
  if (LabelTime < 520 && Winderspeed > 130) {
    Winderspeed = Winderspeed - 5 ;
        
  }
  lcd.clear();
  lcd.print("Label Time:");
  lcd.print(LabelTime,DEC);
  lcd.setCursor(0,1);
  lcd.print("Label speed:");
  lcd.print(Winderspeed,DEC);
  delay(3000);

// } 

  
}

