/*
 * Project Serial_demo
 * Description:
 * Author:
 * Date:
 */

//#include "Particle.h"
//#include "Serial2/Serial2.h"  // Header file necessary for Serial 2 (pins D4 & D5)
#include "FPS_GT511C3_Xenon.h"

FPS_GT511C3 fps;  // fingerprint scanner module - uses Serial1 (Rx:pin14, Tx:pin15)

SYSTEM_MODE(MANUAL);

void setup() {

  Serial.begin(); // defaults to 9600 baud rate
  Serial1.begin(9600);  // Rx (pin 14) & Tx (pin 15) on Particle Xenon
  //Serial2.begin(9600);  // D5 (Rx / pin 21) & D4 (Tx / pin 20) on Particle Xenon

  // Set up fingerprint scanner
  fps.UseSerialDebug = true; // so you can see the messages in the serial debug screen
	fps.Open(); //send serial command to initialize fps

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
  
  // FPS Blink LED Test
	fps.SetLED(true); // turn on the LED inside the fps
	delay(500);
	fps.SetLED(false);// turn off the LED inside the fps
  delay(500);

}