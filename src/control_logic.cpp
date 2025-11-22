#include <Arduino.h>

extern "C" {
#include "control_logic.h"
}

#include "hardware_config.h"
#include "motor_control.h"

void control_logic_init(robot_state_t *state) {
    state->strategy = 1;
    state->start_done = false;
    state->boost_active = false;
    state->central_high_begin_ms = 0;
    state->last_error = 0.0f;
}

void control_logic_set_strategy(robot_state_t *state, int strategy) {
    state->strategy = strategy;
}

void control_logic_reset(robot_state_t *state) {
    state->start_done = false;
    state->boost_active = false;
    state->central_high_begin_ms = 0;
}

static void strategy3_turn_direction(float *left, float *right) {
    const bool use_cw = START3_RANDOM_DIRECTION ? (random(0, 2) == 0) : true;
    if (use_cw) {
        *left = START3_SPEED_LEFT_CW;
        *right = START3_SPEED_RIGHT_CW;
    } else {
        *left = START3_SPEED_LEFT_CCW;
        *right = START3_SPEED_RIGHT_CCW;
    }
}

void control_logic_execute_start(robot_state_t *state) {
    if (state->start_done) {
        return;
    }

    float left = 0.0f;
    float right = 0.0f;
    uint32_t duration_ms = 0;

    switch (state->strategy) {
        case 1:
            left = START1_SPEED_LEFT;
            right = START1_SPEED_RIGHT;
            duration_ms = START1_DURATION_MS;
            break;
        case 2:
            left = START2_SPEED_LEFT;
            right = START2_SPEED_RIGHT;
            duration_ms = START2_DURATION_MS;
            break;
        case 3:
        default:
            strategy3_turn_direction(&left, &right);
            duration_ms = START3_DURATION_MS;
            break;
    }

    motor_control_drive(left, right);
    delay(duration_ms);
    motor_control_stop();
    state->start_done = true;
}

float control_logic_base_speed(robot_state_t *state, int central_value, uint32_t now_ms) {
    if (!state->boost_active) {
        if (central_value > SENSOR_SPEED_BOOST_THRESHOLD) {
            if (state->central_high_begin_ms == 0) {
                state->central_high_begin_ms = now_ms;
            } else if ((now_ms - state->central_high_begin_ms) >=
                       SENSOR_SPEED_BOOST_DURATION_MS) {
                state->boost_active = true;
            }
        } else {
            state->central_high_begin_ms = 0;
        }
    }
    return state->boost_active ? BASE_SPEED_BOOST : BASE_SPEED_DEFAULT;
}

float control_logic_pid(robot_state_t *state, float position, float kp, float kd) {
    const float error = position - LINE_POSITION_TARGET;
    const float prop = kp * error;
    const float deriv = kd * (error - state->last_error);
    state->last_error = error;
    return prop + deriv;
}
