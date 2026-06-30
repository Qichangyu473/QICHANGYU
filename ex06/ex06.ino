// 两路LED引脚定义
const int ledA = 2;
const int ledB = 4;

// 亮度变量与步进
int brightness = 0;
int step = 2;   // 步进越大，交替闪烁速度越快

void setup() {
  Serial.begin(115200);
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
}

void loop() {
  // LEDA亮度：0 → 255 递增
  analogWrite(ledA, brightness);
  // LEDB亮度：255 → 0 递减（反相关系）
  analogWrite(ledB, 255 - brightness);

  // 更新亮度
  brightness = brightness + step;

  // 到达边界后反向变化
  if (brightness >= 255 || brightness <= 0) {
    step = -step;
  }

  delay(15); // 控制渐变顺滑度，数值越大交替越慢
}