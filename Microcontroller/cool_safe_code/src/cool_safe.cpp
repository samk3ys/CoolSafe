/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "a:/Documents/Programming/GitHub/CoolSafe/Microcontroller/cool_safe_code/src/cool_safe.ino"
/*
 * Project: Cool Safe Code
 * Description: Main code that runs our senior capstone project for UK. It is a lock-bin locked by a solenoid that also has a fingerprint scanner and app to register fingerprints
 * Author: Sam Keys, Sylvester Okafor, Tylor Mendez, Shae Henry, Steven Deaton
 * Date: Spring 2020
 */

#include "Particle.h"                         // library for Particle devices (we are using a Particle Xenon)
#include "FPS_GT511C3_Particle.h"             // library for fingerprint scanner functions
#include "playTones.h"                        // library for playing sounds on a buzzer / speaker

// Globals and timings
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
void openBin();
void goodFeedback();
void badFeedback();
int identifyUser();
void enrollUser();
void LEDsequence();
void setup();
void loop();
#line 13 "a:/Documents/Programming/GitHub/CoolSafe/Microcontroller/cool_safe_code/src/cool_safe.ino"
SYSTEM_MODE(MANUAL);                          // Avoid Particle registration. Can't use BLE out-of-the-box without MANUAL control. Could changed to AUTOMATIC for easier cloud integration in the future.
bool connectionFlag = false;                  // flag for remembering if BLE is connected to a device
//#define BLE_DEFAULT_ADVERTISING_INTERVAL 500; // change advertising interval
bool keySwitchFlag = false;                   // tracks state of the manual key switch to limit use to once per turn
const int maxUsers = 200;                     // Maximum number of users able to be enrolled on the FPS
const int unlockTime = 1000;                  // milliseconds to keep solenoid powered (pulled in) when opening the lock-bin
const int delayTime = 100;                    // milliseconds to delay after a round of the main infinite loop
const unsigned long UPDATE_INTERVAL = 1000;   // milliseconds between updating BLE data
unsigned long lastUpdate = 0;                 // used for tracking BLE update interval

// I/O
const int buzzer    = A0;                     // digital pin for sound output
const int relay     = A4;                     // digital pin for relay signal. HIGH connects to normally open, LOW connects to normally closed
const int keySwitch = D5;                     // digital pin for the electro-mechanical switch with a key. Backup to fps.
const int greenLED  = D6;                     // green LED signifies an authorized
const int busyLED   = D7;                     // amber LED signifies the system is busy (usually with registration mode). Same as the Xenon's on-board blue LED
const int redLED    = D8;                     // red LED signifies an unauthorized user
FPS_GT511C3 fps;                              // fingerprint scanner module - uses Serial1 (Rx:pin14, Tx:pin15)

// BLE Service UUID
const BleUuid serviceUuid("6E400000-B5A3-F393-E0A9-E50E24DCCA9E");
// BLE Characteristics UUIDs - UART Protocol regarding mobile device as host (i.e. Rx for app receives data and Tx for app transmits data)
const char* writeUUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
BleCharacteristic RxBLE("receive", BleCharacteristicProperty::WRITE_WO_RSP, writeUUID, serviceUuid, onDataReceived, (void*)writeUUID);
BleCharacteristic TxBLE("transmit", BleCharacteristicProperty::NOTIFY, BleUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E"), serviceUuid);

// Operations
uint8_t noOP = 0x00;          // no operation. blank.
uint8_t newUser = 0x01;       // start the process for adding a new user
uint8_t deleteUser = 0x02;    // remove user and biometric
uint8_t disableUser = 0x03;   // remove user from group of authorized users
uint8_t editUser = 0x04;      // change user name or other property

// Buzzer songs
int startSound[]    = {NOTE_C3, NOTE_C4, NOTE_D4, NOTE_C5, NOTE_D5, NOTE_C6};   // plays when the system boots up
int startDuration[] = {2, 4, 4, 4, 4, 1};
int goodSound[]     = {NOTE_C3, NOTE_E3, NOTE_G3};                              // plays when the lock-bin is unlocked
int goodDuration[]  = {4, 4, 1};
int badSound[]      = {NOTE_G3, NOTE_E3, NOTE_C3};                              // plays when access to the lock-bin is denied
int badDuration[]   = {4, 4, 1};


// Function called when the app sends data through TxBLE to this device
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  // when data for when data is recieved through the RxBLE characteristic
  const char * dataChars = (char *) data;
  Serial.write(dataChars);
  Serial.write('\n');

  char operationID = data[0];

  switch (operationID) {
    case 'a':
      RGB.color(100, 0, 0);
      break;
    case 'b':
      RGB.color(0, 100, 0);
      break;
    case 'c':
      RGB.color(0, 0, 100);
      break;
    default:
      RGB.color(255, 255, 0);
  }
  
  // Hex code in to change LED color
  //uint8_t red = dataToHex(data[0], data[1]);
  //uint8_t green = dataToHex(data[2], data[3]);
  //uint8_t blue = dataToHex(data[4], data[5]);
  //RGB.color(red, green, blue);
}

void openBin() {
  // Open the lock-bin by connecting the relays common and normally open, actuating the solenoid.
  digitalWrite(relay, HIGH);        // Unlock the bin
  goodFeedback();                   // Use lights and sounds to let the user know the lock-bin is unlocked
  delay(unlockTime);                // How long the solenoid stays pulled in
  digitalWrite(relay, LOW);         // Solenoid is back out, ready to lock again
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

int identifyUser() {
  // Once a finger is detected then identify a user with the FPS
  fps.CaptureFinger(false);   // take a low quality image (faster)
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

void enrollUser() {
  // find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true) {
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
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    while(fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      while(fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successful");
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
        }
      }
      else Serial.println("Failed to capture third finger");
    }
    else Serial.println("Failed to capture second finger");
  }
  else Serial.println("Failed to capture first finger");
}

void LEDsequence() {
  // Do a wave of all 3 LEDs on and then off. Used for making sure they work
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
  //Serial.println(EEPROM.length());    // 4096 available
  
  // BLE
  // Attach characteristics
  BLE.addCharacteristic(TxBLE);
  BLE.addCharacteristic(RxBLE);
  RxBLE.onDataReceived(onDataReceived, NULL);
  // Start advertising service
  BleAdvertisingData adverData;
  adverData.appendServiceUUID(serviceUuid);
  BLE.advertise(&adverData);

  // Device I/O
	fps.Open();                   // Send serial command to initialize fps. FPS UART uses Rx (pin 14) & Tx (pin 15) on Particle Xenon. Make sure FPS is connected or the program won't go past this part
  fps.SetLED(true);             // Visual test to make sure the fps is connected. Leave on for being able to detect fingerprints
  //delay(500);
  //fps.SetLED(false);
  pinMode(buzzer, OUTPUT);      // sound buzzer, not necessary for using tone()
  pinMode(relay, OUTPUT);       // signal to relay for switching solenoid
  pinMode(keySwitch, INPUT);    // electro-mechanical switch w/ a key
  pinMode(greenLED, OUTPUT);    // green access permitted LED
  pinMode(busyLED, OUTPUT);     // amber registration mode LED
  pinMode(redLED, OUTPUT);      // red access denied LED
  LEDsequence();                // turn all LEDs on at start to make sure they work

  // Setup done. Play start-up sound, signifying that the system is ready to be used
  play(buzzer, arraySize(startDuration), startSound, startDuration);
  //delay(1000);
}

uint8_t a = (uint8_t) 1;  // variable for testing BLE incrementing data
void loop() {
  RGB.color(0, 0, 255);
 
  // Check for users trying to access using a fingerprint
  if(fps.IsPressFinger()) {
    RGB.color(255, 255, 255);   // white LED for debugging to show that a finger is recognized and the FPS is working on figuring out the user
    identifyUser();
    // TODO: Check if user is allowed access (or is disabled)
  }
  
  // Check for users trying to access using the electro-mechanical tumbler lock switch
  if (digitalRead(keySwitch) == HIGH && keySwitchFlag == false) {     // Key switch turned on
    openBin();                      // Allow access. Includes good feedback
    keySwitchFlag = true;           // Set flag so we don't unlock again before turning the key off
  } 
  else if (digitalRead(keySwitch) == LOW && keySwitchFlag == true) {  // Key switch turned off
    keySwitchFlag = false;          // Reset flag so the key can be used again
  }
  
  // Check for Bluetooth connections periodically
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
	  lastUpdate = millis();

    // Check if bluetooth is connected
    if (BLE.connected()) {
        if (connectionFlag == false) {
          Serial.println("Bluetooth Connected!");
          connectionFlag = true;
          // TODO: update / send users to mobile app
        }
        a++;                // increment testing variable
        TxBLE.setValue(a);  // send test data messages
    }
    else {  // No Bluetooth connection
      if (connectionFlag == true) { // Make sure flag is set correctly
        Serial.println("Bluetooth Disconnected.");
        connectionFlag = false;
      }
    }
  }

  delay(delayTime);
}
