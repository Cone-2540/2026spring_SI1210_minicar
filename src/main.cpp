/**
 * main.cpp — 工程实践循迹小车 主程序
 * 支持两种测试模式：
 *   1. 直道测试（TEST_STRAIGHT）：舵机居中，两电机同速直行
 *   2. 循迹测试（TEST_LINE_TRACK）：5路传感器 + 纯P控制
 * 通过 config.h 中的宏切换
 */

#include "config.h"

// ============================================================
// 前向声明
// ============================================================
void testStraight();
void testLineTracking();
void calibrateWhiteBaseline();
void readSensors();
float computeLineError();
void setMotorSpeed(uint8_t leftPWM, uint8_t rightPWM);
void stopMotor();
void printSensors();

// ============================================================
// 全局对象
// ============================================================
Servo steeringServo;

// 传感器引脚映射
const uint8_t sensorPins[SENSOR_NUM] = {PIN_L2, PIN_L1, PIN_M, PIN_R1, PIN_R2};

// 传感器读数（0~1023）
uint16_t sensorValues[SENSOR_NUM] = {0};

// 各传感器白底基准值（上电自动校准）
float baselineWhite[SENSOR_NUM] = {0};

// 黑线检测阈值偏移（读数高于 baselineWhite + 此偏移 视为检测到黑线）
#define BLACK_OFFSET 50

// ============================================================
// 初始化
// ============================================================
void setup() {
    // 串口初始化
    Serial.begin(115200);
    Serial.println("\n===== 工程实践循迹小车 电控系统 =====");

    // 舵机初始化
    steeringServo.attach(PIN_SERVO);
    steeringServo.write(SERVO_CENTER);

    // 电机引脚初始化
    pinMode(PIN_M1_DIR, OUTPUT);
    pinMode(PIN_M1_PWM, OUTPUT);
    pinMode(PIN_M2_DIR, OUTPUT);
    pinMode(PIN_M2_PWM, OUTPUT);

    // 传感器引脚（模拟输入，默认即为 INPUT，无需额外设置）

    // 初始停止
    stopMotor();

    // 自动白底校准：采集100个样本取平均
    Serial.println("[校准] 正在采集白底基准值... 请将小车放在白色区域");
    calibrateWhiteBaseline();

#ifdef TEST_STRAIGHT
    Serial.println("[模式] 直道测试");
#elif defined(TEST_LINE_TRACK)
    Serial.println("[模式] 循迹测试 — 纯P控制");
#else
    Serial.println(
        "[模式] 未选择测试模式，请在 config.h 中启用 TEST_STRAIGHT 或 TEST_LINE_TRACK");
#endif

    delay(2000); // 等待2秒后启动
    Serial.println("启动!");
}

// ============================================================
// 主循环
// ============================================================
void loop() {
#ifdef TEST_STRAIGHT
    testStraight();
#elif defined(TEST_LINE_TRACK)
    testLineTracking();
#else
    // 未选择模式时，保持停止
    delay(1000);
#endif
}

// ============================================================
// 直道测试
// ============================================================
void testStraight() {
    steeringServo.write(SERVO_CENTER);
    setMotorSpeed(BASE_SPEED, BASE_SPEED);

    // 打印传感器读数用于调试
    readSensors();
    printSensors();

    delay(20);
}

// ============================================================
// 循迹测试 — 纯P控制
// ============================================================
void testLineTracking() {
    // 1. 读取5路传感器
    readSensors();

    // 2. 计算黑线位置误差
    //    加权平均：传感器位置 L2=-2, L1=-1, M=0, R1=+1, R2=+2
    //    误差越大 = 线越偏右，小车需右转
    float error = computeLineError();

    // 3. 纯P控制：舵机转角
    //    误差为正（线偏右）→ 舵机角度增大（向右转）
    //    误差为负（线偏左）→ 舵机角度减小（向左转）
    float servoAngle = SERVO_CENTER + KP * error;

    // 4. 限幅
    servoAngle = constrain(servoAngle, SERVO_LEFT_MAX, SERVO_RIGHT_MAX);
    steeringServo.write((int)servoAngle);

    // 5. 差速修正（辅助转向）
    //    线偏右 → 左轮加速、右轮减速
    float leftSpeed = BASE_SPEED + KP_SPEED * error;
    float rightSpeed = BASE_SPEED - KP_SPEED * error;

    // 限幅
    leftSpeed = constrain(leftSpeed, 0, MOTOR_PWM_MAX);
    rightSpeed = constrain(rightSpeed, 0, MOTOR_PWM_MAX);

    setMotorSpeed((uint8_t)leftSpeed, (uint8_t)rightSpeed);

    // 6. 串口调试输出
    printSensors();
    Serial.print(" Error: ");
    Serial.print(error);
    Serial.print(" | Servo: ");
    Serial.print((int)servoAngle);
    Serial.print(" | L: ");
    Serial.print((int)leftSpeed);
    Serial.print(" R: ");
    Serial.println((int)rightSpeed);

    delay(20);
}

// ============================================================
// 白底基准值自动校准
// ============================================================
void calibrateWhiteBaseline() {
    const uint16_t sampleCount = 100;
    float sum[SENSOR_NUM] = {0};

    for (uint16_t s = 0; s < sampleCount; s++) {
        for (uint8_t i = 0; i < SENSOR_NUM; i++) {
            sum[i] += analogRead(sensorPins[i]);
        }
        delay(5);
    }

    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        baselineWhite[i] = sum[i] / (float)sampleCount;
    }

    Serial.print("[校准] 白底基准值: [");
    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        Serial.print(baselineWhite[i], 0);
        if (i < SENSOR_NUM - 1) Serial.print(", ");
    }
    Serial.println("]");
}

// ============================================================
// 传感器读取
// ============================================================
void readSensors() {
    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        sensorValues[i] = analogRead(sensorPins[i]);
    }
}

// ============================================================
// 计算黑线位置误差（加权平均法 — 每路独立基准值）
// 返回：正值 = 线偏右，负值 = 线偏左，0 = 居中
// ============================================================
float computeLineError() {
    float weightedSum = 0;
    float totalWeight = 0;

    const int8_t positionWeight[SENSOR_NUM] = {-2, -1, 0, 1, 2};
    bool lineDetected = false;

    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        // 计算该传感器检测到的黑线信号强度（高于白底基准值的部分）
        float signal = (float)sensorValues[i] - baselineWhite[i];

        // 只有超过阈值的才认为是检测到了黑线
        if (signal > BLACK_OFFSET) {
            lineDetected = true;
            weightedSum += (float)positionWeight[i] * signal;
            totalWeight += signal;
        }
    }

    // 没检测到黑线 → 保持直行
    if (!lineDetected || totalWeight < 1.0f) {
        return 0.0f;
    }

    return weightedSum / totalWeight;
}

// ============================================================
// 电机控制
// ============================================================

/**
 * 设置左右电机速度
 * @param leftPWM  左轮 PWM (0~255)
 * @param rightPWM 右轮 PWM (0~255)
 */
void setMotorSpeed(uint8_t leftPWM, uint8_t rightPWM) {
    // 左电机 (M1)
    digitalWrite(PIN_M1_DIR, HIGH); // 前进方向
    analogWrite(PIN_M1_PWM, leftPWM);

    // 右电机 (M2)
    digitalWrite(PIN_M2_DIR, HIGH); // 前进方向
    analogWrite(PIN_M2_PWM, rightPWM);
}

/**
 * 停止所有电机
 */
void stopMotor() {
    digitalWrite(PIN_M1_DIR, LOW);
    analogWrite(PIN_M1_PWM, 0);
    digitalWrite(PIN_M2_DIR, LOW);
    analogWrite(PIN_M2_PWM, 0);
}

// ============================================================
// 调试输出
// ============================================================
void printSensors() {
    Serial.print("Sensors [");
    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        Serial.print(sensorValues[i]);
        if (i < SENSOR_NUM - 1) {
            Serial.print(", ");
        }
    }
    Serial.print("] ");
}