#include "CustomPreferences.h"

MqttSettings CustomPreferences::getMqttSettings() {
    this->begin("mqtt");
    MqttSettings preferences {
        this->getString("host"),
        this->getString("user"),
        this->getString("password")
    };
    this->end();
    return preferences;
}

void CustomPreferences::setMqttSettings(MqttSettings settings) {
    this->begin("mqtt");
    this->putString("host", settings.host);
    this->putString("user", settings.user);
    this->putString("password", settings.password);
    this->end();
}

bool CustomPreferences::isStateChanged(std::initializer_list<float> values) {
    CRC16 crc;
    for (float v : values) {
        crc.add((uint8_t*) &v, sizeof(float));
    }
    uint16_t new_crc = crc.calc();

    RTCMemory<RtcSettings> rtc_memory;
    bool has_old = rtc_memory.begin();
    RtcSettings* rtc_settings = rtc_memory.getData();
    if (!has_old || rtc_settings->state_crc != new_crc) {
        LOG("State changed %d <> %d", rtc_settings->state_crc, new_crc)
        rtc_settings->state_crc = new_crc;
        rtc_memory.save();
        return true;
    }
    
    return false;
}
