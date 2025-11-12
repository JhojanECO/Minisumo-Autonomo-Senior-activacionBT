#pragma once

#include <stdbool.h>

#include "hardware_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void status_led_init(void);
void status_led_set(bool red, bool green, bool blue);
void status_led_show_strategy(int strategy);

#ifdef __cplusplus
}
#endif

