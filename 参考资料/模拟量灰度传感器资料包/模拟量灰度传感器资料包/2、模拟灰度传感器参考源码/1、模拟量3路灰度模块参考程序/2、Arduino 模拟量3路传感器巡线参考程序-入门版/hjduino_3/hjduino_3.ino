//Arduino C demo
void motorDrive( int speed, int pin2_pwm,int pin1_dir) {
  pinMode(pin1_dir, OUTPUT);
  pinMode(pin2_pwm, OUTPUT);
  speed = speed > 255 ? 255 : speed;
  speed = speed < -255 ? -255 : speed;

  if (speed >= 0) {
    digitalWrite(pin1_dir, HIGH);
    analogWrite(pin2_pwm, speed);
  } else {
    digitalWrite(pin1_dir, LOW);
    analogWrite(pin2_pwm, -speed);
  }
}

void setup() {
}

void loop() {
  if ((analogRead(A1)) < 600) {
    motorDrive(-180, 5, 7);
    motorDrive(180, 6, 4);
  }
  if ((analogRead(A2)) < 600) {
    motorDrive(200, 5, 7);
    motorDrive(200, 6, 4);
  }
  if ((analogRead(A3)) < 600) {
    motorDrive(180, 5, 7);
    motorDrive(-180, 6, 4);
  }
}
