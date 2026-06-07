const int LED=13;       //D13引脚可以控制板载LED
volatile int hs04;      //用于存放测到的距离值

float checkdistance_8_9() {                     //超声波测距函数
  digitalWrite(8, LOW);
  delayMicroseconds(2);
  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);                         //发送两个脉冲
  float distance = pulseIn(9, HIGH) / 58.00;    //计时 直到D9变为高电平后（超声波回波到达模块） 用时间/58 得到距离
  delay(30);                                    //延时等待30ms
  return distance;                              //返回距离值
}

void setup(){
  hs04 = 40;                //变量赋初值
  Serial.begin(9600);       //串口波特率为9600
  pinMode(LED, OUTPUT);     //D13引脚设置为输出 用于控制LED
  pinMode(8, OUTPUT);       //超声波模块的TRGE引脚 用于触发模块发射超声波
  pinMode(9, INPUT);        //超声波模块的ECHO引脚 用于检测超声波回波是否到达
  delay(200);
}

void loop()
{
  hs04 = checkdistance_8_9(); //控制超声波测量一次距离 并把距离数值赋给hs04变量
  Serial.print("距离 ");     
  Serial.pirntln(hs04);       //通过串口输出测到的距离值
  if (hs04 < 15) {            //判断距离值是否小于15
    digitalWrite(LED, HIGH);  //距离值小于15  板载LED灯亮
  } 
  else {
    digitalWrite(LED, LOW);   //距离值大于等于15  板载LED灯灭
  }
}
