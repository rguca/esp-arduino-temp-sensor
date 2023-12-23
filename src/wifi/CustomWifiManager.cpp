#define TAG "WiFi"
#include "CustomWifiManager.h"

CustomWifiManager::CustomWifiManager() {
	this->setDebugOutput(false);
}

void CustomWifiManager::setup(const char* ap_ssid, const char* ap_password) {
	WiFiManager::setEnableConfigPortal(false);
	this->setConfigPortalBlocking(false);

	#ifdef INFO_LOG
		int start = millis();
	#endif
	bool is_connected = this->autoConnect(ap_ssid, ap_password);
	#ifdef INFO_LOG
		if (is_connected) {
				LOG("Connected as %s", WiFi.localIP().toString().c_str())
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
