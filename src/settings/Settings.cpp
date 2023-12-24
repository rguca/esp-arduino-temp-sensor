#define TAG "Settings"
#include "Settings.h"

Settings::Settings() :
	RTCMemory("/settings")
{
}

void Settings::load() {
	if (LittleFS.begin()) {
		LOG("Filesystem initialized")
	} else {
		LOGE("Filesystem initialization failed")
	}
	this->begin();
	this->rtc_data = RTCMemory::getData();
	LOG("Loaded")
}

Parameters* Settings::getParameters() {
	if (this->parameters == NULL) {
		this->loadParameters();
	}
	return this->parameters;
}

void Settings::loadParameters() {
	RtcData* data = this->rtc_data;
	this->parameters = new Parameters {
		{"mqtt_host", "MQTT host", data->mqtt_host, sizeof(data->mqtt_host) - 1},
		{"mqtt_user", "MQTT user", data->mqtt_user, sizeof(data->mqtt_user) - 1},
		{"mqtt_password", "MQTT password", data->mqtt_password, sizeof(data->mqtt_password) - 1}
	};
}

void Settings::save() {
	if (RTCMemory::save()) {
		LOG("Saved");
	} else {
		LOGE("Save failed")
	}
}

void Settings::persist() {
	RtcData* data = this->rtc_data;
	strcpy(data->mqtt_host, this->parameters->mqtt_host.getValue());
	strcpy(data->mqtt_user, this->parameters->mqtt_user.getValue());
	strcpy(data->mqtt_password, this->parameters->mqtt_password.getValue());
	data->is_rebooted = false;
	data->state_crc = 0;
	if (RTCMemory::persist()) {
		LOG("Persisted");
	} else {
		LOGE("Persist failed")
	}
}

MqttSettings Settings::getMqttSettings() {
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

bool Settings::isRebooted() {
	return this->rtc_data->is_rebooted;
}

void Settings::setRebooted(bool value) {
	this->rtc_data->is_rebooted = value;
}

bool Settings::isStateChanged(std::initializer_list<float> values) {
	CRC16 crc;
	for (float v : values) {
		crc.add((uint8_t*) &v, sizeof(float));
	}
	uint16_t new_crc = crc.calc();

	RtcData* data = this->rtc_data;
	if (!data->is_rebooted || data->state_crc != new_crc) {
		LOG("State changed %d <> %d", data->state_crc, new_crc)
		data->state_crc = new_crc;
		this->save();
		return true;
	} else {
		LOG("State unchanged")
	}
	
	return false;
}
