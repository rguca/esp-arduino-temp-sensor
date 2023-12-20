#ifdef ENABLE_LOG
    #define LOG(...) \
        Serial.printf(__VA_ARGS__);\
        Serial.println();
#else
    #define LOG(format, ...)
#endif