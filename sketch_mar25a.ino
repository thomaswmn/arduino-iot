/************************************************************
 * HTTP Header - insert device ID and Shared Access Signature here
 ***********************************************************/

#define HTTP_HEADER_1 "POST /devices/test-dev-id/messages/events?api-version=2020-03-13 HTTP/1.1\r\n"
#define HTTP_HEADER_2 "Host: test-bosch-sfp-kos.azure-devices.net\r\n"
#define HTTP_HEADER_3 "Content-Type: application/json\r\n"
#define HTTP_HEADER_4 "Authorization: SharedAccessSignature sr=test-bosch-sfp-kos.azure-devices.net%2Fdevices%2Ftest-dev-id&sig=bc7aFCapDtR8ZSQxKjlH3vtXR23Eat3WK%2BLXoIRBuL0%3D&se=1633634949\r\n"

const char http_header[] = HTTP_HEADER_1 HTTP_HEADER_2 HTTP_HEADER_3 HTTP_HEADER_4;
#define HTTP_HEADER_LEN (strlen(http_header))

/************************************************************
 * Command to start TCP+SSL - contains the host name
 ***********************************************************/

#define CIPSTART "AT+CIPSTART=\"SSL\",\"test-bosch-sfp-kos.azure-devices.net\",443\r\n"


/************************************************************
 * Setup of the SoftwareSerial interface
 ***********************************************************/
#define SOFTSERIAL_RX_PIN 2
#define SOFTSERIAL_TX_PIN 3
#include "SoftwareSerial.h"
SoftwareSerial softSerial(SOFTSERIAL_RX_PIN, SOFTSERIAL_TX_PIN);


/************************************************************
 * Buffer for received HTTP data
 ***********************************************************/
#define REC_BUFLEN 128
// make the buffer 1 char larger, initialize with 0,
// so that strstr() comparison never fails
char rec_buf[REC_BUFLEN+1] = {0}; 
// index of the next char to be written into the buffer
int rec_buf_idx = 0;

/*
 * Buffer layout
 * 
 *  |<---------------REC_BUFLEN-------------------->|
 *  
 * |t h e _ r e c e i v e d _ d a t a 0 0 0 0 0 ... 0 0 |
 *  ^                                 ^               ^
 *  |                                 |               |
 *  rec_buf               rec_buf+rec_buf_idx      extra "0" never overwritten
 *  
 */


/************************************************************
 * Upload interval
 ***********************************************************/

// how often to send data to the cloud?
#define TRANSMIT_INTERVAL_MILLIS 10000


/************************************************************
 * Status LED
 ***********************************************************/

// when we did not get a "good" status for this time, we blink "error"
#define STATUS_WAIT_MILLIS (TRANSMIT_INTERVAL_MILLIS*4)
#define STATUS_LED_PIN 13

// last timestamp we detected an HTTP 2xx status code
// (used to blink "OK" or "error")
long last_http_ok_time = 0;






/* 
 *  called whenever a \n or a \r has been received, or when the buffer needs to be flushed
 *  when this is called, the buffer does not contain the respective \n or \r
 *  returns true when we can stop waiting for more received data
 *  
 *  parameter untilOK --> set done = true if the line starts with "OK"
 *  parameter untilErr --> set done = true if the line starts with "ERROR"
*/
bool process_rec_buf(bool untilOKErr) {
  bool done = false;
  if(rec_buf_idx == 0) {
    // skip all checks on empty buf
  } else if(untilOKErr && strncmp(rec_buf, "OK", 2) == 0) {
    // rec_buf starts with "OK"
    done = true;
  } else if(untilOKErr && strncmp(rec_buf, "ERROR", 5) == 0) {
    // rec_buf starts with "ERROR"
    done = true;
  } else if (strncmp(rec_buf, "+IPD", 4) == 0) {
    // extension required to detect return code from HTTP

    // this is the first line of the HTTP response, where the ESP8266 sends +IPD...
    // find the HTTP return code
    // the expected line looks like
    //  "HTTP/1.1 204 No Content"
    // which means "OK" (a number in the 200 range), and that the server does not send any data back
    char* ptr = strstr(rec_buf, "HTTP/1.1");
    // now, ptr points to the start of the "HTTP/1.1" part inside the buffer
    // skip 9 characters, then the number should follow
    int statuscode = atoi(ptr+9);
    if(statuscode >= 200 && statuscode <300) {
      // this is the "OK" range
      last_http_ok_time = millis();
    }
  } 
  rec_buf_idx=0;
  return done;  
}

/*
 * Receive data until timeout or "OK" / "ERROR" is received.
 * Each data byte is also copied to the serial console (to the PC).
 * 
 * Detecting OK and ERROR requires the buffer to be used. 
 * Just waiting for the timeout is easier to implement.
 */
void receive_serial_param(long duration, bool untilOKErr) {
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
        done = process_rec_buf(untilOKErr);
      } else if(rec_buf_idx < REC_BUFLEN - 1) {
        // append to buffer (if space left)
        rec_buf[rec_buf_idx++] = val;
      }
    }
  } while(millis() < end && done == false);
  
  // now flush the buffer
  process_rec_buf(false);
}

/* receive data, until timeout or either "OK" or "ERROR" is received */
void receive_serial(long duration) {
  receive_serial_param(duration, true);
}

/* receive data until timeout. Ignore "OK" or "ERROR" */
void receive_serial_uncond(long duration) {
  receive_serial_param(duration, false);
}

/* transmit the given string and process received data for 1ms */
void transmit_serial(const char * data) {
  softSerial.print(data);
  receive_serial(1);
}


/*
 * transmit the HTTP header. The header is static text stored in flash (not RAM). 
 * Therefore, a special method gpm_read_byte() has to be used to access the array.
  */
void transmit_header() {
  size_t idx = 0;
  size_t todo = HTTP_HEADER_LEN;
  while(todo > 0) {
    softSerial.write(http_header[idx]);
    idx += 1;
    todo -= 1;
  }
}


/*
 * Send the given data string via HTTPS. Initially, the total length is calculated,
 * because the ESP8266 requires this information when starting to transmit. Then, 
 * the header and the given data string are transmitted. Finally, the connection
 * is closed.
 */
void ssl_transmit(char * data) {
  // length of the data string
  int contentLength = strlen(data); 
  // length of the whole transmission, including HTTP header
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

  // send the last line of the header (Content-Length: ...)
  memset(buffer,0,sizeof(buffer));
  snprintf(buffer, 31, "Content-Length: %d\r\n", contentLength);
  transmit_serial(buffer);

  // send the empty line between header and content
  transmit_serial("\r\n");

  // send the content
  transmit_serial(data);
  transmit_serial("\r\n");
  receive_serial(2000); // wait for this time before closing the connection
  // note that this does not understand any HTTP response, so waits unconditionally

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

  // configure ESP8266 to use 9600 baud (higher rates not compatible with SoftwareSerial)
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
}


long last_transmit_time = 0;
/*
 * transmit data in case the respective interval has passed. Otherwise, do nothing.
 */
void transmit_sensor_data() {
  long now = millis();
  
  if(now - last_transmit_time < TRANSMIT_INTERVAL_MILLIS) 
    return; // skip the transmit and wait another round

  // as we transmit now, update the timestamp
  last_transmit_time = now;

  // some dummy sensor value as double
  double value = (1.0/100.0)*(now % 8234);
  long value_int = (long) value;
  long value_frac = (long)((value - value_int)*1000);
  
  
  // prepare the data to be transmitted as JSON
  char buffer[64];
  memset(buffer,0,sizeof(buffer));
  snprintf(buffer,sizeof(buffer)-1,"{\"time\":%ld,\"value\":%ld.%ld}", now, value_int, value_frac);

  Serial.print("will send the following JSON object: ");
  Serial.println(buffer);

  // now send it to the Cloud
  ssl_transmit(buffer);

}

/*
 * blink the LED, depending on the current state (optional extension)
 */
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
}
