

from Azure docs (https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-security#security-tokens)
"A token signed with a shared access key grants access to all the functionality associated with the shared access policy permissions. A token signed with a device identity's symmetric key only grants the DeviceConnect permission for the associated device identity."

Shared access key 
  --> go to Azure portal, your IoT Hub, "Shared access policies", "device"
  --> check only "Device connect"
  --> copy primary key

Device key
  --> go to Azure portal, your IoT Hub, "IoT devices", your device id
  --> copy primary key



scope of the key
 * for device key, the scope is limited to the respective device, e.g.  test-bosch-sfp-kos.azure-devices.net/devices/{device-id}
 * for the shared key, the scope can be broader, e.g. encompassing all devices
 * it is possible to further limit the scope by defining a prefix (see generate_sas_token.py)


