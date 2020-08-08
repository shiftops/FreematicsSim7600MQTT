#include <FreematicsPlus.h>
#include <MQTTClientSIM7600.h>
#include <config.h>
#include <secrets.h>

FreematicsESP32 freematics;
MQTTClientSIM7600 net;

bool initNetwork()
{
    Serial.print("Init cellular module...");

    if (net.begin(&freematics))
    {
        //Serial.print(net.deviceName());
        Serial.println(" OK");
    }
    else
    {
        Serial.println("NO");
        return false;
    }

    /*Serial.print("IMEI:");
    Serial.println(net.IMEI);

    if (net.checkSIM())
    {
      Serial.println("SIM Card OK");
    }
    else
    {
      Serial.println("No SIM Card");
    }*/

    Serial.print("Registering on network...");
    /*if (net.setup(CELL_APN))
    {
      Serial.println("OK");
    }
    else
    {
      Serial.println("NO");
      return false;
    }*/

    /*String op = net.getOperatorName();
    if (op.length())
    {
      Serial.print("Operator:");
      Serial.println(op);
    }

    Serial.print("Obtaining IP address...");
    String ip = net.getIP();
    if (ip)
    {
      Serial.println(ip);
    }
    else
    {
      Serial.println("N/A");
    }

    int signal = net.getSignal();
    if (signal)
    {
      Serial.print("RSSI:");
      Serial.print(signal);
      Serial.println("dBm");
    }*/

    //net.deleteCert("clientcert.pem");
    net.loadCert("clientcert.pem", 1222, AWS_CERT_CRT);

    //net.deleteCert("clientkey.pem");
    net.loadCert("clientkey.pem", 1677, AWS_CERT_PRIVATE);

    //net.deleteCert("cacert.pem");
    net.loadCert("cacert.pem", 1190, AWS_CERT_CA);

    Serial.println("All Certs Loaded! ...");

    if (net.verifyCerts("cacert.pem", "clientcert.pem", "clientkey.pem"))
    {
        Serial.println("Certs verified!");
    }
    else
    {
        Serial.println("Cert verification failed");
    }

    delay(5000);

    Serial.println("Configure SSL");

    if (net.configureSSL())
    {
        Serial.println("SSL CONFIGURED!");
    }
    else
    {
        Serial.println("SSL CONFIGURATION FAILED..");
    }

    if (net.setupSSL("cacert.pem", "clientcert.pem", "clientkey.pem"))
    {
        Serial.println("SSL READY!");
    }
    else
    {
        Serial.println("SSL FAILED..");
    }

    Serial.print("Init MQTT stack...");
    if (net.open())
    {
        Serial.println("CONNECTED");
    }
    else
    {
        Serial.println("NOT CONNECTED");
    }

    return true;
}

void setup()
{
    Serial.begin(115200);

    // use following for Freematics ONE+
    freematics.begin();

    // initialize cellular module
    while (!initNetwork())
        ;
}

void loop()
{

    // put your main code here, to run repeatedly:

    // TODO we should re-check for SIM connection here

    Serial.println("NET STATE: ");
    Serial.print(net.state());
    Serial.print("\r\n");

    if (net.state() != MQTT_CONNECTED)
    {
        Serial.print("Connecting...");
        if (net.open(AWS_IOT_ENDPOINT, AWS_IOT_PORT))
        {
            Serial.println("CONNECTED!");
            delay(1000);
        }
        else
        {
            Serial.println("FAILED TO CONNECT! Retry in 3 seconds...");
            net.close();
            delay(3000);
            return;
        }
    }

    // if (net.subscribe("esp32/pub"))
    // {
    //     Serial.println("SUBSCRIBED!");
    // }
    // else
    // {
    //     Serial.println("FAILED TO SUBSCRIBE!");
    //     net.close();
    //     delay(3000);
    // }

    if (net.publish("esp32/pub", "test\r"))
    {
        Serial.println("SENT MESSAGE!");
    }
    else
    {
        Serial.println("FAILED TO SEND MESSAGE!");
        net.close();
        delay(3000);
    }

    Serial.println("All done.. waiting 10 seconds...");
    delay(10000);
}