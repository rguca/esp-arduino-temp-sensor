#include "OneWireTemperatureSensor.h"

OneWireTemperatureSensor::OneWireTemperatureSensor(uint8_t pin, uint8_t resolution) : 
    pin(pin),
    resolution(resolution)
{
}

void OneWireTemperatureSensor::setup() {
    this->one_wire = new OneWire(pin);

    if (!this->one_wire->search(this->address)) {
        this->one_wire->reset_search();
        LOG("No OneWire temperature sensor found on pin: %d", pin)
        return;
    }
    LOG("OneWire temperature sensor found: 0x%02X%02X%02X%02X%02X%02X%02X%02X", (int) this->address[0], (int) this->address[1], (int) this->address[2], 
        (int) this->address[3], (int) this->address[4], (int) this->address[5], (int) this->address[6], (int) this->address[7])
    if (OneWire::crc8(this->address, 7) != this->address[7]) {
        LOG("Address CRC invalid")
        return;
    }

    const char* type;
    switch (this->address[0]) {
    case 0x10:
      type = "DS18S20";
      break;
    case 0x28:
      type = "DS18B20";
      break;
    case 0x22:
      type = "DS1822";
      break;
    default:
      type = "unknown";
      return;
    }
    LOG("Type: %s", type)

    if (this->resolution) {
        this->setResolution(this->resolution);
    }
}

bool OneWireTemperatureSensor::setResolution(uint8_t resolution) {
    if (resolution < 9 || resolution > 12) {
        return false;
    }
    if (this->one_wire == NULL) {
        this->setup();
    }
    if (this->address[0] == 0x10) { // Type DS18S20 has no resolution register
        return true;
    }

    uint8_t* data = this->readScratchpad();
    if (data == NULL) return false;

    uint8_t th = data[2];
    uint8_t tl = data[3];
    uint8_t old_cfg = data[4];
    uint8_t new_cfg = ((resolution - 9) << 5) | 0b11111;
    delete data;
    if (old_cfg == new_cfg) {
        return true;
    }

    if (!this->writeScratchpad(th, tl, new_cfg)) {
        return false;
    }
    LOG("Set resolution from %d to %dbit", (old_cfg >> 5) + 9, resolution)
    return true;
}

bool OneWireTemperatureSensor::requestTemperature() {
    if (this->one_wire == NULL) {
        this->setup();
    }
    if (!this->one_wire->reset()) {
        LOG("No presence pulse detected. Can't request temperature")
        return false;
    }
    this->one_wire->select(this->address);
    this->one_wire->write(0x44); // Start conversion
    this->is_temperature_requested = true;
    return true;
}

float OneWireTemperatureSensor::getValue() {
    if (!this->is_temperature_requested) {
        if (!this->requestTemperature()) return NAN;
    }
    while (this->one_wire->read() == 0) {
        delay(10);
    }

    uint8_t* data = this->readScratchpad();
    if (data == NULL) return NAN;

    int16_t raw = (data[1] << 8) | data[0];
    if (this->address[0] == 0x10) { // Type DS18S20
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    } else {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms, 0.5C
        else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms, 0.25C
        else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms, 0.125C
        // default is 12 bit resolution, 750 ms conversion time, 0.0625C
    }
    delete data;
    return (float)raw / 16.0;
}

OneWireTemperatureSensor::~OneWireTemperatureSensor() {
    delete this->one_wire;
}

uint8_t* OneWireTemperatureSensor::readScratchpad() {
    if (!this->one_wire->reset()) {
        LOG("No presence pulse detected. Can't read Scratchpad")
        return NULL;
    }
    this->one_wire->select(this->address);
    this->one_wire->write(0xBE); // Read Scratchpad

    uint8_t* data = new uint8_t[9];
    for (int i = 0; i < 9; i++) { // Read 9 bytes
        data[i] = this->one_wire->read();
    }
    if (OneWire::crc8(data, 8) != data[8]) {
        LOG("Scratchpad CRC invalid")
        delete data;
        return NULL;
    }
    return data;
}

bool OneWireTemperatureSensor::writeScratchpad(uint8_t th, uint8_t tl, uint8_t cfg) {
    if (!this->one_wire->reset()) {
        LOG("No presence pulse detected. Can't write Scratchpad")
        return false;
    }
    this->one_wire->select(this->address);
    this->one_wire->write(0x4E); // Write Scratchpad
    this->one_wire->write(th);
    this->one_wire->write(tl);
    this->one_wire->write(cfg);
    return true;
}