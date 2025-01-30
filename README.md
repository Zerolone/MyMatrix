采用PIO编译

如果需要使用mqtt的话，修改common.h文件里面的mqtt设置，下一个版本放到设置页面中

PIO的话需要2步骤
1、Upload Filesystem Image
2、Upload

或者用命令行的话
com5
esptool --chip esp8266 --port COM5 --baud 115200 --before default_reset --after hard_reset write_flash 0x200000 littlefs.bin
esptool --chip esp8266 --port COM5 --baud 115200 --before default_reset --after hard_reset write_flash 0x0      firmware.bin

配网页面支持各种系统

###功能说明
1、显示时间
2、显示室外、室内温度（需要DHT11模块）
3、通过websocket、mqtt发送内容显示
4、网页修改配置


![配网页面](https://raw.githubusercontent.com/Zerolone/ESP8266Tester/main/screenshot/0.jpg)


![设置页面](https://raw.githubusercontent.com/Zerolone/MyMatrix/main/screenshot/01.jpg)


![OTA更新](https://raw.githubusercontent.com/Zerolone/MyMatrix/main/screenshot/02.jpg)


![设置MQTT](https://raw.githubusercontent.com/Zerolone/MyMatrix/main/screenshot/03.jpg)

-------
git add .
git commit -m "2025-01-31 websocket改成EventSource"
git push

