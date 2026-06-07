const int LED=13; //D13可以控制板子上的LED灯

void setup(){
  pinMode(LED, OUTPUT);     //板载LED引脚是D13 设置为输出
}

void loop(){
    digitalWrite(LED,HIGH);          //LED亮
    delay(1000);                     //延时等待1s
    digitalWrite(LED,LOW);           //LED灭
    delay(1000);                     //延时等待1s
}