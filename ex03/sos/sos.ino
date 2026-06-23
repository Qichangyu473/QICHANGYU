// 定义LED引脚，ESP32通常板载LED连接在GPIO 2
const int ledPin = 2; 

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // S: 短闪3次
  for(int i=0; i<3; i++) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON");
    delay(200);
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");
    delay(200);
  }
  delay(500); // 字母间隔
  // O: 长闪3次
  for(int i=0; i<3; i++) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON");
    delay(600);
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");
    delay(200);
  }
  delay(500);
  // S: 短闪3次
  for(int i=0; i<3; i++) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON");
    delay(200);
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");
    delay(200);
  }
  delay(2000); // 单词间隔
}