#include <stdint.h>
#include <OneWire.h>
#include "log/log.h"

class OneWireTemperatureSensor {
public:
    OneWireTemperatureSensor(uint8_t pin, uint8_t resolution = 0);
    ~OneWireTemperatureSensor();
    void setup();
    bool setResolution(uint8_t resolution);
    bool requestTemperature();
    float getValue();

protected:
    uint8_t pin;
    uint8_t resolution;
    OneWire* one_wire;
    uint8_t address[8];
    bool is_temperature_requested{false};

    uint8_t* readScratchpad();
    bool writeScratchpad(uint8_t th, uint8_t tl, uint8_t cfg);
};
