#include <IRremote.h> //库版本 3.7.1
volatile int time;    //闪烁时间间隔
IRrecv irrecv_A3(A3); //设定红外接收引脚为A3

void setup(){
  time = 200;             //赋初值200ms
  Serial.begin(9600);     //初始化串口 波特率9600
  irrecv_A3.enableIRIn(); //使能红外接收引脚
  pinMode(13, OUTPUT);    //板载LED引脚是D13 设置为输出
}

void loop(){
  if (irrecv_A3.decode()) {       //如果成功解码
    struct IRData *pIrData = &irrecv_A3.decodedIRData;  //准备读取解码信息
    long ir_item = pIrData->decodedRawData;             //将解码信息赋值给ir_item
    Serial.println(ir_item, HEX);                       //串口发送收到的编码
    irrecv_A3.resume();                                 //复位 便于下次接收
    if (ir_item == 0xBA45FF00) {                        //如果是按键1
      time = 100;                                       //LED闪烁间隔100ms
    } else if (ir_item == 0xB946FF00) {                 //如果是按键2
      time = 200;                                       //LED闪烁间隔200ms
    } else if (ir_item == 0xB847FF00) {                 //如果是按键3
      time = 300;                                       //LED闪烁间隔300ms
    }

  } else {

  }
  digitalWrite(13,HIGH);  //LED亮
  delay(time);            //延时等待
  digitalWrite(13,LOW);   //LED灭
  delay(time);            //延时等待
}