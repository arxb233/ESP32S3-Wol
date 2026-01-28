#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>  
#include <WakeOnLan.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Preferences.h>
#include "Html.h"

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
WiFiUDP UDP;
WakeOnLan WOL(UDP);

Preferences prefs;
WebServer server(80);

#define CONFIG_PIN 10
#define LED_PIN LED_BUILTIN

String wifi_ssid = "";
String wifi_pass = "";
String mqtt_ip = "";
int mqtt_port = 8883;
bool configMode = false;