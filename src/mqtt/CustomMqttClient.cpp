#include "CustomMqttClient.h"

CustomMqttClient::CustomMqttClient() : MqttClient(wifi_client) {
}

void CustomMqttClient::setup(String device_name, String host, String user, String password) {
    this->device.ids = strdup(String(ESP.getChipId(), HEX).c_str());
    this->device.name = strdup(device_name.c_str());
    this->device.mf = "espressif";
    
    this->setUsernamePassword(user, password);
    if (this->connect(host.c_str())) {
        LOG("MQTT connected to %s", host.c_str())
        this->onConnectCallback();
    } else {
        LOG("MQTT connection to %s failed! Error code = %d", host.c_str(), this->connectError());
    }
}

void CustomMqttClient::sendTemperature(String name, float value) {
    StaticJsonDocument<512> doc;
    doc["dev_cla"] = "temperature";
    doc["unit_of_meas"] = "°C";

    this->sendMessage(name, value, &doc);

    LOG("Temperature %s sent: %f", name.c_str(), value);
}

void CustomMqttClient::sendVoltage(String name, float value) {
    StaticJsonDocument<512> doc;
    doc["dev_cla"] = "voltage";
    doc["unit_of_meas"] = "V";

    this->sendMessage(name, value, &doc);

    LOG("Voltage %s sent: %f", name.c_str(), value);
}

void CustomMqttClient::sendDuration(String name, float value) {
    StaticJsonDocument<512> doc;
    doc["dev_cla"] = "duration";
    doc["unit_of_meas"] = "s";

    this->sendMessage(name, value, &doc);

    LOG("Duration %s sent: %f", name.c_str(), value);
}

void CustomMqttClient::sendMessage(String name, float value, JsonDocument* doc) {
    String url_device_name = String(this->device.name);
    url_device_name.toLowerCase();
    url_device_name.replace(' ', '-');
    String url_name = String(name.c_str());
    url_name.toLowerCase();
    url_name.replace(' ', '-');
    String topic = String(url_device_name + "/sensor/" + url_name + "/state");

    (*doc)["stat_cla"] = "measurement";
    (*doc)["name"] = name;
    (*doc)["stat_t"] = topic;
    (*doc)["uniq_id"] = String(url_device_name + "-" + url_name);

    JsonObject dev = doc->createNestedObject("dev");
    dev["ids"] = this->device.ids;
    dev["name"] = this->device.name;
    if (this->device.sw) dev["sw"] = this->device.sw; // "arduino Dec 19 2023, 16:02:13";
    if (this->device.mdl) dev["mdl"] = this->device.mdl; // "d1_mini";
    if (this->device.mf) dev["mf"] = this->device.mf;
    if (this->device.sa) dev["sa"] = this->device.sa;

    this->beginMessage(String("homeassistant/sensor/" + url_device_name + "/" + url_name + "/config"), true);
    serializeJson(*doc, *this);
    this->endMessage();

    this->beginMessage(topic, true);
    this->print(value);
    this->endMessage();
}

void CustomMqttClient::setOnConnect(std::function<void()> callback) {
    this->onConnectCallback = callback;
}
