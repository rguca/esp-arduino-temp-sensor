#pragma once
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include "log/log.h"

struct MqttSettings  {
    const char* host;
    const char* user;
    const char* password;
};

class CustomMqttClient : public MqttClient {
public:
    CustomMqttClient();

    void setOnConnect(std::function<void()> callback);
    void setup(const char* device_name, MqttSettings* settings);

    void registerTemperature(const char* name);
    void registerVoltage(const char* name);
    void registerDuration(const char* name);
    void sendValue(const char* name, float value);

protected:
    struct Device {
        const char* ids;
        const char* name;
        const char* sw;
        const char* mdl;
        const char* mf;
        const char* sa;
    } device;

    WiFiClient wifi_client;
    std::function<void()> onConnectCallback;

    void registerMeasurement(const char* name, JsonDocument* doc);
    String convertName(const char* name);
};