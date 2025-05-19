#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <HS100.h>
#include <ArduinoJson.h>

const char* ssid = "OnePlus Nord 3 5G";
const char* password = "123456789";
const char* hs100_ip = "192.168.128.29";

HS100 hs100(hs100_ip);
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a WiFi");
  Serial.println(WiFi.localIP());

  // Configura las rutas
  server.on("/on", handleEncender);
  server.on("/off", handleApagar);
  server.on("/temperature", handleTemperature);

  // Inicia el servidor
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();  // Maneja las solicitudes de los clientes
}

void handleEncender() {
  hs100.turnOn();  // Enciende el HS100

  // Crea un objeto JSON con el mensaje
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["message"] = "HS100 encendido";

  // Convierte el objeto JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  server.send(200, "application/json", jsonString);
}

void handleApagar() {
  hs100.turnOff();  // Apaga el HS100

  // Crea un objeto JSON con el mensaje
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["message"] = "HS100 apagado";

  // Convierte el objeto JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  server.send(200, "application/json", jsonString);
}

void handleTemperature() {
  // Lee el sensor de temperatura
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1024.0);
  float temperatura = voltage * 15.0;

  // Crea un objeto JSON con la temperatura
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["temperatura"] = temperatura;

  // Convierte el objeto JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Envía la respuesta JSON al cliente
  server.send(200, "application/json", jsonString);
}