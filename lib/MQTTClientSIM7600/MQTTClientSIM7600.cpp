#include <MQTTClientSIM7600.h>

/**
 * Open MQTT network connection
 **/
bool MQTTClientSIM7600::open(const char *host, uint16_t port)
{
    sendCommand("ATI\r");
    sendCommand("AT+CGMR=?\r");
    sendCommand("AT+CGMR\r");
    delay(10000);
    sendCommand("AT+CGDCONT=1,\"IP\",\"telstra.iph\"\r");
    delay(1000);
    sendCommand("AT+NETOPEN\r");

    delay(1000);

    sendCommand("AT+IPADDR\r");

    //sendCommand("AT+CSOCKSETPN=1\r");
    //sendCommand("AT+CIPMODE=0\r");

    //sendCommand("AT+CGPADDR\r");

    delay(1000);

    sendCommand("AT+CMQTTSTART\r", 1000, "+CMQTTSTART:");

    /*{
        Serial.println("CMQTTSTART failure");
        return false;
    }*/

    delay(1000);

    if (!sendCommand("AT+CMQTTACCQ=0,\"SIMCom_client01\",1\r"))
    {
        Serial.println("CMQTTACCQ failure");
        return false;
    }

    delay(1000);

    if (!sendCommand("AT+CMQTTSSLCFG=0,0\r"))
    {
        Serial.println("CMQTTSSLCFG failure");
        return false;
    }

    delay(1000);

    /* if (!sendCommand("AT+CMQTTWILLTOPIC=1,10\r", 1000, "\r\n>"))
    {
        Serial.println("CMQTTWILLTOPIC failure");
        return false;
    }

    delay(1000);

    if (!sendCommand("esp32/will\r"))
    {
        Serial.println("CMQTTWILLTOPIC send failure");
        return false;
    }

    delay(1000);

    if (!sendCommand("AT+CMQTTWILLMSG=1,4,1\r", 1000, "\r\n>"))
    {
        Serial.println("CMQTTWILLMSG failure");
        return false;
    }

    delay(1000);

    if (!sendCommand("test\r"))
    {
        Serial.println("CMQTTWILLMSG send failure");
        return false;
    }*/

    Serial.println("OK, CONNECTION SETUP READY!");
    delay(1000);

    //memset(m_buffer, 0, sizeof(m_buffer));
    //char cBuffer[36 + strlen(host)];
    //sprintf(cBuffer, "AT+CMQTTCONNECT=0,\"tcp://%s:%u\",60,1\r", host, port);

    Serial.println("CMQTTCONNECT Connecting...");
    if (sendCommand("AT+CMQTTCONNECT=0,\"tcp://a1kvoec1nbfoig-ats.iot.ap-southeast-2.amazonaws.com:8883\",60,1\r"))
    {
        Serial.println("CMQTTCONNECT SUCCESS");
        m_state = MQTT_CONNECTED;
        sendCommand("AT+CMQTTCONNECT?\r");
        //checkGPS();
        return true;

        /*if (sendCommand(0, HTTP_CONN_TIMEOUT, "+CHTTPSOPSE: 0"))
            {
                m_state = HTTP_CONNECTED;
                m_host = host;
                checkGPS();
                return true;
            }*/
    }
    Serial.println("CMQTTCONNECT FAILED");
    // checkGPS();
    Serial.println(m_buffer);
    m_state = MQTT_ERROR;
    return false;
}

bool MQTTClientSIM7600::loadCert(const char *filename, const int size, const char *contents)
{
    Serial.println("Loading certificate: ");
    Serial.print(filename);
    Serial.print("\r\n");

    char buffer[2000];

    //memset(m_buffer, 0, sizeof(m_buffer));
    sprintf(buffer, "AT+CCERTDOWN=\"%s\",%d\r", filename, size);
    Serial.println(buffer);

    if (sendCommand(buffer, 1000, ">"))
    {
        Serial.println("CERT ACCEPTED!");
        char filebuffer[sizeof(contents) + 1];
        sprintf(filebuffer, "\r%s\r", contents);

        if (sendCommand(filebuffer, 1000))
        {
            Serial.println("CERT LOADED!");
            return true;
        }
    }
    else
    {
        Serial.println("CERT FAILED");
    }

    return false;
}

bool MQTTClientSIM7600::deleteCert(const char *filename)
{
    char dBuffer[16 + strlen(filename)];
    sprintf(dBuffer, "AT+CCERTDELE=\"%s\"\r", filename);
    if (!sendCommand(dBuffer))
    {
        return false;
    }
    return true;
}

bool MQTTClientSIM7600::verifyCerts(const char *fn1, const char *fn2, const char *fn3)
{
    if (sendCommand("AT+CCERTLIST\r"))
    {
        return true;
    }
    return false;
}

bool MQTTClientSIM7600::configureSSL()
{
    if (sendCommand("AT+CSSLCFG=\"sslversion\",0,4\r") && sendCommand("AT+CSSLCFG=\"authmode\",0,2\r") && sendCommand("AT+CSSLCFG=\"ignorelocaltime\",0,1\r") && sendCommand("AT+CSSLCFG=\"negotiatetime\",0,120\r"))
    {
        return true;
    }

    return false;
}

bool MQTTClientSIM7600::setupSSL(const char *fn1, const char *fn2, const char *fn3)
{
    char caBuffer[28 + strlen(fn1)];
    sprintf(caBuffer, "AT+CSSLCFG=\"cacert\",0,\"%s\"\r", fn1);
    if (!sendCommand(caBuffer))
    {
        return false;
    }

    char ccBuffer[32 + strlen(fn2)];
    sprintf(ccBuffer, "AT+CSSLCFG=\"clientcert\",0,\"%s\"\r", fn2);
    if (!sendCommand(ccBuffer))
    {
        return false;
    }

    char ckBuffer[31 + strlen(fn3)];
    sprintf(ckBuffer, "AT+CSSLCFG=\"clientkey\",0,\"%s\"\r", fn3);
    if (!sendCommand(ckBuffer))
    {
        return false;
    }

    return true;
}

bool MQTTClientSIM7600::subscribe(const char topic[])
{
    if (sendCommand("AT+CMQTTSUBTOPIC=0,10,0\r", 1000, "\r\n>"))
    {
        Serial.println("OK, INPUT TOPIC...");
        if (sendCommand("esp32/sub\r"))
        {
            Serial.println("OK, SUBSCRIBED!");
            sendCommand("AT+CMQTTSUB=0\r");
            return true;
        }
    }

    return false;
}

bool MQTTClientSIM7600::publish(const char topic[], const char payload[])
{
    //char tBuffer[23 + strlen(topic)];
    //sprintf(tBuffer, "AT+CMQTTTOPIC=0,%d", strlen(topic) + 1);
    if (!sendCommand("AT+CMQTTTOPIC=0,10\r", 1000, "\r\n>"))
    {
        Serial.println("CMQTTTOPIC failure");
        return false;
    }

    delay(1000);

    if (!sendCommand("esp32/pub\r"))
    {
        Serial.println("CMQTTTOPIC set failure");
        return false;
    }

    delay(1000);

    if (!sendCommand("AT+CMQTTPAYLOAD=0,4", 1000, "\r\n>"))
    {
        Serial.println("CMQTTPAYLOAD failure");
        return false;
    }

    delay(1000);

    if (!sendCommand(payload))
    {
        Serial.println("CMQTTTOPIC set payload failure");
        return false;
    }

    delay(1000);

    if (sendCommand("AT+CMQTTPUB=0,1,60"))
    {
        Serial.println("SENT MESSAGE!!!");
        return true;
    }

    Serial.println("Failed to send message");
    return false;
}

/**
 * Close MQTT network connection
 **/
void MQTTClientSIM7600::close()
{
    delay(1000);
    sendCommand("AT+CMQTTDISC=0,120\r");
    delay(1000);
    sendCommand("AT+CMQTTREL=0\r");
    delay(1000);
    sendCommand("AT+CMQTTSTOP\r");
    m_state = MQTT_DISCONNECTED;
     sendCommand("AT+NETCLOSE\r");
}