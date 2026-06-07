/**
 * config.h — 工程实践循迹小车 引脚定义 & 控制参数
 *
 * 引脚来源：官方文档 & MotorTest.ino
 *   A组电机: D7=DIR, D6=PWM  (左后轮)
 *   B组电机: D4=DIR, D5=PWM  (右后轮)
 *
 * 循迹策略：PD 控制（5路加权 → 连续舵机 + 差速 + 动态降速）
 *   每路传感器独立黑白基准值，死区抗抖动
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Servo.h>

// ============================================================
// 引脚定义
// ============================================================

#define PIN_L2 A0
#define PIN_L1 A1
#define PIN_M A2
#define PIN_R1 A3
#define PIN_R2 A4
#define SENSOR_NUM 5

#define PIN_SERVO 2

#define PIN_M1_DIR 7
#define PIN_M1_PWM 6
#define PIN_M2_DIR 4
#define PIN_M2_PWM 5

#define PIN_JUMP_STRAIGHT 8
#define PIN_JUMP_TRACK 9
#define PIN_JUMP_DEBUG 10

// ============================================================
// 舵机参数
// ============================================================
#define SERVO_CENTER 90
#define SERVO_MAX 180
#define SERVO_MIN 0

// ============================================================
// ★ 每路传感器黑白基准值
//   方法：DEBUG模式下发 'r' 看读数，记录纯白/纯黑下的值
//   影响：巡线精度和方向判断
// ============================================================
#define WHITE_L2 970 // L2 白底读数
#define WHITE_L1 990 // L1 白底读数
#define WHITE_M 990  // M  白底读数
#define WHITE_R1 990 // R1 白底读数
#define WHITE_R2 990 // R2 白底读数

#define BLACK_L2 210 // L2 黑线读数
#define BLACK_L1 250 // L1 黑线读数
#define BLACK_M 190  // M  黑线读数
#define BLACK_R1 400 // R1 黑线读数（此路读数偏高）
#define BLACK_R2 230 // R2 黑线读数

// ============================================================
// ★【主要调参区】PD 控制 & 死区
// ============================================================

// P 增益：误差 1.0 → 偏转 KP_GAIN 度
//   【震荡/蛇形】→ 调小；【反应迟钝/压线出弯】→ 调大
#define KP_GAIN 40.0f

// D 增益：抑制震荡，抵抗高速超调（当前设为0，需要时调大）
//   【高速时蛇形震荡】→ 逐步调大（建议从 5 开始，每次 +5）
//   【反应变迟钝】→ 调小或清0
#define KD_GAIN 15.0f

// 死区：黑度小于此值视为白底噪声
//   【直道也左右晃】→ 调大；【弯道反应慢/光线暗断线→误转直行】→ 调小
#define DEADBAND 110

// 丢线保持状态机（3态 latch：LEFT/MIDDLE/RIGHT）
// 读数 < 700 视为黑线（黑度 > LATCH_THRESHOLD），
// L1/L2 任一触发 → LEFT，R1/R2 任一 → RIGHT，M → MIDDLE
// 只有 M 重新检测到黑线才解除 LEFT/RIGHT
#define LATCH_THRESHOLD 270 // 黑度 > 270 → 检测到黑线（≈读数<700）
#define HOLD_ANGLE_LEFT 180
#define HOLD_ANGLE_RIGHT 0
#define HOLD_TIMEOUT_MS 10000

// ============================================================
// ★【速度参数】只改 SPEED_BASE 即可全局调速
// ============================================================
#define SPEED_BASE 170 // 基础直行速度 ← ★比赛主调项
#define SPEED_LOW 100  // 直道考核速度（独立）

// 转弯差速（非对称：左弯40cm温和，右弯28cm激进）
//   【弯道内侧轮打滑/推头】→ 调大 RATIO_OUT_*（外侧更猛）
//   【弯道转不过来】→ 调小 RATIO_IN_*（内侧再慢）
#define RATIO_IN_LEFT 0.85f   // 左弯内侧（40cm理论速比1.41）
#define RATIO_OUT_LEFT 1.20f  // 左弯外侧
#define RATIO_IN_RIGHT 0.70f  // 右弯内侧（28cm理论速比1.64）
#define RATIO_OUT_RIGHT 1.40f // 右弯外侧

// 差速渐变：误差=0时差速=0，误差达 DIFF_RAMP 时满差速
//   【直道出弯差速反复跳】→ 调大（差速更温和）；【弯道差速不够】→ 调小
#define DIFF_RAMP 1.0f // 误差>0.5时满差速

// 动态降速（根据偏离程度自动降速）
//   【直道也降速了】→ 调大 SLOW_THRESHOLD（更难触发）
//   【弯道冲出去】→ 调小 SLOW_THRESHOLD（更容易触发）
//   【降太慢弯道动力不足】→ 调大 SLOW_RATIO（更接近1.0）
#define SLOW_RATIO_L2 0.60f    // 最大降速比（1.0弯道最慢时）
#define SLOW_THRESHOLD_L1 0.5f // 误差>此值开始线性降速 ← 弯道冲出去时调小
#define SLOW_THRESHOLD_L2 1.0f // 误差≥此值达到最大降速

#define MOTOR_PWM_MAX 255

// ============================================================
// 启动延迟（ms）
// ============================================================
#define START_DELAY_MS 3000

// ============================================================
// 电机方向（已确定，无需调整）
// ============================================================
#define MOTOR_LEFT_FORWARD LOW
#define MOTOR_RIGHT_FORWARD HIGH

#endif // CONFIG_H