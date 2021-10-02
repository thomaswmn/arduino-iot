# SoftwareSerial

Weitere Informationen und ein Beispiel finden sich in der [Arduino Dokumentation SoftwareSerial](https://www.arduino.cc/en/Reference/softwareSerial).


## Nutzung

Der Arduino UNO hat nur eine in Hardware aufgebaute Serielle Schnittstelle. Diese ist mit den Arduino-Pins 0 (`RX`) und 1 (`TX`) verbunden. 

Die Bibliothek SoftwareSerial erlaubt es, beliebige Ports des Arduino als serielle Schnittstelle zu nutzen. Allerdigns wird sie duch Software-Funktionen realisiert. Daher bietet sie nur eingeschränkte Funktionen und Leistung.

Die Bibliothek unterstützt Geschwindigkeiten bis zu 115200 baud (Bit pro Sekunde). Die Geschwindigkeit wird beim Aufruf der `begin()` Methode übergeben. In unserem Beispiel unten wird 9600 baud konfiguriert.

Hinweis: Bei einer seriellen Schnittstelle muss auf beiden Seiten der Übertragungsstrecke die gleiche Geschwindigkeit konfiguriert werden.

## Beispiel

```C++
#include <SoftwareSerial.h>

// which pins do you want to use?
// do not use pins 0 and 1 - these are occupied by the hardware serial
#define SOFTSERIAL_RX_PIN 2
#define SOFTSERIAL_TX_PIN 3

// instanciate the SoftwareSerial object
SoftwareSerial softSerial(SOFTSERIAL_RX_PIN, SOFTSERIAL_TX_PIN);

void setup()
{
    // start up the hardware serial (to the PC)
    Serial.begin(9600);

    // start up the software serial
    softSerial.begin(9600);
}

void loop() // run over and over
{
    // copy data from software Serial to hardware serial
    if (softSerial.available())
        Serial.write(softSerial.read());
}
```