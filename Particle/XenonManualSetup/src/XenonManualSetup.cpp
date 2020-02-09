/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "a:/Documents/Programming/Particle/XenonManualSetup/src/XenonManualSetup.ino"
/*
 * Project XenonManualSetup
 * Description:
 * Author:
 * Date:
 */

#include "Particle.h"

#include "dct.h"

void setup();
void blink();
void dit();
void dah();
void loop();
#line 12 "a:/Documents/Programming/Particle/XenonManualSetup/src/XenonManualSetup.ino"
SYSTEM_MODE(MANUAL);    // avoid registration

const uint8_t LED = 11;
const uint8_t smdLED = 7;
const uint timeUnit = 300;   // morse code time unit
const uint letterGap = timeUnit*2; // gap btwn letters (3 - intrinsic 1)
const uint wordSpace = timeUnit*4; // gap btwn words (7 - intrinsic 3)

void setup() {
    // avoid registration / setup
    // 0x01 = setup done
    // 0xff = setup not done (go into listening mode at boot)
    const uint8_t val = 0x01;
    dct_write_app_data(&val, DCT_SETUP_DONE_OFFSET, 1);

    // morse code communicator
    pinMode(LED, OUTPUT);
    
    //pinMode(smdLED, OUTPUT);
    //digitalWrite(smdLED, HIGH);

}

void blink() {
    // blink LED twice in a second
    digitalWrite(LED, HIGH);
    delay(500);
    
    digitalWrite(LED, LOW);
    delay(500);
}

void dit() {
    // light LED for 1 time unit
    digitalWrite(LED, HIGH);
    delay(timeUnit);
    digitalWrite(LED, LOW);
    delay(timeUnit);
}

void dah() {
    // light LED for 3 time units
    digitalWrite(LED, HIGH);
    delay(timeUnit*3);
    digitalWrite(LED, LOW);
    delay(timeUnit);
}

void loop() {
    //String morseMessage = "Hello, World!";
    String morseMessage = "S||O||S";
    //String morseMessage = "115";
    morseMessage.toUpperCase();
    
    for (uint i = 0; i < morseMessage.length(); i++) {
        char current = morseMessage.charAt(i);

        switch (current)
        {
        case 'A': //.-
            dit(); dah();                       delay(letterGap);
            break;
        case 'B': //-...
            dah(); dit(); dit(); dit();         delay(letterGap);
            break;
        case 'C': //-.-.
            dah(); dit(); dah(); dit();         delay(letterGap);
            break;
        case 'D': //-..
            dah(); dit(); dit();                delay(letterGap);
            break;
        case 'E': //. 
            dit();                              delay(letterGap);
            break;
        case 'F': //..-.
            dit(); dit(); dah(); dit();         delay(letterGap);
            break;
        case 'G': //--.
            dah(); dah(); dit();                delay(letterGap);
            break;
        case 'H': //....
            dit(); dit(); dit(); dit();         delay(letterGap);
            break;
        case 'I': //..
            dit(); dit();                       delay(letterGap);
            break;
        case 'J': //.---
            dit(); dah(); dah(); dah();         delay(letterGap);
            break;
        case 'K': //-.-
            dah(); dit(); dah();                delay(letterGap);
            break;
        case 'L': //.-..
            dit(); dah(); dit(); dit();         delay(letterGap);
            break;
        case 'M': //--;
            dah(); dah();                       delay(letterGap);
            break;
        case 'N': //-.;
            dah(); dit();                       delay(letterGap);
            break;
        case 'O': //---
            dah(); dah(); dah();                delay(letterGap);
            break;
        case 'P': //.--.
            dit(); dah(); dah(); dit();         delay(letterGap);
            break;
        case 'Q': //--.-
            dah(); dah(); dit(); dah();         delay(letterGap);
            break;
        case 'R': //.-.
            dit(); dah(); dit();                delay(letterGap);
            break;
        case 'S': //...
            dit(); dit(); dit();                delay(letterGap);
            break;
        case 'T': //-; 
            dah();                              delay(letterGap);
            break;
        case 'U': //..-
            dit(); dit(); dah();                delay(letterGap);
            break;
        case 'V': //...-
            dit(); dit(); dit(); dah();         delay(letterGap);
            break;
        case 'W': //.--
            dit(); dah(); dah();                delay(letterGap);
            break;
        case 'X': //-..-
            dah(); dit(); dit(); dah();         delay(letterGap);
            break;
        case 'Y': //-.--
            dah(); dit(); dah(); dah();         delay(letterGap);
            break;
        case 'Z': //--..
            dah(); dah(); dit(); dit();         delay(letterGap);
            break;
        case '1': // .----
            dit(); dah(); dah(); dah(); dah();  delay(letterGap);
            break;
        case '2': // ..---
            dit(); dit(); dah(); dah(); dah();  delay(letterGap);
            break;
        case '3': // ...--
            dit(); dit(); dit(); dah(); dah();  delay(letterGap);
            break;
        case '4': // ....-
            dit(); dit(); dit(); dit(); dah();  delay(letterGap);
            break;
        case '5': // .....
            dit(); dit(); dit(); dit(); dit();  delay(letterGap);
            break;
        case '6': // -....
            dah(); dit(); dit(); dit(); dit();  delay(letterGap);
            break;
        case '7': // --...
            dah(); dah(); dit(); dit(); dit();  delay(letterGap);
            break;
        case '8': // ---..
            dah(); dah(); dah(); dit(); dit();  delay(letterGap);
            break;
        case '9': // ----.
            dah(); dah(); dah(); dah(); dit();  delay(letterGap);
            break;
        case '0': // -----
            dah(); dah(); dah(); dah(); dah();  delay(letterGap);
            break;
        case ' ': // gap
            delay(wordSpace);
            break;
        default:
            // don't blink
            break;
        }
    } // end for (done w/ morseMessage)

    delay(wordSpace); delay(wordSpace);

    //exit(0);
}