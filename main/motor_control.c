#include "motor_control.h"

#include <math.h>

#include "driver/mcpwm.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "motor_control";

static int map_value(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void setup_mcpwm_gpio(void) {
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PIN_MOTOR_A_IN1));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PIN_MOTOR_A_IN2));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, PIN_MOTOR_B_IN1));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, PIN_MOTOR_B_IN2));
}

void motor_control_init(void) {
    setup_mcpwm_gpio();

    const mcpwm_config_t pwm_config = {
        .frequency = 1000,
        .cmpr_a = 0,
        .cmpr_b = 0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };

    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config));
    ESP_LOGI(TAG, "Motor control initialized");
}

static int map_speed_command(int command) {
    const int pwm_min = 0;
    const int pwm_max = 100;
    if (command > 0) {
        return map_value(command, 1, 255, pwm_max, pwm_min);
    } else if (command < 0) {
        return map_value(command, -1, -255, -pwm_max, -pwm_min);
    }
    return 0;
}

static int clamp_command(float value) {
    if (value > 255.0f) {
        return 255;
    }
    if (value < -255.0f) {
        return -255;
    }
    return (int)roundf(value);
}

static void drive_single_motor(mcpwm_unit_t unit, float command) {
    const int mapped = map_speed_command(clamp_command(command));
    if (command > 0) {
        ESP_ERROR_CHECK(mcpwm_set_signal_high(unit, MCPWM_TIMER_0, MCPWM_OPR_A));
        ESP_ERROR_CHECK(mcpwm_set_duty(unit, MCPWM_TIMER_0, MCPWM_OPR_B, mapped));
        ESP_ERROR_CHECK(
            mcpwm_set_duty_type(unit, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0));
    } else if (command < 0) {
        ESP_ERROR_CHECK(mcpwm_set_signal_high(unit, MCPWM_TIMER_0, MCPWM_OPR_B));
        ESP_ERROR_CHECK(mcpwm_set_duty(unit, MCPWM_TIMER_0, MCPWM_OPR_A, -mapped));
        ESP_ERROR_CHECK(
            mcpwm_set_duty_type(unit, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0));
    } else {
        ESP_ERROR_CHECK(mcpwm_set_signal_high(unit, MCPWM_TIMER_0, MCPWM_OPR_A));
        ESP_ERROR_CHECK(mcpwm_set_signal_high(unit, MCPWM_TIMER_0, MCPWM_OPR_B));
    }
}

void motor_control_drive(float left_cmd, float right_cmd) {
    drive_single_motor(MCPWM_UNIT_0, left_cmd);
    drive_single_motor(MCPWM_UNIT_1, right_cmd);
}

void motor_control_stop(void) {
    motor_control_drive(0.0f, 0.0f);
}

