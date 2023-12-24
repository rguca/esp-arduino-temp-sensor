const char* DEVICE_NAME = "Temp Sensor";
const char* DEVICE_PASSWORD = "keller123";

#include "log/log.h"
#include "temperature/OneWireTemperatureSensor.h"
#include "settings/Settings.h"
#include "wifi/CustomWifiManager.h"
#include "mqtt/CustomMqttClient.h"

void setup();
void setup_wifi();
void setup_mqtt();
void sendState();
void loop();
void deepSleep();
