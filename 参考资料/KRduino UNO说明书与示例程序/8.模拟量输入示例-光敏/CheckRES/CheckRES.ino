const int RES=A0; //光敏模块接在A0
const int LED=13; //D13可以控制板子上的LED灯
int value=0;      //存放AD数值

void setup(){
  pinMode(LED, OUTPUT);     //板载LED引脚是D13 设置为输出
}

void loop(){
  value=analogRead(RES);    //读取滑杆模块的AD数值（0~1023）
  if(value>500){            //判断数值是否大于500  
    digitalWrite(LED,HIGH);  //条件成立（value大于500）LED亮
  }else{
    digitalWrite(LED,LOW);   //条件不成立（value小于等于500）LED灭
  }
}