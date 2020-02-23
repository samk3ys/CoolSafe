/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "a:/Documents/Programming/GitHub/CoolSafe/Particle/BLE_demo/src/BLE_demo.ino"
/*
 * Project BLE demo
 * Description: demonstration of using BLE to communicate data between MCU (xenon) and an app
 * Author: Sam Keys
 * Date: 11/14/2019
 */

#include "Particle.h"
//#include "dct.h"

uint8_t dataToHex(uint8_t a, uint8_t b);
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
void setup();
void loop();
#line 11 "a:/Documents/Programming/GitHub/CoolSafe/Particle/BLE_demo/src/BLE_demo.ino"
SYSTEM_MODE(MANUAL);    // avoid registration

// Data formatting //////////////////////////////////////////////////////
// Operations
uint8_t noOP = 0x00;          // no operation. blank.
uint8_t newUser = 0x01;       // start the process for adding a new user
uint8_t deleteUser = 0x02;    // remove user and biometric
uint8_t disableUser = 0x03;   // remove user from group of authorized users
uint8_t editUser = 0x04;      // change user name or other property

// I/O
const uint8_t smdLED = 7;

// Set update timing
const unsigned long UPDATE_INTERVAL_MS = 2000;
unsigned long lastUpdate = 0;

// BLE Service UUID
const BleUuid serviceUuid("6E400000-B5A3-F393-E0A9-E50E24DCCA9E");
// BLE Characteristics UUIDs
//BleCharacteristic characteristicFirstName("First Name", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A8A"), serviceUuid);
//BleCharacteristic characteristicLastName("Last Name", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A90"), serviceUuid);
BleCharacteristic characteristicNameOut("Name", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A3D"), serviceUuid);
BleCharacteristic characteristicID("User Index", BleCharacteristicProperty::NOTIFY, BleUuid("0x2A9A"), serviceUuid);  // 0xFF for "Unknown User"
const char* myWriteUUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
BleCharacteristic myCharacteristic("myCharacteristic", BleCharacteristicProperty::WRITE_WO_RSP, myWriteUUID, serviceUuid, onDataReceived, (void*)myWriteUUID);
BleCharacteristic characteristicAccess("Access Enabled", BleCharacteristicProperty::NOTIFY, BleUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E"), serviceUuid);
BleCharacteristic characteristicB("b", BleCharacteristicProperty::NOTIFY, BleUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E"), serviceUuid);
BleCharacteristic characteristicC("c", BleCharacteristicProperty::NOTIFY, BleUuid("6E400004-B5A3-F393-E0A9-E50E24DCCA9E"), serviceUuid);

bool connectionFlag = false;

//#define BLE_DEFAULT_ADVERTISING_INTERVAL 500; // change advertising interval

/* void onDisconnected(BleOnDisconnectedCallback callback, void* context) {
  Serial.println("Bluetooth Disconnected.");
  connectionFlag = false;
} */

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
  // when data for myCharacteristic is recieved
  //RGB.color(100, 50, 100);
  
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

void setup() {
  
  // BLE ///////////////////////////////////
  // Attach characteristics
  BLE.addCharacteristic(myCharacteristic);
  myCharacteristic.onDataReceived(onDataReceived, NULL);
  BLE.addCharacteristic(characteristicAccess);
	//BLE.addCharacteristic(characteristicB);
	//BLE.addCharacteristic(characteristicC);

  // Start advertising service
  BleAdvertisingData adverData;
  adverData.appendServiceUUID(serviceUuid);
  BLE.advertise(&adverData);
  //////////////////////////////////////////

  // Setup USB Serial port
  Serial.begin(); // defaults to 9600 baud rate

  // take control of the On-Board RGB LED
  RGB.control(true);
  RGB.color(255, 255, 0); // Format red, green, blue, from 0 to 255
  RGB.brightness(64); // scales brightness of all three colors, 0-255

  // On-board LED
  pinMode(smdLED, OUTPUT);
  //digitalWrite(smdLED, HIGH);

  // Check EEPROM
  Serial.print("EEPROM Available: ");
  Serial.println(EEPROM.length());
}


uint8_t a = (uint8_t) 1;  // data sent over characteristic

void loop() {
  
  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
		lastUpdate = millis();

    if (BLE.connected()) {
        if (connectionFlag == false) {
          Serial.println("Bluetooth Connected!");
          connectionFlag = true;
        }
        
        // update data
        a++; 
        characteristicAccess.setValue(a);

        uint8_t b = (uint8_t) 2;
        //characteristicB.setValue(b);

        uint8_t c = (uint8_t) 3;
        //characteristicC.setValue(c);
    }
    else {  // No Bluetooth connection
      if (connectionFlag == true) {
        Serial.println("Bluetooth Disconnected.");
        connectionFlag = false;
      }
    }
  }

}