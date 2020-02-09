/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "a:/Documents/Programming/Particle/Serial_demo/src/Serial_demo.ino"
/*
 * Project Serial_demo
 * Description:
 * Author:
 * Date:
 */

#include "Particle.h"
#include "Serial2/Serial2.h"
#include <string.h>

void setup();
void loop();
#line 12 "a:/Documents/Programming/Particle/Serial_demo/src/Serial_demo.ino"
SYSTEM_MODE(MANUAL);

String input = "n/a";

void setup() {

  Serial.begin(); // defaults to 9600 baud rate
  Serial1.begin(9600);  // Rx (pin 14) & Tx (pin 15) on Particle Xenon
  Serial2.begin(9600);  // D5 (Rx / pin 21) & D4 (Tx / pin 20) on Particle Xenon

  Serial.println("Press RETURN to enter a message.");

}

void loop() {

  //Serial.println("Hello, World!");
  //Serial1.println("Hello, Serial 1");
  //Serial2.println("Hello, Serial 2");
  //delay(1000);

  //Serial.write("write");  // write something w/o an endline

  /* char in = Serial.read();  // read in a single byte (int or char)
  Serial.print("I recieved: ");
  Serial.println(in);
  delay(1000); */
  
  if (Serial.read() == '\r') {
    char inChar = ' ';
    String message = "";
    Serial.println("Enter a message.");

    do {
      inChar = Serial.read();
      message.concat(String(inChar));
    } while (inChar != '\r');

    Serial.write("Your message is: ");
    Serial.println(message);

  }
  delay(100);

}