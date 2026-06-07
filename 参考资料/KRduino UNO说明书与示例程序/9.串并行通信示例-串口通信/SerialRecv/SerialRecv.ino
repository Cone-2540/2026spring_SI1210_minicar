String recv;    //声明字符串变量 用于缓存接收到的数据

void serialEvent() {                  //串口事件服务程序
  recv = recv + Serial.readString();  //将新的字符串追加到recv字符串后面
}

void setup(){
  Serial.begin(9600);                 //串口波特率为9600
  pinMode(13, OUTPUT);                //D13为板载LED控制引脚
}

void loop(){
  if (recv.indexOf("open") >= 0) {    //判断recv字符串（缓存区）内是否包含open这个字符串
    digitalWrite(13,HIGH);            //包含 LED灯亮
    recv = "";                        //清空缓存
  }
  if (recv.indexOf("close") >= 0) {   //判断recv字符串（缓存区）内是否包含close这个字符串
    digitalWrite(13,LOW);             //包含 LED灯灭
    recv = "";                        //清空缓存
  }
  
  if (recv.length() > 100) {          //判断缓存区是否过大
    recv = "";                        //过大说明接收到了许多用不到的信息 清空缓存
  }
}