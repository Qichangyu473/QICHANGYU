// 触摸通道与PWM引脚定义（规避D4/D2未定义报错）
const int touchCh = T0;      // T0对应硬件D4(GPIO4)触摸引脚
const int ledPwmPin = 2;     // GPIO2对应板载D2板载LED
const int touchThresh = 40;  // 触摸判定阈值
const unsigned long debounceDelay = 50;

// 触摸防抖、边缘检测变量
bool lastTouch = false;
unsigned long lastDebounceTime = 0;

// 呼吸档位：1/2/3三档循环
int speedLevel = 1;
// 不同档位亮度步进，step越大呼吸越快
int stepSlow = 1;    // 1档：最慢
int stepMid = 3;     // 2档：中等速度
int stepFast = 7;    // 3档：最快急促呼吸

// PWM呼吸基础变量
int brightness = 0;
int fadeDir = 1;     // 1=变亮，-1=变暗

void setup() {
  Serial.begin(115200);
  pinMode(ledPwmPin, OUTPUT);
}

void loop() {
  // ========== 触摸检测+档位切换逻辑 ==========
  int touchVal = touchRead(touchCh);
  bool currentTouch = touchVal < touchThresh;
  unsigned long now = millis();

  // 软件防抖过滤手抖误触发
  if (now - lastDebounceTime > debounceDelay) {
    // 触摸上升沿：仅刚碰到一瞬间切换档位
    if (currentTouch && !lastTouch) {
      speedLevel++;
      // 三档循环 1→2→3→1
      if (speedLevel > 3) {
        speedLevel = 1;
      }
      Serial.print("当前呼吸档位：");
      Serial.println(speedLevel);
    }
    lastTouch = currentTouch;
    lastDebounceTime = now;
  }

  // ========== 根据档位选择亮度步进 ==========
  int currentStep;
  switch (speedLevel) {
    case 1: currentStep = stepSlow; break;
    case 2: currentStep = stepMid; break;
    case 3: currentStep = stepFast; break;
    default: currentStep = stepSlow;
  }

  // ========== PWM呼吸亮度更新 ==========
  brightness += fadeDir * currentStep;

  // 边界反转明暗方向（亮度区间0~255）
  if (brightness <= 0) {
    brightness = 0;
    fadeDir = 1;
  } else if (brightness >= 255) {
    brightness = 255;
    fadeDir = -1;
  }

  analogWrite(ledPwmPin, brightness);
  delay(10); // 基础刷新延时，保证呼吸顺滑
}