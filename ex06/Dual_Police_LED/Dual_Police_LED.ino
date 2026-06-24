HAO// ex06 双通道反相PWM渐变双闪
#define LED_RED 16    // 红灯引脚
#define LED_YELLOW 18 // 黄灯引脚

// PWM参数
const int freq = 5000;
const int resolution = 8; // 亮度范围0~255

int redDuty = 0;
int step = 1;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 两路独立PWM绑定引脚
  ledcAttach(LED_RED, freq, resolution);
  ledcAttach(LED_YELLOW, freq, resolution);

  Serial.println("==== ex06 Dual Reverse PWM Light ====");
  Serial.println("Red LED & Yellow LED anti-phase brightness");
  Serial.println("======================================");
}

void loop() {
  // 红灯亮度增减
  redDuty += step;
  // 边界反转变化方向
  if (redDuty >= 255) {
    redDuty = 255;
    step = -1;
  } else if (redDuty <= 0) {
    redDuty = 0;
    step = 1;
  }

  // 红灯输出当前亮度
  ledcWrite(LED_RED, redDuty);
  // 黄灯反相亮度：255 - 红灯亮度
  ledcWrite(LED_YELLOW, 255 - redDuty);

  // 串口打印两路亮度，直观观察反相关系
  Serial.print("Red Bright: ");
  Serial.print(redDuty);
  Serial.print(" | Yellow Bright: ");
  Serial.println(255 - redDuty);

  delay(8);
}