#include <Servo.h>      //引用舵机库
Servo servo_11;         //声明Servo对象为servo_11

void setup(){
  servo_11.attach(11);  //初始化舵机控制口为D11
}

void loop(){
  servo_11.write(0);    //控制舵机转动到0度
  delay(1000);          //延时等待1S
  servo_11.write(90);   //控制舵机转动到90度
  delay(1000);          //延时等待1S
}