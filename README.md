采用PIO编译

如果需要使用mqtt的话，修改common.h文件里面的mqtt设置，下一个版本放到设置页面中

PIO的话需要2步骤
1、Upload Filesystem Image
2、Upload

或者用命令行的话
com5
esptool --chip esp8266 --port COM5 --baud 115200 --before default_reset --after hard_reset write_flash 0x200000 littlefs.bin
esptool --chip esp8266 --port COM5 --baud 115200 --before default_reset --after hard_reset write_flash 0x0      firmware.bin


