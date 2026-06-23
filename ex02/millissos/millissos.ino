// 定义LED引脚，ESP32板载LED GPIO2
const int ledPin = 2;

// 时间参数
const unsigned long dotOn = 200;    // 短亮时长 S
const unsigned long dashOn = 600;   // 长亮时长 O
const unsigned long intraGap = 200; // 同一个字母内灯灭间隔
const unsigned long letterGap = 500;// 字母之间间隔
const unsigned long wordGap = 2000; // 一组SOS结束后的长停顿

// 状态机变量
unsigned long prevTime = 0;
bool ledState = LOW;
// SOS流程状态 0=空闲等待下一组SOS 1=发S短闪 2=间隔S-O 3=发O长闪 4=间隔O-S 5=发S短闪
byte state = 0;
// 计数：当前字母闪了几次
byte flashCnt = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long now = millis();
  unsigned long delta = now - prevTime;

  switch(state) {
    case 0:
      // 空闲，等待wordGap后启动下一轮SOS
      if(delta >= wordGap) {
        prevTime = now;
        state = 1;
        flashCnt = 0;
        Serial.println("=== 开始发送一组SOS ===");
      }
      break;

    case 1:
      // 发送S：3次短闪 dot
      if(ledState == LOW) {
        // 关灯状态，等待intraGap后开灯
        if(delta >= intraGap) {
          ledState = HIGH;
          digitalWrite(ledPin, HIGH);
          Serial.println("LED ON 短闪");
          prevTime = now;
        }
      } else {
        // 开灯状态，等待dotOn后关灯，计数+1
        if(delta >= dotOn) {
          ledState = LOW;
          digitalWrite(ledPin, LOW);
          Serial.println("LED OFF");
          prevTime = now;
          flashCnt++;
          // S闪完3次，进入S-O字母间隔
          if(flashCnt >= 3) {
            state = 2;
            prevTime = now;
          }
        }
      }
      break;

    case 2:
      // S 和 O 之间字母间隔
      if(delta >= letterGap) {
        state = 3;
        flashCnt = 0;
        prevTime = now;
      }
      break;

    case 3:
      // 发送O：3次长闪 dash
      if(ledState == LOW) {
        if(delta >= intraGap) {
          ledState = HIGH;
          digitalWrite(ledPin, HIGH);
          Serial.println("LED ON 长闪");
          prevTime = now;
        }
      } else {
        if(delta >= dashOn) {
          ledState = LOW;
          digitalWrite(ledPin, LOW);
          Serial.println("LED OFF");
          prevTime = now;
          flashCnt++;
          // O闪完3次，进入O-S字母间隔
          if(flashCnt >= 3) {
            state = 4;
            prevTime = now;
          }
        }
      }
      break;

    case 4:
      // O 和 S 之间字母间隔
      if(delta >= letterGap) {
        state = 5;
        flashCnt = 0;
        prevTime = now;
      }
      break;

    case 5:
      // 再次发送S：3次短闪
      if(ledState == LOW) {
        if(delta >= intraGap) {
          ledState = HIGH;
          digitalWrite(ledPin, HIGH);
          Serial.println("LED ON 短闪");
          prevTime = now;
        }
      } else {
        if(delta >= dotOn) {
          ledState = LOW;
          digitalWrite(ledPin, LOW);
          Serial.println("LED OFF");
          prevTime = now;
          flashCnt++;
          // 全部SOS发送完成，回到空闲等待长停顿
          if(flashCnt >= 3) {
            state = 0;
            prevTime = now;
            Serial.println("=== 本组SOS发送完毕，等待2秒再发送 ===");
          }
        }
      }
      break;
  }
}