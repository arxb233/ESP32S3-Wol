void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("正在连接到 ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_pass);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (millis() - startTime > 60000) {
      Serial.println("\nWiFi 连接超时，进入 AP 配置模式...");
      configMode = true;
      startConfigAP();
      return;
    }
  }
  Serial.println("");
  Serial.println("Wi-Fi连接成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
  calculateBroadcastAddress();
}