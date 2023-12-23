#define TAG "WiFi"
#include "CustomWifiManager.h"

CustomWifiManager::CustomWifiManager() :
	parameters {
		{"mqtt_host", "MQTT host", "192.168.1.5", 20},
		{"mqtt_user", "MQTT user", "user", 20},
		{"mqtt_password", "MQTT password", "", 20}
	}
{
}

void CustomWifiManager::setup(const char* ap_ssid, const char* ap_password) {
	this->addParameter(&parameters.mqtt_host);
	this->addParameter(&parameters.mqtt_user);
	this->addParameter(&parameters.mqtt_password);
	WiFiManager::setEnableConfigPortal(false);
	this->setConfigPortalBlocking(false);

	#ifdef LOG_INFO
		int start = millis();
	#endif
	bool is_connected = this->autoConnect(ap_ssid, ap_password);
	#ifdef LOG_INFO
		if (is_connected) {
				LOG("Connected")
				LOG("Time: %dms", (int)(millis() - start))
		}
	#endif
	if (this->is_portal_enabled) {
		if (is_connected) this->startWebPortal();
		else this->startConfigPortal(ap_ssid, ap_password);
	}
}

void CustomWifiManager::setEnableConfigPortal(bool enable) {
	this->is_portal_enabled = enable;
}

void CustomWifiManager::setOnSave(std::function<void()> callback) {
	this->setSaveParamsCallback(callback);
}

bool CustomWifiManager::isConnected() {
	return WiFi.status() == WL_CONNECTED;
}