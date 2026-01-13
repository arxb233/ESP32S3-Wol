const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="zh">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 配置</title>
<style>
  body { font-family: Arial; background:#f1f1f1; margin:0; padding:0; }
  .container {
    max-width:360px; background:#fff; padding:20px; margin:40px auto;
    border-radius:12px; box-shadow:0 0 10px rgba(0,0,0,0.1);
  }
  h2 { text-align:center; }
  label { margin-top:10px; display:block; }
  input, select {
    width:100%; padding:10px; margin-top:6px;
    border:1px solid #ccc; border-radius:6px;
  }
  input[type=submit] {
    margin-top:20px; background:#4CAF50; color:white;
    border:none; border-radius:6px; padding:10px;
  }
</style>
</head>
<body>
  <div class="container">
    <h2>ESP32 配置</h2>
    <form action='/save' method='POST'>
      <label>WiFi SSID：</label>
      <input name='ssid' type='text' placeholder='路由器名称'>

      <label>WiFi 密码：</label>
      <input name='pass' type='password' placeholder='路由器密码'>

      <label>MQTT 服务器 IP：</label>
      <input name='mqttip' type='text' placeholder='192.168.1.10'>

      <label>MQTT 端口：</label>
      <input name='mqttport' type='number' value='1883'>

      <input type='submit' value='保存配置'>
    </form>
  </div>
</body>
</html>
)=====";
