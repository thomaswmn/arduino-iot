# JSON

JSON steht für JavaScript Object Notation. Dieses Dokumentenformat wurde ursprünglich gemeinsam mit der Programmiersprache JavaScript entwickelt. Es hat sich mittlerweile aber auch in vielen anderen Bereichen durchgesetzt.

JSON erlaubt es, auch komplexe Datenstrukturen als Text zu codieren. Da das Format Text-basiert ist, lassen sich die Dokumente einfach lesen und auch von Hand bearbeiten. Die vorgegebene Syntax erlaubt gleichzeitig die programmatische Verarbeitung.


## JSON Datentypen

JSON kennt die einfachen Datentypen String, Zahl, Boolscher Wert und Null. Außerdem können Listen (Arrays) und Objekte (als Schlüssel-Wert-Listen) codiert werden. Eine Erklärung und ein Beispiel finden sich in der [Wikipedia](https://de.wikipedia.org/wiki/JavaScript_Object_Notation#Zeichencodierung_und_Datentypen). Hier eine Zusammenfassung daraus:

* `null` ist der Nullwert. Es wird nicht in Anführungszeichen gesetzt.
* `true` und `false` sind die beiden boolschen Werte. Auch sie werden ohne Anführungszeichen gesetzt.
* Eine Zahl besteht aus den Ziffern `0-9`, optional mit Vorzeichen und / oder Dezimalpunkt. Auch Exponenten sind möglich (mit `e` oder `E` und eigenem Vorzeichen).
* Strings oder Zeichenketten beginnen und enden mit doppelten Anführungszeichen (`"`).
* Eine Liste (oder Array) beginnt mit einer öffnenden eckigen Klammer `[` und endet mit einer schließenden eckigen Klammer `]`. Dazwischen stehen 0 oder mehr Elemente. Diese werden durch Kommata (`,`) getrennt. Die Elemente dürfen verschiedene Datentypen haben.
* Ein Objekt beginnt mit einer öffnenden geschweiften Klammer `{` und endet mit einer schließenden geschweiften Klammer `}`. Ein Objekt definiert 0 oder mehr Eigenschaften. Diese werden durch Kommata (`,`) getrennt. Jede Eigenschaft besteht aus einem Schlüssel und einem Wert. Diese sind durch den Doppelpunkt (`:`) voneinander getrennt. Schlüssel sind Strings (Zeichenketten) und sollten innerhalb eines Objektes eindeutig sein. Als Wert sind beliebige Datentypen erlaubt.

Zeilenwechsel und Leerzeichen, die nicht innerhalb von Strings stehen, sind optional.

## Beispiele

### Einfaches Objekt

Das folgende Beispiel zeigt ein einfaches Objekt mit zwei Eigenschaften. Die erste Eigenschaft hat den Schlüssel `"sensor"` und einen Wert vom Datentyp String. Die zweite Eigenschaft hat den Schlüssel `"value"` und einen Wert vom Typ Zahl.

```json
{
    "sensor": "temp",
    "value": 20.03
}
```

Das gleiche Objekt kann auch kompakter dargestellt werden:

```json
{"sensor":"temp","value":20.03}
```

### Komplexes Objekt

Dieses Beispiel zeigt ein komplexeres Objekt. Es hat wieder zwei Eigenschaften, `"sensor"` und `"data"`. Der Wert der Eigenschaft `"data"` ist eine Liste von Objekten. Jedes Objekt in dieser Liste hat wieder zwei Eigenschaften, `"timestamp"` und "`value`".
```json
{
    "sensor": "temp",
    "data": [
        {
            "timestamp": 1631386830,
            "value": 20.03
        },
        {
            "timestamp": 1631386831,
            "value": 20.12
        },
        {
            "timestamp": 1631386832,
            "value": 20.08
        },
        {
            "timestamp": 1631386833,
            "value": 19.85
        },
        {
            "timestamp": 1631386834,
            "value": 20.01
        },
    ]   
}
```



