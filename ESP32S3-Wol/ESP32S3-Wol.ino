#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <WakeOnLan.h>
#include <ArduinoJson.h>  // 新增：引入JSON解析库

// ====== 网络和MQTT配置 ======
const char* ssid = "HKRN";
const char* password = "83315373";
const char* mqtt_server = "192.168.20.22"; // 例如 "192.168.1.100"
const int mqtt_port = 1883; // 默认端口
const char* mqtt_topic_sub = "wol/command"; // 订阅的主题，用于接收指令

// ====== 全局对象声明 ======
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP UDP;
WakeOnLan WOL(UDP);

// ====== 连接Wi-Fi ======
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
    // 计算广播地址，用于发送WOL包
    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
}

// ====== MQTT消息回调函数 ======
// 当从订阅的主题收到消息时，此函数被自动调用
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("收到主题消息 [");
    Serial.print(topic);
    Serial.print("]: ");

    // 将消息载荷（payload）转换为字符串
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    // ====== 新增：JSON解析逻辑 ======
    // 创建JSON文档（静态分配，适合小数据）
    StaticJsonDocument<128> doc;
    
    // 解析JSON字符串
    DeserializationError error = deserializeJson(doc, message);
    
    // 检查解析是否成功
    if (error) {
        Serial.print("JSON解析失败: ");
        Serial.println(error.c_str());
        return;
    }
    
    // 提取MAC地址字段
    const char* macAddress = doc["mac"];
    
    // 验证MAC地址是否存在且长度合法
    if (macAddress == nullptr || strlen(macAddress) < 17) {
        Serial.println("错误：JSON中未找到合法的mac字段");
        return;
    }

    // 调用函数发送WOL魔术包
    Serial.print("尝试唤醒MAC: ");
    Serial.println(macAddress);
    sendWOLPacket(macAddress);
}

// ====== 发送WOL魔术包函数 ======
// 优化：直接接收const char*，避免String转换
void sendWOLPacket(const char* macAddress) {
    // 发送魔术包
    WOL.sendMagicPacket(macAddress);
    Serial.println("魔术包已发送！");
}

// ====== 连接/重连MQTT服务器 ======
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("尝试连接MQTT服务器...");
        // 使用唯一的客户端ID连接，这里用芯片ID的一部分
        String clientId = "ESP32-WOL-Gateway-" + String((uint32_t)ESP.getEfuseMac(), HEX);
        // 如果连接成功
        if (mqttClient.connect(clientId.c_str())) {
            Serial.println("连接成功!");
            // 连接成功后，订阅指定主题
            mqttClient.subscribe(mqtt_topic_sub);
            Serial.print("已订阅主题: ");
            Serial.println(mqtt_topic_sub);
        } else {
            // 连接失败，打印原因并等待重试
            Serial.print("失败， rc=");
            Serial.print(mqttClient.state());
            Serial.println(" 5秒后重试...");
            delay(5000);
        }
    }
}

// ====== Arduino初始化函数 ======
void setup() {
    Serial.begin(115200);
    setup_wifi();
    // 设置MQTT服务器和回调函数
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqttCallback);
    // 初始化WOL
    WOL.setRepeat(1, 100); // 可选：发送3次，每次间隔100ms，提高唤醒成功率
}

// ====== Arduino主循环 ======
void loop() {
    // 维持MQTT连接，并处理 incoming 消息
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }
    mqttClient.loop(); // 这个调用至关重要，用于保持连接和接收消息
    // 你的其他主循环代码可以放在这里
}