String JsonParse(const String& message) {
  Serial.println(message);
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("JSON解析失败: ");
    Serial.println(error.c_str());
    return "";
  }
  const char* macAddress = doc["mac"];
  if (macAddress == nullptr || strlen(macAddress) < 17) {
    Serial.println("错误：JSON中未找到合法的mac字段");
    return ""; 
  }
  return String(macAddress); 
}
