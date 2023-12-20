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