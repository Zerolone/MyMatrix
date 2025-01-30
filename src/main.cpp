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
// EventSource事件回调函数
void onEventHandle(AsyncEventSourceClient *client){
  Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
  // 向客户端发送hello!
  // events.count()为已经结客户端数，这里用作给客户端的id号
  // 1000表示告诉客户端如果连接断开则再1000毫秒后尝试重新连接
  //client->send("hello!", NULL, events.count(), 1000);

  //发送配置
  client->send("hello!", NULL, events.count(), 1000);

  //内置灯
  strTmp = "0";
  if (digitalRead(PIN_LED) == 1 ) strTmp = "1";
    
  sinfo("strTmp=", strTmp);
  esend("led|"  + strTmp);
  esend("date|" + String(chkDate));
  esend("b|"    + String(chkB));
  esend("temp|" + String(chkTemp));
}
/////////

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

    // 配置 CORS 全局中间件
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET");

    //连上wifi的话
    if(ssid != "0" && initWiFi()) {
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html", false, processor);
      });

      //POST
      server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasArg("c")){
          strTmp    = request->arg("c");
          
          sinfo(strTmp);
          esend("post succ");

          //切开|
          String input = String(strTmp);
          unsigned int partCount = 0;  
          int pos = 0;  
          
          // 计算部分数量  
          for (unsigned int i = 0; i < input.length(); i++) {  
            if (input[i] == '|') {  
              partCount++;  
            }  
          }  
          partCount++; // 因为字符串末尾没有'|'，所以部分数量需要加1  
          
          // 分配字符串数组来存储每个部分  
          String parts[partCount];  
          
          // 切开字符串并存储每个部分  
          for (unsigned int i = 0; i < partCount; i++) {  
            int nextPos = input.indexOf('|', pos);  
            if (nextPos == -1) {  
              nextPos = input.length(); // 如果找不到'|'，则到字符串末尾  
            }  
            parts[i] = input.substring(pos, nextPos);  
            pos = nextPos + 1; // 更新位置以查找下一个部分  
          }

          String strMode = parts[0];
          int isHas   = 0;

          /*
          //ws2812灯珠
          if(strMode == "ws2812"){isHas = 1; ws2812Test(parts[1], parts[2], parts[3], parts[4]);}

          //sr04距离感应
          if(strMode == "sr04")  {isHas = 1; client->text(sr04Test(parts[1], parts[2]));}

          //iHost
          if(strMode == "iHost"){isHas = 1; otahost = parts[1]; writeFile(LittleFS, "sys");}
          */
          
          if(strMode == "otahost"){ isHas = 1; otahost = parts[1];  writeFile(LittleFS, "sys");}

          //ota, otafs, reboot, reset
          if(strMode == "auto")  { isHas = 1; ota = "1";  writeFile(LittleFS, "sys"); ESP.restart();}
          if(strMode == "autofs"){ isHas = 1; ota = "2";  writeFile(LittleFS, "sys"); ESP.restart();}
          if(strMode == "reboot"){ isHas = 1; ESP.restart();}
          if(strMode == "reset") { isHas = 1; ssid = "0"; writeFile(LittleFS, "sys"); ESP.restart();}
          
          //tmr
          if(strMode == "tmr"){
            isHas = 1;
            int tmrSec = parts[2].toInt();
            if(parts[1] == "on"){
              ticker.once(tmrSec, tickOn);
            }else{
              ticker.once(tmrSec, tick);
            }
          }
          
          //led 内置灯
          if(strMode == "led"){
            isHas = 1; switchTest(PIN_LED, parts[1]);
            //events.send("led|" + parts[1]);
          }
          
          //是否显示日期、b站、日期
          if(strMode == "date"){ isHas = 1; chkDate = parts[1].toInt(); writeFile(LittleFS, "soft");}
          if(strMode == "b")   { isHas = 1; chkB    = parts[1].toInt(); writeFile(LittleFS, "soft");}
          if(strMode == "temp"){ isHas = 1; chkTemp = parts[1].toInt(); writeFile(LittleFS, "soft");}
          
          //修改 mqtt
          if(strMode == "mqtt"){ 
            isHas = 1; 
            
            mqtt_server = parts[1];
            mqtt_port   = parts[2];
            mqtt_user   = parts[3];
            mqtt_pass   = parts[4];
            checkid     = parts[5];
            
            writeFile(LittleFS, "mqtt");
          }
          
          //soft，各种配置
          if(strMode == "soft"){ 
            isHas = 1; 
            intBright = parts[1].toInt();
            mType     = parts[2].toInt();
            biliID    = parts[3].toInt();
            wUserKey  = parts[4];
            wLocation = parts[5];
            
            secDate   = parts[6].toInt();
            secB      = parts[7].toInt();
            secTemp   = parts[8].toInt();
            
            matrix->setBrightness(intBright);
            
            writeFile(LittleFS, "soft");
          }

          //str，即时滚动字符串
          if(strMode == "str"){ 
            isHas = 1;

            strRole = input;
            strRole.replace(strMode+"|", "");

            sinfo("strRole=", strRole);

          }

          //
          strTmp = "功能未命中";
          if(isHas == 1) strTmp = "功能命中";
          esend(strTmp);
          //结束

          // 发送响应
          request->send(200, "text/plain", strTmp);  // 返回文本响应


        }
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

    events.onConnect(onEventHandle); // 绑定当有客户端连接时的回调函数
    server.addHandler(&events);      // 将EventSource添加到服务器中

    server.begin(); //启动服务器
    delay(1000);
      
    Scheduler.start(&mem_task);
    Scheduler.begin();
  }
}

void loop() {

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