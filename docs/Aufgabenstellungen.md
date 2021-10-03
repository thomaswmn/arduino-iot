# Aufgabenstellungen


## 1) Zweite serielle Schnittstelle am Arduino

Weitere Informationen: [SoftwareSerial](./SoftwareSerial.md)

In dieser Aufgabe wird die Verbindung zwischen Arduino und WLAN Modul geplant.

* Der Arduino UNO hat (in der Hardware) nur eine serielle Schnittstelle. Diese wird bereits zur Kommunikation mit dem PC genutzt, z.B. um Debug-Meldungen auszugeben. Über die gleiche Schnittstelle kann der ESP8266 daher nicht angeschlossen werden.
* Schaut euch die Dokumentation zur Library [SoftwareSerial](./SoftwareSerial.md) an.
* An welche Pins am Arduino kann das WLAN-Modul angeschlossen werden?


## 2) Hardware Aufbau

Weitere Informationen: [ESP8266](ESP8266.md)

In dieser Aufgabe wird der Aufbau der Hardware geplant und umgesetzt.

* Plant den Hardware-Aufbau, erstellt eine Skizze. 
* Vergleicht eure Skizze mit der Musterlösung.
* Achtet darauf, die richtige Spannungsquelle (3,3V) zu verwenden.
* Achtet darauf, die Anpassung des Spannungspegels richtig umgesetzt zu haben.
* Verbindet das Modul wie geplant mit dem Arduino.


## 3) Daten weiterleiten
Weitere Informationen: [SoftwareSerial](./SoftwareSerial.md) 

Der Arduino soll über die serielle (Software-) Schnittstelle AT-Kommandos an den ESP8266 senden und Antworten empfangen. Gleichzeitig soll der Arduino über die serielle (Hardware-) Schnittstelle Debug-Ausgaben an den PC senden.
* Schreibt eine `setup()` Routine, die die beiden seriellen Schnittstellen konfiguriert. 
* Welche weiteren Ergänzungen sind notwendig, um die SoftwareSerial Library zu nutzen?
* Schreibt eine Methode `receive(long duration)` mit dem Parameter `duration` in Millisekunden. Diese soll für die angegebene Zeit Daten vom ESP8266 empfangen. Jedes Zeichen soll vom Arduino an den PC weitergegeben werden. 

Hinweise:
* Orientiert euch am Beispiel zur [SoftwareSerial](./SoftwareSerial.md) Library.
* Versucht, die Methode `delay()` nicht zu verwenden.
* Die aktuelle "Uhrzeit" kann über die Methode `millis()` abgefragt werden. Diese Methode gibt die Zeit seit  dem Start des Arduinos in Millisekunden zurück.
  

## 4) Erste Kommandos an das WLAN Modul

Weitere Informationen: [AT-Kommandos](AT-Commands.md)

In dieser Aufgabe soll die Kommunikation mit dem WLAN-Modul getestet werden. Ein einfaches AT-Kommando wird an das Modul gesendet. Die Ausgabe wird auf der seriellen Konsole beobachtet.

* Welches Kommando kann zum Test der Kommunikation verwendet werden?  (siehe [AT-Kommandos](AT-Commands.md))
* Sendet in eurer `loop()` Methode ein einfaches AT-Kommando an den ESP8266. Achtet darauf, das Kommando mit dem erwarteten Zeilenwechsel `\r\n` abzuschließen. Ruft im Anschluss die vorher implementierte Empfangs-Methode auf, um die Daten entgegenzunehmen.
* Testet eure Implementierung. Beobachtet dazu die Ausgabe des WLAN-Moduls im seriellen Monitor in der Arduino-Software. Funktioniert die Kommunikation mit dem WLAN-Modul?


## 5) Konfiguration WLAN

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

## 6) Erste HTTP Verbindung

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


## 7) HTTP Verbindung zum Cloud-Server

Weitere Informationen: [HTTP](HTTP.md), [JSON](JSON.md)

Im nächsten Schritt sollen regelmäßig (z.B. alle 30s) Sensor-Informationen vom Arduino an den Server in der Cloud geschickt werden. Dieser Server erlaubt nur verschlüsselte Verbindungen. 

Unser Server in der Cloud erwartet Daten im JSON-Format. 
Beispiel: `{"value":00000123}`. Der Wert wird als ganze Zahl erwartet. Damit die Zeichenkette immer gleich lang ist, verwenden wir hier führende Nullen.

* Ergänzt die `setup()` Methode, so dass diese das Kommando `AT+CIPSSLSIZE=4096` ausführt. Dies reserviert im ESP8266 einen ausreichend großen Puffer für die SSL Verschlüsselung.

* Implementiert eine Methode `upload(long sensorwert)`. Diese bekommt den aktuellen Messwert übergeben. Die Methode soll die Daten über HTTP an den Server übertragen.

  * Erstellt zunächst einen String, der den Sensor-Wert beschreibt. Dazu kann die bereits vorhandene Methode `sprintf(Puffer, Vorlage, Wert)` verwendet werden. `Puffer` ist eine Variable, die auf einen vorher reservierten String-Puffer zeigt. `Vorlage` ist ein Beispiel-String. An der Stelle, an der die Zahl eingefügt werden soll, muss `%08ld` stehen. `Wert` ist eine Variable mit dem Typ `long`.

    ```C++
    long sensorwert = ...
    char buffer[32];
    sprintf(buffer, "{\"value\":%08ld}", sensorwert);
    ```

  * Legt fest, welche Device-ID euer Gerät nutzt. Abhängig von der Device-ID muss auch die richtige `SharedAccessSignature` verwendet werden. Fragt die Bosch-Mitarbeiter nach den richtigen Angaben für euer Team.

  * Welche HTTP-Header-Zeilen müssen vor den Daten übertragen werden?  Welcher Wert muss beim Header `Content-Length` eingesetzt werden? Orientiert euch am Beispiel in [HTTP](HTTP.md).

  * Am Ende der Methode `upload()` öffnet eine Verbindung zum Server. Nutzt dafür diese Eigenschaften:
    * Protokoll: `SSL`
    * Host: `test-bosch-sfp-kos.azure-devices.net`
    * Port: 443

* Ergänzt eure Methode `loop()` so, dass regelmäßig die Sensor-Daten an den Server übertragen werden.

* Kontrolliert die Rückmeldung des ESP8266 über die Konsole des Arduinos. In der Konsole sollte die Antwort des Servers `HTTP/1.1 204 No Content...` erscheinen.

* Prüft gemeinsam mit einem der Bosch Mitarbeiter, ob die Daten im Dashboard dargestellt werden.


## 8) Rückmeldung des WLAN-Moduls auswerten (optional)

* Der ESP8266 braucht für jedes AT-Kommando etwas Verarbeitungszeit. Bisher muss dazu jedesmal so lange gewartet werden, dass das WLAN-Modul auf jeden Fall wieder bereit für das nächste Kommando ist. In einer der vorhergehenden Aufgaben habt ihr dazu die Methode `receive(long duration)` implementiert. Diese wartet bisher immer für die angegebene Zeit.
* Der ESP8266 antwortet auf die meisten Kommandos mit eindeutigen Zeichenketten, z.B. `OK` oder `ERROR`.
* Ändert die Methode `receive(long duration)` so ab, dass sie sich direkt beendet, wenn `OK` oder `ERROR` erkannt werden. Im längsten Fall soll sie für die angegebene Zeit Daten empfangen sich dann beenden.
* Beachtet: vom ESP8266 wird immer nur ein Zeichen empfangen. Wie kann trotzdem eine Kombination mehrerer Zeichen erkannt werden?

## 9) Rückmeldung am Arduino anzeigen (optional)

* Wie kann man am Arduino sichtbar machen, dass die Datenübertragung ins Internet funktioniert?

