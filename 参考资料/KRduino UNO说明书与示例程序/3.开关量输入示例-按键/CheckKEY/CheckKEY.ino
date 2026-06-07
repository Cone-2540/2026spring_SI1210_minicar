const int KEY=12; //按键模块接在D12
const int LED=13; //D13可以控制板子上的LED灯

void setup(){
  pinMode(KEY, INPUT);      //检测按键需要设置为输入
  pinMode(LED, OUTPUT);     //板载LED引脚是D13 设置为输出
}

void loop(){
  if (digitalRead(KEY)==HIGH) {     //如果按键按下
    digitalWrite(LED,HIGH);          //LED亮
  } 
  else{                             //如果按键没有按下 
    digitalWrite(LED,LOW);           //LED灭
  }
}