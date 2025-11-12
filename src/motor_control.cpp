#include <Arduino.h>
#include <math.h>

extern "C" {
#include "motor_control.h"
}

static const uint16_t PWM_FREQ = 20000;
static const uint8_t PWM_RES_BITS = 8;
static const uint8_t PWM_MAX = (1 << PWM_RES_BITS) - 1;

static const uint8_t MOTOR_CHANNELS[4] = {0, 1, 2, 3};

static inline int clamp_command(float value) {
    if (value > 255.0f) {
        return 255;
    }
    if (value < -255.0f) {
        return -255;
    }
    return (int)roundf(value);
}

static void setup_pwm_pin(uint8_t channel, uint8_t pin) {
    ledcSetup(channel, PWM_FREQ, PWM_RES_BITS);
    ledcAttachPin(pin, channel);
    ledcWrite(channel, 0);
}

void motor_control_init(void) {
    setup_pwm_pin(MOTOR_CHANNELS[0], PIN_MOTOR_A_IN1);
    setup_pwm_pin(MOTOR_CHANNELS[1], PIN_MOTOR_A_IN2);
    setup_pwm_pin(MOTOR_CHANNELS[2], PIN_MOTOR_B_IN1);
    setup_pwm_pin(MOTOR_CHANNELS[3], PIN_MOTOR_B_IN2);
    motor_control_stop();
}

static void drive_single_motor(uint8_t forward_ch, uint8_t reverse_ch, float command) {
    const int cmd = clamp_command(command);
    const uint8_t duty = (uint8_t)abs(cmd);
    if (cmd > 0) {
        ledcWrite(forward_ch, duty);
        ledcWrite(reverse_ch, 0);
    } else if (cmd < 0) {
        ledcWrite(forward_ch, 0);
        ledcWrite(reverse_ch, duty);
    } else {
        ledcWrite(forward_ch, 0);
        ledcWrite(reverse_ch, 0);
    }
}

void motor_control_drive(float left_cmd, float right_cmd) {
    drive_single_motor(MOTOR_CHANNELS[0], MOTOR_CHANNELS[1], left_cmd);
    drive_single_motor(MOTOR_CHANNELS[2], MOTOR_CHANNELS[3], right_cmd);
}

void motor_control_stop(void) {
    for (int i = 0; i < 4; ++i) {
        ledcWrite(MOTOR_CHANNELS[i], 0);
    }
}
