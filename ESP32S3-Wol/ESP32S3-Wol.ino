#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <WakeOnLan.h>
#include <ArduinoJson.h>

// ── 配置区 ─────────────────────────────────────────────────────────
const char* ssid           = "HKRN";
const char* password       = "83315373";

const char* mqtt_server    = "192.168.20.22";
const int   mqtt_port      = 1883;
const char* mqtt_user      = "";
const char* mqtt_pass      = "";

const char* wol_cmd_topic   = "wol/command";   // 客户端接收命令
const char* wol_reply_topic = "wol/reply";     // 客户端回复状态
// ────────────────────────────────────────────────────────────────

WiFiClient espClient;
WiFiUDP udp;
WakeOnLan WOL(udp);
PubSubClient client(espClient);


//================================================================
//  WiFi 初始化
//================================================================
void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


//================================================================
//  MQTT 回调函数：收到 MQTT 消息时触发
//================================================================
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n===== MQTT MESSAGE RECEIVED =====");
  Serial.printf("Topic: %s\n", topic);

  // 将 payload 转成字符串
  String payloadStr;
  payloadStr.reserve(length + 1);
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.printf("Payload: %s\n", payloadStr.c_str());

  // JSON 解析
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payloadStr);
  if (err) {
    String m = "JSON parse error: ";
    m += err.c_str();
    Serial.println(m);
    client.publish(wol_reply_topic, m.c_str());
    return;
  }

  // 提取 mac 字段
  if (!doc.containsKey("mac")) {
    client.publish(wol_reply_topic, "Error: missing 'mac' field");
    Serial.println("Error: missing 'mac' field");
    return;
  }

  String mac = doc["mac"].as<String>();

  // 校验 MAC 格式
  if (mac.length() != 17 ||
      (mac.indexOf(':') == -1 && mac.indexOf('-') == -1)) {

    String m = "Error: invalid MAC format (XX:XX:XX:XX:XX:XX)";
    client.publish(wol_reply_topic, m.c_str());
    Serial.println(m);
    return;
  }

  // 发送 Magic Packet
  Serial.printf("Sending WOL Magic Packet to %s\n", mac.c_str());
  WOL.sendMagicPacket(mac.c_str());

  // 回包
  String success = "WOL sent to ";
  success += mac;
  client.publish(wol_reply_topic, success.c_str());
  Serial.println(success);
}


//================================================================
//  MQTT 重连
//================================================================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");

    String clientId = "ESP32S3-WOL-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");

      // 订阅命令主题
      client.subscribe(wol_cmd_topic);
      Serial.printf("Subscribed to: %s\n", wol_cmd_topic);

    } else {
      Serial.printf("failed, rc=%d → retry in 5s\n", client.state());
      delay(5000);
    }
  }
}


//================================================================
//  主程序入口
//================================================================
void setup() {
  Serial.begin(115200);
  delay(200);

  setup_wifi();

  // 初始化 UDP，用于发送 WOL
  udp.begin(0);

  // 自动计算广播地址
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());

  // 设置 WOL 发送次数（提高成功率）
  WOL.setRepeat(3, 100);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("ESP32-S3 WOL Ready. Waiting for MQTT messages...");
}


//================================================================
//  主循环
//================================================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
