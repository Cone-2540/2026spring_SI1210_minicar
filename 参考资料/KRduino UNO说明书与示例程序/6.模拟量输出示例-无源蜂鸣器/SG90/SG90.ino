void setup(){
  pinMode(10, OUTPUT);  //无源蜂鸣器接到D10引脚 设置D10引脚为输出
}

void NewTone(int pin,int hz,int time) //音符发声函数 pin是频率输出引脚 hz是频率 time是发声时间
{
  tone(pin,hz);   //发声
  delay(time);    //持续时间
  noTone(pin);    //停止发声
}

void loop(){
  NewTone(10,196,375);
  NewTone(10,196,375);
  NewTone(10,220,750);
  NewTone(10,196,750);
  NewTone(10,262,750);
  NewTone(10,247,1000);
  delay(1000);
}