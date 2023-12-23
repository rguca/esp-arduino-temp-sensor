#include "CustomPreferences.h"

CustomPreferences::CustomPreferences() {
	this->is_rtc_clean = !rtc_memory.begin();
}

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
	}
	
	return false;
}
