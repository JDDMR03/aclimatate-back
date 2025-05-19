#ifndef HS100_H
#define HS100_H

#include <ESP8266WiFi.h>

class HS100 {
public:
    HS100(const char* ip, uint16_t port = 9999);
    void turnOn();
    void turnOff();
    String getStatus();

private:
    const char* _ip;
    uint16_t _port;
    void sendCommandNoResponse(const char* payload);  // Declaración añadida
    String sendCommandWithResponse(const char* payload);  // Declaración añadida
    String base64Decode(const char* input);
    uint8_t base64CharToValue(char c);
    String decodeResponse(String response);
};

#endif
