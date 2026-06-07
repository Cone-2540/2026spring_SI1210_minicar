const int IR=12; //红外模块信号输出接在D12
const int LED=13; //D13可以控制板子上的LED灯

void setup(){
  pinMode(IR, INPUT);       //检测信号需要设置为输入
  pinMode(LED, OUTPUT);     //板载LED引脚是D13 设置为输出
}

void loop(){
  if (digitalRead(IR)==HIGH) {      //判断是否有信号
    digitalWrite(LED,HIGH);         //LED亮
    delay(100);                     //延时等待100ms
    digitalWrite(LED,LOW);          //LED灭
    delay(100);                     //延时等待100ms
  } 
  else                              //如果没有信号
  {
    digitalWrite(LED,HIGH);         //LED亮
    delay(300);                     //延时等待300ms
    digitalWrite(LED,LOW);          //LED灭
    delay(300);                     //延时等待300ms
  }
}