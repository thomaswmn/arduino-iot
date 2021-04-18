#include <avr/pgmspace.h>

/**
 * AT commands
 * https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf
 * 
 * AT+UART_DEF=9600,8,1,0,0                                    --> configure UART to 9600 baud, 1 stop bit, no flow control (saved in flash)
 * AT+CWMODE_DEF=1                                             --> set WLAN mode to "station" (saved in flash)
 * AT+CWJAP_DEF="gastnetz@familie-werthmann.de","834534587324" --> set WLAN SSID and password (saved in flash)
 * AT+CIPSSLSIZE=4096                                          --> set SSL buffer to 4kB
 * AT+CIPSTART="SSL","test-bosch-sfp-kos.azure-devices.net",443
 * 
 * AT+CIPSTATUS              --> status of WLAN and TCP connection
 * AT+CIPSTART               --> start TCP / UDP / SSL connection
 * AT+CIPCLOSE               --> close of connection
 * AT+PING                   --> ping
 * AT+CIPRECVDATA            --> get received data
 * AT+CIPSSLSIZE=<size>      --> set SSL buffer size - might be required in case of problems
 * <NTP setup required for SSL?>
 */


#define HTTP_HEADER_1 "POST /devices/test-dev-id/messages/events?api-version=2020-03-13 HTTP/1.1\r\n"
#define HTTP_HEADER_2 "Host: test-bosch-sfp-kos.azure-devices.net\r\n"
#define HTTP_HEADER_3 "User-Agent: Arduino\r\n"
#define HTTP_HEADER_4 "Accept: */*\r\n"
#define HTTP_HEADER_5 "Content-Type: application/json\r\n"
#define HTTP_HEADER_6 "Authorization: SharedAccessSignature sr=test-bosch-sfp-kos.azure-devices.net%2Fdevices%2Ftest-dev-id&sig=ZGo3kDxXM%2Bdrj49ZiIlbXOJNVxWQpDF0IdWEhvrZW3M%3D&se=1618591213\r\n"

const char http_header[] PROGMEM = HTTP_HEADER_1 HTTP_HEADER_2 HTTP_HEADER_3 HTTP_HEADER_4 HTTP_HEADER_5 HTTP_HEADER_6; 
#define HTTP_HEADER_LEN (strlen_P(http_header))

//#define CIPSTART "AT+CIPSTART=\"SSL\",\"test-bosch-sfp-kos.azure-devices.net\",443\r\n"
#define CIPSTART "AT+CIPSTART=\"TCP\",\"192.168.3.192\",8080\r\n"


#define USE_SOFT_SERIAL

#ifdef USE_SOFT_SERIAL
#define SOFTSERIAL_RX_PIN 2
#define SOFTSERIAL_TX_PIN 3
//#define _SS_MAX_RX_BUFF 256 // RX buffer size - default 64
#include "SoftwareSerial.h"
SoftwareSerial softSerial(SOFTSERIAL_RX_PIN, SOFTSERIAL_TX_PIN); // RX, TX
#endif



void receive_serial(long duration) {
  long end = millis() + duration;
  do {
#ifdef USE_SOFT_SERIAL
    while (softSerial.available()) {
      char val = softSerial.read();
      Serial.write(val);
    }
#else
    while (Serial.available()) {
      char val;
      val = Serial.read();
    }
#endif
  } while(millis() < end);
}



void transmit_serial(const char * data) {
#ifdef COPY_COMMANDS
  while(Serial.availableForWrite() < sizeof(data)) {
    delay(1);
  }
  Serial.print(data);
#endif
#ifdef USE_SOFT_SERIAL
  softSerial.print(data);
#endif
  receive_serial(1);
}


/*
 * transmit the HTTP header. The header is static text stored in flash (not RAM). 
 * Therefore, it has to be copied to RAM first, before it can be transmitted. 
 * To do so, we use this loop with a small buffer. This avoids overflow of the 
 * rather small memory of the Arduino.
 */
#define HEADER_BUFSIZE 16
void transmit_header() {
  const char * ptr = http_header;
  char buf[HEADER_BUFSIZE];
  size_t todo = strlen_P(http_header);
  while(todo > 0) {
    size_t batch = min(HEADER_BUFSIZE, todo);
    for(char i = 0; i < batch; i++) {
      buf[i] = pgm_read_byte(ptr+i);
    }
    size_t written;
#ifdef COPY_COMMANDS
    while(Serial.availableForWrite() < batch) {
      delay(1);
    }
    written = Serial.write(buf, batch);
#endif
#ifdef USE_SOFT_SERIAL
    written = softSerial.write(buf, batch);
    delay(1); // just to be sure...
#endif
    ptr += written;
    todo -= written;
  }
}



void ssl_transmit(char * data) {
  int contentLength = strlen(data); 
  int totalLength = contentLength + HTTP_HEADER_LEN + 18 /* "Content-Length: " + "\r\n" */;
  // account for integer in header with varialbe number of digits
  if(contentLength < 10)
    totalLength+=1;
  else if(contentLength < 100)
    totalLength+=2;
  else if(contentLength < 1000)
    totalLength+=3;
  else
    return; // not supported
  // 2x \r\n - newline before data and one after data
  totalLength+=4; 

  // start TCP / SSL connection
  transmit_serial("AT+CIPCLOSE\r\n");
  receive_serial(2000);
  transmit_serial(CIPSTART);
  receive_serial(2000);

  // start transmitting data
  char buffer[32];
  memset(buffer,0,sizeof(buffer));
  snprintf(buffer,31,"AT+CIPSEND=%d\r\n", totalLength);
  transmit_serial(buffer);
  receive_serial(2000);

  // send the header (static part)
  transmit_header();

  // send the last line of the header
  memset(buffer,0,sizeof(buffer));
  snprintf(buffer, 31, "Content-Length: %d\r\n", contentLength);
  transmit_serial(buffer);

  // send the empty line between header and content
  transmit_serial("\r\n");

  // send the content
  transmit_serial(data);
  transmit_serial("\r\n");
  receive_serial(1000);

  // close the connection
  transmit_serial("AT+CIPCLOSE\r\n");
  receive_serial(10);
}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("startup...");

#ifdef USE_SOFT_SERIAL
  // configure ESP8266 to use 9600 baud (higher rates not compatible with SoftwareSerial)
  //softSerial.begin(115200);
  //softSerial.print("AT+CIOBAUD=9600\r\n");
  pinMode(SOFTSERIAL_RX_PIN, INPUT);
  pinMode(SOFTSERIAL_TX_PIN, OUTPUT);
  softSerial.begin(9600);
#endif

  receive_serial(2000);
  transmit_serial("AT\r\n"); // should return "OK"
  receive_serial(2000);
  transmit_serial("AT+GMR\r\n"); // returns version information
  receive_serial(10000);
  transmit_serial("AT+CWMODE?\r\n"); // ensure that mode is "station"
  receive_serial(100);
  transmit_serial("AT+CIPSSLSIZE=4096\r\n"); // set SSL buffer to 4kiB (otherwise we will get errors later)
  receive_serial(100);
  //transmit_serial("ATE0\r\n"); // disable echo
  //receive_serial(100);
}



void loop() {
  
#ifdef SERIAL_TO_SOFTSERIAL
  // if enabled, copy data the Arduino receives to the ESP8266 module (for testing)
  while (Serial.available()) {
    char val = Serial.read();
    softSerial.write(val);
  }
#endif

  // prepare the data to be transmitted as JSON
  char buffer[20];
  memset(buffer,0,sizeof(buffer));
  snprintf(buffer,19,"{\"time\":%ld}", millis());

  // now send it to the Cloud
  ssl_transmit(buffer);

  // some delays
  receive_serial(10000);
  transmit_serial("AT+CIPSTATUS\r\n");
  receive_serial(1000);
}
