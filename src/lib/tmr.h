//定时
#include <Ticker.h>
Ticker ticker;

void tick() {
  // 定时器回调函数
  Serial.println("Ticker tick");

  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(0, HIGH);
  digitalWrite(2, LOW);
}

void tickOn() {
  // 定时器回调函数
  Serial.println("Ticker tick On");

  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(0, LOW);
  digitalWrite(2, HIGH);
}

