#include <stdint.h>
#include <OneWire.h>
#include "log/log.h"

class OneWireTemperatureSensor {
public:
    static bool requestAllTemperatures();

    OneWireTemperatureSensor(uint8_t pin, uint8_t resolution = 0);
    ~OneWireTemperatureSensor();
    void setup();
    bool setResolution(uint8_t resolution);
    bool requestTemperature();
    float getValue();
    float getLastValue();

protected:
    const unsigned int MAX_WAIT_TIME = 1000;

    uint8_t pin;
    uint8_t resolution;
    OneWire* one_wire;
    uint8_t* address;
    uint8_t* scratch_pad;
    bool is_conversion_in_progress{false};

    bool resetAndSelectDevice();
    bool waitForResult();
    bool readScratchpad();
    bool writeScratchpad(uint8_t th, uint8_t tl, uint8_t cfg);
};
