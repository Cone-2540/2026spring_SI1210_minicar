const int RES=A0; //滑杆模块接在A0
int value=0;      //存放数值

void setup(){
  Serial.begin(9600);   //串口波特率9600
}

void loop(){
  value=analogRead(RES);        //读取滑杆模块的AD数值（0~1023）
  Serial.print("value=");       //串口输出英文字符 不换行
  Serial.println(value);        //串口输出value的值  并换行       
  delay(100);
}