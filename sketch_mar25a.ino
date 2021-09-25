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
#define HTTP_HEADER_6 "Authorization: SharedAccessSignature sr=test-bosch-sfp-kos.azure-devices.net%2Fdevices%2Ftest-dev-id&sig=XsqoF%2FnRw2LNCSldqOdZtAENokoMv4bWmrJdQRoYfD0%3D&se=1621203903\r\n"

const char http_header[] PROGMEM = HTTP_HEADER_1 HTTP_HEADER_2 HTTP_HEADER_3 HTTP_HEADER_4 HTTP_HEADER_5 HTTP_HEADER_6; 
#define HTTP_HEADER_LEN (strlen_P(http_header))

#define CIPSTART "AT+CIPSTART=\"SSL\",\"test-bosch-sfp-kos.azure-devices.net\",443\r\n"
//#define CIPSTART "AT+CIPSTART=\"TCP\",\"192.168.3.192\",8080\r\n"



// how often to send data to the cloud?
#define TRANSMIT_INTERVAL_MILLIS 10000


// when we did not get a "good" status for this time, we blink "error"
#define STATUS_WAIT_MILLIS 20000
#define STATUS_LED_PIN 13


// define the software serial interface
#define SOFTSERIAL_RX_PIN 2
#define SOFTSERIAL_TX_PIN 3
//#define _SS_MAX_RX_BUFF 256 // RX buffer size - default 64
#include "SoftwareSerial.h"
SoftwareSerial softSerial(SOFTSERIAL_RX_PIN, SOFTSERIAL_TX_PIN); // RX, TX

// the receive buffer (to accumulate received data)
#define REC_BUFLEN 256
char rec_buf[REC_BUFLEN+1] = {0}; // make the buffer 1 char larger, initialize with 0, so that strstr() comparison never fails
int rec_buf_idx = 0;

// last timestamp we detected an HTTP 2xx status code
long last_http_ok_time = 0;

/* 
 *  called whenever a \n or a \r has been received, or when the buffer needs to be flushed
 *  when this is called, the buffer does not contain the respective \n or \r
 *  returns true when we can stop waiting for more received data
 *  
 *  parameter untilOK --> set done = true if the line starts with "OK"
 *  parameter untilErr --> set done = true if the line starts with "ERROR"
*/
bool process_rec_buf(bool untilOK, bool untilErr) {
  bool done = false;
  if(rec_buf_idx == 0) {
    // skip all checks on empty buf
  } else if(untilOK && strncmp(rec_buf, "OK", 2) == 0) {
    //Serial.println("received \"OK\" --> done");
    done = true;
  } else if(untilErr && strncmp(rec_buf, "ERROR", 5) == 0) {
    //Serial.println("received \"ERROR\" --> done");
    done = true;
  } else if (strncmp(rec_buf, "+IPD", 4) == 0) {
    // this is the first line of the HTTP response, where the ESP8266 sends +IPD...
    // find the HTTP return code
    char* ptr = strstr(rec_buf, "HTTP/1.1");
    int statuscode = atoi(ptr+9);
    if(statuscode >= 200 && statuscode <300) {
      // this is the "OK" range
      last_http_ok_time = millis();
    }
  } 
  rec_buf_idx=0;
  return done;  
}

void receive_serial_param(long duration, bool untilOK, bool untilErr) {
  long end = millis() + duration;
  char val;
  bool done = false;
  do {
    while (softSerial.available()) {
      val = softSerial.read();
      // debug-copy to serial
      Serial.write(val);
      // check the received data
      if(val == '\r' || val == '\n') {
        // process buffer when the line ends
        done = process_rec_buf(untilOK, untilErr);
      } else if(rec_buf_idx < REC_BUFLEN - 1) {
        // append to buffer (if space left)
        rec_buf[rec_buf_idx++] = val;
      }
    }
  } while(millis() < end && done == false);
  
  // now flush the buffer
  process_rec_buf(false, false);
}

void receive_serial(long duration) {
  receive_serial_param(duration, true, true);
}
void receive_serial_uncond(long duration) {
  receive_serial_param(duration, false, false);
}



void transmit_serial(const char * data) {
#ifdef COPY_COMMANDS
  while(Serial.availableForWrite() < sizeof(data)) {
    delay(1);
  }
  Serial.print(data);
#endif
  softSerial.print(data);
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
    written = softSerial.write(buf, batch);
    delay(1); // just to be sure...
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

  // stop existing TCP connection (just try it - in case there is one...)
  transmit_serial("AT+CIPCLOSE\r\n");
  receive_serial(2000);
  
  // start TCP / SSL connection
  transmit_serial(CIPSTART);
  receive_serial(5000); // wait 5s to establish connection

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
  receive_serial(5000);

  // close the connection (in case of errors, it is closed by the server; in case all is OK, it is not)
  transmit_serial("AT+CIPCLOSE\r\n");
  receive_serial(100);
}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("startup...");

  // set up LED pin
  pinMode(STATUS_LED_PIN, OUTPUT);

  // configure ESP8266 to use 9600 baud (higher rates not compatible with SoftwareSerial?)
  //softSerial.begin(115200);
  //softSerial.print("AT+CIOBAUD=9600\r\n");
  pinMode(SOFTSERIAL_RX_PIN, INPUT);
  pinMode(SOFTSERIAL_TX_PIN, OUTPUT);
  softSerial.begin(9600);

  receive_serial(2000);
  transmit_serial("AT\r\n"); // should return "OK"
  receive_serial(2000);
  transmit_serial("AT+RST\r\n"); // restart the module
  receive_serial_uncond(10000); // wait unconditionally, otherwise it just get's errors for the following commands
  transmit_serial("AT+GMR\r\n"); // returns version information
  receive_serial(10000);
  transmit_serial("AT+CWMODE?\r\n"); // ensure that mode is "station"
  receive_serial(100);
  transmit_serial("AT+CIPSSLSIZE=4096\r\n"); // set SSL buffer to 4kiB (otherwise we will get errors later)
  receive_serial(100);
  transmit_serial("AT+CIPRECVMODE=1\r\n"); // set receive mode to active (module does not buffer)
  receive_serial(100);
  //transmit_serial("ATE0\r\n"); // disable echo
  //receive_serial(100);
}


long last_transmit_time = 0;

void transmit_sensor_data() {
  long now = millis();
  
  if(now - last_transmit_time < TRANSMIT_INTERVAL_MILLIS) 
    return; // skip the transmit and wait another round

  // as we transmit now, update the timestamp
  last_transmit_time = now;
  
  // prepare the data to be transmitted as JSON
  char buffer[20];
  memset(buffer,0,sizeof(buffer));
  snprintf(buffer,19,"{\"time\":%ld}", millis());

  // now send it to the Cloud
  ssl_transmit(buffer);

}

void update_status_led() {
  long now = millis();
  bool state_ok = (now - last_http_ok_time < STATUS_WAIT_MILLIS);
  if(state_ok) {
    digitalWrite(STATUS_LED_PIN, now % 2000 < 1000 ? LOW : HIGH);
    //Serial.println("status: OK");
  } else {
    // blink with 500ms period
    digitalWrite(STATUS_LED_PIN, now % 500 < 250 ? LOW : HIGH);
    //Serial.println("status: err");
  }
}

void loop() {
 
  transmit_sensor_data();

  update_status_led();

  // some delays
  receive_serial_uncond(10);
}
