#pragma once
#include <WiFiManager.h>
#include "log/log.h"

class CustomWifiManager: public WiFiManager {
public:
    CustomWifiManager();
    void setup(const char* ap_ssid, const char* ap_password);
    void setEnableConfigPortal(bool enable);
    void setOnSave(std::function<void()> callback);
    void connect();
    bool isConnected();
    bool awaitConnect(unsigned long timeout = 3000);

protected:
};
