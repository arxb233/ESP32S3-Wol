#include <WiFi.h>

bool needReconnect = false;
unsigned long wifiReconnectTime = 0;

void WiFiEvent(WiFiEvent_t event);
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("[WiFi] STA 模式已启动");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("[WiFi] 已连接到 AP");
      needReconnect = false;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("[WiFi] WiFi 已断开");
      needReconnect = true;
      wifiReconnectTime = millis();
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("[WiFi] 已获取 IP: ");
      Serial.println(WiFi.localIP());
      needReconnect = false;
      break;
    default:
      break;
  }
}
void setup_wifi() {
  WiFi.disconnect(true);
  delay(1000);
  WiFi.onEvent(WiFiEvent);
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("[WiFi] 断线，原因码: ");
    Serial.println(info.wifi_sta_disconnected.reason);
  }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(wifi_ssid, wifi_pass.c_str());
  Serial.println("开始连接 WiFi...");
}
void WifiHeart() {
  unsigned long now = millis();
  if (needReconnect && (now - wifiReconnectTime > 5000)) {
    Serial.println("Heartbeat: 强制重新连接...");
    WiFi.disconnect(true);
    delay(100);
    Serial.println(wifi_ssid);
    Serial.println(wifi_pass.c_str());
    WiFi.begin(wifi_ssid, wifi_pass.c_str());
    wifiReconnectTime = now;
    needReconnect = false;
  }
}
