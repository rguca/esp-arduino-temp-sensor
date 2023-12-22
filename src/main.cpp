#include "main.h"

OneWireTemperatureSensor temperature_sensor(D3, 9);
OneWireTemperatureSensor temperature_sensor2(D2, 9);

CustomPreferences preferences;

CustomWifiManager wifi_manager;
CustomMqttClient mqtt_client;

bool is_config_enabled;

void setup() {
  #ifdef ENABLE_LOG
    Serial.begin(74880);
  #endif

  temperature_sensor.requestTemperature();
  temperature_sensor2.requestTemperature();
  // OneWireTemperatureSensor::requestAllTemperatures();

  pinMode(D0, INPUT_PULLDOWN_16); 
  is_config_enabled = digitalRead(D0) == 0;
  pinMode(D0, INPUT);
  LOG("Config: %d", is_config_enabled);

  if (!preferences.isStateChanged({temperature_sensor.getValue(), temperature_sensor2.getValue()}) && !is_config_enabled) {
    deepSleep();
    return;
  }

  wifi_manager.setEnableConfigPortal(is_config_enabled);
  wifi_manager.setOnSave(saveParameters);
  wifi_manager.setup("Temp Sensor", "keller123");

  MqttSettings settings = preferences.getMqttSettings();
  mqtt_client.setOnConnect(sendState);
  mqtt_client.setup("Temp Sensor", settings.host, settings.user, settings.password);
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
  mqtt_client.sendTemperature("T1", temperature_sensor.getLastValue());
  mqtt_client.sendTemperature("T2", temperature_sensor2.getLastValue());
}

void loop() {
    wifi_manager.process();
    mqtt_client.poll();

    if (!wifi_manager.isConnected()) return;
    if (!mqtt_client.connected()) return;

    deepSleep();
}

void deepSleep() {
  mqtt_client.stop(); // Stop client so that outgoing messages are send

  if (is_config_enabled) return;
  int time = 60;
  LOG("Entering deep sleep for %ds", time)
  ESP.deepSleep(time * 1000000);
}
