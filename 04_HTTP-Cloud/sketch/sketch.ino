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


const char http_header[] = "POST /devices/test-dev-id/messages/events?api-version=2020-03-13 HTTP/1.1\r\n"
      "Host: test-bosch-sfp-kos.azure-devices.net\r\n"
      "Content-Type: application/json\r\n"
      "Authorization: SharedAccessSignature sr=test-bosch-sfp-kos.azure-devices.net%2Fdevices%2Ftest-dev-id&sig=bc7aFCapDtR8ZSQxKjlH3vtXR23Eat3WK%2BLXoIRBuL0%3D&se=1633634949\r\n"
      "Content-Length: 18\r\n\r\n";
      
void upload(long sensorwert) {
  char buffer[32];
  sprintf(buffer, "{\"value\":% 8ld}", sensorwert); // 18 Zeichen

  softSerial.print("AT+CIPSTART=\"SSL\",\"test-bosch-sfp-kos.azure-devices.net\",443\r\n");
  receive(2000);
  softSerial.print("AT+CIPSEND=");
  softSerial.print(strlen(http_header)+18);
  softSerial.print("\r\n");
  receive(2000);
  softSerial.print(http_header);
  softSerial.print(buffer);
  receive(10000);
  softSerial.print("AT+CIPCLOSE\r\n");
  receive(1000);  
}
void loop() {
  upload(0);
  delay(20000);  
}
