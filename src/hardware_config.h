#pragma once

#include <stdint.h>

// Motor pins
#define PIN_MOTOR_A_IN1 18
#define PIN_MOTOR_A_IN2 5
#define PIN_MOTOR_B_IN1 19
#define PIN_MOTOR_B_IN2 21

// Sensor pins
#define PIN_SENSOR_LEFT 33
#define PIN_SENSOR_CENTER 35
#define PIN_SENSOR_RIGHT 32

// IO pins
#define PIN_LED_R 12
#define PIN_LED_G 14
#define PIN_LED_B 13
#define PIN_BUTTON 0

// Control parameters
#define SENSOR_COUNT 3
#define LINE_POSITION_TARGET 50.0f
#define SENSOR_KEEP_TRACK_THRESHOLD 600
#define SENSOR_NOISE_THRESHOLD 600
#define SENSOR_POSITION_STEP 50
#define SENSOR_SPEED_BOOST_THRESHOLD 3000
#define SENSOR_SPEED_BOOST_DURATION_MS 1000

#define BASE_SPEED_DEFAULT 150.0f
#define BASE_SPEED_BOOST 250.0f

#define CONTROL_LOOP_DELAY_MS 10
#define KP_DEFAULT 1.5f
#define KD_DEFAULT 0.2f
