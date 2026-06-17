#define LED_PIN 2
const unsigned long DOT_ON  = 200;
const unsigned long DOT_GAP = 200;
const unsigned long DASH_ON = 600;
const unsigned long SOS_END_GAP = 2000;

unsigned long lastTime = 0;
bool ledLightOn = false;
int workStage = 1;
int flashNum = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  unsigned long currentMs = millis();
  unsigned long keepTime;

  if(ledLightOn){
    keepTime = (workStage == 1 || workStage == 3) ? DOT_ON : DASH_ON;
  }else{
    keepTime = DOT_GAP;
  }

  if(currentMs - lastTime < keepTime) return;

  lastTime = currentMs;
  ledLightOn = !ledLightOn;
  digitalWrite(LED_PIN, ledLightOn);

  if(!ledLightOn){
    flashNum++;
    if(workStage == 1 && flashNum >= 3){
      workStage = 2;
      flashNum = 0;
    }
    else if(workStage == 2 && flashNum >= 3){
      workStage = 3;
      flashNum = 0;
    }
    else if(workStage == 3 && flashNum >= 3){
      workStage = 0;
      flashNum = 0;
      digitalWrite(LED_PIN, LOW);
      delay(SOS_END_GAP);
      workStage = 1;
    }
  }
}