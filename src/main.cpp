/**
 * main.cpp — 工程实践循迹小车 统一主程序（连续P控制版）
 *
 * 三模式切换（串口命令，115200 baud）：
 *   d / debug    → DEBUG 模式
 *   s / straight → 直道考核
 *   t / track    → 循迹模式（5路加权 P 控制 + 死区 + 动态降速）
 *   stop         → 停止电机
 *   help         → 帮助
 *
 * 跳线启动（短接对应引脚到 GND，电池上电即用）：
 *   D8↔GND = 直道考核
 *   D9↔GND = 循迹模式
 *   无跳线  = DEBUG
 *
 * 循迹策略：
 *   每路传感器独立黑白基准值 → 黑度计算 → 死区滤波 →
 *   加权位置误差（连续 -2~+2）→ P 控制连续舵机角度 →
 *   差速 + 基于误差的动态降速
 */

#include "config.h"

// ============================================================
// 枚举
// ============================================================
enum Mode { MODE_DEBUG, MODE_STRAIGHT, MODE_TRACK };

// ============================================================
// 全局变量
// ============================================================
Servo steeringServo;
Mode currentMode = MODE_DEBUG;

int servoAngle = SERVO_CENTER;

// 传感器
const uint8_t sensorPins[SENSOR_NUM] = {PIN_L2, PIN_L1, PIN_M, PIN_R1, PIN_R2};
uint16_t sensorValues[SENSOR_NUM] = {0};

// 每路传感器的黑白基准值（来自 config.h）
const float whiteBaseline[SENSOR_NUM] = {WHITE_L2, WHITE_L1, WHITE_M, WHITE_R1, WHITE_R2};
const float blackLevel[SENSOR_NUM] = {BLACK_L2, BLACK_L1, BLACK_M, BLACK_R1, BLACK_R2};

bool motorRunning = false;
bool pendingStart = false;
unsigned long startTimestamp = 0;

// ============================================================
// 前向声明
// ============================================================
void setMode(Mode mode);
void modeDebug();
void modeStraight();
void modeTrack();
void handleDebugCommand(const String& cmd);
void readSensors();
void printSensors();
float computeLineError();
void setMotorSpeed(uint8_t left, uint8_t right);
void stopMotor();
void printHelp();

// ============================================================
// 初始化
// ============================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n========================================");
    Serial.println("  工程实践循迹小车 — 连续P控制");
    Serial.println("========================================");

    steeringServo.attach(PIN_SERVO);
    steeringServo.write(servoAngle);

    digitalWrite(PIN_M1_DIR, LOW);
    digitalWrite(PIN_M1_PWM, LOW);
    digitalWrite(PIN_M2_DIR, LOW);
    digitalWrite(PIN_M2_PWM, LOW);
    pinMode(PIN_M1_DIR, OUTPUT);
    pinMode(PIN_M1_PWM, OUTPUT);
    pinMode(PIN_M2_DIR, OUTPUT);
    pinMode(PIN_M2_PWM, OUTPUT);

    pinMode(PIN_JUMP_STRAIGHT, INPUT_PULLUP);
    pinMode(PIN_JUMP_TRACK, INPUT_PULLUP);
    pinMode(PIN_JUMP_DEBUG, INPUT_PULLUP);

    Mode bootMode;
    if (digitalRead(PIN_JUMP_STRAIGHT) == LOW) {
        bootMode = MODE_STRAIGHT;
        Serial.println("[跳线] D8↔GND → 直道考核");
    } else if (digitalRead(PIN_JUMP_TRACK) == LOW) {
        bootMode = MODE_TRACK;
        Serial.println("[跳线] D9↔GND → 循迹模式");
    } else {
        bootMode = MODE_DEBUG;
        Serial.println("[跳线] 无跳线 → DEBUG");
    }

    printHelp();

    if (bootMode == MODE_STRAIGHT || bootMode == MODE_TRACK) {
        setMode(bootMode);
    } else {
        Serial.println("\n[DEBUG] 等待串口命令...");
    }
}

// ============================================================
// 主循环
// ============================================================
void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() == 0) return;

        if (input == "d" || input == "debug") {
            setMode(MODE_DEBUG);
        } else if (input == "s" || input == "straight") {
            setMode(MODE_STRAIGHT);
        } else if (input == "t" || input == "track") {
            setMode(MODE_TRACK);
        } else if (input == "stop" || input == "x") {
            stopMotor();
            motorRunning = false;
            Serial.println("[停止]");
        } else if (input == "help") {
            printHelp();
        } else if (currentMode == MODE_DEBUG || currentMode == MODE_STRAIGHT) {
            handleDebugCommand(input);
        }
    }

    switch (currentMode) {
        case MODE_DEBUG:
            modeDebug();
            break;
        case MODE_STRAIGHT:
            modeStraight();
            break;
        case MODE_TRACK:
            modeTrack();
            break;
    }
}

// ============================================================
// 模式切换
// ============================================================
void setMode(Mode mode) {
    stopMotor();
    motorRunning = false;
    pendingStart = false;
    currentMode = mode;

    switch (mode) {
        case MODE_DEBUG:
            steeringServo.write(servoAngle);
            Serial.println("\n>>> DEBUG 模式");
            break;
        case MODE_STRAIGHT:
            steeringServo.write(servoAngle);
            pendingStart = true;
            startTimestamp = millis();
            Serial.print("\n>>> 直道考核  倒计时 ");
            Serial.print(START_DELAY_MS / 1000);
            Serial.println("s...");
            break;
        case MODE_TRACK:
            steeringServo.write(servoAngle);
            pendingStart = true;
            startTimestamp = millis();
            Serial.print("\n>>> 循迹模式（连续P控制）倒计时 ");
            Serial.print(START_DELAY_MS / 1000);
            Serial.print("s  基础速度=");
            Serial.println(SPEED_BASE);
            break;
    }
}

// ============================================================
// DEBUG 模式
// ============================================================
void modeDebug() { delay(10); }

// ============================================================
// 直道考核模式
// ============================================================
void modeStraight() {
    if (pendingStart) {
        unsigned long elapsed = millis() - startTimestamp;
        if (elapsed >= START_DELAY_MS) {
            pendingStart = false;
            motorRunning = true;
            setMotorSpeed(SPEED_LOW, SPEED_LOW);
            Serial.println(">>> 启动！");
        } else {
            static unsigned long lastTick = 0;
            unsigned long rem = (START_DELAY_MS - elapsed) / 1000 + 1;
            if (rem != lastTick) {
                lastTick = rem;
                Serial.print("    ");
                Serial.print(rem);
                Serial.println("s...");
            }
            delay(10);
            return;
        }
    }
    if (motorRunning) {
        steeringServo.write(servoAngle);
        setMotorSpeed(SPEED_LOW, SPEED_LOW);
    }
    delay(10);
}

// ============================================================
// 循迹模式 — 连续 P 控制
// ============================================================
void modeTrack() {
    if (pendingStart) {
        unsigned long elapsed = millis() - startTimestamp;
        if (elapsed >= START_DELAY_MS) {
            pendingStart = false;
            motorRunning = true;
            Serial.println(">>> 启动！");
        } else {
            static unsigned long lastTick = 0;
            unsigned long rem = (START_DELAY_MS - elapsed) / 1000 + 1;
            if (rem != lastTick) {
                lastTick = rem;
                Serial.print("    ");
                Serial.print(rem);
                Serial.println("s...");
            }
            delay(10);
            return;
        }
    }

    if (!motorRunning) {
        stopMotor();
        delay(10);
        return;
    }

    // 1. 读取传感器
    readSensors();

    // 2. 计算加权位置误差（连续值 -2 ~ +2）
    float error = computeLineError();
    float absError = fabs(error);

    // 3. P 控制 → 连续舵机角度
    float targetAngle = SERVO_CENTER + KP_GAIN * error;

    // 4. 限幅
    targetAngle = constrain(targetAngle, SERVO_MIN, SERVO_MAX);

    // 5. 转弯差速（左右轮速比跟随误差方向）
    uint8_t baseSpeed = SPEED_BASE;
    uint8_t leftSp, rightSp;

    if (absError < 0.15f) {
        // 直行：等速
        leftSp = baseSpeed;
        rightSp = baseSpeed;
    } else if (error > 0) {
        // 左转（error>0 = 线偏左）：左轮内侧减速，右轮外侧加速
        leftSp = (uint8_t)(baseSpeed * RATIO_IN);
        rightSp = (uint8_t)(baseSpeed * RATIO_OUT);
    } else {
        // 右转（error<0 = 线偏右）：左轮外侧加速，右轮内侧减速
        leftSp = (uint8_t)(baseSpeed * RATIO_OUT);
        rightSp = (uint8_t)(baseSpeed * RATIO_IN);
    }

    // 6. 动态降速（偏离越大整体越慢）
    float slowRatio = 1.0f;
    if (absError > SLOW_THRESHOLD_L2) {
        slowRatio = SLOW_RATIO_L2;
    } else if (absError > SLOW_THRESHOLD_L1) {
        slowRatio = SLOW_RATIO_L1;
    }
    leftSp = (uint8_t)(leftSp * slowRatio);
    rightSp = (uint8_t)(rightSp * slowRatio);

    // 7. 限幅 & 执行
    leftSp = constrain(leftSp, 0, MOTOR_PWM_MAX);
    rightSp = constrain(rightSp, 0, MOTOR_PWM_MAX);
    steeringServo.write((int)targetAngle);
    setMotorSpeed(leftSp, rightSp);

    // 8. 串口调试输出
    printSensors();
    Serial.print(" Err:");
    Serial.print(error, 2);
    Serial.print(" A:");
    Serial.print((int)targetAngle);
    Serial.print(" L:");
    Serial.print(leftSp);
    Serial.print(" R:");
    Serial.print(rightSp);
    Serial.print(" D:");
    Serial.println(slowRatio, 2);

    delay(20);
}

// ============================================================
// 加权位置误差计算（连续值，带死区）
// ============================================================
float computeLineError() {
    float wSum = 0, wTotal = 0;
    const int8_t pos[SENSOR_NUM] = {-2, -1, 0, 1, 2};
    bool detected = false;

    for (int i = 0; i < SENSOR_NUM; i++) {
        // 黑度 = 白底基准 - 当前读数
        float darkness = whiteBaseline[i] - (float)sensorValues[i];

        // 死区：黑度不够 = 视为白底，忽略
        if (darkness < DEADBAND) continue;

        detected = true;
        wSum += (float)pos[i] * darkness;
        wTotal += darkness;
    }

    if (!detected || wTotal < 1.0f) return 0.0f;

    // 归一化到 -2 ~ +2 范围
    return wSum / wTotal;
}

// ============================================================
// DEBUG 子命令
// ============================================================
void handleDebugCommand(const String& cmd) {
    if (cmd == "+") {
        servoAngle = constrain(servoAngle + 1, 0, 180);
        steeringServo.write(servoAngle);
        Serial.print("舵机: ");
        Serial.print(servoAngle);
        Serial.println("°");
    } else if (cmd == "-") {
        servoAngle = constrain(servoAngle - 1, 0, 180);
        steeringServo.write(servoAngle);
        Serial.print("舵机: ");
        Serial.print(servoAngle);
        Serial.println("°");
    } else if (cmd == "++") {
        servoAngle = constrain(servoAngle + 5, 0, 180);
        steeringServo.write(servoAngle);
        Serial.print("舵机: ");
        Serial.print(servoAngle);
        Serial.println("° (+5)");
    } else if (cmd == "--") {
        servoAngle = constrain(servoAngle - 5, 0, 180);
        steeringServo.write(servoAngle);
        Serial.print("舵机: ");
        Serial.print(servoAngle);
        Serial.println("° (-5)");
    } else if (cmd == "r") {
        readSensors();
        printSensors();
        // 同时打印黑度
        Serial.print(" 暗度:[");
        for (int i = 0; i < SENSOR_NUM; i++) {
            Serial.print(whiteBaseline[i] - (float)sensorValues[i], 0);
            if (i < SENSOR_NUM - 1) Serial.print(",");
        }
        Serial.println("]");
    } else if (cmd == "servo") {
        Serial.print("舵机: ");
        Serial.print(servoAngle);
        Serial.println("°");
    } else if (cmd.length() > 0 && cmd.charAt(0) >= '0' && cmd.charAt(0) <= '9') {
        int val = cmd.toInt();
        if (val >= 0 && val <= 180) {
            servoAngle = val;
            steeringServo.write(servoAngle);
            Serial.print("舵机-> ");
            Serial.print(servoAngle);
            Serial.println("°");
        }
    }
}

// ============================================================
// 传感器
// ============================================================
void readSensors() {
    for (uint8_t i = 0; i < SENSOR_NUM; i++)
        sensorValues[i] = analogRead(sensorPins[i]);
}

void printSensors() {
    Serial.print("[");
    for (uint8_t i = 0; i < SENSOR_NUM; i++) {
        Serial.print(sensorValues[i]);
        if (i < SENSOR_NUM - 1) Serial.print(",");
    }
    Serial.print("]");
}

// ============================================================
// 电机控制
// ============================================================
void setMotorSpeed(uint8_t left, uint8_t right) {
    digitalWrite(PIN_M1_DIR, MOTOR_LEFT_FORWARD);
    analogWrite(PIN_M1_PWM, left);
    digitalWrite(PIN_M2_DIR, MOTOR_RIGHT_FORWARD);
    analogWrite(PIN_M2_PWM, right);
}

void stopMotor() {
    analogWrite(PIN_M1_PWM, 0);
    analogWrite(PIN_M2_PWM, 0);
    digitalWrite(PIN_M1_DIR, LOW);
    digitalWrite(PIN_M2_DIR, LOW);
}

// ============================================================
// 帮助
// ============================================================
void printHelp() {
    Serial.println("\n--- 命令 ---");
    Serial.println(" d / debug    调试模式（+/- 调舵机, r 读传感器+暗度）");
    Serial.println(" s / straight 直道考核 (3s 延迟)");
    Serial.println(" t / track    循迹模式 (3s 延迟, 连续P控制)");
    Serial.println(" stop / x     停止电机");
    Serial.println(" help         帮助");
}