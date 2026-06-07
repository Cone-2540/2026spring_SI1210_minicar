//HJMCU
//模拟三路灰度传感器模块
//www.hlmcu.com

int sensor[3] = {A0, A1, A2}; //定义传感器IO口
int sensorReading[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600); // 打开串口，设置波特率为9600 bps
}

void loop() {
  for(int i=0;i<3;i++){
    sensorReading[i]=analogRead(sensor[i]);
  }
  Serial.print(sensorReading[0]); //读取A0传感器数据
  Serial.print("--");
  Serial.print(sensorReading[1]);  //读取A1传感器数据
  Serial.print("--");
  Serial.println(sensorReading[2]);  //读取A2传感器数据
  delay(200); //读取延时
}
