const char* mqtt_topic_sub = "wol/command";
unsigned long MQTTReconnectTime = 0;
void setup_mqtt() {
  mqttClient.setServer(mqtt_ip.c_str(), mqtt_port);
  mqttClient.setCallback(mqttCallback);
  WOL.setRepeat(1, 100);
}
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("收到主题消息 [%s]: \n", topic);
  String message;
  for (int i = 0; i < length; i++) { message += (char)payload[i]; }
  String mac = JsonParse(message);
  if (mac.length() != 0) { sendWOLPacket(mac.c_str()); }
}
void MQTTHeart() {
  if (!mqttClient.connected() && millis() - MQTTReconnectTime > 5000) {
    Serial.print("尝试连接MQTT服务器...");
    String clientId = "ESP32-WOL-Gateway-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("连接成功!");
      mqttClient.subscribe(mqtt_topic_sub);
      Serial.print("已订阅主题: ");
      Serial.println(mqtt_topic_sub);
    } else {
      Serial.print("失败， rc=");
      Serial.print(mqttClient.state());
      Serial.println(" 5秒后重试...");
      MQTTReconnectTime = millis();
    }
  }
  mqttClient.loop();
}