/* Project BLE-PowerSource
 * Description: BLE example from Particle website. https://rickkas7.github.io/ble-powersource/
 */

#include "Particle.h"

// Requires the DiagnosticsHelperRK library
#include "DiagnosticsHelperRK.h"

// This example does not require the cloud so you can run it in manual mode or
// normal cloud-connected mode
SYSTEM_MODE(MANUAL);

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

const unsigned long UPDATE_INTERVAL_MS = 2000;
unsigned long lastUpdate = 0;

// Private battery and power service UUID
const BleUuid serviceUuid("5c1b9a0d-b5be-4a40-8f7a-66b36d0a5176");

BleCharacteristic batStateCharacteristic("batState", BleCharacteristicProperty::NOTIFY, BleUuid("fdcf4a3f-3fed-4ed2-84e6-04bbb9ae04d4"), serviceUuid);
BleCharacteristic powerSourceCharacteristic("powerSource", BleCharacteristicProperty::NOTIFY, BleUuid("cc97c20c-5822-4800-ade5-1f661d2133ee"), serviceUuid);
BleCharacteristic batLevelCharacteristic("batLevel", BleCharacteristicProperty::NOTIFY, BleUuid("d2b26bf3-9792-42fc-9e8a-41f6107df04c"), serviceUuid);


// We don't actually have a thermometer here, we just randomly adjust this value
float lastValue = 37.0; // 98.6 deg F;

uint8_t lastBattery = 100;

void setup() {
	(void)logHandler; // Does nothing, just to eliminate the unused variable warning

	BLE.addCharacteristic(batStateCharacteristic);
	BLE.addCharacteristic(powerSourceCharacteristic);
	BLE.addCharacteristic(batLevelCharacteristic);

	BleAdvertisingData advData;
	// Advertise our private service only
	advData.appendServiceUUID(serviceUuid);
	// Continuously advertise when not connected
	BLE.advertise(&advData);
}

void loop() {
	if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
		lastUpdate = millis();

		if (BLE.connected()) {
		    // POWER_SOURCE_UNKNOWN = 0,
			// POWER_SOURCE_VIN = 1,
			// POWER_SOURCE_USB_HOST = 2,
			// POWER_SOURCE_USB_ADAPTER = 3,
			// POWER_SOURCE_USB_OTG = 4,
			// POWER_SOURCE_BATTERY = 5
			//uint8_t powerSource = (uint8_t) DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_POWER_SOURCE);
			uint8_t powerSource = (uint8_t) 2;
			powerSourceCharacteristic.setValue(powerSource);

			// BATTERY_STATE_UNKNOWN = 0,
			// BATTERY_STATE_NOT_CHARGING = 1,
			// BATTERY_STATE_CHARGING = 2,
			// BATTERY_STATE_CHARGED = 3,
			// BATTERY_STATE_DISCHARGING = 4,
			// BATTERY_STATE_FAULT = 5,
			// BATTERY_STATE_DISCONNECTED = 6
			//uint8_t batState = (uint8_t) DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_BATTERY_STATE);
			uint8_t batState = (uint8_t) 6;
			batStateCharacteristic.setValue(batState);

			// Diagnostics data is a fixed point number, convert to integer 0-100 %
			//uint8_t batLevel = (uint8_t) (DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_BATTERY_CHARGE) >> 8);
			uint8_t batLevel = (uint8_t) lastBattery--;
			batLevelCharacteristic.setValue(batLevel);
			if (lastBattery <= 0) {
				lastBattery = 100;
			}				
		}
	}
}