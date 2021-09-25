# Aufgabenstellungen


## Anpassung Spannungspegel

* Der ESP8266 arbeitet mit 3,3V. Eingangs-Signale sollten keine höheren Pegel als 3,5V haben. Der Arduino arbeitet mit 5V. Ausgangspegel haben in der Regel entweder 0V oder 5V.
* Wie können die Spannungspegel durch eine möglichst einfache Schaltung angepasst werden?


## Zweite Serielle Schnittstelle am Arduino

* Der Arduino UNO hat (in der Hardware) nur eine serielle Schnittstelle. Diese wird bereits zur Kommunikation mit dem PC genutzt, z.B. um Debug-Meldungen auszugeben. Über die gleiche Schnittstelle kann der ESP8266 daher nicht angeschlossen werden.
* Schaut euch die Arduino Dokumentation zur Library [SoftwareSerial](https://www.arduino.cc/en/Reference/softwareSerial) an.
* An welche Pins am Arduino kann das WLAN-Modul angeschlossen werden?


## Hardware Aufbau

* Plant den Hardware-Aufbau, erstellt eine Skizze. 
  * Achtet darauf, die Anpassung des Spannungspegels richtig umgesetzt zu haben.
  * Achtet darauf, die richtige Spannungsquelle (3,3V) zu verwenden.
* Verbindet das Modul wie geplant mit dem Arduino.


## Inbetriebnahme des WLAN Moduls

* Der Arduino soll über die Serielle (Software-) Schnittstelle AT-Kommandos an den ESP8266 senden und Antworten empfangen. Gleichzeitig soll der Arduino über die Serielle (Hardware-) Schnittstelle Debug-Ausgaben an den PC senden.
* Schreibt eine `setup()` Routine, die erste einfache AT-Kommandos an den ESP8266 schickt.
* Programmiert den Arduino so, dass alle Rückmeldungen des ESP8266 auch am PC ausgegeben werden. Möglicherweise braucht der ESP8266 etwas Zeit für die Verarbeitung.
* Testet eure Implemntierung. Funktioniert die Kommunikation mit dem WLAN-Modul?


## Konfiguration WLAN

* Der ESP8266 kann so konfiguriert werden, das er sich mit einem WLAN Netz verbindet.
* Sendet die entsprechenden Konfigurations-Kommandos in euerer `setup()` Routine.
* Wie könnt ihr testen, das die Verbindung aufgebaut wurde?


## Progmem-Speicher für Variablen

* Der Arduino hat 32 kiB Speicher (Flash), in dem das Programm abgelegt wird. Er hat aber nur 2 Kilobyte Arbeitsspeicher (SRAM). In diesem werden alle Variablen und weitere Informationen abgelegt, die sich zur Laufzeit ändern können.
* Für die Kommunikation mit unserem Server in der Cloud müssen allein für das HTTP-Protokoll 350 bis 400 Byte gesendet werden. Dieser Teil ist statisch, d.h. es wird bei jedem Aufruf der gleiche Text gesendet. Erst danach folgen die veränderlichen Daten. Es ist relativ ineffizient, die statischen Header-Informationen in einer Variable im Arbeitsspeicher abzulegen. 
* Es gibt die Arduino Library [progmem](https://www.arduino.cc/reference/en/language/variables/utilities/progmem/). Mit dieser können über einfache Hilfsfunktionen unveränderliche Variablen im Programmspeicher abgelegt und von dort gelesen werden.
* Schreibt ein einfaches Testprogramm. Dies soll einen längeren Text im Programmspeicher ablegen. Beim Start des Arduino soll dieser Text von dort aus auf der Seriellen Schnittstelle zum PC ausgegeben werden.


## HTTP Verbindung zum Cloud-Server

* t.b..c



## Auswertung des HTTP Status Code

