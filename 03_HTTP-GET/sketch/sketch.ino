/************************************************************
 * Setup of the SoftwareSerial interface
 ***********************************************************/
#define SOFTSERIAL_RX_PIN 2
#define SOFTSERIAL_TX_PIN 3
#include "SoftwareSerial.h"
SoftwareSerial softSerial(SOFTSERIAL_RX_PIN, SOFTSERIAL_TX_PIN);


/*
 * Receive data. Each data byte is also copied to the serial 
 * console (to the PC).
 */
void receive(long duration) {
  long end = millis() + duration;
  while(millis() < end) {
    if(softSerial.available()) {
      char val = softSerial.read();
      Serial.write(val);
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("startup...");

  // configure the soft-serial
  softSerial.begin(9600);

  softSerial.print("AT+RST\r\n");
  receive(2000);
  softSerial.print("AT+CWJAP_DEF=\"Thomass iPhone\",\"passwort\"\r\n");
  receive(10000);
  softSerial.print("AT+CIPSSLSIZE=4096\r\n");
  receive(2000);
}

const char http_header[] = "GET / HTTP/1.1\r\n" "Host: neverssl.com\r\n\r\n";

void loop() {
  // put your main code here, to run repeatedly:

  softSerial.print("AT+CIPSTART=\"TCP\",\"neverssl.com\",80\r\n");
  receive(2000);
  softSerial.print("AT+CIPSEND=");
  softSerial.print(strlen(http_header));
  softSerial.print("\r\n");
  receive(2000);
  softSerial.print(http_header);
  receive(10000);
  softSerial.print("AT+CIPCLOSE\r\n");
  receive(20000);
  
}
