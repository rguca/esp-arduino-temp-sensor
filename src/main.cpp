#include "main.h"

OneWireTemperatureSensor temperature_sensor(D3, 9);
OneWireTemperatureSensor temperature_sensor2(D2, 9);

Settings settings;

CustomWifiManager wifi_manager;
CustomMqttClient mqtt_client;

bool is_config_enabled;
float battery_voltage;

void setup() {
	#ifdef ENABLE_LOG
		Serial.begin(74880);
		LOG("Start")
	#endif

	temperature_sensor.requestTemperature();
	temperature_sensor2.requestTemperature();

	settings.load();

	pinMode(D0, INPUT_PULLDOWN_16); 
	is_config_enabled = digitalRead(D0) == 0;
	pinMode(D0, INPUT);
	LOG("Config: %d", is_config_enabled);

	if (!settings.isStateChanged({temperature_sensor.getValue(), temperature_sensor2.getValue()}) && !is_config_enabled) {
		deepSleep();
		return;
	}

	battery_voltage = analogRead(A0) / 1024.0 * 5.66; // Needs to be measured early to not drop it too much

	if (is_config_enabled) {
		Parameters* parameters = settings.getParameters();
		wifi_manager.addParameter(&parameters->mqtt_host);
		wifi_manager.addParameter(&parameters->mqtt_user);
		wifi_manager.addParameter(&parameters->mqtt_password);
		wifi_manager.setOnSave([](void) {
			settings.persist();
			ESP.restart();
		});
		wifi_manager.setEnableConfigPortal(is_config_enabled);
	}
	wifi_manager.setup(DEVICE_NAME, DEVICE_PASSWORD);

	MqttSettings mqtt_settings = settings.getMqttSettings();
	mqtt_client.setOnConnect(sendState);
	mqtt_client.setup(DEVICE_NAME, &mqtt_settings);
}

void sendState() {
	if (!settings.isRebooted()) {
		mqtt_client.registerTemperature("T1");
		mqtt_client.registerTemperature("T2");
		mqtt_client.registerVoltage("Batterie");
		mqtt_client.registerDuration("Laufzeit");
	}
	mqtt_client.sendValue("T1", temperature_sensor.getLastValue());
	mqtt_client.sendValue("T2", temperature_sensor2.getLastValue());
	mqtt_client.sendValue("Batterie", battery_voltage);
	mqtt_client.sendValue("Laufzeit", millis() / 1000.0);
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

	settings.setRebooted();
	settings.save();

	if (is_config_enabled) return;
	int time = 3 * 60;
	if (temperature_sensor.getLastValue() < 40)
		time = 15 * 60;
	LOG("Entering deep sleep for %ds", time)
	ESP.deepSleep(time * 1000000);
}
