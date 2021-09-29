# How to Flash the Firmware


## Connect to USB-TTL Adapter

* `VCC` to `VCC`, should be 3,3V
* `GND` to `GND`
* `TX` to `RX`
* `RX` to `TX`
* pull `CH_PD` up (connet to `VCC`)
* to enable flash mode: pull `GPIO_0` down to `GND`
* to disable flash mode: leave `GPIO_0` open


## Check Flash Size

To enable "flash mode", connect `GPIO_0` to `GND` and reset the device.

```
esptool.py --port <serial-port> flash_id
```

Should print something like
```
esptool.py v3.0
Serial port /dev/ttyUSB0
Connecting....
Detecting chip type... ESP8266
Chip is ESP8266EX
Features: WiFi
Crystal is 26MHz
MAC: e8:db:84:da:2c:3d
Uploading stub...
Running stub...
Stub running...
Manufacturer: d8
Device: 4014
Detected flash size: 1MB
Hard resetting via RTS pin...
```


## Flash the firmware

This is a combination of two firmware versions. Unclear why this works, but I assume: the first firmware includes a partition which is not provided by the second one, but is required.

```
esptool.py write_flash 0x00000 ai-thinker-v1.1.1.bin
```

(reset the module)

```
esptool.py write_flash \
 0x01000 ESP8266_NonOS_AT_Bin_V1.7.4/ESP8266_NonOS_AT_Bin_V1.7.4/bin/at/512+512/user1.1024.new.2.bin \
 0xfc000 ESP8266_NonOS_AT_Bin_V1.7.4/ESP8266_NonOS_AT_Bin_V1.7.4/bin/esp_init_data_default_v08.bin \
 0xfe000 ESP8266_NonOS_AT_Bin_V1.7.4/ESP8266_NonOS_AT_Bin_V1.7.4/bin/blank.bin 
```

After flashing, check the firmware version as shown below.

## Connect to Firmware

To start the device in normal mode, do not connect `GPIO_0`. In case it was pulled down before, reset the device.

Device expects CR+LF for a line break. To emulate this combination when pressing enter, use picocom like this:

```
picocom --omap crcrlf /dev/ttyUSB0
```
You might need to set a different baud rate. Initially, the device is configured with 115200 baud.

```
picocom --omap crcrlf -b115200 /dev/ttyUSB0
```


Check that the firmware responds
```
AT

OK
```

Check the version
```
AT+GMR
AT version:1.7.4.0(May 11 2020 19:13:04)
SDK version:3.0.4(9532ceb)
compile time:May 27 2020 10:12:17
Bin version(Wroom 02):1.7.4
OK
```

Configure UART to 9600 baud (stored in flash)
```
AT+UART_DEF=9600,8,1,0,0
```

Set WLAN mode to "station" (stored in flash)
```
AT+CWMODE_DEF=1
```