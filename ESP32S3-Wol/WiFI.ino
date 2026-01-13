const char* ssid = "HKRN";
const char* password = "83315373";
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("正在连接到 ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi连接成功");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
  calculateBroadcastAddress();
}