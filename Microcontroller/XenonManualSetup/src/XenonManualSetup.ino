/*
 * Project XenonManualSetup
 * Description:
 * Author:
 * Date:
 */

#include "Particle.h"

#include "dct.h"

SYSTEM_MODE(MANUAL);    // avoid registration

const uint8_t smdLED = 7;


void setup() {
    // avoid registration / setup
    // 0x01 = setup done
    // 0xff = setup not done (go into listening mode at boot)
    const uint8_t val = 0x01;
    dct_write_app_data(&val, DCT_SETUP_DONE_OFFSET, 1);
        
    // light to notify of success
    pinMode(smdLED, OUTPUT);
    digitalWrite(smdLED, HIGH);

}

void loop() {

}