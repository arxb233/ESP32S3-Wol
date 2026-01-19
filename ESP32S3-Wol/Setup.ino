void setup() {
  Serial.begin(115200);
  pinMode(CONFIG_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  prefs.begin("config", true);
  wifi_ssid = prefs.getString("ssid", "");
  wifi_pass = prefs.getString("pass", "");
  mqtt_ip = prefs.getString("mqttip", "");
  mqtt_port = prefs.getInt("mqttport", 1883);
  prefs.end(); 
  setCpuFrequencyMhz(80);  
  checkConfigMode();
}
