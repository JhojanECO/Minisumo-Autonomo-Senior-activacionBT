#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int strategy;
    bool start_done;
    bool boost_active;
    uint32_t central_high_begin_ms;
    float last_error;
} robot_state_t;

void control_logic_init(robot_state_t *state);
void control_logic_set_strategy(robot_state_t *state, int strategy);
void control_logic_reset(robot_state_t *state);
void control_logic_execute_start(robot_state_t *state);
float control_logic_base_speed(robot_state_t *state, int central_value, uint32_t now_ms);
float control_logic_pid(robot_state_t *state, float position, float kp, float kd);

