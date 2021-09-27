#!/bin/bash

data='{"test":"val"}'
devid=test-dev-id
token="$(./generate_sas_token.py)"
#token="SharedAccessSignature sr=test-bosch-sfp-kos.azure-devices.net%2Fdevices%2Ftest-dev-id&sig=ZGo3kDxXM%2Bdrj49ZiIlbXOJNVxWQpDF0IdWEhvrZW3M%3D&se=1618591213"

#curl \
#  -v \
#  -X POST \
#  -d "$data" \
#  -H "Content-Type: application/json" \
#  -H "Authorization: $token" \
#  https://test-bosch-sfp-kos.azure-devices.net/devices/${devid}/messages/events?api-version=2020-03-13


curl \
  -v \
  -X POST \
  --user-agent "" \
  --header "Accept:" \
  -d "$data" \
  -H "Content-Type: application/json" \
  -H "Authorization: $token" \
  https://test-bosch-sfp-kos.azure-devices.net/devices/${devid}/messages/events?api-version=2020-03-13
