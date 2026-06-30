#include <WiFi.h>
#include <WebServer.h>

// WiFi配置
const char* ssid     = "X100s";
const char* password = "12345679";

// 触摸引脚定义
const int touchChannel = T0;
WebServer server(80);

// 主页仪表盘HTML（AJAX自动轮询数据）
String dashboardHtml = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<title>触摸传感器实时仪表盘</title>
<style>
*{margin:0;padding:0;box-sizing:border-box;}
body{background:#0f172a;color:#fff;text-align:center;padding-top:120px;font-family:Arial;}
h1{font-size:36px;margin-bottom:50px;color:#93c5fd;}
.data-box{width:320px;height:160px;margin:0 auto;background:#1e293b;border-radius:20px;display:flex;align-items:center;justify-content:center;box-shadow:0 0 30px #2563eb80;}
#touchVal{font-size:72px;font-weight:bold;color:#38bdf8;}
.tip{margin-top:30px;font-size:18px;color:#94a3b8;}
</style>
</head>
<body>
    <h1>电容触摸传感器实时数值</h1>
    <div class="data-box">
        <span id="touchVal">--</span>
    </div>
    <p class="tip">手指靠近D4引脚，数值变小；松开数值恢复</p>

<script>
// 定时轮询接口获取传感器数据
function fetchSensorData(){
    fetch("/data")
    .then(res=>res.text())
    .then(val=>{
        document.getElementById("touchVal").innerText = val;
    })
    .catch(err=>console.log("数据获取失败"));
}
// 每100毫秒刷新一次数据，实时流畅
setInterval(fetchSensorData, 100);
// 页面加载立刻读取一次
fetchSensorData();
</script>
</body>
</html>
)HTML";

// 路由1：访问主页，返回仪表盘网页
void handleDashboard(){
  server.send(200, "text/html", dashboardHtml);
}

// 路由2：数据接口，返回当前触摸原始值（AJAX拉取专用）
void handleGetData(){
  int rawTouch = touchRead(touchChannel);
  server.send(200, "text/plain", String(rawTouch));
}

void setup() {
  Serial.begin(115200);

  // WiFi连接
  Serial.print("正在连接WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.print("仪表盘访问地址：");
  Serial.println(WiFi.localIP());

  // 注册网页与数据接口路由
  server.on("/", handleDashboard);
  server.on("/data", handleGetData);
  server.begin();
  Serial.println("Web仪表盘服务启动完成");
}

void loop() {
  server.handleClient(); // 持续处理网页、数据请求
}