#include "HS100.h"

HS100::HS100(const char* ip, uint16_t port) : _ip(ip), _port(port) {}

void HS100::turnOn() {
    Serial.println("[HS100] Encendiendo...");
    sendCommandNoResponse("AAAAKtDygfiL/5r31e+UtsWg1Iv5nPCR6LfEsNGlwOLYo4HyhueT9tTu36Lfog==");
    Serial.println("[HS100] Comando de encendido enviado.");
}

void HS100::turnOff() {
    Serial.println("[HS100] Apagando...");
    sendCommandNoResponse("AAAAKtDygfiL/5r31e+UtsWg1Iv5nPCR6LfEsNGlwOLYo4HyhueT9tTu3qPeow==");
    Serial.println("[HS100] Comando de apagado enviado.");
}

String HS100::getStatus() {
    Serial.println("[HS100] Consultando estado...");
    return sendCommandWithResponse("AAAAI9Dw0qHYq9+61/XPtJS20bTAn+yV5o/hh+jK8J7rh+vLtpbr");
}

void HS100::sendCommandNoResponse(const char* payload) {
    WiFiClient client;
    if (!client.connect(_ip, _port)) {
        Serial.println("[HS100] Error: No se pudo conectar al dispositivo.");
        return;
    }

    // Decodificar el payload en base64
    String decodedPayload = base64Decode(payload);
    Serial.println("[HS100] Payload decodificado: " + decodedPayload);

    // Enviar el payload
    client.print(decodedPayload);
    Serial.println("[HS100] Payload enviado.");

    // No esperamos respuesta, cerramos la conexión inmediatamente
    client.stop();
    Serial.println("[HS100] Conexión cerrada.");
}

String HS100::sendCommandWithResponse(const char* payload) {
    WiFiClient client;
    if (!client.connect(_ip, _port)) {
        Serial.println("[HS100] Error: No se pudo conectar al dispositivo.");
        return "Connection failed";
    }

    // Decodificar el payload en base64
    String decodedPayload = base64Decode(payload);
    Serial.println("[HS100] Payload decodificado: " + decodedPayload);

    // Enviar el payload
    client.print(decodedPayload);
    Serial.println("[HS100] Payload enviado.");

    // Esperar la respuesta
    String response = "";
    unsigned long startTime = millis();
    while (client.connected() || client.available()) {
        if (client.available()) {
            char c = client.read();
            response += c;
        }
        if (millis() - startTime > 5000) { // Timeout de 5 segundos
            Serial.println("[HS100] Timeout: No se recibió respuesta.");
            break;
        }
    }

    client.stop();
    Serial.println("[HS100] Respuesta recibida: " + response);
    return decodeResponse(response);
}

String HS100::base64Decode(const char* input) {
    String decoded = "";
    int inputLength = strlen(input);
    for (int i = 0; i < inputLength; i += 4) {
        char a = input[i];
        char b = input[i+1];
        char c = input[i+2];
        char d = input[i+3];

        uint32_t n = (base64CharToValue(a) << 18) |
                     (base64CharToValue(b) << 12) |
                     (base64CharToValue(c) << 6) |
                     base64CharToValue(d);

        decoded += char((n >> 16) & 0xFF);
        decoded += char((n >> 8) & 0xFF);
        decoded += char(n & 0xFF);
    }
    return decoded;
}

uint8_t HS100::base64CharToValue(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return 0;
}

String HS100::decodeResponse(String response) {
    String decoded = "";
    uint8_t key = 171;
    for (size_t i = 0; i < response.length(); i++) {
        uint8_t c = response[i];
        uint8_t decodedChar = c ^ key;
        decoded += (char)decodedChar;
        key = c;
    }
    return decoded;
}
