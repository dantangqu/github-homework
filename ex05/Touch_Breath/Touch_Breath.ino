#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400

// PWM参数（新版ledcAttach无报错）
const int freq = 5000;
const int resolution = 8;

// 呼吸灯档位
int brightness = 0;
int dir = 1;
int gearStep[4] = {0, 1, 3, 6}; // 1慢 2中 3快
int gear = 1;

// 触摸防抖、状态记录
bool lastTouch = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 150;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  ledcAttach(LED_PIN, freq, resolution);

  Serial.println("==== ex05 触摸调速呼吸灯（松手切换档位）====");
  Serial.print("初始档位：");
  Serial.print(gear);
  Serial.print(" 步进速度：");
  Serial.println(gearStep[gear]);
  Serial.println("============================================");
}

void loop() {
  int touchVal = touchRead(TOUCH_PIN);
  bool nowTouch = (touchVal < THRESHOLD);

  // 串口打印调试信息
  Serial.print("触摸值：");
  Serial.print(touchVal);
  Serial.print(" | 当前档位：");
  Serial.println(gear);

  unsigned long nowTime = millis();
  // 核心逻辑：松手瞬间（当前未触摸，上一帧是触摸）才切换档位
  if (!nowTouch && lastTouch && (nowTime - lastDebounceTime > debounceDelay))
  {
    lastDebounceTime = nowTime;
    gear++;
    if (gear > 3) gear = 1;

    Serial.println("\n========================");
    Serial.print("松手触发，档位切换！新档位：");
    Serial.print(gear);
    Serial.print(" 速度步进：");
    Serial.println(gearStep[gear]);
    Serial.println("========================\n");
  }
  lastTouch = nowTouch;

  // 呼吸渐变
  brightness += gearStep[gear] * dir;
  if (brightness >= 255)
  {
    brightness = 255;
    dir = -1;
  }
  else if (brightness <= 0)
  {
    brightness = 0;
    dir = 1;
  }
  ledcWrite(LED_PIN, brightness);

  delay(10);
}