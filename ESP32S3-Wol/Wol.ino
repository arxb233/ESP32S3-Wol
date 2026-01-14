void sendWOLPacket(const char* macAddress) {
  Serial.printf("尝试唤醒MAC: %s\n", macAddress);
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
  WOL.sendMagicPacket(macAddress);
  Serial.println("魔术包已发送！");
}
