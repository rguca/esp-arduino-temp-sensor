#include <RTCMemory.h>
#include <CRC16.h>
#include "log/log.h"
#include "wifi/CustomWifiManager.h"
#include "mqtt/CustomMqttClient.h"

#pragma pack(push, 1)
struct RtcData {
	bool is_rebooted;
	uint16_t state_crc;

	char mqtt_host[21];
	char mqtt_user[21];
	char mqtt_password[21];
};
#pragma pack(pop)
// char (*__kaboom)[sizeof( RtcSettings )] = 1; // Prints the size in error

struct Parameters {
	WiFiManagerParameter mqtt_host;
	WiFiManagerParameter mqtt_user;
	WiFiManagerParameter mqtt_password;
};

class Settings : public RTCMemory<RtcData> {
public:
	Settings();

	void load();
	void persist();
	void save();
	Parameters* getParameters();
	MqttSettings getMqttSettings();
	bool isStateChanged(std::initializer_list<float> values);
	bool isRebooted();
	void setRebooted(bool value = true);

protected:
	RtcData* rtc_data;
	Parameters* parameters;

	void loadParameters();
};
