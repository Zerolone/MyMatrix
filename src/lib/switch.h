void switchTest(int pin, String on) {
  pinMode(pin, OUTPUT);

  if (on == "0"){
    digitalWrite(pin, LOW);
  }else{
    digitalWrite(pin, HIGH);
  }
}

/*
void switchTest2(String pin, String on, AsyncWebSocket ws) {
  int thePin = io[pin.toInt()];
  
  pinMode(thePin, OUTPUT);

  sinfo("thePin=", String(thePin));

  if (on == "0"){
    digitalWrite(thePin, LOW);
  }else{
    digitalWrite(thePin, HIGH);
  }

  Serial.println(digitalRead(thePin));

  //发送websocket消息
  //ws.textAll(String("stat|") + String(digitalRead(0)) + "|" + String(digitalRead(2)));
}
*/