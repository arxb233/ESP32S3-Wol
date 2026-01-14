void setup() {
  Serial.begin(115200);
  pinMode(CONFIG_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  prefs.begin("config", true);
  wifi_ssid = prefs.getString("ssid", "");
  prefs.end();
  checkConfigMode();
}
