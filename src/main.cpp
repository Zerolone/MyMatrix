#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_Neomatrix.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include "LittleFS.h"
#include <ESP8266httpUpdate.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Ticker.h>

/*
Task     真伪线程 Free Heap: 39736 bytes
LeanTask 伪伪线程 Free Heap: 52144 bytes
*/
#include <Scheduler.h>
//#include <Task.h>
#include <LeanTask.h>

//室内温度
#include <DHT.h>

//变量 + 通用函数
#include "common.h"

//自定义lib-----------------------------------
#include "lib/switch.h"  //开关
#include "lib/tmr.h"     //定时器

//函数部分
#include <func.h>
//////////////////////////

//更新所有数据
void updateAll(){
  updateTime();
  updateWeather();
  //updateAQI();
  updateBili();
}

//切换显示效果， 10秒切换
void swithShow(){
  if(isShow){
    matrix->clear();
    
    unsigned int secDate10 = secDate * 10;
    unsigned int secB10    = secDate10 + secB    * 10;
    unsigned int secTemp10 = secB10    + secTemp * 10;
    
    //是否显示判断
    if(intTimer <= secDate10 and chkDate == 0)                       intTimer = secDate10 + 1;
    if(intTimer <= secB10    and intTimer > secDate10 and chkB == 0) intTimer = secB10    + 1;
    if(intTimer <= secTemp10 and intTimer > secB10 and chkTemp == 0) intTimer = secTemp10 + 1;
    
    //时间、日期
    if(intTimer <= secDate10 and intTimer % 10 == 0){
      
      //日期
      displayImage(cald);
      displayImage(cald,1);
      
      if(strDay.toInt() < 10) strDay  = "0" + strDay;

      isClear = false;
      zPrint(strDay, cBlack, 1, 2);

      //时间呼吸效果
      intTimeStep++;
      strTmp = " ";
      if(intTimeStep > 1){
        intTimeStep = 0;
        strTmp = ":";
      }

      strTmp  = strHour + strTmp + strMin;

      //星期
      for(int i = 1; i <= 7;  i++){
        matrix->drawPixel(i*3 + 7, 7, cWhite);
        matrix->drawPixel(i*3 + 8, 7, cWhite);
        
        if(i == strWday.toInt()){
          matrix->drawPixel(i*3 + 7, 7, cRed);
          matrix->drawPixel(i*3 + 8, 7, cRed);
        }
      }

      isClear = false;

      //显示时间
      zPrint(strTmp,  cWhite, 12, 1);matrix->show();

      intCurr = 1;
    }

    //关注数
    //bili intCurr = 2 
    if(intTimer <= secB10 and intTimer > secDate10  and intTimer % 5 == 0){
      strTmp = " " + biliNum;

      intTimeStep++;
      if(intTimeStep > 1){
        intTimeStep = 0;
      }
      displayImage(bilibili[intTimeStep]);
      
      isClear=false;

      //数字下来两格
      zPrint(strTmp, cGreen, 7, 2);

      matrix->show();

      intCurr = 2;
    }

    //天气 intCurr = 3
    if(intTimer <= secTemp10 and intTimer > secB10 ){
      if(intCurr != 3) intWea = 0;
      showWeather();

      intCurr = 3;
    }

    //再开循环
    intTimer++;
    if(intTimer > secTemp10) intTimer = 0;
  }
}

//-------------------------------------
class MemTask : public LeanTask {
 public:
  void loop() {
    Serial.print("Free Heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");

    delay(10000);
  }
} mem_task;

void setup() {
  Serial.begin(SERIAL_SPEED);

  //初始化littleFS
  initFS();

  //---------------------------
  //初始化系统配置
  initConfig();

  willTopic = checkid + "/available"; // 遗嘱主题
  
  //测试用，平时注释掉
  if(isTest == 1){
    /*
    //动画滚动
    for(int i=0;i<32;i++){
      displayImage(coin, i, 0);
      delay(100);
    }
    */

    //zPrint("|小|面|熊|来|打|侍|魂"); matrix->show();

  //zPrint("123456789");
    //X Y Width Height


  /**
  //日期
  int i=1;
  while(true){
    matrix->clear();
    
    matrix->fillRect(0, 0, 9, 2, cRed);
    matrix->fillRect(0, 2, 9, 6, cWhite);

    isClear = false;
    strDay  = String(i);
    if(i<10) strDay  = "0" + strDay;
    zPrint(strDay, cBlack, 1, 2);
    matrix->show();

    i++;
    if(i > 99) i=1;

    delay(500);
  }
  **/

  /**超人部分
   int i = 32;
   int j = 0;
   while(true){
    displayImage(s[j], 0, 0); 
    //isClear = false; 
    //zPrint("|小|面|熊|来|打|侍|魂", cYan, i+8); 
    matrix->show();
    delay(100);
    
    
    //displayImage(s2, i, 0); isClear = false; zPrint("|小|面|熊|来|打|侍|魂", cYan, i+8); matrix->show();delay(50);
    //displayImage(s3, i, 0); isClear = false; zPrint("|小|面|熊|来|打|侍|魂", cYan, i+8); matrix->show();delay(50);
    //displayImage(s4, i, 0); isClear = false; zPrint("|小|面|熊|来|打|侍|魂", cYan, i+8); matrix->show();delay(50);
    //displayImage(s5, i, 0); isClear = false; zPrint("|小|面|熊|来|打|侍|魂", cYan, i+8); matrix->show();delay(50);
    //displayImage(s6, i, 0); isClear = false; zPrint("|小|面|熊|来|打|侍|魂", cYan, i+8); matrix->show();delay(50);
  
    j++;  if(j > 5) j = 0;
    i--;  if(i < -63) i = 32;
   }
   **/

    // 定义两个字符数组
    /*
    char str1[] = "11";
    char str2[] = ":";
    char str3[] = "30";

    // 使用strcat()函数将str2拼接到str1后面
    strcat(str1, str2, str3);
    */



  }else{
    zPrint("|启|动|中|…");  //zPrint("BOOTING");
    matrix->show();
    delay(200);

    //启用webserver
    server.serveStatic("/", LittleFS, "/");

    //搜索网络
    sinfo("Search Wifi");
    zPrint("|搜|索|网|络", cGreen);matrix->show();

    //连上wifi的话
    if(ssid != "0" && initWiFi()) {
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html", false, processor);
      });

      server.begin();
      canRun = 1;

      ///////////
      // 初始化OTA
      
      if(ota == "1") updateOTA();
      if(ota == "2") updateOTAFS();

      if(ota == "1" || ota == "2"){
        ota = 0;
        writeFile(LittleFS, "sys");
        ESP.restart();
      }

      //////
      /*mqtt部分*/
      //测试用
      //client.setServer(mqtt_server, mqtt_port);
      client.setServer(mqtt_server.c_str(), mqtt_port.toInt());
      client.setCallback(callbackx);

      unsigned tmpLoop = 0;
      while (!client.connected()) {
        Serial.print("Setup Connecting to MQTT Server...");
        //if (client.connect(mqttclientId.c_str(), mqtt_user, mqtt_password)) {
        if (client.connect(mqttclientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str(), willTopic.c_str(), willQoS, willRetain, willMsg)) {
          sinfo("connected");
          sinfo("topic=", checkid);
          client.subscribe((checkid + "/#").c_str());

          online();
        } else {
          sinfo("mqtt failed, rc=");
          Serial.print(client.state());
          sinfo(" try again in 3 seconds");
          tmpLoop++;
          
          if (tmpLoop >= 2) {
            sinfo("MQTT Connect Error");
            break;
          }        
          
          delay(3000);
        }
      }
      
      /////////////
      //搜索到
      zPrint("|连|上|无|线", cYan);matrix->show();

      for (int i = 32; i >= -64; i--) {
        //zPrint(ip, cYan, i+8); matrix->show();delay(50);
        zPrint(ip, cYan, i+1, 2); matrix->show();delay(30);
      }

      sinfo("Search Wifi Succ");

      //更新所有后， 加一个0.1秒的定时器
      updateAll();
      timer.attach(.1, swithShow);

      zPrint("|开|始|运|行", cGreen);matrix->show();
      
    } else {
      sinfo("Setting AP (Access Point)");
      zPrint("AP|模|式", cYan);matrix->show();
      
      //ap名
      strTmp = "ZJY-" + String(ESP.getChipId());
      WiFi.softAP(strTmp, "123456");

      delay(3000);
      zPrint("19216841", cRed);matrix->show();

      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/wifimanager.html", "text/html");
      });

      server.serveStatic("/", LittleFS, "/");
      
      //POST
      server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
        // 获取提交表单的内容
        if (request->hasArg("ssid"))    ssid    = request->arg("ssid");
        if (request->hasArg("pass"))    pass    = request->arg("pass");
        if (request->hasArg("ip"))      ip      = request->arg("ip");
        if (request->hasArg("gateway")) gateway = request->arg("gateway");
        if (request->hasArg("checkid")) checkid = request->arg("checkid");
        
        writeFile(LittleFS, "sys");

        request->send(200, "text/html",  "<html><meta http-equiv='Content-Type' content='text/html; charset=UTF-8'/><body><h1>写入完毕，重启中，修改后的IP为：<a href='http://"+ip+"'>"+ip+" 点击直达</a></h1></body></html>");

        //重启标志， 等loop去执行
        restart = true;
      });

      server.begin();
    }

    ws.onEvent(onEventHandle); // 绑定回调函数
    server.addHandler(&ws);    // 将WebSocket添加到服务器中
    server.on("/ws", HTTP_GET, handleRoot); //注册链接"/"与对应回调函数
    server.begin(); //启动服务器
    delay(1000);
    ws.textAll("websocket server On"); // 向所有建立连接的客户端发送数据
      
    Scheduler.start(&mem_task);
    Scheduler.begin();
  }
}

void loop() {
  // 关闭过多的WebSocket连接以节省资源
  ws.cleanupClients();

  //非测试模式则执行循环
  if(isTest == 0){
    //重启守护
    if (restart){
      delay(5000);
      ESP.restart();
    }

    //mqtt部分
    if(canRun==1){
      if (!client.connected()) {
        //ticker.once(1, reconnect);
        reconnect();
      }
      client.loop();
    }     
    
    if(canRun==1){
      //Serial.println("runing version 1.0.0");
      delay(1000);

      intLoop++;
      //时间可以多获取
      if(intLoop % 5 == 0) {
        updateTime();
      }

      /**/
      //其他的10分钟更新一下 , 2是倍数前面delay 500 倍数 就是 2， 1000倍数就是1
      if(intLoop > 10 * 60 * 1){
        updateAll();
        intLoop = 0;
      }
      /**/
    }

    if (Serial.available() > 0) { // 检查串口是否有数据可读
      isShow = false;

      String inputString = ""; // 创建一个空字符串来存储接收到的数据

      while (Serial.available() > 0) {
        char incomingChar = Serial.read(); // 读取串口中的字符
        inputString += incomingChar; // 将字符添加到字符串末尾
        delay(2); // 稍作延迟
      }

      Serial.print("接收到的数据为: ");
      Serial.println(inputString); // 输出接收到的字符串

      /**
      matrix->clear();
      zPrintSingle(inputString, cBlue, 0);
      matrix->show();      
      **/

      for (int i = 32; i >= -64; i--) { 
        zPrint(inputString, cYan, i+8); matrix->show();delay(50);
      }

      delay(500);

      isShow = true;

    }


    if (strRole != "") {
      isShow = false;

      for (int i = 32; i >= -64; i--) { 
        zPrint(strRole, cYan, i+8, 0); matrix->show();delay(50);
      }

      strRole = "";

      delay(500);

      isShow = true;
    }


    //zPrint("10 10", cBlue, 0);matrix->show();delay(500);
  }
}