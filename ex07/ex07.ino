#include <WiFi.h>
#include <WebServer.h>

// ========== WiFi配置（已按你提供信息填好） ==========
const char* ssid     = "X100s";
const char* password = "12345679";

// ========== 引脚与服务器定义 ==========
const int ledPin = 2;
WebServer server(80); // 开启80端口网页服务

// 存储滑动条传递的亮度值 0~255
int ledBright = 0;

// ========== 网页HTML页面（含滑动条+JS） ==========
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>ESP32 网页无极调光</title>
    <style>
        body {text-align:center; font-size:24px; margin-top:100px;}
        .slider {width:80%; height:30px; margin:40px 0;}
        #showVal {color:#ff4400; font-weight:bold; font-size:32px;}
    </style>
</head>
<body>
    <h2>LED亮度滑动调节</h2>
    <input type="range" class="slider" id="brightSlider" min="0" max="255" value="0">
    <p>当前亮度值：<span id="showVal">0</span></p>

    <script>
        // 获取滑动条与显示文本
        const slider = document.getElementById("brightSlider");
        const valText = document.getElementById("showVal");

        // 监听滑动条拖动事件
        slider.addEventListener("input", function(){
            let val = this.value;
            valText.innerText = val;
            // fetch发送GET请求到ESP32地址，携带亮度参数
            fetch(`/set?bright=${val}`);
        });
    </script>
</body>
</html>
)HTML";

// ========== 网页路由处理函数 ==========
// 主页路由：返回带滑动条的HTML页面
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 调光路由：解析URL参数bright，更新LED亮度
void handleSetBright() {
  if(server.hasArg("bright")){
    // 提取URL里的亮度数字，转为整型
    ledBright = server.arg("bright").toInt();
    // 限制范围0~255，防止越界
    ledBright = constrain(ledBright, 0, 255);
    analogWrite(ledPin, ledBright);
  }
  // 刷新页面，保持滑动条状态
  server.sendHeader("Location", "/");
  server.send(302);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, ledBright);

  // 1. 连接WiFi
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.print("ESP32网页访问地址：");
  Serial.println(WiFi.localIP()); // 打印访问IP，浏览器输入此地址

  // 2. 注册网页路由
  server.on("/", handleRoot);
  server.on("/set", handleSetBright);

  // 3. 启动网页服务器
  server.begin();
  Serial.println("Web调光服务器运行中");
}

void loop() {
  server.handleClient(); // 持续监听浏览器请求
}