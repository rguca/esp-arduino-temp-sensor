#include "log/log.h"
#include "temperature/OneWireTemperatureSensor.h"
#include "preferences/CustomPreferences.h"
#include "wifi/CustomWifiManager.h"
#include "mqtt/CustomMqttClient.h"

void setup();
void setup_wifi();
void setup_mqtt();
void saveParameters();
void sendState();
void loop();
void deepSleep();