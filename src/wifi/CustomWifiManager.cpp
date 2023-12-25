#define TAG "WiFi"
#include "CustomWifiManager.h"

CustomWifiManager::CustomWifiManager() {
	this->setDebugOutput(false);
}

void CustomWifiManager::setupAP(const char* ap_ssid, const char* ap_password) {
	bool is_connected = this->autoConnect(ap_ssid, ap_password);
	if (is_connected) {
		this->startWebPortal();
	}
}

void CustomWifiManager::setOnSave(std::function<void()> callback) {
	this->setSaveParamsCallback(callback);
}

void CustomWifiManager::connect() {	
	LOG("Connecting WiFi...")
	WiFi.mode(WIFI_STA);
	WiFi.begin();
}

bool CustomWifiManager::isConnected() {
	return WiFi.status() == WL_CONNECTED;
}

bool CustomWifiManager::awaitConnect(unsigned long timeout) {
	if (WiFi.waitForConnectResult(timeout) == WL_CONNECTED) {
		LOG("Connected as %s", WiFi.localIP().toString().c_str());
		return true;
	}
	LOGE("Connect failed")
	return false;
}
