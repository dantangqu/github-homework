#include <WiFi.h>
#include <WebServer.h>

#define TOUCH_PIN 4
#define THRESHOLD 400

const char* ap_ssid = "ESP32-LAB-040";
const char* ap_pass = "12345678";

WebServer server(80);

// 返回触摸原始数值接口
void handleGetTouch() {
  int touchVal = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(touchVal));
}

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 实时触摸仪表盘 ex09</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: "Microsoft YaHei", system-ui, sans-serif;
    }
    body {
      background: linear-gradient(145deg, #fffbeb, #fff9e0);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .container {
      width: 100%;
      max-width: 520px;
      background: #ffffff;
      border-radius: 20px;
      box-shadow: 0 8px 30px rgba(230, 162, 60, 0.18);
      padding: 50px 35px;
      text-align: center;
    }
    h1 {
      font-size: 30px;
      color: #d48806;
      margin-bottom: 12px;
      letter-spacing: 1px;
    }
    .divider {
      width: 120px;
      height: 3px;
      background: linear-gradient(90deg, #fbbf24, #f59e0b);
      margin: 0 auto 35px;
      border-radius: 3px;
    }
    .data-box {
      background: #fff7e0;
      border-radius: 14px;
      padding: 40px 24px;
      margin-bottom: 35px;
      border: 1px solid #fef3c7;
    }
    .data-title {
      font-size: 18px;
      color: #92400e;
      margin-bottom: 16px;
    }
    .data-num {
      font-size: 60px;
      font-weight: bold;
      color: #e6a23c;
    }
    footer {
      margin-top: 45px;
      font-size: 13px;
      color: #a16207;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>触摸传感器实时仪表盘</h1>
    <div class="divider"></div>

    <div class="data-box">
      <div class="data-title">当前触摸模拟量数值</div>
      <div class="data-num" id="touchData">0</div>
    </div>

    <footer>ESP32 AP热点 | ex09 实时传感器监控实验</footer>
  </div>

<script>
// 定时AJAX轮询获取触摸值
const dataText = document.getElementById("touchData");
function updateData() {
  fetch("/getTouch")
    .then(res => res.text())
    .then(val => {
      dataText.innerText = val;
    });
}
// 每100ms刷新一次数据
setInterval(updateData, 100);
</script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("==== ex09 实时传感器仪表盘 ====");
  Serial.print("热点名称：");
  Serial.println(ap_ssid);
  Serial.print("访问地址：");
  Serial.println(WiFi.softAPIP());
  Serial.println("触摸引脚GPIO4，阈值400");
  Serial.println("===============================");

  // 注册网页与数据接口
  server.on("/", handleRoot);
  server.on("/getTouch", handleGetTouch);
  server.begin();
}

void loop() {
  server.handleClient();
  // 串口同步打印触摸数值方便调试
  Serial.print("触摸原始值：");
  Serial.println(touchRead(TOUCH_PIN));
  delay(100);
}