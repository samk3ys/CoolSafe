const int buttonPin = A2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
int buttonState = 0;         // variable for reading the pushbutton status
 byte keySwitch = A5;
 byte  Relay= 6;
 int buzzer = 9;
 int rdLED = 10;
 int orLED = 11;
 int gnLED = 12;
// int fpSignal = 13;
int LED;
//bool  fp = false; // fingerprint testing
int key;
int fp;
bool flag = true;
bool fp_flag = true;
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

// set up software serial pins for Arduino's w/ Atmega328P's
// FPS (TX) is connected to pin 4 (Arduino's Software RX)
// FPS (RX) is connected through a converter to pin 5 (Arduino's Software TX)
FPS_GT511C3 fps(4, 5); // (Arduino SS_RX = pin 4, Arduino SS_TX = pin 5)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //set up Arduino's hardware serial UART
  delay(100);
//  pinMode(fpSignal, INPUT);
  pinMode(orLED, OUTPUT);
  pinMode(Relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(keySwitch, INPUT_PULLUP);
  pinMode(rdLED, OUTPUT);
  pinMode(gnLED, OUTPUT);
  digitalWrite(Relay, LOW);
   
  fps.Open();         //send serial command to initialize fps
  fps.SetLED(true);   //turn on LED so fps can see fingerprint
  pinMode(12, OUTPUT); // GOOD PRINT
  pinMode(13, OUTPUT); //BAD PRINT
  pinMode(buttonPin, INPUT);
  pinMode(9, OUTPUT);
  //fps.DeleteAll();
}

void identify()
{
  // Identify fingerprint test
  if (fps.IsPressFinger())
  {
    fps.SetLED(true); //added
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();

       /*Note:  GT-521F52 can hold 3000 fingerprint templates
                GT-521F32 can hold 200 fingerprint templates
                 GT-511C3 can hold 200 fingerprint templates. 
                GT-511C1R can hold 20 fingerprint templates.
       Make sure to change the id depending on what
       model you are using */
    if (id <200) //<- change id value depending model you are using
    {//if the fingerprint matches, provide the matching template ID
      Serial.print("Verified ID:");
      Serial.println(id);
      digitalWrite(9, HIGH);
      delay(100);
      digitalWrite(9, LOW);
      digitalWrite(12, HIGH); // GOOD PRINT
      delay(500);
      digitalWrite(12, LOW);
      //digitalWrite(Relay, HIGH);
      open();
    }
    else
    {//if unable to recognize
      Serial.println("Finger not found");
      digitalWrite(13, HIGH); // BAD PRINT 
      delay(500);
      digitalWrite(13, LOW);
      notopen();
     
    }
  }
  else
  {
    Serial.println("Please press finger");
      digitalWrite(13, HIGH); // BAD PRINT 
      delay(500);
      digitalWrite(13, LOW);
  }
  delay(100);
}

void Enroll()
{
  // Enroll test

  // find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  // enroll
  Serial.print("Press finger to Enroll #");
  Serial.println(enrollid);
  while(fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    Serial.println("Remove finger");
      digitalWrite(12, HIGH); // GOOD PRINT
      delay(100);
      digitalWrite(12, LOW);
      delay(100);
      digitalWrite(12, HIGH); // GOOD PRINT
      delay(100);
      digitalWrite(12, LOW);
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    while(fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      digitalWrite(12, HIGH); // GOOD PRINT
      delay(100);
      digitalWrite(12, LOW);
      delay(100);
      digitalWrite(12, HIGH); // GOOD PRINT
      delay(100);
      digitalWrite(12, LOW);
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      while(fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
      digitalWrite(12, HIGH); // GOOD PRINT
      delay(100);
      digitalWrite(12, LOW);
      delay(100);
      digitalWrite(12, HIGH); // GOOD PRINT
      delay(100);
      digitalWrite(12, LOW);
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successful");
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
          digitalWrite(13, HIGH); // BAD PRINT 
          delay(800);
          digitalWrite(13, LOW);
        }
      }
      else Serial.println("Failed to capture third finger");

    }
    else Serial.println("Failed to capture second finger");

  }
  else Serial.println("Failed to capture first finger");

}




void open(){ // hello authorized user :)
  digitalWrite(Relay, HIGH);
    digitalWrite(gnLED, HIGH);
    
    tone(buzzer, 500);
    delay(1000);
    digitalWrite(Relay, LOW);
    noTone(buzzer);
    delay(1000);
    tone(buzzer, 500);
    delay(1000);
    noTone(buzzer);
    digitalWrite(gnLED, LOW);
    delay(10);
}

void notopen(){ // unauthorized user tried to access bin
    digitalWrite(rdLED, HIGH);
    tone(buzzer, 500);
    delay(1000);
    noTone(buzzer);
    delay(1000);
    digitalWrite(rdLED, LOW);
}

void loop() {
//digitalRead(fpSignal);
buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) 
   {
      //digitalWrite(12, HIGH); // GOOD PRINT
      //delay(500);
    //  digitalWrite(12, LOW);
    Enroll();
  }
  else if (fps.IsPressFinger())
   {
    identify();
   }
  delay(100);


 key = digitalRead(keySwitch);
 
 if (key == LOW)
 {
   flag = false;
 }

 if (fp == LOW)
 {
   fp_flag = false;
 }
 
  if (key == HIGH && flag == false)
  {
    open();
    flag = true; 
  }
  //fp = digitalRead(fpSignal);
 /* if (fp == HIGH && fp_flag == false)
  {
    open();
    fp_flag = true; 
  }
  delay(100);*/
 // digitalWrite(rdLED, HIGH);
  //delay(1000);
  //digitalWrite(rdLED, LOW);
  //delay(1000);
  //digitalWrite(orLED, HIGH);
  //delay(1000);
  //digitalWrite(orLED, LOW);
  //delay(1000);
}
