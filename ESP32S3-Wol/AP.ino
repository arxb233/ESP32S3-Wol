void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
}

void handleSave() {
  wifi_ssid = server.arg("ssid");
  wifi_pass = server.arg("pass");
  mqtt_ip = server.arg("mqttip");
  mqtt_port = server.arg("mqttport").toInt();
  prefs.begin("config", false);
  prefs.putString("ssid", wifi_ssid);
  prefs.putString("pass", wifi_pass);
  prefs.putString("mqttip", mqtt_ip);
  prefs.putInt("mqttport", mqtt_port);
  prefs.end();
  server.send(200, "text/html; charset=utf-8", "<h3>保存成功！重启设备并连接 WiFi...</h3>");
  delay(1000);
  ESP.restart();
}
void startConfigAP() {
  Serial.println("启动配置 AP...");
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32_Config_AP", "12345678");
  Serial.print("AP IP 地址：");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();
  Serial.println("WebServer 已启动");
}
