#include <Preferences.h>
#include <RTCMemory.h>
#include <CRC16.h>
#include "log/log.h"
#include "wifi/CustomWifiManager.h"
#include "mqtt/CustomMqttClient.h"

struct Parameters {
	WiFiManagerParameter mqtt_host;
	WiFiManagerParameter mqtt_user;
	WiFiManagerParameter mqtt_password;
};

class CustomPreferences : public Preferences {
public:
	CustomPreferences();

	void loadParameters();
	void saveParameters();
	Parameters* getParameters();
	MqttSettings getMqttSettings();
	void saveMqttSettings(MqttSettings settings);
	bool isStateChanged(std::initializer_list<float> values);
	bool isRtcClean();

protected:
	struct RtcSettings {
		uint16_t state_crc;
	};

	RTCMemory<RtcSettings> rtc_memory;
	bool is_rtc_clean{true};
	Parameters* parameters;
};
