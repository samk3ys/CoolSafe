/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "a:/Documents/Programming/GitHub/CoolSafe/Microcontroller/cool_safe_code-StandAlone/src/cool_safe.ino"
/*
 * Project: Cool Safe Code
 * Description: Main code that runs our senior capstone project for UK. It is a lock-bin locked by a solenoid that also has a fingerprint scanner and app to register fingerprints
 * Author: Sam Keys, Sylvester Okafor, Tylor Mendez, Shae Henry, Steven Deaton
 * Date: Spring 2020
 */

#include "Particle.h"                         // library for Particle devices (we are using a Particle Xenon)
#include "FPS_GT521F32_Particle.h"            // library for fingerprint scanner functions
#include "playTones.h"                        // library for playing sounds on a buzzer / speaker

// Globals and timings
void openBin();
void goodFeedback();
void badFeedback();
void enrollScanFeedback();
int identifyUser();
bool enrollUser();
void LEDsequence();
void setup();
void loop();
#line 13 "a:/Documents/Programming/GitHub/CoolSafe/Microcontroller/cool_safe_code-StandAlone/src/cool_safe.ino"
SYSTEM_MODE(MANUAL);                          // Avoid Particle registration. Can't use BLE out-of-the-box without MANUAL control. Could changed to AUTOMATIC for easier cloud integration in the future.
bool keySwitchFlag = false;                   // tracks state of the manual key switch to limit use to once per turn
const int maxUsers = 200;                     // Maximum number of users able to be enrolled on the FPS
const int unlockTime = 1000;                  // milliseconds to keep solenoid powered (pulled in) when opening the lock-bin
const int delayTime = 100;                    // milliseconds to delay after a round of the main infinite loop
const unsigned long ENROLLMENT_WAIT = 60;     // seconds to wait for fingerprint enrollment
unsigned long enrollStartTime = 0;            // tracks time since an enrollment started
unsigned long enrollmentWatch = 0;            // tracks how much time has passed while waiting for fingerprint enrollment

// I/O
const int buzzer    = A0;                     // digital pin for sound output
const int solenoid  = A4;                     // digital pin for relay/driver signal. HIGH connects to normally open, LOW connects to normally closed
const int keySwitch = D5;                     // digital pin for the electro-mechanical switch with a key. Backup to fps.
const int greenLED  = D6;                     // green LED signifies an authorized
const int busyLED   = D7;                     // amber LED signifies the system is busy (usually with registration mode). Same as the Xenon's on-board blue LED
const int redLED    = D8;                     // red LED signifies an unauthorized user
FPS_GT521F32 fps;                             // fingerprint scanner module - uses Serial1 (Rx:pin14, Tx:pin15)

// Buzzer songs
int startSound[]    = {NOTE_C3, NOTE_C4, NOTE_D4, NOTE_C5, NOTE_D5, NOTE_C6};   // plays when the system boots up
int startDuration[] = {2, 4, 4, 4, 4, 1};
int goodSound[]     = {NOTE_C3, NOTE_E3, NOTE_G3};                              // plays when the lock-bin is unlocked
int goodDuration[]  = {4, 4, 1};
int badSound[]      = {NOTE_G3, NOTE_E3, NOTE_C3};                              // plays when access to the lock-bin is denied
int badDuration[]   = {4, 4, 1};

// Operation function prototypes
bool addUser(void);
bool removeUser(uint8_t id);
bool deauthorizeUser(uint8_t id);
bool authorizeUser(uint8_t id);
bool changeUserName(uint8_t id, String name);

int manualEnrollTimer = 0;  // tracks time for manually initiating enrollment with the key switch
int manualEnrollTriggerTime = 500;  // time (in ms) alotted for triggering manual enrollment

void openBin() {
  // Open the lock-bin by connecting the relays common and normally open, actuating the solenoid.
  digitalWrite(solenoid, HIGH);        // Unlock the bin
  goodFeedback();                   // Use lights and sounds to let the user know the lock-bin is unlocked
  delay(unlockTime);                // How long the solenoid stays pulled in
  digitalWrite(solenoid, LOW);         // Solenoid is back out, ready to lock again
}

void goodFeedback() {
  // green light and good sound
  digitalWrite(greenLED, HIGH);                                 // external green light
  RGB.color(0, 255, 0);                                         // on-board green light
  play(buzzer, arraySize(goodSound), goodSound, goodDuration);  // sound has some delay
  digitalWrite(greenLED, LOW);                                  // turn lights back off
  RGB.color(0, 0, 0);
}

void badFeedback() {
  // red light and bad sound
  digitalWrite(redLED, HIGH);                                   // external red light
  RGB.color(255, 0, 0);                                         // on-board red light
  play(buzzer, arraySize(badSound), badSound, badDuration);     // sound has some delay
  digitalWrite(redLED, LOW);                                    // turn lights back off
  RGB.color(0, 0, 0);
}

void enrollScanFeedback() {
  // Blink amber (busy) LED and make a sound to signify a scan has been taken and the user can remove their finger
  int sound[] = {NOTE_C3, NOTE_E3};
  int duration[] = {2, 2};
  play(buzzer, 2, sound, duration);
  digitalWrite(busyLED, LOW);
  RGB.color(0, 0, 0);
  delay(100);
  digitalWrite(busyLED, HIGH);
  RGB.color(100, 100, 0);
}

int identifyUser() {
  // Once a finger is detected then identify a user with the FPS
  //fps.CaptureFinger(false);   // false = take a low quality image (faster)
  fps.CaptureFinger(true);
  int id = fps.Identify1_N(); // check if the fingerprint is stored in the FPS

  if (id >= 0 && id < 200) {  // valid value for a user id on the GT-521F32 FPS
    Serial.print("Verified ID:");
    Serial.println(id);
    openBin();  // Allow access. Includes good feedback
  }
  else {//if unable to recognize
    Serial.println("Finger not found");
    badFeedback();  // Deny access
  }

  return id;  // Return id of the user from the FPS
}

bool enrollUser() {
  // Enroll a new fingerprint for the scanner. Returns true if successful. Returns false if timed out or error.

  // Signal to users that the system is busy. Normal functions won't work at this time
  digitalWrite(busyLED, HIGH);
  RGB.color(100, 100, 0);
 
  // Is there room?
  if (fps.GetEnrollCount() == maxUsers) {  // FPS full
    Serial.println("Error: no more enrollment IDs available.");
    digitalWrite(busyLED, LOW);
    RGB.color(0, 0, 0);
    return false;
  }

  // Jot down the time this enrollment started.
  enrollStartTime = millis();

  int enrollid = 0;
  bool usedid = true;
  while (usedid == true) {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  Serial.printlnf("ID #%d is next available on FPS.", enrollid);

  Serial.println("start Enroll");
  fps.EnrollStart(enrollid);
  Serial.println("Enroll started");

  // enroll w/ 3 seperate scans
  Serial.print("Press finger to Enroll #");
  Serial.println(enrollid);
  while(fps.IsPressFinger() == false) {
    if ( (millis() - enrollStartTime) >= (ENROLLMENT_WAIT * 1000) ) {
      digitalWrite(busyLED, LOW);
      RGB.color(0, 0, 0);
      return false; // exit w/ timeout function if we've been here for too long
    }
    delay(100);
  }
  bool bret = fps.CaptureFinger(true);  // true = use high quality scan
  int iret = 0;
  if (bret != false) {
    // Successful first scan
    enrollScanFeedback();  //goodFeedback();
    Serial.println("Remove finger");
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    while(fps.IsPressFinger() == false) {
      if ( (millis() - enrollStartTime) >= (ENROLLMENT_WAIT * 1000) ) {
        digitalWrite(busyLED, LOW);
        RGB.color(0, 0, 0);
        return false; // exit w/ timeout function if we've been here for too long
      }
      delay(100);
    }
    bret = fps.CaptureFinger(true);
    if (bret != false) {
      // Successful second scan
      enrollScanFeedback();  //goodFeedback();
      Serial.println("Remove finger");
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      while(fps.IsPressFinger() == false)  {
        if ( (millis() - enrollStartTime) >= (ENROLLMENT_WAIT * 1000) ) {
          digitalWrite(busyLED, LOW);
          RGB.color(0, 0, 0);
          return false; // exit w/ timeout function if we've been here for too long
        }
        delay(100);
      }
      bret = fps.CaptureFinger(true);
      if (bret != false) {
        // Successful third scan
        enrollScanFeedback();  //goodFeedback();
        Serial.println("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0) {
          // All scans successful
          goodFeedback();
          Serial.println("Enrolling Successful");
        }
        else {
          // Something went wrong
          badFeedback();
          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
          digitalWrite(busyLED, LOW);
          RGB.color(0, 0, 0);
          return false; // stop w/ failure
        }
      }
      else {
        badFeedback();
        Serial.println("Failed to capture third finger");
        digitalWrite(busyLED, LOW);
        RGB.color(0, 0, 0);
        return false; // stop w/ failure
      }
    }
    else {
      badFeedback();
      Serial.println("Failed to capture second finger");
      digitalWrite(busyLED, LOW);
      RGB.color(0, 0, 0);
      return false; // stop w/ failure
    }
  }
  else {
    badFeedback();
    Serial.println("Failed to capture first finger");
    digitalWrite(busyLED, LOW);
    RGB.color(0, 0, 0);
    return false; // stop w/ failure
  }

  // Let the users know the system is back to normal
  digitalWrite(busyLED, LOW);
  RGB.color(0, 0, 0);
  return true;  // exit w/ success
}

void LEDsequence() {
  // Do a wave of all 3 LEDs on and then off. Used for making sure they work at startup
  digitalWrite(greenLED, HIGH);
  delay(100);
  digitalWrite(busyLED, HIGH);
  delay(100);
  digitalWrite(redLED, HIGH);
  delay(500);
  digitalWrite(greenLED, LOW);
  delay(100);
  digitalWrite(busyLED, LOW);
  delay(100);
  digitalWrite(redLED, LOW);
}

void setup() {
  // Debugging
  //Serial.begin(9600);         // Open USB serial port for debugging
  //fps.UseSerialDebug = true;  // Sends messages from FPS to USB Serial for debugging
  // On-board LED control
  RGB.control(true);            // take control of the On-Board RGB LED
  RGB.color(255, 255, 255);     // Format red, green, blue, from 0 to 255
  RGB.brightness(64);           // scales brightness of all three colors, 0-255
  // Check EEPROM
  //Serial.print("EEPROM Available: ");
  //Serial.println(EEPROM.length());    // 4096 bytes available

  // Device I/O
	fps.Open();                   // Send serial command to initialize fps. FPS UART uses Rx (pin 14) & Tx (pin 15) on Particle Xenon. Make sure FPS is connected or the program won't go past this part
  fps.SetLED(true);             // Visual test to make sure the fps is connected. Leave on for being able to detect fingerprints
  pinMode(buzzer, OUTPUT);          // sound buzzer, not necessary for using tone()
  pinMode(solenoid, OUTPUT);        // signal to relay/driver for switching solenoid
  pinMode(keySwitch, INPUT_PULLUP); // detect when the switch is activated w/ a key
  pinMode(greenLED, OUTPUT);        // green access permitted LED
  pinMode(busyLED, OUTPUT);         // amber registration mode LED
  pinMode(redLED, OUTPUT);          // red access denied LED
  
  // Reset System: DO NOT UNCOMMENT UNLESS YOU ARE SURE
  //EEPROM.clear();   // ERASES ALL EEPROM (inits all bits to 1)
  //fps.DeleteAll();  // ERASES FPS ENROLLMENTS

  // Setup done. Signify that the system is ready to be used with some light and sound
  LEDsequence();  // turn all LEDs on at start to make sure they work
  play(buzzer, arraySize(startDuration), startSound, startDuration);  // play start-up sound
}

void loop() {
  RGB.color(0, 0, 255);

  // Check for users trying to access using a fingerprint
  if(fps.IsPressFinger()) {
    RGB.color(255, 255, 255); // white LED for debugging to show that a finger is recognized and the FPS is working on figuring out the user
    identifyUser(); // Is this finger recognized by the FPS. If yes open bin, if no send bad feedback
  }

  // Check for users trying to access using the electro-mechanical tumbler lock switch
  // 2nd PCB: HIGH=open switch, LOW=closed switch
  if (digitalRead(keySwitch) == LOW && keySwitchFlag == false) {     // Key switch turned on
    //openBin();                    // Allow access. Includes good feedback
    goodFeedback();                 // Only need feedback for 2nd PCB onward b/c switch directly connects 12V to solenoid
    keySwitchFlag = true;           // Set flag so we don't unlock again before turning the key off
    
    manualEnrollTimer = millis();
    while ( (millis() - manualEnrollTimer) < manualEnrollTriggerTime) { // Enter registration mode manually by flicking the switch on and off and on quickly      
      if (digitalRead(keySwitch) == HIGH) {   // Key switch turned off
        while ( (millis() - manualEnrollTimer) < manualEnrollTriggerTime) { // wait a max of 500ms
          if (digitalRead(keySwitch) == LOW) {  // Key switch turned on
              enrollUser();
          }
        }
      }
    }
  } 
  else if (digitalRead(keySwitch) == HIGH && keySwitchFlag == true) {  // Key switch turned off
    keySwitchFlag = false;          // Reset flag so the key can be used again
  }

  delay(delayTime);
}
