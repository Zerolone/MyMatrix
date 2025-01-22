//调试显示
void sinfo(String str, String strInfo=""){
  if (SHOWINFO == 1) {
    if (strInfo == ""){
      Serial.println(str);
    }else{
      Serial.println(str + "" + strInfo);
    }
  }
  //  sprintf(output, "Sensor value: %d, Temperature: %.2f", sensorValue, temperature);
}

//mqtt---------------------------------------
void getStatus() {
  String msg = "{'io0':"+String(digitalRead(LED_BUILTIN))+"}";

  if (client.publish((checkid+"/status").c_str(), msg.c_str())) {
    sinfo("Message sent successfully");
  } else {
    sinfo("Message failed to send");
  }
}

void sendMsg(String topic, String msg) {
  if (client.publish((checkid + "/" + topic).c_str(), msg.c_str())) {
    sinfo("Message sent successfully");
  } else {
    sinfo("Message failed to send");
  }
}

//无斜杠
void sendMsg2(String topic, String msg) {
  if (client.publish((checkid + topic).c_str(), msg.c_str())) {
    sinfo("Message sent successfully");
  } else {
    sinfo("Message failed to send");
  }
}

//--homeassistant 用---------------------------------
void online() {
  sendMsg2("/available/led" ,   "online");
  sendMsg2("/available/switch" ,"online");
  sendMsg2("/status/led" ,   "");
  sendMsg2("/status/switch" ,"");
}

//调试提交
void zpost(String postdata){
    WiFiClient client;
    HTTPClient http;
    const char* url = "http://47.107.103.89:59000/3.php";
    
    // 发送POST请求
    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // postdata = "继电器 " + checkid + ":" + postdata;
    
    int httpResponseCode = http.POST(postdata.c_str());
  
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
}



//同上
void zinfo(String str, String strInfo=""){
  sinfo(str, strInfo);
}

// 显示到网页的变量
String processor(const String& var) {
  //Serial.println(var);

  if(var == "wUserKey")  return wUserKey;
  if(var == "wLocation") return wLocation;
  if(var == "biliID")    return String(biliID);
  if(var == "mType")     return String(mType);
  if(var == "intBright") return String(intBright);
  if(var == "VERSION")   return VERSION;
  if(var == "BOARD")     return BOARD;
  if(var == "OTAHOST")   return otahost;
  if(var == "IP")        return ip.c_str();
  
  if(var == "secDate")   return String(secDate);
  if(var == "secB")      return String(secB);
  if(var == "secTemp")   return String(secTemp);
  
  if(var == "mqtt_server") return mqtt_server;
  if(var == "mqtt_port") return mqtt_port;
  if(var == "mqtt_user") return mqtt_user;
  if(var == "mqtt_pass") return mqtt_pass;
  if(var == "checkid") return checkid;

  return String();
}

//------------------
//初始化littlefs
void initFS() {
  if (!LittleFS.begin()) {
    sinfo("LittleFS Mount Err");
  }else{
    sinfo("LittleFS mounted Succ");
  }
}

//写文件内容
void writeFile(fs::FS &fs, String type){
  StaticJsonDocument<200> c;
  
  String path = sysPath;
  if(type=="sys"){
    c["ssid"]    = ssid;
    c["pass"]    = pass;
    c["ip"]      = ip;
    c["gateway"] = gateway;
    c["ota"]     = ota;
    c["otahost"] = otahost;
  }

  if(type=="soft") {
    path   = softPath;
    
    c["intBright"] = intBright;
    c["mType"]     = mType;
    c["biliID"]    = biliID;
    c["wUserKey"]  = wUserKey;
    c["wLocation"] = wLocation;
    c["chkDate"]   = chkDate;
    c["chkB"]      = chkB;
    c["chkTemp"]   = chkTemp;
    c["secDate"]   = secDate;
    c["secB"]      = secB;
    c["secTemp"]   = secTemp;
    //c[""]     = ;
  }
  
  if(type=="mqtt") {
    path   = mqttPath;
    
    c["mqtt_server"] = mqtt_server;
    c["mqtt_port"]   = mqtt_port;
    c["mqtt_user"]   = mqtt_user;
    c["mqtt_pass"]   = mqtt_pass;
    c["checkid"]     = checkid;
    //c[""]     = ;
  }  
  
  /**/
  // 将JSON对象写入文件
  File file = fs.open(path, "w");
  if (file) {
    serializeJson(c, file);
    file.close();
    sinfo("- file written");
  } else {
    sinfo("- frite failed");
  }
}

//读取文件内容
String readFile(fs::FS &fs, const char * path){
  sinfo("读取文件：", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    sinfo("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

//初始化针脚
void initConfig(){
  // 从 JSON 字符串中读取数据
  StaticJsonDocument<200> c; // 分配内存大小
  
  //读取 系统配置
  DeserializationError error = deserializeJson(c, readFile(LittleFS, sysPath));
  // 检查解析是否成功
  if (error) {
    sinfo("sys JSON parsing failed: ", error.c_str());
  }
  
  ssid    = c["ssid"].as<String>();
  pass    = c["pass"].as<String>();
  ip      = c["ip"].as<String>();
  gateway = c["gateway"].as<String>();
  ota     = c["ota"].as<String>();
  otahost = c["otahost"].as<String>();
  
  sinfo("ssid=",    ssid);
  sinfo("pass=",    pass);
  sinfo("ip=",      ip);
  sinfo("gateway=", gateway);
  sinfo("ota=",     ota);
  sinfo("otahost=", otahost);
  
  //---------------------------
  //读取 软件配置
  strTmp  = readFile(LittleFS, softPath);
  
  error = deserializeJson(c, strTmp);

  // 检查解析是否成功
  if (error) {
    sinfo("soft JSON parsing failed: ", error.c_str());
  }
  
  intBright = c["intBright"].as<int>();
  mType     = c["mType"].as<int>();
  biliID    = c["biliID"].as<String>();
  wUserKey  = c["wUserKey"].as<String>();
  wLocation = c["wLocation"].as<String>();
              
  chkDate   = c["chkDate"].as<int>();
  chkB      = c["chkB"].as<int>();
  chkTemp   = c["chkTemp"].as<int>();
              
  secDate   = c["secDate"].as<int>();
  secB      = c["secB"].as<int>();
  secTemp   = c["secTemp"].as<int>();
  
  sinfo("intBright=", String(intBright));
  sinfo("mType=",     String(mType));
  sinfo("biliID=",    biliID);
  sinfo("wUserKey=",  wUserKey);
  sinfo("wLocation=", wLocation);
  
  sinfo("chkDate=", String(chkDate));
  sinfo("chkB=",    String(chkB));
  sinfo("chkTemp=", String(chkTemp));
  sinfo("secDate=", String(secDate));
  sinfo("secB=",    String(secB));
  sinfo("secTemp=", String(secTemp));
  
  //默认类型
  if(mType < 1) mType = 1;
  
  //默认亮度
  if(intBright < 1) intBright = 1;
  
  //初始化
  if(mType == 1) matrix = new Adafruit_NeoMatrix(8, 8, 4, 1, PIN_LIGHT, NEO_MATRIX_TOP     + NEO_MATRIX_LEFT  + NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE, NEO_GRB + NEO_KHZ800);
  if(mType == 2) matrix = new Adafruit_NeoMatrix(32, 8,      PIN_LIGHT, NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE, NEO_GRB + NEO_KHZ800);
  
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(intBright);
  
  //---------------------------
  //读取 mqtt配置
  strTmp  = readFile(LittleFS, mqttPath);
  
  error = deserializeJson(c, strTmp);
  
  // 检查解析是否成功
  if (error) {
    sinfo("mqtt JSON parsing failed: ", error.c_str());
  }
  
  mqtt_server  = c["mqtt_server"].as<String>();
  mqtt_port    = c["mqtt_port"].as<String>();
  mqtt_user    = c["mqtt_user"].as<String>();
  mqtt_pass    = c["mqtt_pass"].as<String>();
  checkid      = c["checkid"].as<String>();
  
  mqttclientId = "z124_" + WiFi.macAddress();
  willTopic    = checkid + "/available"; // 遗嘱主题
  
  sinfo("mqtt_server=",  mqtt_server);
  sinfo("mqtt_port=",    mqtt_port);
  sinfo("mqtt_user=",    mqtt_user);
  sinfo("mqtt_pass=",    mqtt_pass);
  sinfo("checkid=",      checkid);
  sinfo("mqttclientId=", mqttclientId);
  sinfo("willTopic=",    willTopic);
}

// 初始化WIFI
bool initWiFi() {
  if(ssid=="" || ip==""){
    sinfo("SSID 或 IP 地址未定义");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());

  if (!WiFi.config(localIP, localGateway, subnet)){
    sinfo("STA 配置错误");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());

  sinfo("连接WiFi中...");

  int intLoop = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    sinfo("连接WiFi中...");

    intLoop+=1;
    if(intLoop > 20){
      sinfo("连接WiFi失败");
      return false;
    }
  }
  
  //Serial.println(WiFi.localIP());
  return true;
}

int findValueByKey(String key) {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (keys[i] == key) {

      //Serial.print('i=');Serial.println(i);
      return values[i];
    }
  }

  return key.toInt(); // 如果未找到匹配的键，说明是数字，直接返回
}

//单个字符打印
void zPrintSingle(String str="A", uint16_t color=cRed, int intX=0, int intY=0) {
  // 使用字符串作为索引查找数组元素
  int intKey = 0;
  if(str!="0"){
    intKey    = findValueByKey(str);
  }
  //sinfo("intKey=", String(intKey));

  int intPosAdd = 3; //0-9宽度为3

  if(intKey >= 10) intPosAdd=1; //
  if(intKey >= 14) intPosAdd=2; //
  if(intKey >= 15) intPosAdd=3; //
  if(intKey >= 42) intPosAdd=4; //
  if(intKey >= 60) intPosAdd=5; //
  if(intKey >= 64) intPosAdd=7; //中文已经有空行
  

  //为数字"1"单独写一个 取消了都是3 2024-4-10 10:06:54
  //if(intKey == 1)  intPosAdd=2;

  //加个空列
  intPosAdd+=1;

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (charEn[intKey][y] & (1 << (7 - x))) {
        matrix->drawPixel(x + intPos + intX, y + intY, color);
      }
    }
    //Serial.println();
  }

  //起始点
  intPos+=intPosAdd;

  //matrix->show();
}

//切开字符串
void zPrint(String strs="ABC", uint16_t color=cWhite, int intX=0, int intY=0) {
  if(isClear) matrix->clear();
  String str = "";
  for (unsigned int i = 0; i < strs.length(); i++) {    
    str = strs.charAt(i);
    if(str=="|"){
      str=strs.substring(i+1, i+4);
      i+=3;
    }

    //sinfo("str=", str);

    zPrintSingle(str, color, intX, intY);
  }

  //重置一下
  intPos=0;
  isClear=true;
}

//////////////////////////////
//OTA 在线更新部分
void update_started() {
  sinfo("CALLBACK:  HTTP update process started");
}

void update_finished() {
  sinfo("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  sprintf(charTmp, "CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
  sinfo(charTmp);
}

void update_error(int err) {
  sprintf(charTmp, "CALLBACK:  HTTP update fatal error code %d\n", err);
  sinfo(charTmp);
}


void updateOTA() {
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
    ESPhttpUpdate.rebootOnUpdate(false); // remove automatic update
      
    t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, otahost + OTAURL);

    ota = "0";
    writeFile(LittleFS, "sys");

    switch (ret) {
      case HTTP_UPDATE_FAILED: Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); break;
      case HTTP_UPDATE_NO_UPDATES: Serial.println("HTTP_UPDATE_NO_UPDATES"); break;
      case HTTP_UPDATE_OK: Serial.println("HTTP_UPDATE_OK"); break;
    }  
}

void updateOTAFS() {
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  // Add optional callback notifiers
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  ESPhttpUpdate.rebootOnUpdate(false); // remove automatic update
  
  t_httpUpdate_return ret = ESPhttpUpdate.updateFS(espClient, otahost + OTAFSURL);

  ota = "0";
  writeFile(LittleFS, "sys");
  writeFile(LittleFS, "soft");

  switch (ret) {
    case HTTP_UPDATE_FAILED: Serial.printf("FS HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); break;
    case HTTP_UPDATE_NO_UPDATES: Serial.println("FS HTTP_UPDATE_NO_UPDATES"); break;
    case HTTP_UPDATE_OK: Serial.println("FS HTTP_UPDATE_OK"); break;
  }  
}

//更新时间
void updateTime(){
  String url = "http://47.107.103.89:59000/time.php";

  HTTPClient http;  
  http.begin(espClient, url);
  String res = "";
  int httpCode = http.GET();
  if(httpCode > 0) {  
    res = http.getString();  
    //sinfo(res);
  }  
  http.end();

  if (httpCode > 0)  {
    //DynamicJsonDocument doc(1024);
    JsonDocument doc;

    deserializeJson(doc, res);
    //biliSubscriberCount = doc["data"]["follower"].as<uint32>();
    intSec   = String(doc["second"]);
    strMin   = String(doc["minute"]);
    strHour  = String(doc["hour"]);
    strWday  = String(doc["dayOfWeek"]);
    intYear  = String(doc["year"]);
    strMonth = String(doc["month"]);
    strDay   = String(doc["day"]);
  }
}

//将图标显示出来 8*8 的
void displayImage(const uint16_t *image, int start_x=0, int start_y=0) {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int pixel_index = y * 8 + x;
      uint16_t color = pgm_read_word_near(image + pixel_index);
      matrix->drawPixel(x + start_x, y + start_y, color);
    }
  }
}

//显示天气图标
void showWeather(){
  hasWeather = false;

  zPrint(wTemp + "|℃", cYan,   10, 2);
  
  matrix->drawPixel(31, 2, cWhite);

  isClear = false;
  zPrint(String(wTemp2.toInt()), cWhite, 23, 2);

  //wTemp = w100;

  //测试用
  // wIcon = "3001";
  // zinfo("wIcon=", wIcon);

  //晴天
  //if(wIcon == "100"){ displayImage(w100[intTmp]); if(intTmp > sizeof(w100) / sizeof(w100[0]) - 1) intTmp = 0; }
  
  //数组长度-2， 否则会乱
  //晴
  if(wIcon == "100" or wIcon == "150"){ 
    intWeaMax = sizeof(w100) / sizeof(w100[0]);
    displayImage(w100[intWea]);
    hasWeather = true;
  }

  //多云-少云
  if(wIcon == "101" or wIcon == "102" or wIcon == "103" or wIcon == "151" or wIcon == "152" or wIcon == "153"){
    intWeaMax = sizeof(w101) / sizeof(w101[0]);
    displayImage(w101[intWea]);
    hasWeather = true;
  }
  
  //阴
  if(wIcon == "104"){
    intWeaMax = sizeof(w104) / sizeof(w104[0]);
    displayImage(w104[intWea]);
    hasWeather = true;
  }
  
  //雨
  if(wIcon.toInt() >= 300 and wIcon.toInt() <= 399){
    intWeaMax = sizeof(w300) / sizeof(w300[0]);
    displayImage(w300[intWea]);
    hasWeather = true;
  }

  intWea++;
  if(intWea > intWeaMax - 1) intWea = 0; 

  //不存在天气的话， 用图标
  if(!hasWeather){
    displayImage(w000);
  }

  matrix->show();
}

//-----------------
//更新bilibili订阅
void updateBili(){
  String url = "http://47.107.103.89:59000/bilibili.php?vmid=" + biliID;

  HTTPClient http;  
  http.begin(espClient, url);
  String res = "";
  int httpCode = http.GET();
  if(httpCode > 0) {  
    res = http.getString();  
    sinfo(res);
  }  
  http.end();

  biliNum = 0;
  if (httpCode > 0){
    JsonDocument doc;
    deserializeJson(doc, res);
    biliNum = String(doc["data"]["follower"]);

    sinfo("biliSubscriberCount=", biliNum);
  }
}

//更新天气
void updateWeather(){
  String url = "http://47.107.103.89:59000/weather.php?location=" + wLocation + "&key=" + wUserKey;

  HTTPClient http;  
  http.begin(espClient, url);
  String res = "";
  int httpCode = http.GET();
  if(httpCode > 0) {  
    res = http.getString();  
    sinfo(res);
    zpost(res);
  }  
  http.end();

  wTemp = "|无";
  wHumi = "|无";
  if (httpCode > 0){
    JsonDocument doc;
    deserializeJson(doc, res);

    wTemp = String(doc["now"]["temp"]);     // 实况温度
    wHumi = String(doc["now"]["humidity"]); // 实况相对湿度百分比数值
    wIcon = String(doc["now"]["icon"]);     // 天气图标编号

    sinfo("======Weahter Now Info======");
    sinfo(url);
    sinfo("温度：", wTemp);
    sinfo("图标：", wIcon);
  }

  DHT dht(PIN_DHT11, DHT11);
  dht.begin();

  //float humidity    = dht.readHumidity();
  wTemp2 = dht.readTemperature();
}


//mqtt---------------------------------------
void callbackx(char* topic, byte* message, unsigned int length) {
  String strMessage = "";
  String strStat = "";
  for (unsigned int i=0;i<length;i++) {
    strMessage += (char)message[i];
  }

  sinfo("Topic:",   topic);
  sinfo("Message:", strMessage);

  ws.textAll(topic);
  ws.textAll(strMessage);

  //判断topic
  //获取LED状态
  if(String(topic) == checkid + "/" + topicstatus + "/led" && !bolStatusLed){
    String strTmp = "ON";
    if(digitalRead(LED_BUILTIN) == HIGH){
      strTmp = "OFF";
    }

    //发送状态
    bolStatusLed = true;
    sendMsg(topicstatus+"/led", strTmp);
    ws.textAll("LED_BUILTIN="+strTmp);
  }

  //
  String topicsub = "set";
  if(String(topic) == checkid + "/" + topicsub + "/led"){
    sinfo("hit:set topic led");

    pinMode(LED_BUILTIN, OUTPUT);

    //设置后， bolStatus
    bolStatusLed = false;

    if(strMessage == "ON"){
      digitalWrite(LED_BUILTIN, LOW);
    }else{
      digitalWrite(LED_BUILTIN, HIGH);
    }

    //发送状态
    sendMsg(topicstatus+"/led", strMessage);
    bolStatusLed = true;

    ws.textAll("status|io2|" + strMessage);
  }

  //是否在线
  topicsub = "available";
  if(String(topic) == checkid + "/" + topicsub && !bolOnlineLed){
    sendMsg(topicsub, "online");

    bolOnlineLed = true;
  }

  topicsub = "str";
  if(String(topic) == checkid + "/" + topicsub){
    strRole = strMessage;

    sinfo("strRole=", strRole);
  }

}

//mqtt重连
void reconnect() {
  while (!client.connected()) {
    if (client.connect(mqttclientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str(), willTopic.c_str(), willQoS, willRetain, willMsg)) {
      sinfo("reconnected");
      sinfo("topic=", checkid);
      client.subscribe((checkid + "/#").c_str());

      bolOnlineLed    = false;
      bolOnlineSwitch = false;
      bolStatusLed    = false;
      bolStatusSwitch = false;

      reMQTT = false;
    } else {
      sinfo("re failed, rc=");
      Serial.print(client.state());
      sinfo(" try again in 3 seconds");
      delay(3000);
    }
  }
}


//websocket部分----------------
//回调函数
void handleRoot(AsyncWebServerRequest *request){
  sinfo("User requested.");
  request->send(200, "html", "<p>Hello World!</p>");
}

// WebSocket事件回调函数
void onEventHandle(AsyncWebSocket *server, AsyncWebSocketClient *wsclient, AwsEventType type, void *arg, uint8_t *data, size_t len){
  String strMessage = "";
  String strTmp     = "";

  // 有客户端建立连接
  if (type == WS_EVT_CONNECT) {
    Serial.printf("ws[%s][%u] connect\n", server->url(), wsclient->id());

    //client->printf("Hello Client %u !", client->id()); // 向客户端发送数据

    //pinMode(LED, INPUT);
    strTmp = "0";
    if (digitalRead(PIN_LED) == 1 ) strTmp = "1";
    
    sinfo("strTmp=", strTmp);
    
    //Serial.println(digitalRead(LED));

    //返回led的状态
    wsclient->text("led|"  + strTmp);
    wsclient->text("date|" + String(chkDate));
    wsclient->text("b|"    + String(chkB));
    wsclient->text("temp|" + String(chkTemp));
    
    sinfo("strMessage=", strMessage);

    wsclient->ping();                                    // 向客户端发送ping
  }
  else if (type == WS_EVT_DISCONNECT) // 有客户端断开连接
  {
    Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), wsclient->id());
  }
  else if (type == WS_EVT_ERROR) // 发生错误
  {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), wsclient->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG) // 收到客户端对服务器发出的ping进行应答（pong消息）
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), wsclient->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA) // 收到来自客户端的数据
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), wsclient->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);
    data[len] = 0;
    Serial.printf("%s\n", (char *)data);

    char strc[info->index + len]; // 额外一个位置用于存储字符串结束符 '\0'
    // 将 uint8_t 数组转换为字符串
    for (unsigned int i = 0; i < info->index + len; i++) {
      strc[i] = (char)data[i];
    }

    //client->printf((char *)data); // 向客户端发送数据

    String input = String(strc);
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
      ws.textAll("led|" + parts[1]);
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
    if(isHas == 1){
      wsclient->printf("功能命中");
    }else{
      wsclient->printf("功能未命中");
    } 
    wsclient->ping(); 
  }
}