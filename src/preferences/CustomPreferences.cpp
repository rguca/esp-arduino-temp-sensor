#define TAG "Prefs"
#include "CustomPreferences.h"

CustomPreferences::CustomPreferences() {
	this->is_rtc_clean = !rtc_memory.begin();
}

Parameters* CustomPreferences::getParameters() {
	if (this->parameters == NULL) {
		this->loadParameters();
	}
	return this->parameters;
}

void CustomPreferences::loadParameters() {
	this->begin("mqtt");
	this->parameters = new Parameters {
		{"mqtt_host", "MQTT host", this->getString("host").c_str(), 20},
		{"mqtt_user", "MQTT user", this->getString("user").c_str(), 20},
		{"mqtt_password", "MQTT password", this->getString("password").c_str(), 20}
	};
	this->end();
	LOG("Loaded")
}

void CustomPreferences::saveParameters() {
	this->begin("mqtt");
	this->putString("host", this->parameters->mqtt_host.getValue());
	this->putString("user", this->parameters->mqtt_user.getValue());
	this->putString("password", this->parameters->mqtt_password.getValue());
	this->end();
	LOG("Saved");
}

MqttSettings CustomPreferences::getMqttSettings() {
	if (this->parameters == NULL) {
		this->loadParameters();
	}
	MqttSettings preferences {
		this->parameters->mqtt_host.getValue(),
		this->parameters->mqtt_user.getValue(),
		this->parameters->mqtt_password.getValue(),
	};
	return preferences;
}

bool CustomPreferences::isRtcClean() {
	return this->is_rtc_clean;
}

bool CustomPreferences::isStateChanged(std::initializer_list<float> values) {
	CRC16 crc;
	for (float v : values) {
		crc.add((uint8_t*) &v, sizeof(float));
	}
	uint16_t new_crc = crc.calc();

	RtcSettings* rtc_settings = this->rtc_memory.getData();
	if (this->is_rtc_clean || rtc_settings->state_crc != new_crc) {
		LOG("State changed %d <> %d", rtc_settings->state_crc, new_crc)
		rtc_settings->state_crc = new_crc;
		this->rtc_memory.save();
		return true;
	} else {
		LOG("State unchanged")
	}
	
	return false;
}
