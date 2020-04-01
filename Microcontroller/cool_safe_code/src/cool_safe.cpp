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

#include "Particle.h"               // library for Particle devices (we are using a Particle Xenon)
#include "FPS_GT511C3_Particle.h"   // library for fingerprint scanner functions
#include "playTones.h"              // file for playing sounds on a buzzer / speaker

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
void openBin();
void goodFeedback();
void badFeedback();
void setup();
void loop();
#line 12 "a:/Documents/Programming/GitHub/CoolSafe/Microcontroller/cool_safe_code/src/cool_safe.ino"
SYSTEM_MODE(MANUAL);          // avoid Particle registration. Can't use BLE out-of-the-box without MANUAL control

// I/O
const uint8_t smdLED = 7;     // digital pin connected to a blue on-board LED on the Particle Xenon
const int buzzer = A0;        // digital pin for sound output
const int Relay= A4;          // digital pin for relay signal. HIGH connects to normally open, LOW connects to normally closed
const int keySwitch = D5;     // digital pin for the electro-mechanical switch with a key. Backup to fps.
const int greenLED = D6;      // green LED signifies an authorized
const int orLED = D7;         // amber LED signifies registration mode
const int redLED = D8;        // red LED signifies an unauthorized user

// Timings
const int unlockTime = 1000;                  // milliseconds to keep solenoid powered (pulled in) when opening the lock-bin
const unsigned long UPDATE_INTERVAL = 1000;   // milliseconds between updating BLE data
unsigned long lastUpdate = 0;                 // used for tracking BLE update interval

// BLE Service UUID
const BleUuid serviceUuid("6E400000-B5A3-F393-E0A9-E50E24DCCA9E");
//#define BLE_DEFAULT_ADVERTISING_INTERVAL 500; // change advertising interval
// BLE Characteristics UUIDs
//BleCharacteristic characteristicFirstName("First Name", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A8A"), serviceUuid);
//BleCharacteristic characteristicLastName("Last Name", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A90"), serviceUuid);
//BleCharacteristic characteristicNameOut("Name", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A3D"), serviceUuid);
//BleCharacteristic characteristicID("User Index", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A9A"), serviceUuid);  // 0xFF for "Unknown User"
const char* writeUUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
BleCharacteristic RxBLE("receive", BleCharacteristicProperty::WRITE_WO_RSP, writeUUID, serviceUuid, onDataReceived, (void*)writeUUID);
BleCharacteristic TxBLE("transmit", BleCharacteristicProperty::NOTIFY, BleUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E"), serviceUuid);

bool connectionFlag = false;

// Operations
uint8_t noOP = 0x00;          // no operation. blank.
uint8_t newUser = 0x01;       // start the process for adding a new user
uint8_t deleteUser = 0x02;    // remove user and biometric
uint8_t disableUser = 0x03;   // remove user from group of authorized users
uint8_t editUser = 0x04;      // change user name or other property

FPS_GT511C3 fps;              // fingerprint scanner module - uses Serial1 (Rx:pin14, Tx:pin15)

// Buzzer songs
int MarioNotes[] =    {NOTE_E5, NOTE_E5, 0, NOTE_E5, 0, NOTE_C5, NOTE_E5, 0, NOTE_G5, 0, 0, NOTE_G4};	// notes in the melody
int MarioDuration[] = {4,		    4, 	     4, 4,	     4, 4,	     4,		    4, 4,		    2, 4, 4		   };	// note durations: 4 = quarter note, 2 = half note, etc.
int DoomNotes[] =    {NOTE_DS3, NOTE_DS3, 0, NOTE_DS3, NOTE_DS3, 0, NOTE_DS3, NOTE_DS3, NOTE_B3, NOTE_DS3, NOTE_DS3, NOTE_A3, 0, NOTE_DS3, NOTE_AS3, NOTE_B3, NOTE_DS3, NOTE_DS3, 0, NOTE_DS3, NOTE_DS3, NOTE_B3, NOTE_DS3, NOTE_DS3};
int DoomDuration[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
int Tetris[] =         {NOTE_E6, NOTE_B6, NOTE_C6, NOTE_D6, NOTE_C6, NOTE_B6, NOTE_A6,
                        NOTE_A6, NOTE_C6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_B6, NOTE_C6,
                        NOTE_D6, NOTE_E6, NOTE_C6, NOTE_A6, NOTE_A6};
int TetrisDuration[] = {2, 4, 4, 2, 4, 4, 4,
                        4, 4, 2, 4, 4, 2, 4,
                        4, 4, 4, 4, 4};
int startSound[] =    {NOTE_C3, NOTE_C4, NOTE_D4, NOTE_C5, NOTE_D5, NOTE_C6};
int startDuration[] = {2, 4, 4, 4, 4, 1};
int goodSound[] = {NOTE_C3, NOTE_E3, NOTE_G3};
int goodDuration[] = {4, 4, 1};
int badSound[] = {NOTE_G3, NOTE_E3, NOTE_C3};
int badDuration[] = {4, 4, 1};

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  // when data for when data is recieved through the RxBLE characteristic
  const char * dataChars = (char *) data;
  Serial.write(dataChars);
  Serial.write('\n');
  
  // Hex code in to change LED color
  //uint8_t red = dataToHex(data[0], data[1]);
  //uint8_t green = dataToHex(data[2], data[3]);
  //uint8_t blue = dataToHex(data[4], data[5]);
  //RGB.color(red, green, blue);
}

void openBin() {
  // Open the lock-bin by connecting the relays common and normally open, actuating the solenoid.
  digitalWrite(Relay, HIGH);
  goodFeedback();
  delay(unlockTime);                // How long the solenoid stays pulled in
  digitalWrite(Relay, LOW);
}

void goodFeedback() {
  // green light and good sound
  digitalWrite(greenLED, HIGH);
  RGB.color(0, 255, 0);
  play(buzzer, arraySize(goodSound), goodSound, goodDuration);
  digitalWrite(greenLED, LOW);
  RGB.color(0, 0, 0);
}

void badFeedback() {
  // red light and bad sound
  digitalWrite(redLED, HIGH);
  RGB.color(255, 0, 0);
  play(buzzer, arraySize(badSound), badSound, badDuration);
  digitalWrite(redLED, LOW);
  RGB.color(0, 0, 0);
}
/*
void identify() {
  // Identify fingerprint test
  if (fps.IsPressFinger()) {
    fps.SetLED(true); //added
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    
    int maxIDs = 200; //<- change id value depending model you are using
    if (id < maxIDs) {
      //if the fingerprint matches, provide the matching template ID
      Serial.print("Verified ID:");
      Serial.println(id);
      openBin();
      goodFeedback();
    }
    else {
      //if unable to recognize
      Serial.println("Finger not found");
      badFeedback();
    }
  }
  else
  {
    Serial.println("Please press finger");
  }
  delay(100);
}

void enroll() {
  // Enroll test

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
*/
void setup() {
  // BLE ///////////////////////////////////
  // Attach characteristics
  BLE.addCharacteristic(TxBLE);
  BLE.addCharacteristic(RxBLE);
  RxBLE.onDataReceived(onDataReceived, NULL);
  // Start advertising service
  BleAdvertisingData adverData;
  adverData.appendServiceUUID(serviceUuid);
  BLE.advertise(&adverData);
  //////////////////////////////////////////

  // Device I/O ////////////////////////////
  //Serial1.begin(9600);          // FPS UART uses Rx (pin 14) & Tx (pin 15) on Particle Xenon
  //fps.UseSerialDebug = true;    // Sends messages to USB Serial for debugging
	fps.Open();                   // send serial command to initialize fps. Make sure FPS is connected or the program won't go past this part
  fps.SetLED(true);             // visual test to make sure the fps is connected. Leave on for being able to detect fingerprints
  //delay(500);
  //fps.SetLED(false);

  pinMode(keySwitch, INPUT);    // electro-mechanical switch w/ a key
  pinMode(Relay, OUTPUT);       // signal to relay for switching solenoid
  pinMode(buzzer, OUTPUT);      // sound buzzer, not necessary for using tone()
  pinMode(orLED, OUTPUT);       // amber registration mode LED
  pinMode(redLED, OUTPUT);      // red access denied LED
  pinMode(greenLED, OUTPUT);    // green access permitted LED
  //////////////////////////////////////////

  // Debugging /////////////////////////////
  // Setup USB Serial port
  Serial.begin(); // defaults to 9600 baud rate
  // On-board LED control
  //pinMode(smdLED, OUTPUT);      // blue on-board LED attached to pin 7
  //digitalWrite(smdLED, HIGH);
  RGB.control(true);            // take control of the On-Board RGB LED
  RGB.color(255, 255, 255);     // Format red, green, blue, from 0 to 255
  RGB.brightness(64);           // scales brightness of all three colors, 0-255
  // Check EEPROM
  //Serial.print("EEPROM Available: ");
  //Serial.println(EEPROM.length());    // 4096 available
  //goodFeedback();
  //badFeedback();
  //play(buzzer, arraySize(Tetris), Tetris, TetrisDuration);  // Play Tetris-ish sound
  play(buzzer, arraySize(startDuration), startSound, startDuration); // setup feedback
  delay(1000);
  //////////////////////////////////////////
}

 uint8_t a = (uint8_t) 1;  // variable for testing BLE incrementing data
void loop() {
  RGB.color(0, 0, 255);
 
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
    
    // Check for users trying to access using a fingerprint
    if(fps.IsPressFinger()) {
      RGB.color(255, 255, 255);   // white LED
      
      fps.CaptureFinger(false);   // take a low quality image (faster)
      int id = fps.Identify1_N(); // check if the fingerprint is stored in the FPS

      if (id >= 0 && id < 200) {  // valid value for a user id on the GT-521F32 FPS
        Serial.print("Verified ID:");
        Serial.println(id);
        // TODO: Check if user is allowed access (or is disabled)
        openBin();  // includes good feedback
      }
      else {//if unable to recognize
        Serial.println("Finger not found");
        badFeedback();
      }
    }
    
    // Check for users trying to access using the electro-mechanical tumbler lock switch
    if (digitalRead(keySwitch) == HIGH) {
      openBin();  // includes goodFeedback();
      delay(2000);
    }

  } // end update
  //RGB.color(0, 0, 255);
  //delay(1000);
}

/*
void loop()
{
	RGB.color(0, 0, 255);
  // Identify fingerprint test
	if (fps.IsPressFinger())
	{
		fps.CaptureFinger(false);
		int id = fps.Identify1_N();

		if (id <200) //<- change id value depending model you are using
		{//if the fingerprint matches, provide the matching template ID
			RGB.color(0, 255, 0);
      Serial.print("Verified ID:");
			Serial.println(id);
		}
		else
		{//if unable to recognize
			Serial.println("Finger not found");
      RGB.color(255, 0, 0);
		}
	}
	else
	{
		Serial.println("Please press finger");
	}
	delay(500);
}
*/