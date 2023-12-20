#include <Preferences.h>

struct MqttSettings  {
    String host;
    String user;
    String password;
};

class CustomPreferences : public Preferences {
public:
    MqttSettings getMqttSettings();
    void setMqttSettings(MqttSettings settings);
};