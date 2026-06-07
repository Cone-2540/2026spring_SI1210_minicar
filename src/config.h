/**
 * config.h — 工程实践循迹小车 引脚定义 & 控制参数
 *
 * 引脚来源：参照官方《小车电控模块及连线-2024版》及 MotorTest.ino
 *   A组电机: D7=DIR, D6=PWM  (左后轮)
 *   B组电机: D4=DIR, D5=PWM  (右后轮)
 *
 * 循迹策略：模拟连续 P 控制（5路加权 → 连续舵机 + 差速 + 动态降速）
 *   每路传感器独立黑白基准值，死区抗抖动，连续角度避震震荡
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Servo.h>

// ============================================================
// 引脚定义
// ============================================================

// --- 循迹传感器（5路模拟量） ---
#define PIN_L2 A0
#define PIN_L1 A1
#define PIN_M A2
#define PIN_R1 A3
#define PIN_R2 A4
#define SENSOR_NUM 5

// --- 舵机 ---
#define PIN_SERVO 2

// --- 电机驱动（A=左后, B=右后）---
#define PIN_M1_DIR 7
#define PIN_M1_PWM 6
#define PIN_M2_DIR 4
#define PIN_M2_PWM 5

// --- 模式选择跳线 ---
#define PIN_JUMP_STRAIGHT 8
#define PIN_JUMP_TRACK 9
#define PIN_JUMP_DEBUG 10

// ============================================================
// 舵机参数
// ============================================================
#define SERVO_CENTER 90
#define SERVO_MAX 180 // 最左极限
#define SERVO_MIN 0   // 最右极限

// ============================================================
// 每路传感器黑白基准值（比赛当天根据光照调整）
// 白底 ≈ 950，黑线 ≈ 200-400（各路有差异）
// ============================================================
#define WHITE_L2 950
#define WHITE_L1 950
#define WHITE_M 950
#define WHITE_R1 950
#define WHITE_R2 950

#define BLACK_L2 300
#define BLACK_L1 300
#define BLACK_M 300
#define BLACK_R1 400 // R1 在黑线上读数偏高
#define BLACK_R2 300

// ============================================================
// 循迹 P 控制参数
// ============================================================
#define KP_GAIN 30.0f // P 增益 (error 1.0 → 30° 偏转)
#define DEADBAND 100  // 黑度 < DEADBAND 忽略（滤除白底波动）

// ============================================================
// 速度参数
// ============================================================
#define SPEED_BASE 150 // 基础速度
#define SPEED_LOW 100  // 直道考核速度

// 转弯差速比例
#define RATIO_IN 0.65f  // 内侧
#define RATIO_OUT 1.35f // 外侧

// 动态降速（偏离越大降速越多）
#define SLOW_RATIO_L1 0.80f    // 轻微偏离
#define SLOW_RATIO_L2 0.60f    // 大幅偏离
#define SLOW_THRESHOLD_L1 0.3f // 误差 > 0.3 触发 L1 降速
#define SLOW_THRESHOLD_L2 1.0f // 误差 > 1.0 触发 L2 降速

#define MOTOR_PWM_MAX 255

// ============================================================
// 启动延迟（ms）
// ============================================================
#define START_DELAY_MS 3000

// ============================================================
// 电机方向
// ============================================================
#define MOTOR_LEFT_FORWARD LOW
#define MOTOR_RIGHT_FORWARD HIGH

#endif // CONFIG_H