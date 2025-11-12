#pragma once

#include "hardware_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void motor_control_init(void);
void motor_control_drive(float left_cmd, float right_cmd);
void motor_control_stop(void);

#ifdef __cplusplus
}
#endif

