#include <WiFi.h>
#include <PubSubClient.h>
#include <WakeOnLan.h>  // 需要安装这个库

// ---------- 配置区 ----------
const char* ssid = "HKRM";
const char* password = "88315373";

const char* mqtt_server = "192.168.20.22";  // 如 192.168.1.100 或云端broker
const int mqtt_port = 1883;
const char* mqtt_user = "";     // 如果没有认证，可留空 ""
const char* mqtt_password = "";   // 如果没有，可留空 ""

const char* wol_topic = "wol/command";    // 订阅的MQTT主题

const char* target_mac = "8C-82-B9-95-33-86";  // 要唤醒电脑的MAC地址（用-或:分隔均可）

// 可选：指定目标IP（如果知道固定IP，可直接发送到该IP，否则用广播）
const char* target_ip = "192.168.10.8";  // 如果留空 ""，则使用广播
// ---------------------------

WiFiClient espClient;
PubSubClient client(espClient);
WakeOnLan WOL(espClient);  // 使用同一个WiFiClient的UDP

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Payload: ");
  Serial.println(message);

  // 判断是否是WOL命令（可自定义，如 "wake" 或直接是MAC）
  if (message == "wake" || message.indexOf(target_mac) >= 0) {
    Serial.println("Received WOL command, sending Magic Packet...");

    WOL.sendMagicPacket(target_mac);  // 默认端口9，广播发送

    // 如果你想指定端口或重复发送多次（更可靠）
    // WOL.setRepeat(3, 100);  // 重复3次，间隔100ms（可在setup中设置）
    // WOL.sendMagicPacket(target_mac, 9);

    // 如果知道目标电脑IP（避免广播），可以用：
    // if (String(target_ip) != "") {
    //   WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // 可选优化广播地址
    //   WOL.sendMagicPacket(target_mac, target_ip, 9);  // 直接发到指定IP
    // }

    Serial.println("Magic Packet sent!");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32S3Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(wol_topic);
      Serial.print("Subscribed to: ");
      Serial.println(wol_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // 可选：设置WOL重复发送（提高成功率）
  WOL.setRepeat(3, 100);  // 重复3次，间隔100ms
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}