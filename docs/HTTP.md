# HTTP

HTTP ist die Abkürzung für HyperText Transport Protocol. Es wurde urspünglich zur Übertragung von Webseiten entwickelt. Mittlerweile wird es aber für viele weitere Zwecke eingesetzt.

Wir verwenden hier die Version 1.1. Diese ist formal in [RFC 2616](https://datatracker.ietf.org/doc/html/rfc2616) spezifiziert. Einen ersten Überblick liefert auch die [Wikipedia](https://de.wikipedia.org/wiki/Hypertext_Transfer_Protocol). Wir benötigen hier aber nur einen kleinen Teil dessen, was das Protokoll kann. Dieser wird im Folgenden erklärt.

Im Protokoll spricht ein "Client" (hier unser Arduino) mit einem "Server" (hier unser Setup in der Public Cloud). Bei HTTP beginnt immer der Client die Kommunikation mit einer Anfrage, der Server beantwortet diese. 

Anfrage und Antwort werden als Text übertragen. Die ersten Zeilen (der Header) werden vom HTTP Protokoll definiert. Nach einer Leerzeile folgt ein beliebier Inhalt. Am Ende jeder Zeile folgen die zwei Steuerzeichen "Carriage Return" (`"\r"`) und "Line Feed" (`"\n"`). Diese sind in den Beispielen unten nicht dargestellt.

## Einfache Anfrage

Im folgenden Beispiel wird eine Webseite abgerufen. Der Client öffnet eine Verbindung zum Server und überträgt eine Anfrage. Die Anfrage besteht aus wenigen Zeilen, dem HTTP Header. Sie wird durch eine Leerzeile abgeschlossen.

```
GET / HTTP/1.1
Host: www.wikipedia.de

```

Kurze Erklärung der Zeilen
* `GET / HTTP/1.1`
    * `GET` ist die HTTP Methode. `GET` bedeutet, dass Daten heruntergeladen (abgerufen) werden.
    * `/` ist der Pfad des Dokuments auf dem Server, das abgerufen wird. Hier fragt der Client nach dem "obersten" Verzeichnis.
    * `HTTP/1.1` ist die verwendete Version des Protokolls. Die wird hier mitgeschickt, damit der Server die folgenden Zeilen richtig interpretieren kann.
* `Host:` ist der Name des Servers, an den diese Anfrage gesendet wird. Manchmal übernimmt ein Server Dienst für mehrere solche Namen. Durch diese Angabe kann er die Anfragen unterscheiden.


Sobald der Server die Anfrage (und die abschließende Leerzeile) erhalten hat, beginnt er diese zu verarbeiten. Die Verbindung bleibt dabei offen. Der Server antwortet dann z.B. mit:

```
HTTP/1.1 200 OK
Server: nginx/1.6.2
Date: Mon, 27 Sep 2021 19:09:16 GMT
Content-Type: text/html; charset=UTF-8
Transfer-Encoding: chunked
Connection: keep-alive

<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, maximum-scale=1.0, minimum-scale=1.0, initial-scale=1.0" />
    <title>Wikipedia, die freie Enzyklop&auml;die</title>
...
```
Erklärung
* `HTTP/1.1 ...`
    * `HTTP/1.1` ist wieder die Protokoll-Version. Der Server hat also die vom Client gesendete Versionsnummer verstanden und antwortet entsprechend.
    * `200` ist der Status-Code. Zahlen aus dem Bereich 200-299 kennzeichnen Erfolg. Zahlen im Bereich 400-499 bedeuten, dass der Client etwas falsch gemacht hat (z.B. eine ungültige Anfrage, falsches Passwort, ...). Der Bereich 500-599 steht für Fehler auf der Seite des Servers.
    * `OK` ist eine weitere Erklärung zum Status.
* `Server:` gibt an, welche Software auf dem Server läuft.
* `Date:` Datum und Uhrzeit, zu der der Server die Antwort versendet hat.
* `Content-Type: text/html; charset=UTF-8` gibt an, welches Format an Daten der Server schickt. Hier eine HTML-Seite im Zeichensatz UTF-8.
* `Transfer-Encoding` und `Connection` dienen der Verbindungsverwaltung zwischen Client und Server. Diese Informationen können hier ignoriert werden.
* Nach diesen Header-Zeilen folgt eine Leerzeile. Diese zeigt den Beginn der Daten an. Nach der Leerzeile werden die Daten gesendet, hier eine HTML Seite.


## Upload von Informationen

Ähnlich wie die Abfrage funktioniert auch der Upload von Daten. Dieser wird im Header der Anfrage angekündigt. Nach der Leerzeile, die das Ende des Headers markiert, folgen dann direkt die hochzuladenden Daten. Im folgenden Beispiel schickt der Client ein einfaches JSON Dokument an den Server.

```
POST /devices/test-dev-id/messages/events?api-version=2020-03-13 HTTP/1.1
Host: test-bosch-sfp-kos.azure-devices.net
Content-Type: application/json
Authorization: SharedAccessSignature ...
Content-Length: 12

{"test":"1"}
```

Kurze Erklärung der Zeilen
* `POST /device/... HTTP/1.1`
    * `POST` ist die HTTP Methode. `POST` bedeutet, dass Daten gesendet werden ("upload"). 
    * `/device/... ` ist der Pfad des Dokuments auf dem Server, für das die Daten bestimmt sind. Hier nicht wirklich ein Dokument, sondern stattdessen nutzt der Server diesen Teil, um zwischen verschiedenen Geräten (hier `test-dev-id`) und Funktionen zu unterscheiden.
    * `HTTP/1.1` ist die verwendete Version des Protokolls. Die wird hier mitgeschickt, damit der Server die folgenden Zeilen richtig interpretieren kann.
* `Host:` ist der Name des Servers, an den diese Anfrage gesendet wird. Manchmal übernimmt ein Server Dienst für mehrere solche Namen. Durch diese Angabe kann er die Anfragen unterscheiden.
* `Content-Type:` ist das Datenformat, das der Client zum Server schickt. In unserem Fall senden wir ein JSON Dokument.
* `Authorization:` ist eine Art Passwort. Der Server verarbeitet die Anfrage nur, wenn hier ein gültiger Text gesendet wird.
* `Content-Length:` ist die Länge des Dokuments, das der Client zum Server schickt (in Bytes).
* Die Leerzeile markiert das Ende der HTTP Protokoll-Informationen. Nach der Leerzeile folgt das Dokument.
* Die Zeile `{"test":"1"}` ist das Dokument, das der Client zum Server schickt. Der String ist 12 Zeichen lang.

Sobald der Server die angekündigten 12 Bytes empfangen hat, wird die Anfrage bearbeitet. Die Verbindung bleibt dabei offen. Dann antwortet der Server, z.B. so:

```
HTTP/1.1 204 No Content
Content-Length: 0
Vary: Origin
Server: Microsoft-HTTPAPI/2.0
x-ms-request-id: f5116577-0a75-40db-9824-ee0385cbaa45
Date: Sat, 11 Sep 2021 07:17:35 GMT

```

Erklärung
* `HTTP/1.1 ...`
    * `HTTP/1.1` ist wieder die Protokoll-Version. Der Server hat also die vom Client gesendete Versionsnummer verstanden und antwortet entsprechend.
    * `204` ist der Status-Code. Zahlen aus dem Bereich 200-299 kennzeichnen Erfolg. Zahlen im Bereich 400-499 bedeuten, dass der Client etwas falsch gemacht hat (z.B. eine ungültige Anfrage, falsches Passwort, ...). Der Bereich 500-599 steht für Fehler auf der Seite des Servers.
    * `No Content` ist eine weitere Erklärung zum Status. In diesem Fall: Alles OK, aber der Server sendet kein Dokument zurück.
* `Content-Length:` ist die Länge des vom Server gesendeten Dokuments.
* `Vary:` ist für Proxy-Server relevant.
* `Server:` gibt an, welche Software auf dem Server läuft.
* `x-ms-request-id:` Eindeutige ID für diese Anfrage und Antwort. Diese findet man z.B. auch in Logs auf dem Server.
* `Date:` Datum und Uhrzeit, zu der der Server die Antwort versendet hat.