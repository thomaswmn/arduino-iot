# Cloud Service

Der hier genutzte Cloud Service basiert auf verschiedenen Produkten der Microsoft Azure Plattform.

## Datenformat

Es werden Daten im JSON-Format erwartet. Leerzeichen (falls nicht zwischen Anführungszeichen) werden ignoriert.

| Eigenschaft  | optional?    | Typ                    | Bedeutung                                                             |
| ------------ | ------------ | ---------------------- | --------------------------------------------------------------------- |
| `value`      | nein         | Integer oder Float     | Sensorwert                                                            |
| `factor`     | ja           | Float                  | Skalierung für den Wert, `value` wird damit multipliziert             | 
| `devicename` | ja           | String                 | Gerätebezeichnung, wenn nicht angegeben, wird die Device-ID verwendet |

Beispiel für das einfachste Format:
```json
{"value": 12}
```

Beispiel für ein komplexes Format:
```json
{"value": 1232,"factor":0.01,"devicename":"TempSensor"}
```

## Authentifizierung

Zur Authentifzierung wird eine SharedAccessSignature benötigt. Diese kann über ein kleines Python-Script aus dem Shared Access Key generiert werden. Der erzeugte Schlüssel hat nur eine begrenzte Gültigkeit, und läuft danach automatisch ab. Er gilt außerdem, je nach Scope, nur für ein einzelnes Device.

```python
#!/usr/bin/python

from base64 import b64encode, b64decode
from hashlib import sha256
from time import time
from urllib import parse
from hmac import HMAC

def generate_sas_token(uri, key, policy_name, expiry=3600):
    ttl = time() + expiry
    sign_key = "%s\n%d" % ((parse.quote_plus(uri)), int(ttl))
    #print(sign_key)
    signature = b64encode(HMAC(b64decode(key), sign_key.encode('utf-8'), sha256).digest())

    rawtoken = {
        'sr' :  uri,
        'sig': signature,
        'se' : str(int(ttl))
    }

    if policy_name is not None:
        rawtoken['skn'] = policy_name

    return 'SharedAccessSignature ' + parse.urlencode(rawtoken)


print(generate_sas_token('test-bosch-sfp-kos.azure-devices.net/devices/test-dev-id', '<insert key here>', None))
#print(generate_sas_token('test-bosch-sfp-kos.azure-devices.net/devices', '<insert key here>', 'device'))
```

## Aufbau

* IoT Hub
* Stream Analytics job
* Power BI

t.b.c.
