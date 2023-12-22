#include <Preferences.h>
#include <RTCMemory.h>
#include <CRC16.h>
#include "log/log.h"

struct MqttSettings  {
    String host;
    String user;
    String password;
};

class CustomPreferences : public Preferences {
public:
    MqttSettings getMqttSettings();
    void setMqttSettings(MqttSettings settings);
    bool isStateChanged(std::initializer_list<float> values);

protected:
    struct RtcSettings {
        uint16_t state_crc;
    };
};