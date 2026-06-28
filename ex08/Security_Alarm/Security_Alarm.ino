#include <WiFi.h>
#include <WebServer.h>

#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400

const char* ap_ssid = "ESP32-LAB-040";
const char* ap_pass = "12345678";

bool isArm = false;
bool alarmFlag = false;

// 触摸防抖变量
bool lastTouch = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 150;

// 非阻塞LED闪烁，不卡住循环
unsigned long blinkTimer = 0;
const unsigned long blinkInterval = 80;
bool ledState = false;

WebServer server(80);

String makePage() {
  String armText = isArm ? "已布防" : "已撤防";
  String armColor = isArm ? "#ef4444" : "#22c55e";
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 物联网安防报警器 ex08</title>
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
    }
    .btn-group {
      display: flex;
      gap: 20px;
      justify-content: center;
      flex-wrap: wrap;
    }
    button {
      width: 140px;
      padding: 14px 0;
      border: none;
      border-radius: 12px;
      font-size: 17px;
      cursor: pointer;
      transition: all 0.25s ease;
    }
    .arm-btn {
      background: #ef4444;
      color: white;
    }
    .disarm-btn {
      background: #22c55e;
      color: white;
    }
    button:hover {
      transform: translateY(-3px);
      box-shadow: 0 6px 18px rgba(0,0,0,0.15);
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
    <h1>物联网安防报警主机</h1>
    <div class="divider"></div>

    <div class="status-box">
      <div class="status-title">系统当前状态</div>
      <div class="status-text" style="color:)"rawliteral" + armColor + R"rawliteral(;">)rawliteral" + armText + R"rawliteral(</div>
    </div>

    <div class="btn-group">
      <a href="/arm"><button class="arm-btn">布防(Arm)</button></a>
      <a href="/disarm"><button class="disarm-btn">撤防(Disarm)</button></a>
    </div>

    <footer>ESP32 AP热点 | ex08 物联网安防报警器实验</footer>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleArm() {
  isArm = true;
  Serial.println("【操作】已布防，触摸引脚会触发报警");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDisarm() {
  isArm = false;
  alarmFlag = false;
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  Serial.println("【操作】已撤防，报警关闭");
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("==== ex08 安防报警器 ====");
  Serial.print("热点：");
  Serial.println(ap_ssid);
  Serial.print("访问地址：");
  Serial.println(WiFi.softAPIP());
  Serial.println("触摸引脚GPIO4，阈值400");
  Serial.println("=========================");

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long nowTime = millis();

  int touchVal = touchRead(TOUCH_PIN);
  bool nowTouch = (touchVal < THRESHOLD);

  // 串口实时打印数值，方便观察
  Serial.print("触摸值：");
  Serial.print(touchVal);
  Serial.print(" | 触碰：");
  Serial.print(nowTouch ? "是" : "否");
  Serial.print(" | 布防：");
  Serial.print(isArm ? "是" : "否");
  Serial.print(" | 报警锁定：");
  Serial.println(alarmFlag ? "开启" : "关闭");

  // 核心修复：持续触摸期间也能触发报警，不限制松手触发
  if (isArm && !alarmFlag) {
    // 只要当前触摸，且上一帧没触摸，防抖通过就触发报警
    if (nowTouch && (nowTime - lastDebounceTime > debounceDelay)) {
      lastDebounceTime = nowTime;
      alarmFlag = true;
      Serial.println("======================");
      Serial.println("检测到触碰，启动报警！");
      Serial.println("======================");
    }
  }
  lastTouch = nowTouch;

  // 非阻塞闪烁，不阻塞网页和触摸检测
  if (alarmFlag) {
    if (nowTime - blinkTimer >= blinkInterval) {
      blinkTimer = nowTime;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(20);
}
