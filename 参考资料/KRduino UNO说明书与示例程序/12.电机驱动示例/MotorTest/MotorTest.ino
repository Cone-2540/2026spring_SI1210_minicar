 //A组电机驱动  
int A_PWM = 6; //控制速度   
int A_DIR = 7; //控制方向  
//B组电机驱动
int B_PWM = 5; //控制速度 
int B_DIR = 4; //控制方向 

//初始化
void setup(){  
  pinMode(A_DIR, OUTPUT);  
  pinMode(A_PWM, OUTPUT);  
  pinMode(B_DIR, OUTPUT);  
  pinMode(B_PWM, OUTPUT);//全部都设置为输出
}

//A组电机驱动控制函数
void A_Motor(int dir,int speed)
{
  digitalWrite(A_DIR, dir);   
  analogWrite(A_PWM, speed);  
}

//B组电机驱动控制函数
void B_Motor(int dir,int speed)
{
  digitalWrite(B_DIR, dir);   
  analogWrite(B_PWM, speed);  
}

//主循环
void loop(){  
  A_Motor(HIGH,125);//A电机正转（默认HIGH为正转），速度值125
  B_Motor(HIGH,125);//B电机正转（默认HIGH为正转），速度值125
  delay(1000);      //延时等待1000ms
  A_Motor(LOW,80);  //A电机反转（默认LOW为转），速度值80
  B_Motor(LOW,80);  //B电机反转（默认LOW为转），速度值80
  delay(1000);      //延时等待1000ms
} 

