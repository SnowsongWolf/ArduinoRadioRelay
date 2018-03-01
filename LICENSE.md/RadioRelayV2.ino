// ---- Radio Relay version 2 by Zack "Snow" Podany
// ----
// ---- This program assumes you have an nRF24L01 board connected via SPI to an Arduino or compatable board
// ---- using standard SPI pinouts.  Typically this is:
// ---- CE to pin 7
// ---- CSN to pin 8
// ---- SCK to pin 13
// ---- MOSI to pin 11
// ---- MISO to pin 12
// ---- IRQ ignored
// ----
// ---- This pinout is known to work for Arduino Uno and Nano, as well as Teensy 3.x
// ----
// ---- The purpose of this program is to provide for easy 2 way radio communication in prototyping wireless
// ---- circuits.  Text entered into the serial terminal will be transmitted through the radio, while text
// ---- received in the radio will be outputted to the serial terminal.

#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Wire.h>

// -- Begin address assignment.  On one of the 2 boards, the addresses should be flipped

#define ADDR1 "arf02"
#define ADDR2 "arf01"

// -- End address assignment.

RF24 radio(7, 8); // CE, CSN not the other way around Snow!
const byte address[][6] = {ADDR1, ADDR2};

void setup() {
  Serial.begin(115200);                           // Talkback
  delay(250);                                     // To ensure radio initializes
  Serial.println("Radio initializing");

  radio.begin();
  radio.setChannel(108);                          // Generally outside WiFi interference range
  radio.openWritingPipe(address[0]);      
  radio.openReadingPipe(1, address[1]);
  radio.setPALevel(RF24_PA_LOW);                  // PA Low works over a distance of several feet, only increase if range is an issue
  radio.startListening();

  if (radio.isChipConnected()) Serial.println("Radio connected"); else Serial.println("Radio is not connected");
}

void loop() {
  radioUpdate();
  serialUpdate();
}

// ------------------------------------------------------
// custom functions
// ------------------------------------------------------

void serialUpdate() {
  char buf[32] = "";                            // Input buffer object, max length of 32 characters per line sent.
  uint16_t i = 0;
  if (Serial.available()) {                     // If there is serial data waiting to be read
    Serial.print("Sending ");
    radio.stopListening();                      // MUST stop listening before sending or system will hang.
    
    while (Serial.available()) {                // While there is serial data waiting to be read

      buf[i] = Serial.read();                   // Read a single character into the command buffer.
      i++;
      
    }
    
    radio.write(&buf, i);                       // Write the command buffer to the radio.
    Serial.print('\n');                         // Write a line break since these are rarely transmitted
    Serial.println(buf);                        // Print back the command for the record
    Serial.print('\n');
    radio.startListening();                     // Resume listening for radio traffic
  }
}

void radioUpdate() {
  if (radio.available()) {                      // If there is radio data waiting to be read
    Serial.println("Message for you cap'n!");
    char gotmsg[32] = "";                       // Initialize the message variable
    uint16_t len = radio.getDynamicPayloadSize(); // Get the length of the radio message
    radio.read( &gotmsg, len );                 // Read the radio message into the message variable
    Serial.write(gotmsg);                       // Write the message out to the console
    Serial.write('\n');                         // Write a line break since these are rarely transmitted
  }
}
