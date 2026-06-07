const int RES=A0; //滑杆模块接在A0
const int LED=13; //D13可以控制板子上的LED灯
int time=300;     //延时等待的时间

void setup(){
  pinMode(LED, OUTPUT);     //板载LED引脚是D13 设置为输出
}

void loop(){
  time=analogRead(RES);         //读取滑杆模块的AD数值（0~1023）
  time=map(time,0,1023,1,500);  //将0~1023同比映射为1~500（类似缩放）
  digitalWrite(13,HIGH);        //LED亮
  delay(time);                  //延时等待
  digitalWrite(13,LOW);         //LED灭
  delay(time);                  //延时等待
}