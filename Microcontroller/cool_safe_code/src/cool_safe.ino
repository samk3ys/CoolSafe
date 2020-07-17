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
SYSTEM_MODE(MANUAL);                          // Avoid Particle registration. Can't use BLE out-of-the-box without MANUAL control. Could changed to AUTOMATIC for easier cloud integration in the future.
bool connectionFlag = false;                  // flag for remembering if BLE is connected to a device
//#define BLE_DEFAULT_ADVERTISING_INTERVAL 500; // change advertising interval
bool keySwitchFlag = false;                   // tracks state of the manual key switch to limit use to once per turn
const int maxUsers = 200;                     // Maximum number of users able to be enrolled on the FPS
const int unlockTime = 1000;                  // milliseconds to keep solenoid powered (pulled in) when opening the lock-bin
const int delayTime = 100;                    // milliseconds to delay after a round of the main infinite loop
const unsigned long UPDATE_INTERVAL = 1000;   // milliseconds between updating BLE data
unsigned long lastUpdate = 0;                 // used for tracking BLE update interval
const unsigned long ENROLLMENT_WAIT = 60;     // seconds to wait for fingerprint enrollment
unsigned long enrollStartTime = 0;            // tracks time since an enrollment started
unsigned long enrollmentWatch = 0;            // tracks how much time has passed while waiting for fingerprint enrollment

// I/O
const int buzzer    = A0;                     // digital pin for sound output
const int relay     = A4;                     // digital pin for relay signal. HIGH connects to normally open, LOW connects to normally closed
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

// BLE Service UUID
const BleUuid serviceUuid("6E400000-B5A3-F393-E0A9-E50E24DCCA9E");
// BLE Characteristics UUIDs - UART Protocol regarding mobile device as host (i.e. Rx for app receives data and Tx for app transmits data)
const char* writeUUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
BleCharacteristic RxBLE("receive", BleCharacteristicProperty::WRITE_WO_RSP, writeUUID, serviceUuid, onDataReceived, (void*)writeUUID);
BleCharacteristic TxBLE("transmit", BleCharacteristicProperty::NOTIFY, BleUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E"), serviceUuid);

// Operations
uint8_t operation;  // tracks the next operation. Set by BLE handler, executed in main loop
uint8_t storedID;   // holds a user ID to be operated on
char* storedName;  // holds a name for editing a user
const uint8_t noOP        = 0x00;      // NUL. no operation. blank.
const uint8_t newUser     = 0x41;      // A. start the process for adding a new user
const uint8_t deleteUser  = 0x42;      // B. remove user and biometric
const uint8_t disableUser = 0x43;      // C. remove user from group of authorized users
const uint8_t enableUser  = 0x44;      // D. add a disabled user to the group of authorized users
const uint8_t editUser    = 0x45;      // E. change user name or other property
// Operation function prototypes
bool addUser(void);
bool removeUser(uint8_t id);
bool deauthorizeUser(uint8_t id);
bool authorizeUser(uint8_t id);
bool changeUserName(uint8_t id, String name);

// Function called when the app sends data through TxBLE to this device
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  // for when data is recieved through the RxBLE characteristic
  const char * dataChars = (char *) data;
  //String dataChars = (char *) data;
  Serial.printlnf("BLE Received: %s Hex: %x", dataChars, data);

  // Data byte format: 
  //    0=op, 
  //    1=id (up to 256 users), 
  //    rest of the data = extra arguments (i.e. name)
  uint8_t operationID = data[0];
  //uint8_t userID = data[1];
  storedID = data[1];
  bool error = false;  // Tracks if there's an error in executing an op. Initialized to 0 = no error

  switch (operationID) {
    case newUser:
      RGB.color(0, 100, 0);
      operation = newUser;
      break;
    case deleteUser:
      RGB.color(100, 0, 0);
      operation = deleteUser;
      break;
    case disableUser:
      RGB.color(100, 0, 100);
      operation = disableUser;
      break;
    case enableUser:
      RGB.color(0, 100, 100);
      operation = enableUser;
      break;
    case editUser:
      RGB.color(100, 100, 100);
      //storedName = dataChars.remove(0,1);    // remove op and id
      strcpy(storedName, dataChars);
      Serial.printlnf("Passing name: %s", storedName);
      operation = editUser;
      break;
    default:  // no-op or unrecognized
      RGB.color(0, 0, 0);
  }

  /*switch (operationID) {
    case newUser:
      RGB.color(0, 100, 0);
      //error = addUser(userID);
      addUser();
      break;
    case deleteUser:
      RGB.color(100, 0, 0);
      error = removeUser(userID);
      break;
    case disableUser:
      RGB.color(100, 0, 100);
      error = deauthorizeUser(userID);
      break;
    case enableUser:
      RGB.color(0, 100, 100);
      error = authorizeUser(userID);
      break;
    case editUser:
      RGB.color(100, 100, 100);
      dataChars.remove(0,1);    // remove op and id
      error = changeUserName(userID, dataChars);
      break;
    default:  // no-op or unrecognized
      RGB.color(0, 0, 0);
  }*/
  
  Serial.printlnf("BLE operation results: %d", error);
  // Return the results
  /*if (BLE.connected()) {
    Serial.println("Sending results.");
    TxBLE.setValue(error);  // send result of operation (app wants to know "error or no error?")
  } else {
    Serial.println("Bluetooth device disconnected before our operation results were sent.");
  }*/
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

bool isBitSet(int8_t byte, int8_t bit) {
  // return true if the bit in byte is a 1, false if it's a 0
  if (byte & (1 << (bit)))
    return true;
  else
    return false;
}

bool idAvailable(int id) {  // Reads user flag bit to see if id is being used currently (1=available, 0=used)
  // 1st 200 bits (25 bytes) correlate to is this ID being used? This is true for max = 200 users
  // Note: each EEPROM bit inits to 1. So 0 = used, 1 = available
  return isBitSet(EEPROM.read(id/8), id%8);
}

void userWrite(int id, int value) {
  // Write value to the flag bit for if this user id is being used or not (0 = used, 1= available)
  // First 25 bytes of EEPROM available for this purpose
  int addr = id/8;                // EEPROM data address for that contains this ID's "availability flag" bit
  char data = EEPROM.read(addr);  // data currently in EEPROM address
  char mask = 1<<(id%8);          // mask the flag's bit position
  if (value == 0) {
    Serial.printlnf("Writing to EEPROM. Addr: %d. Read: %x. Write: %x.", addr, data,  data & ( ~mask ));
    EEPROM.write(addr, data & (~mask) );  // AND the flag bit with 0 to take availability
    Serial.printlnf("User ID #%d now used", id);
  }
  else if (value == 1) {
    Serial.printlnf("Writing to EEPROM. Addr: %d. Read: %x. Write: %x.", addr, data,  data | mask );
    EEPROM.write(addr, data | (mask) );  // OR the flag bit with 1 to set availability
    Serial.printlnf("User ID #%d now available", id);
  }
}

bool isAuthorized(int id) {  
  // Reads user's authorized flag bit (1=authorization enabled, 0=authorization disabled)
  int offset = 25;  // 1st 200 bits (25 bytes) for user available flag bits
  return isBitSet(EEPROM.read(id/8+offset), id%8);
}

void authWrite(int id, int value) { // write 1 to enable a user or 0 to disable a user
  int offset = 25;  // 1st 200 bits (25 bytes) for user available flag bits
  int addr = id/8+offset;         // EEPROM data address for that contains this ID's "authorization flag" bit
  char data = EEPROM.read(addr);  // data currently in EEPROM address
  char mask = 1<<(id%8);          // mask the flag's bit position
  if (value == 0) {
    Serial.printlnf("Disabling user#%d. Addr: %d. Read: %x. Write: %x.", addr, data,  data & ( ~mask ));
    EEPROM.write(addr, data & (~mask) );  // AND the flag bit with 0 to take availability
  }
  else if (value == 1) {
    Serial.printlnf("Authorizing user#%d. Addr: %d. Read: %x. Write: %x.", addr, data,  data | mask );
    EEPROM.write(addr, data | (mask) );  // OR the flag bit with 1 to set availability
  }
  Serial.printlnf("User ID #%d now has access level: %d", id, value);
}

void nameWrite(int id, String name) {
  // write a name to EEPROM for this user ID
  int offset = 50;  // 25 bytes for used flags, 25 bytes for authorized flags
  int maxNameLength = 19;
  int len = name.length();
  
  Serial.printlnf("Naming user ID #%d: %s", id, name);

  if (len < maxNameLength) {        // add NULL chars to replace any garbage
    for (int i = 0; i < maxNameLength-len; i++)
      name.concat(" "); // Null is skipped, use space, remember to trim
  }
  // if len == maxNameLength it's already good to go
  else if (len > maxNameLength) {   // trim name
    name.remove(maxNameLength);
  }

  Serial.printlnf("Naming user ID #%d: %s", id, name);

  EEPROM.put(id*maxNameLength+offset, name);  // Write 19 bytes that correspond to the user ID
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

bool enrollUser() {
  // Enroll a new fingerprint for the scanner. Returns true if successful. Returns false if timed out or error.

  // Signal to users that the system is busy. Normal functions won't work at this time
  digitalWrite(busyLED, HIGH);
  RGB.color(100, 100, 0);
  // Is there room?
  /*if (fps.GetEnrollCount() == maxUsers) {  // FPS full
    Serial.println("Error: no more enrollment IDs available.");
    digitalWrite(busyLED, LOW);
    RGB.color(0, 0, 0);
    return false;
  }*/

  // Jot down the time this enrollment started.
  enrollStartTime = millis();

  // find open enroll id
  /*
  int enrollid = (id=-1) ? 0 : id;  // manual or app addition?
  if (id == -1) {  // manual addition. Enroll the next available ID  
    bool usedid = true;
    while (usedid == true) {
      usedid = fps.CheckEnrolled(enrollid);
      if (usedid==true) enrollid++;
    } 
  } 
  else {  // Enroll the same ID provided - which currently is n/a
    if (fps.CheckEnrolled(id)) {  // space occupied
        fps.DeleteID(id); 
    }
  }
  */
  //int enrollid = 0;
  int enrollid = 32; // start at actual ascii chars so I can send commands thru a terminal app on my phone
  bool usedid = true;
  while (usedid == true) {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  Serial.printlnf("ID #%d is next available on FPS.", enrollid);

  // Check if EEPROM is using this ID
  if ( idAvailable(enrollid) ) {
    Serial.printlnf("ID #%d available.", enrollid);
  }
  else {
    Serial.printlnf("ID #%d NOT available.", enrollid);
    digitalWrite(busyLED, LOW);
    RGB.color(0, 0, 0);
    return false;
  }

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

  // Add user to EEPROM as "New User"
  userWrite(enrollid, 0);           // Mark this ID as used
  authWrite(enrollid, 1);           // Make sure they're authorized
  nameWrite(enrollid, "New User");  // Give them a new name

  // Let the users know the system is back to normal
  digitalWrite(busyLED, LOW);
  RGB.color(0, 0, 0);
  return true;  // exit w/ success
}

bool addUser(void) {
  // Add a new user and enroll their fingerprint
  // Return 0 if success, 1 if there's an error
  bool success = enrollUser();  // Add fingerprint to FPS memory and MCU EEPROM
  return success;
}

bool removeUser(uint8_t id) {
  // Delete a user from the list and FPS module
  // Return 0 if success, 1 if there's an error
  bool success = fps.DeleteID(id);  // Delete user from FPS
  // Delete user from EEPROM. Re-init corresponding EEPROM bits to 1
  userWrite(id, 1); 
  authWrite(id, 1);
  return success;
}

bool deauthorizeUser(uint8_t id) {
  // Disallow a user to be granted access
  // Return 0 if success, 1 if there's an error
  authWrite(id, 0);   // Authorization bit (0 = not authorized)
  return true;
}

bool authorizeUser(uint8_t id) {
  // Allow a disabled user access again
  // Return 0 if success, 1 if there's an error
  authWrite(id, 1);   // Authorization bit (1 = authorized)
  return true;
}

bool changeUserName(uint8_t id, String name) {
  // Change the name associated with an already existing user
  // Return 0 if success, 1 if there's an error

  // 4096 EEPROM available on Xenon 
  //  1 bytes required per user: ID (+ 25 extra for all authorization bits (8*25=200))
  //  Means up to 19 chars in user names
  //  3800 bytes for user names, 200 for user IDs, 96 left (for a password, misc.)

  nameWrite(id, name);
  return true;
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
  Serial.print("EEPROM Available: ");
  Serial.println(EEPROM.length());    // 4096 bytes available
  
  // BLE
  //BLE.selectAntenna(BleAntennaType::EXTERNAL);
  // Attach characteristics
  BLE.addCharacteristic(TxBLE);
  BLE.addCharacteristic(RxBLE);
  RxBLE.onDataReceived(onDataReceived, NULL);
  // Start advertising service
  BleAdvertisingData adverData;
  adverData.appendServiceUUID(serviceUuid);
  BLE.advertise(&adverData);
  // On Connected/Disconnected
  //void onConnected(BleOnConnectedCallback bleConnect, void* context);
  //void onDisconnected(BleOnConnectedCallback bleDisconnect, void* context);

  // Device I/O
	fps.Open();                   // Send serial command to initialize fps. FPS UART uses Rx (pin 14) & Tx (pin 15) on Particle Xenon. Make sure FPS is connected or the program won't go past this part
  fps.SetLED(true);             // Visual test to make sure the fps is connected. Leave on for being able to detect fingerprints
  //delay(500);
  //fps.SetLED(false);
  pinMode(buzzer, OUTPUT);      // sound buzzer, not necessary for using tone()
  pinMode(relay, OUTPUT);       // signal to relay for switching solenoid
  pinMode(keySwitch, INPUT_PULLUP);    // electro-mechanical switch w/ a key
  pinMode(greenLED, OUTPUT);    // green access permitted LED
  pinMode(busyLED, OUTPUT);     // amber registration mode LED
  pinMode(redLED, OUTPUT);      // red access denied LED
  LEDsequence();                // turn all LEDs on at start to make sure they work

  // Setup done. Play start-up sound, signifying that the system is ready to be used
  play(buzzer, arraySize(startDuration), startSound, startDuration);

  // Enroll test
  //digitalWrite(busyLED, HIGH);
  //enrollUser();
  //digitalWrite(busyLED, LOW);

  // Initialize operation as none
  operation = noOP;

  // Reset System: DO NOT UNCOMMENT UNLESS YOU ARE SURE
  //EEPROM.clear();   // ERASES ALL EEPROM (inits all bits to 1)
  //fps.DeleteAll();  // ERASES FPS ENROLLMENTS
}

uint8_t a = (uint8_t) 1;  // variable for testing BLE - incrementing data
void loop() {
  RGB.color(0, 0, 255);
 
  // Check for users trying to access using a fingerprint
  if(fps.IsPressFinger()) {
    RGB.color(255, 255, 255);   // white LED for debugging to show that a finger is recognized and the FPS is working on figuring out the user
    identifyUser();
    // TODO: Check if user is allowed access (or is disabled)
  }
  
  // Check for users trying to access using the electro-mechanical tumbler lock switch
  if (digitalRead(keySwitch) == LOW && keySwitchFlag == false) {     // Key switch turned on
    delay(250);
    if (digitalRead(keySwitch) == HIGH) {  // Enter registration mode manually by flicking the switch on and off quickly      
      enrollUser();
    }
    else {  // normal use of switch to open lock-bin
      //openBin();                      // Allow access. Includes good feedback
      goodFeedback();                 // Only need feedback for 2nd PCB onward b/c switch directly connects 12V to solenoid
      keySwitchFlag = true;           // Set flag so we don't unlock again before turning the key off
    }
  } 
  else if (digitalRead(keySwitch) == HIGH && keySwitchFlag == true) {  // Key switch turned off
    keySwitchFlag = false;          // Reset flag so the key can be used again
  }
  
  else if (operation != noOP) {  // execute the next operation
    bool error;
    Serial.printlnf("Operation: %x, User: %d", operation, storedID);
    switch (operation) {
      case newUser:
        addUser();
        break;
      case deleteUser:
        //removeUser(storedID);
        removeUser(storedID);
        break;
      case disableUser:
        error = deauthorizeUser(storedID);
        break;
      case enableUser:
        error = authorizeUser(storedID);
        break;
      case editUser:
        error = changeUserName(storedID, storedName);
        break;
      default:  // no-op or unrecognized
        ;
    }
    Serial.printlnf("Operation %d returned %d", operation, error);
    operation = noOP; // reset operation so we don't loop
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
