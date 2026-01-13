#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <WakeOnLan.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Preferences.h>
#include "Html.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP UDP;
WakeOnLan WOL(UDP);

Preferences prefs;
WebServer server(80);

String wifi_ssid = "";
String wifi_pass = "";
String mqtt_ip = "";
int mqtt_port = 1883;
bool configMode = false;

void setup() {
  Serial.begin(115200);
  prefs.begin("config", true);
  wifi_ssid = prefs.getString("ssid", "");
  prefs.end();
  if (wifi_ssid.length() == 0) {
    configMode = true;
    startConfigAP();
  } else {
    configMode = false;
    setup_wifi();
    mqttClient.setServer(mqtt_ip.c_str(), mqtt_port);
    mqttClient.setCallback(mqttCallback);
    WOL.setRepeat(1, 100);
  }
}
void loop() {
  if (configMode) {
    server.handleClient();
    return;
  }
  if (!mqttClient.connected()) { reconnectMQTT(); }
  mqttClient.loop();
}