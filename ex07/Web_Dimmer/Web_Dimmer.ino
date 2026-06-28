#include <WiFi.h>
#include <WebServer.h>

const char* ap_ssid = "ESP32-LAB-040";
const char* ap_pass = "12345678";

const int LED_PIN = 2;
int brightness = 0;

WebServer server(80);

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Web无极调光 ex07</title>
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
    .status-box {
      background: #fff7e0;
      border-radius: 14px;
      padding: 24px;
      margin-bottom: 35px;
      border: 1px solid #fef3c7;
    }
    .status-title {
      font-size: 16px;
      color: #92400e;
      margin-bottom: 10px;
    }
    .status-text {
      font-size: 28px;
      font-weight: bold;
      color: #e6a23c;
    }
    .slider-area {
      margin: 40px 0;
    }
    /* 滑动条轨道底色 浅黄 */
    input[type="range"] {
      width: 100%;
      height: 12px;
      border-radius: 10px;
      background: #fef3c7;
      outline: none;
      -webkit-appearance: none;
    }
    /* 滑块圆球 亮黄色 */
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 26px;
      height: 26px;
      border-radius: 50%;
      background: #f59e0b;
      cursor: pointer;
      box-shadow: 0 2px 10px rgba(245, 158, 11, 0.35);
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
    <h1>LED 网页无极调光器</h1>
    <div class="divider"></div>

    <div class="status-box">
      <div class="status-title">当前亮度数值</div>
      <div class="status-text" id="num">0</div>
    </div>

    <div class="slider-area">
      <input type="range" min="0" max="255" id="lightSlider">
    </div>

    <footer>ESP32 AP热点 | ex07 网页PWM调光实验</footer>
  </div>

<script>
const slider = document.getElementById("lightSlider");
const showNum = document.getElementById("num");

slider.addEventListener("input", function(){
  let val = this.value;
  showNum.innerText = val;
  fetch("/setBright?val=" + val);
});
</script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleSetBright() {
  if (server.hasArg("val")) {
    int val = server.arg("val").toInt();
    if (val >= 0 && val <= 255) {
      brightness = val;
      analogWrite(LED_PIN, brightness);
    }
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("热点开启成功");
  Serial.print("访问地址：");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/setBright", handleSetBright);
  server.begin();
}

void loop() {
  server.handleClient();
}