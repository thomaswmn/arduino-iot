# AT Commands

AT Kommandos wurden ursprünglich zur Kommunikation zwischen Computer und Modem entwickelt (siehe auch [Wikipedia](https://de.wikipedia.org/wiki/AT-Befehlssatz)). Sie werden als Text über eine serielle Schnittstelle an das Modem gesendet. Dieses antwortet auf dem gleichen Weg. Alle Kommandos beginnen mit den Zeichen `AT` (für "attention"). Nach jedem Kommando folgt ein Zeilenwechsel. Dieser besteht aus den zwei Steuerzeichen "Carriage Return" (`"\r"`) und "Line Feed" (`"\n"`). Das Modem beantwortet die Kommandos in der Regel mit `OK` oder `ERROR`.

Der ESP8266 ist mit einer Software vorkonfiguriert, die einen Speziellen Satz dieser AT Kommandos unterstützt. Die Kommandos können genutzt werden, um das WLAN Modul zu konfigurieren, und um Daten über WLAN zu senden und zu empfangen.

Hinweis: Nach jedem Kommando muss man warten, bis dieses verarbeitet wurde. In der Regel wird das durch die Rückmeldung `OK` oder `ERROR` angezeigt.

## Wichtige Kommandos

Die hier zusammenfassend erklärten Befehle stammen aus dem [Handbuch](https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf) des Herstellers.


Einfache Abfragen
* `AT` --> leeres Kommando, sollte immer `OK` zurückgeben
* `AT+RST` --> Neustart des Moduls
* `AT+GMR` --> zeigt Versionsinformationen an

Konfiguration
* `AT+CWMODE_DEF=1` --> setzt den Geräte-Modus auf "Client". Wird im Modul gespeichert.
* `AT+CWJAP_CUR="Netz","Passwort"` --> konfiguriert das Modul für eine Verbindung mit einem WLAN Netz "Netz" mit dem Passwort "Passwort".
* `AT+CWJAP_DEF="Netz","Passwort"` --> wie `AT+CWJAP_CUR`, aber die Konfiguration wird im Modul gespeichert.
* `AT+CIPSSLSIZE=4096` --> konfiguriert die Größe eines Puffers im WLAN-Adapter, hier auf 4kiB. Nötig wenn man über SSL kommuniziert. Kleinere Puffer führen zu Fehlern bei der Datenverarbeitung. Größere Puffer können das Modul instabil machen.

Datenübertragung
* `AT+CIPSTART="TCP","neverssl.com",80` --> öffnet eine neue TCP Verbindung ohne SSL Verschlüsselung. Das Zielsystem ist `neverssl.com`. Der Ziel-Port ist 80 (HTTP unverschlüsselt).
* `AT+CIPSTART="SSL","test-bosch-sfp-kos.azure-devices.net",443` --> öffnet eine neue TCP Verbindung mit SSL Verschlüsselung. Das Zielsystem ist `test-bosch-sfp-kos.azure-devices.net`. Der Ziel-Port ist 443 (HTTPS, also HTTP verschlüsselt).
* `AT+CIPCLOSE` --> beendet eine bestehende TCP-Verbindung
* `AT+CIPSEND=123` --> Beginnt die Datenübertragung. Hier werden 123 Bytes gesendet. Nach kurzer Wartezeit können die Daten geschickt werden. Sobald die angekündigte Anzahl Bytes übertragen wurden, ist das Modul wieder bereit für neue AT-Kommandos.

Netzwerk Infos
* `AT+CWMODE?` --> fragt den Geräte-Modus ab (1=Client, 2=Access Point, 3=beides)
* `AT+CWLAP` --> zeigt eine Liste aller gefundenen WLAN Access Points an
* `AT+CWJAP_CUR?` --> zeigt an, zu welchem WLAN Netz das Modul verbunden ist
* `AT+CIFSR` --> zeigt die IP-Adresse und die MAC-Adresse des ESP8266 im WLAN an
* `AT+PING=<IP>` --> sendet ein "ping" Paket an die angegebene Adresse. Es werden auch Host-Namen unterstützt.

## Datenübertragung

Die Software des ESP8266 übernimmt die Kommunikation mit dem WLAN und implementiert die Protokolle IP, TCP und SSL. Dadurch ist die Kommunikation mit typischen Gegenstellen im Internet ohne großen Aufwand realisierbar.

Der ESP8266 unterstützt mehrere mögliche Konfigurationen für den Datenempfang. Wir verwenden hier den aktiven Modus. Immer, wenn das Gerät auf der bestehenden Verbindung Daten aus dem Internet empfängt, werden diese direkt an den Arduino weitergeleitet. Jedes empfangene Paket wird durch den String `+IPD` eingeleitet. Darauf folgen die vom Server empfangenen Daten.

Für jede Anfrage wird zunächst eine SSL-Verbindung geöffnet (`AT+CIPSTART... `). Im Anschluss können ein oder mehrere Datenpakete gesendet werden (`AT+CIPSEND=... `). Sobald auf der Verbindung Daten vom Server empfangen werden, gibt der ESP8266 diese über die serielle Schnittstelle aus. Zum Abschluss eines Vorgangs sollte die Verbindung  mit `AT+CIPCLOSE` wieder beendet werden.

1. `AT+CIPSTART... `
1. warten, bis die Verbindung aufgebaut ist
1. `AT+CIPSEND=AnzahlBytes`
1. kurz warten, bis der ESP8266 bereit ist
1. Daten senden, genaue Datenmenge wie im Kommando angegeben
1. warten auf Daten vom Server
1. `AT+CIPCLOSE` 
1. warten, bis die Verbindung abgebaut ist


