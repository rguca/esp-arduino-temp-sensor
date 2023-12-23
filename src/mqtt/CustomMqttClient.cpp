#define TAG "MQTT"
#include "CustomMqttClient.h"

CustomMqttClient::CustomMqttClient() : MqttClient(wifi_client) {
}

void CustomMqttClient::setup(const char* device_name, MqttSettings* settings) {
    this->device.ids = strdup(String(ESP.getChipId(), HEX).c_str());
    this->device.name = device_name;
    this->device.mf = "espressif";
    
    this->setUsernamePassword(settings->user, settings->password);
    if (this->connect(settings->host)) {
        LOG("Connected to %s", settings->host)
        this->onConnectCallback();
    } else {
        LOGE("Connection to %s failed! Error code = %d", settings->host, this->connectError())
    }
}

void CustomMqttClient::registerTemperature(const char* name) {
    StaticJsonDocument<512> doc;
    doc["dev_cla"] = "temperature";
    doc["unit_of_meas"] = "°C";
    doc["suggested_display_precision"] = 1;
    this->registerMeasurement(name, &doc);
    LOG("Temperature %s registered", name)
}

void CustomMqttClient::registerVoltage(const char* name) {
    StaticJsonDocument<512> doc;
    doc["dev_cla"] = "voltage";
    doc["unit_of_meas"] = "V";
    this->registerMeasurement(name, &doc);
    LOG("Voltage %s registered", name)
}

void CustomMqttClient::registerDuration(const char* name) {
    StaticJsonDocument<512> doc;
    doc["dev_cla"] = "duration";
    doc["unit_of_meas"] = "s";
    this->registerMeasurement(name, &doc);
    LOG("Duration %s registered", name)
}

void CustomMqttClient::registerMeasurement(const char* name, JsonDocument* doc) {
    String device_name = this->convertName(this->device.name);
    String name_ = this->convertName(name);
    String topic = String(device_name + "/sensor/" + name_ + "/state");

    (*doc)["stat_cla"] = "measurement";
    (*doc)["name"] = name;
    (*doc)["stat_t"] = topic;
    (*doc)["uniq_id"] = String(device_name + "-" + name_);

    JsonObject dev = doc->createNestedObject("dev");
    dev["ids"] = this->device.ids;
    dev["name"] = this->device.name;
    if (this->device.sw) dev["sw"] = this->device.sw;
    if (this->device.mdl) dev["mdl"] = this->device.mdl;
    if (this->device.mf) dev["mf"] = this->device.mf;
    if (this->device.sa) dev["sa"] = this->device.sa;

    this->beginMessage(String("homeassistant/sensor/" + device_name + "/" + name_ + "/config"), true);
    serializeJson(*doc, *this);
    this->endMessage();
}

void CustomMqttClient::sendValue(const char* name, float value) {
    String device_name = this->convertName(this->device.name);
    String name_ = this->convertName(name);
    String topic = String(device_name + "/sensor/" + name_ + "/state");
    this->beginMessage(topic, true);
    this->print(value);
    this->endMessage();
    LOG("Published %s: %f", name, value)
}

String CustomMqttClient::convertName(const char* name) {
    String name_ = String(name);
    name_.toLowerCase();
    name_.replace(' ', '-');
    return name_;
}

void CustomMqttClient::setOnConnect(std::function<void()> callback) {
    this->onConnectCallback = callback;
}
