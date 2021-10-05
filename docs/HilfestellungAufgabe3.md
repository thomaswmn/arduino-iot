# 3) Daten weiterleiten

## Anforderung

Methode `receive()`

Ziel: Daten vom ESP8266 lesen. Jedes Zeichen direkt an den PC weiterleiten.

Parameter: `duration`, Typ `long` --> Zeit in Millisekunden, für die die Methode (mindestens) Daten empfangen und weiterleiten soll

## Pseudo-Code

* Start-Zeit auslesen und Ende-Zeit berechnen
* solange Ende-Zeit noch nicht erreicht
  * sind Zeichen zum Auslesen vom ESP8266 bereit?
  * wenn ja, dann
    * ein Zeichen lesen
    * Zeichen an PC weiterleiten


