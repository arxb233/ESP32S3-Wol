bool needReconnect = false;
void WiFiEvent(arduino_event_id_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("WiFi started");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("WiFi connected");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("WiFi disconnected");
            needReconnect = true;
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("Got IP: ");
            Serial.println(WiFi.localIP());
            needReconnect = false;
            break;
        default:
            // Serial.printf("Event: %d\n", event);
            break;
    }
}

void setup_wifi() {
  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.println("Connecting WiFi...");
}
void WifiHeart() {
  if (needReconnect && millis() - lastReconnect > 5000) {
    Serial.println("Heartbeat: forcing reconnect...");
    WiFi.disconnect(true);
    delay(100);
    WiFi.begin(wifi_ssid, wifi_pass);
    lastReconnect = millis();
  }
}
