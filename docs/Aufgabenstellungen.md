# Aufgabenstellungen


## Zweite Serielle Schnittstelle am Arduino

In dieser Aufgabe wird die Verbindung zwischen Arduino und WLAN Modul geplant.

* Der Arduino UNO hat (in der Hardware) nur eine serielle Schnittstelle. Diese wird bereits zur Kommunikation mit dem PC genutzt, z.B. um Debug-Meldungen auszugeben. Über die gleiche Schnittstelle kann der ESP8266 daher nicht angeschlossen werden.
* Schaut euch die Arduino Dokumentation zur Library [SoftwareSerial](./SoftwareSerial.md) an.
* An welche Pins am Arduino kann das WLAN-Modul angeschlossen werden?


## Hardware Aufbau

Weitere Informationen: [ESP8266](ESP8266.md)

In dieser Aufgabe wird der Aufbau der Hardware geplant und umgesetzt.

* Plant den Hardware-Aufbau, erstellt eine Skizze. 
* Vergleicht eure Skizze mit der Musterlösung.
* Achtet darauf, die richtige Spannungsquelle (3,3V) zu verwenden.
* Achtet darauf, die Anpassung des Spannungspegels richtig umgesetzt zu haben.
* Verbindet das Modul wie geplant mit dem Arduino.


## Daten Weiterleiten
Weitere Informationen: [SoftwareSerial](./SoftwareSerial.md) 


Der Arduino soll über die Serielle (Software-) Schnittstelle AT-Kommandos an den ESP8266 senden und Antworten empfangen. Gleichzeitig soll der Arduino über die Serielle (Hardware-) Schnittstelle Debug-Ausgaben an den PC senden.
* Schreibt eine `setup()` Routine, die die beiden Seriellen Schnittstellen konfiguriert. 
* Schreibt eine Methode `receive(long duration)` mit dem Parameter `duration` in Millisekunden. Diese soll für die angegebene Zeit Daten vom ESP8266 empfangen. Jedes Zeichen soll vom Arduino an den PC weitergegeben werden. 

Hinweise:
* Orientiert euch am Beispiel zur [SoftwareSerial](./SoftwareSerial.md) Library.
* Versucht, die Methode `delay()` nicht zu verwenden.
* Die aktuelle "Uhrzeit" kann über die Methode `millis()` abgefragt werden. Die Methode gibt die Zeit seit  dem Start des Arduinos in Millisekunden zurück.
  

## Erste Kommandos an das WLAN Modul

Weitere Informationen: [AT-Kommandos](AT-Commands.md)

In dieser Aufgabe soll die Kommunikation mit dem WLAN-Modul getestet werden. Ein einfaches AT-Kommando wird an das Modul gesendet. Die Ausgabe wird auf der Seriellen Konsole beobachtet.

* Welches Kommando kann zum Test der Kommunikation verwendet werden?  (siehe [AT-Kommandos](AT-Commands.md))
* Sendet in eurer `loop()` Methode ein einfaches AT-Kommando an den ESP8266. Achtet darauf, das Kommando mit dem erwarteten Zeilenwechsel `\r\n` abzuschließen. Ruft im Anschluss die vorher implementierte Empfangs-Methode auf, um die Daten entgegenzunehmen.
* Testet eure Implementierung. Beobachtet dazu die Ausgabe des WLAN-Moduls im Seriellen Monitor in der Arduino-Software. Funktioniert die Kommunikation mit dem WLAN-Modul?


## Konfiguration WLAN

Weitere Informationen: [AT-Kommandos](AT-Commands.md)

In dieser Aufgabe soll sich das WLAN-Modul mit einem WLAN verbinden.

* Der ESP8266 kann so konfiguriert werden, das er sich mit einem WLAN Netz verbindet.
* Welche AT-Kommandos müssen dazu an das Modul gesendet werden?
* Sendet die entsprechenden Konfigurations-Kommandos in euerer `setup()` Routine.
* Beachtet, dass der ESP8266 nach jedem Kommando ausreichend Zeit für die Verarbeitung braucht.
* Wie könnt ihr testen, dass die Verbindung aufgebaut wurde?

Hinweis:
* Manche Kommandos an das WLAN-Modul enthalten Anführungszeichen. Diese müssen beim Programmieren durch einen vorgestellten Backslash (\\) maskiert ("versteckt") werden. Anderenfalls werden sie als Ende der Zeichenkette verstanden. 
* Beispiel: `const char cmd[] = "AT+CWJAP_DEF=\"Netzname\",\"Passwort\"\r\n");`

## Erste HTTP Verbindung

Weitere Informationen: [AT-Kommandos](AT-Commands.md), [HTTP](HTTP.md)

In dieser Aufgabe soll eine Internet-Verbindung genutzt werden, um eine Webseite abzurufen. Dazu wird zunächst eine unverschlüsselte Verbindung genutzt.

* Welche Kommandos sind notwendig, um eine Netzwerkverbindung zu öffnen und Daten zu senden? (siehe  [AT-Kommandos](AT-Commands.md))
* Welcher Text muss als Anfrage über die Netzwerkverbindung gesendet werden? (siehe [HTTP](HTTP.md))
* Schreibt eine Methode, die die Webseite über HTTP abruft (z.B. `neverssl.com`).
* Kontrolliert die korrekte Funktion, indem ihr die Antworten des ESP8266 über die Konsole des Arduinos anschaut.

Hinweise:
* Viele Webseiten bieten mittlerweile nur noch verschlüsselte Verbindungen an. Verwendet hier die eine unverschlüsselte Webseite, z.B. `neverssl.com`.
* Der ESP8266 unterstützt unverschlüsselte Verbindungen (Parameter `TCP`) und verschlüsselte Verbindungen (Parameter `SSL`). Für den unverschlüsselten Abruf von Webseiten wird Port 80 verwendet. Für verschlüsselte Webseiten nutzt man Port 443. Siehe Beispiele in [AT-Kommandos](AT-Commands.md).
* Beachtet, dass der Zeilenwechsel in HTTP aus den beiden Zeichen `\r\n` besteht. Die Leerzeile am Ende der Anfrage muss mit übertragen werden. Dazu werden einfach zwei Zeilenwechsel nacheinander gesendet: `\r\n\r\n`
* Nach dem Senden der Anfrage muss der Webserver ausreichend viel Zeit haben, diese zu verarbeiten und zu beantworten. Erst danach darf die Verbindung geschlossen werden. Verwendet dazu die vorher programmierte Methode `receive(duration)`.


## Sensor-Daten als JSON Dokument

Weitere Informationen: [JSON](JSON.md)

Unser Server in der Cloud erwartet Daten im JSON-Format. 
Beispiel: `{"value":00123}`. Der Wert wird als ganze Zahl erwartet. Damit die Zeichenkette immer gleich lang ist, verwenden wir hier führende Nullen.

* Erstellt zunächst einen String, der den Sensor-Wert beschreibt. Dazu kann die bereits vorhandene Methode `sprintf(Puffer, Vorlage, Wert)` verwendet werden. `Puffer` ist eine Variable, die auf einen vorher reservierten String-Puffer zeigt. `Vorlage` ist ein Beispiel-String. An der Stelle, an der die Zahl eingefügt werden soll, muss `%05ld` stehen. `Wert` ist eine Variable mit dem Typ `long`.

```C++
  long sensorwert = ...
  char buffer[64];
  sprintf(buffer, "{\"value\":%05ld}", sensorwert);
```


## HTTP Verbindung zum Cloud-Server

Weitere Informationen: [HTTP](HTTP.md)

Im nächsten Schritt sollen regelmäßig (z.B. alle 30s) Sensor-Informationen vom Arduino an den Server in der Cloud geschickt werden.

* Ergänzt die `setup()` Methode, so dass diese das Kommando `AT+CIPSSLSIZE=4096` ausführt. Dies reserviert im ESP8266 einen ausreichend großen Puffer für die SSL Verschlüsselung.


* Implementiert eine Methode `upload(long sensorwert)`. Diese soll die Daten über HTTP an den Server überträgt. Orientiert euch dazu am Beispiel in [HTTP](HTTP.md).

* Übertragt regelmäßig die Sensor-Daten an den Server.

* Kontrolliert die Rückmeldung des ESP8266 über die Konsole des Arduinos.

* Prüft gemeinsam mit einem der Bosch Mitarbeiter, ob die Daten im Dashboard dargestellt werden.

## Rückmeldung am Arduino

* Wie kann man am Arduino sichtbar machen, dass die Datenübertragung ins Internet funktioniert?
