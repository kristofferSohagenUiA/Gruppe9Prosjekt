//
//Denne koden er brukt i samarbeid med gruppe 2 og skrevet av gruppe 2: Lars Herman Jonsaas Olafsen, Simon Storaker og Daniel Nicolai Grønn Moen
//

/* Teensy CAN-Bus with OLED 128x64 */

// -------------------------------------------------------------
// CANtest for Teensy 3.6 dual CAN bus
// by Collin Kidder, Based on CANTest by Pawelsky (based on CANtest by teachop)
//
// Both buses are left at default 250k speed and the second bus sends frames to the first
// to do this properly you should have the two buses linked together. This sketch
// also assumes that you need to set enable pins active. Comment out if not using
// enable pins or set them to your correct pins.
//
// This sketch tests both buses as well as interrupt driven Rx and Tx. There are only
// two Tx buffers by default so sending 5 at a time forces the interrupt driven system
// to buffer the final three and send them via interrupts. All the while all Rx frames
// are internally saved to a software buffer by the interrupt handler.
//

#include <FlexCAN.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>




#ifndef __MK66FX1M0__
  #error "Teensy 3.6 with dual CAN bus is required to run this example"
#endif

static CAN_message_t msg;
CAN_message_t inMsg;

uint8_t data[4];
float accel_g;
float accel_SI;
uint8_t bytes[sizeof(accel_SI)];
CAN_message_t outMsg;


void setup() 
{
Can0.begin(250000);  

  //if using enable pins on a transceiver they need to be set on
  // Sjekk at du bruker riktig pin
  pinMode(2, OUTPUT); //can0

  digitalWrite(2, HIGH);

  msg.ext = 0;
  msg.id = 0x22; 
  msg.len = 8;
  // Meldingene som skal sendes
  msg.buf[0] = 0;
  msg.buf[1] = 0;
  msg.buf[2] = 0;
  msg.buf[3] = 0;
  msg.buf[4] = 0;
  msg.buf[5] = 0;
  msg.buf[6] = 0;
  msg.buf[7] = 0;
}


void loop() 
{
while (Can0.available()) 
  {
    Can0.read(inMsg);
    if (inMsg.id == 0x21) // Mottar CAN-melding fra adresse 0x21
    {
      //Mottar IMU-melding og lagrer den i data-bufferene
      data[0] = inMsg.buf[0];
      data[1] = inMsg.buf[1];
      data[2] = inMsg.buf[2];
      data[3] = inMsg.buf[3];

      // memcpy brukes her til å konvertere buffer-dataen fra HEX til flyt-tall
      // g-verdien lagres i accel_g variablen
      memcpy(&accel_g, data, 4);
      Serial.println(accel_g); // Sjekk om melding er mottatt

  accel_SI = accel_g * (-9.81); // Konvertering av g til m/s^2
  // Konverterer fra flyt-tall tilbake til HEX for overføring
  memcpy(bytes, &accel_SI, sizeof(accel_SI));
  outMsg.len = 4;
  outMsg.id = 0x22; // Sender melding på adresse 0x22
  outMsg.buf[0] = bytes[0];
  outMsg.buf[1] = bytes[1];
  outMsg.buf[2] = bytes[2];
  outMsg.buf[3] = bytes[3];
  //CAN-melding sendes
  Can0.write(outMsg);
    }
  }  
}
