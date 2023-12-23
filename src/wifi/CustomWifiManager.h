#pragma once
#include <WiFiManager.h>
#include "log/log.h"

class CustomWifiManager: public WiFiManager {
public:
    CustomWifiManager();
    void setup(const char* ap_ssid, const char* ap_password);
    void setEnableConfigPortal(bool enable);
    void setOnSave(std::function<void()> callback);
    bool isConnected();

protected:
    bool is_portal_enabled{false};
};
