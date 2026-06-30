// 引脚定义
const int touchPin = T0;    // T0 = GPIO4 = 板载丝印D4，触摸专用常量
const int ledPin = 2;       // GPIO2 = 板载丝印D2，直接用数字GPIO
const int touchThresh = 40; // 触摸判定阈值
const unsigned long debounceDelay = 50; // 软件防抖时间

// 布尔状态变量（题目要求）
bool ledState = LOW;         // LED自锁保存状态
bool lastTouchStatus = false;// 上一次触摸状态，用于边缘检测
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState); // 初始熄灭
}

void loop() {
  // 读取触摸原始数值
  int touchRaw = touchRead(touchPin);
  bool currentTouch = touchRaw < touchThresh;

  unsigned long now = millis();
  // 防抖判断：间隔足够久才处理触摸信号
  if (now - lastDebounceTime > debounceDelay) {
    // 边缘检测：上一次未触摸，当前刚触摸（触发瞬间）
    if (currentTouch == true && lastTouchStatus == false) {
      ledState = !ledState; // 翻转LED自锁状态
      digitalWrite(ledPin, ledState);
      Serial.print("触发自锁，LED：");
      Serial.println(ledState ? "点亮" : "熄灭");
    }
    // 更新历史触摸状态
    lastTouchStatus = currentTouch;
    lastDebounceTime = now;
  }

  // 串口调试输出触摸值
  Serial.print("触摸采样值：");
  Serial.println(touchRaw);
  delay(20);
}