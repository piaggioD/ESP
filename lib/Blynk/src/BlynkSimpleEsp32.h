/**
 * @file       BlynkSimpleEsp32.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Oct 2016
 * @brief
 *
 */

#ifndef BlynkSimpleEsp32_h
#define BlynkSimpleEsp32_h

#ifndef ESP32
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

#define BLYNK_SEND_ATOMIC

#include <BlynkApiArduino.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <WiFi.h>

class BlynkWifi
//Toto je název třídy. BlynkWifi je třída, která dědí z BlynkProtocol<BlynkArduinoClient>. To znamená, že BlynkWifi přebírá veškeré vlastnosti a 
//metody z BlynkProtocol, kde BlynkProtocol je šablonová třída, která přijímá jako šablonový parametr třídu BlynkArduinoClient.

    : public BlynkProtocol<BlynkArduinoClient>
{
    typedef BlynkProtocol<BlynkArduinoClient> Base;
public:
    BlynkWifi(BlynkArduinoClient& transp)  //BlynkArduinoClient& transp: Toto je referenční parametr na instanci třídy BlynkArduinoClient. 
                                           // Použití reference znamená, že místo kopie objektu se předává odkaz na původní objekt, což šetří paměť a výkon.
        : Base(transp) //Base je typedef pro BlynkProtocol<BlynkArduinoClient>. V podstatě, Base je alias pro základní třídu BlynkProtocol s parametrem BlynkArduinoClient
    {}

    void config(const char* auth,
                void(*handleInterruptCallback)(void),
                const char* domain = BLYNK_DEFAULT_DOMAIN,
                uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth,handleInterruptCallback);
        this->conn.begin(domain, port);
    }

    void config(const char* auth,
                void(*handleInterruptCallback)(void),
                IPAddress   ip,
                uint16_t    port = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth,handleInterruptCallback);
        this->conn.begin(ip, port);
    }

};

static WiFiClient _blynkWifiClient;
static BlynkArduinoClient _blynkTransport(_blynkWifiClient);
BlynkWifi Blynk(_blynkTransport);

// #include <BlynkWidgets.h>

#endif
