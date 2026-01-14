unsigned long lastBlink = 0;
bool ledState = false;
const int blinkInterval = 300;

void checkConfigMode() {
  pinMode(CONFIG_PIN, INPUT_PULLUP);
  delay(10);
  if (digitalRead(CONFIG_PIN) == LOW) {
    Serial.println("检测到 GPIO10 短接到 GND，进入 AP 配置模式...");
    configMode = true;
    startConfigAP();
  } else {
    Serial.println("正常启动 WiFi");
    configMode = false;
    setup_wifi();
    setup_mqtt();
  }
}
void LedHeart() {
  unsigned long now = millis();
  if (now - lastBlink > blinkInterval) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  }
}