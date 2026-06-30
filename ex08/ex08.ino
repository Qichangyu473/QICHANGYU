#include <WiFi.h>
#include <WebServer.h>

// WiFi信息
const char* ssid     = "X100s";
const char* password = "12345679";

// 引脚定义
const int touchCh = T0;
const int alarmLed = 2;
const int touchThresh = 40;
const unsigned long debounceDelay = 50;

// 网页服务端口
WebServer server(80);

// 全局安防状态变量（题目要求）
bool systemArm = false;    // false=撤防  true=布防
bool isAlarm = false;      // 是否触发报警
bool lastTouch = false;
unsigned long lastDebounceTime = 0;

// 网页HTML界面（布防/撤防双按钮）
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<title>物联网安防报警器</title>
<style>
body{text-align:center;margin-top:80px;font-size:26px;}
button{width:220px;height:70px;font-size:24px;margin:20px;border:none;border-radius:8px;cursor:pointer;}
#armBtn{background:#c00;color:white;}
#disBtn{background:#080;color:white;}
#status{font-size:30px;margin:30px 0;}
</style>
</head>
<body>
<h2>ESP32安防主机控制端</h2>
<div id="status">当前状态：撤防</div>
<button id="armBtn" onclick="sendCmd('arm')">布防 ARM</button>
<br>
<button id="disBtn" onclick="sendCmd('disarm')">撤防 DISARM</button>

<script>
// 发送布防/撤防指令
function sendCmd(cmd){
    fetch("/ctrl?state="+cmd).then(()=>location.reload());
}
</script>
</body>
</html>
)HTML";

// 主页路由
void handleRoot(){
  String page = htmlPage;
  // 根据系统状态修改页面文字
  if(systemArm){
    page.replace("当前状态：撤防","当前状态：已布防");
  }else{
    page.replace("当前状态：已布防","当前状态：撤防");
  }
  server.send(200,"text/html",page);
}

// 安防控制路由：处理布防/撤防指令
void handleCtrl(){
  if(server.hasArg("state")){
    String cmd = server.arg("state");
    if(cmd == "arm"){
      systemArm = true;
      isAlarm = false; // 布防时清除历史报警
    }else if(cmd == "disarm"){
      systemArm = false;
      isAlarm = false; // 撤防关闭报警
      digitalWrite(alarmLed,LOW);
    }
  }
  server.sendHeader("Location","/");
  server.send(302);
}

void setup() {
  Serial.begin(115200);
  pinMode(alarmLed,OUTPUT);
  digitalWrite(alarmLed,LOW);

  // 连接WiFi
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.print("安防主机访问地址：");
  Serial.println(WiFi.localIP());

  // 注册网页路由
  server.on("/",handleRoot);
  server.on("/ctrl",handleCtrl);
  server.begin();
}

void loop() {
  server.handleClient(); // 持续处理网页请求

  unsigned long now = millis();
  int touchRaw = touchRead(touchCh);
  bool currentTouch = touchRaw < touchThresh;

  // 触摸防抖
  if(now - lastDebounceTime > debounceDelay){
    // 边缘检测：刚触摸瞬间
    if(currentTouch && !lastTouch){
      // 仅系统处于布防状态，才触发锁定报警
      if(systemArm){
        isAlarm = true;
      }
    }
    lastTouch = currentTouch;
    lastDebounceTime = now;
  }

  // 报警逻辑：高频闪烁，锁定不解除
  if(isAlarm){
    digitalWrite(alarmLed,HIGH);
    delay(80);
    digitalWrite(alarmLed,LOW);
    delay(80);
  }else{
    digitalWrite(alarmLed,LOW);
  }
}