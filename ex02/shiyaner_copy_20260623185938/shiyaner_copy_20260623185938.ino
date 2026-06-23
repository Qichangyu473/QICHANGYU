const int ledPin = 2;
unsigned long previousTime = 0;
const unsigned long interval = 1000; // 1Hz 周期1000ms
bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis();
  // 判断是否达到切换时间
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    
    if (ledState == HIGH) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }
  }
}