#pragma once
#ifndef XBEE_BAUDRATE
#include <FreematicsNetwork.h>
#endif

typedef enum
{
    MQTT_DISCONNECTED = 0,
    MQTT_CONNECTED,
    MQTT_SENT,
    MQTT_ERROR,
} MQTT_STATES;

class MQTTClientSIM7600 : public ClientSIM7600
{
public:
    MQTT_STATES state() { return m_state; }
    bool open(const char *host = 0, uint16_t port = 0);
    bool loadCert(const char *filename, const int size, const char *contents);
    bool deleteCert(const char *filename);
    bool verifyCerts(const char *fn1, const char *fn2, const char *fn3);
    bool configureSSL();
    bool setupSSL(const char *fn1, const char *fn2, const char *fn3);
    bool subscribe(const char topic[]);
    bool publish(const char topic[], const char payload[]);
    void close();

protected:
    MQTT_STATES m_state = MQTT_DISCONNECTED;
    String m_host;
};