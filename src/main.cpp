#include "main.h"
#include "log/log.h"
#include "preferences/CustomPreferences.h"
#include "wifi/CustomWifiManager.h"
#include "mqtt/CustomMqttClient.h"

CustomPreferences preferences;

CustomWifiManager wifi_manager;
CustomMqttClient mqtt_client;

void setup() {
  #ifdef ENABLE_LOG
    Serial.begin(115200);
  #endif

  pinMode(D0, INPUT_PULLDOWN_16); 
  bool is_config_enabled = digitalRead(D0) == 0;
  pinMode(D0, INPUT);
  LOG("Config: %d", is_config_enabled);

  wifi_manager.setEnableConfigPortal(is_config_enabled);
  wifi_manager.setOnSave(saveParameters);
  wifi_manager.setup("Temp Sensor", "keller123");

  MqttSettings settings = preferences.getMqttSettings();
  mqtt_client.setOnConnect(sendState);
  mqtt_client.setup("Temp Sensor", settings.host, settings.user, settings.password);
  preferences.end();
}

void saveParameters() {
  MqttSettings settings {
    wifi_manager.parameters.mqtt_host.getValue(),
    wifi_manager.parameters.mqtt_user.getValue(),
    wifi_manager.parameters.mqtt_password.getValue()
  };
  preferences.setMqttSettings(settings);
  LOG("Preferences saved");
}

void sendState() {
  mqtt_client.sendTemperature("T1", 22.0);
}

void loop() {
    wifi_manager.process();
    mqtt_client.poll();

    if (!wifi_manager.isConnected()) return;
    if (!mqtt_client.connected()) return;
}
