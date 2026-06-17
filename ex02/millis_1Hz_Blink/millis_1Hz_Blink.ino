#define LED_PIN 2
unsigned long lastTime = 0;
const unsigned long interval = 500;
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  unsigned long now = millis();
  if(now - lastTime >= interval){
    lastTime = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.print("系统运行毫秒：");
    Serial.println(now);
  }
}
