#include <WiFiManager.h>
#include "log/log.h"

#ifdef ENABLE_LOG
    #define LOGI()
#endif

class CustomWifiManager: public WiFiManager {
public:
    struct Parameters {
        WiFiManagerParameter mqtt_host;
        WiFiManagerParameter mqtt_user;
        WiFiManagerParameter mqtt_password;
    } parameters;

    CustomWifiManager();
    void setup(const char* ap_ssid, const char* ap_password);
    void setEnableConfigPortal(bool enable);
    void setOnSave(std::function<void()> callback);
    bool isConnected();

protected:
    bool is_portal_enabled{false};
};