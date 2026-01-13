#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <WakeOnLan.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP UDP;
WakeOnLan WOL(UDP);

const char* mqtt_server = "192.168.20.22";
const int mqtt_port = 1883;
void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  WOL.setRepeat(1, 100);
}
void loop() {
  if (!mqttClient.connected()) { reconnectMQTT(); }
  mqttClient.loop();
}