/*
 * Project: Cool Safe Code
 * Description: Main code that runs our senior capstone project for UK. It is a lock-bin locked by a solenoid that also has a fingerprint scanner and app to register fingerprints
 * Author: Sam Keys, Sylvester Okafor, Tylor Mendez, Shae Henry, Steven Deaton
 * Date: Spring 2020
 */

#include "Particle.h"               // library for Particle devices (we are using a Particle Xenon)
#include "FPS_GT511C3_Particle.h"   // library for fingerprint scanner functions
#include "playTones.h"              // file for playing sounds on a buzzer / speaker

SYSTEM_MODE(MANUAL);          // avoid Particle registration. Can't use BLE out-of-the-box without MANUAL control

// I/O
const uint8_t smdLED = 7;     // digital pin connected to a blue on-board LED on the Particle Xenon
const int keySwitch = 5;      // digital pin for the electro-mechanical switch with a key. Backup to fps.
const int Relay= 6;           // digital pin for relay signal. HIGH connects to normally open, LOW connects to normally closed
const int buzzer = A0;        // digital pin for sound output
const int orLED = 10;         // amber LED signifies registration mode
const int redLED = 11;         // red LED signifies an unauthorized user
const int greenLED = 12;         // green LED signifies an authorized

// Set update timing
const unsigned long UPDATE_INTERVAL = 1000;   // Milliseconds between updating BLE data
unsigned long lastUpdate = 0;

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

FPS_GT511C3 fps;  // fingerprint scanner module - uses Serial1 (Rx:pin14, Tx:pin15)

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
int goodSound[] = {NOTE_C5, NOTE_E5, NOTE_G5};
int goodDuration[] = {4, 4, 1};
int badSound[] = {NOTE_G4, NOTE_E4, NOTE_C4};
int badDuration[] = {4, 4, 1};

uint8_t dataToHex(uint8_t a, uint8_t b) {
  uint8_t hex = 0x00;
  switch (a) {
    case 'f':
      hex = 0xf0;
      break;
    case 'e':
      hex = 0xe0;
      break;
    case 'd':
      hex = 0xd0;
      break;
    case 'c':
      hex = 0xc0;
      break;
    case 'b':
      hex = 0xb0;
      break;
    case 'a':
      hex = 0xa0;
      break;
    case '9':
      hex = 0x90;
      break;
    case '8':
      hex = 0x80;
      break;
    case '7':
      hex = 0x70;
      break;
    case '6':
      hex = 0x60;
      break;
    case '5':
      hex = 0x50;
      break;
    case '4':
      hex = 0x40;
      break;
    case '3':
      hex = 0x30;
      break;
    case '2':
      hex = 0x20;
      break;
    case '1':
      hex = 0x10;
      break;
    default: // '0' or other:
      hex = 0x00;
  }
  switch (b) {
    case 'f':
      hex |= 0xf;
      break;
    case 'e':
      hex |= 0xe;
      break;
    case 'd':
      hex |= 0xd;
      break;
    case 'c':
      hex |= 0xc;
      break;
    case 'b':
      hex |= 0xb;
      break;
    case 'a':
      hex |= 0xa;
      break;
    case '9':
      hex |= 0x9;
      break;
    case '8':
      hex |= 0x8;
      break;
    case '7':
      hex |= 0x7;
      break;
    case '6':
      hex |= 0x6;
      break;
    case '5':
      hex |= 0x5;
      break;
    case '4':
      hex |= 0x4;
      break;
    case '3':
      hex |= 0x3;
      break;
    case '2':
      hex |= 0x2;
      break;
    case '1':
      hex |= 0x1;
      break;
    default: // '0' or other:
      hex |= 0x00;
  }
  return hex;
}

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  // when data for when data is recieved through the RxBLE characteristic
  
  const char * dataChars = (char *) data;
  Serial.write(dataChars);
  Serial.write('\n');
  
  // Hex code in to change LED color
  uint8_t red = dataToHex(data[0], data[1]);
  uint8_t green = dataToHex(data[2], data[3]);
  uint8_t blue = dataToHex(data[4], data[5]);
  RGB.color(red, green, blue);

  //char operationID = data[0];
  /*uint8_t operationID = 0;      // default to No operation
  for (size_t i=0; i<1; i++) {  // first byte
    operationID = data[i];
  }*/

  /* switch (operationID) {
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
  } */
  
}

void openBin() {
  // Open the lock-bin by connecting the relays common and normally open, actuating the solenoid.
  digitalWrite(Relay, HIGH);
  delay(1000);                // How long the solenoid stays pulled in
  digitalWrite(Relay, LOW);
}

void goodFeedback() {
  digitalWrite(greenLED, HIGH);
  // play buzzer song
  play(buzzer, arraySize(MarioDuration), MarioNotes, MarioDuration);
  digitalWrite(greenLED, LOW);
  RGB.color(255, 255, 0);
}

void badFeedback() {
  digitalWrite(redLED, HIGH);
  // play buzzer song
  play(buzzer, arraySize(DoomDuration), DoomNotes, DoomDuration);
  digitalWrite(redLED, LOW);
}

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
  Serial1.begin(9600);          // FPS UART uses Rx (pin 14) & Tx (pin 15) on Particle Xenon
  fps.UseSerialDebug = true;    // Sends messages to USB Serial for debugging
	//fps.Open();                 //send serial command to initialize fps. Make sure FPS is connected or the program won't go past this part
  
  pinMode(keySwitch, INPUT);    // electro-mechanical switch w/ a key
  pinMode(Relay, OUTPUT);       // signal to relay for switching solenoid
  //pinMode(buzzer, OUTPUT);      // sound buzzer, not necessary for using tone()
  pinMode(orLED, OUTPUT);       // amber registration mode LED
  pinMode(redLED, OUTPUT);       // red access denied LED
  pinMode(greenLED, OUTPUT);       // green access permitted LED
  //////////////////////////////////////////

  // Debugging /////////////////////////////
  // Setup USB Serial port
  Serial.begin(); // defaults to 9600 baud rate
  // On-board LED control
  pinMode(smdLED, OUTPUT);      // blue on-board LED attached to pin 7
  //digitalWrite(smdLED, HIGH);
  RGB.control(true);            // take control of the On-Board RGB LED
  RGB.color(0, 255, 0);         // Format red, green, blue, from 0 to 255
  RGB.brightness(64);           // scales brightness of all three colors, 0-255
  // Check EEPROM
  //Serial.print("EEPROM Available: ");
  //Serial.println(EEPROM.length());    // 4096 available
  //goodFeedback();
  //badFeedback();
  //play(buzzer, arraySize(Tetris), Tetris, TetrisDuration);  // Play Tetris-ish sound
  play(buzzer, arraySize(startSound), startSound, startDuration); // setup feedback
  delay(1000);
  play(buzzer, arraySize(goodSound), goodSound, goodDuration); // setup feedback
  delay(1000);
  play(buzzer, arraySize(badSound), badSound, badDuration); // setup feedback
  //////////////////////////////////////////
}

uint8_t a = (uint8_t) 1;  // variable for testing BLE incrementing data
void loop() {
  RGB.color(0, 255, 255);
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
	  lastUpdate = millis();

    if (BLE.connected()) {
        if (connectionFlag == false) {
          Serial.println("Bluetooth Connected!");
          connectionFlag = true;
          // TODO: update / send users to mobile app
        }
        a++;                // increment testing variable
        TxBLE.setValue(a);  // semd test data messages

    }
    else {  // No Bluetooth connection
      if (connectionFlag == true) {
        Serial.println("Bluetooth Disconnected.");
        connectionFlag = false;
      }
    }

    // Check for user input (fingerprint or key)
    if (0) { //(fps.IsPressFinger() || digitalRead(keySwitch) == HIGH) {  // Make sure fps is connected and defined
      RGB.color(255, 0, 0);
      int userID = fps.Identify1_N();
      // TODO: Check if userID has access or is disabled
      if (userID || digitalRead(keySwitch) == HIGH) { // authorized user
        openBin();
        goodFeedback();
      }
      else {  // unauthorized user
        badFeedback();
      }
    }

  } // end update
  RGB.color(0, 0, 255);
  delay(1000);
}