#pragma once

#include <stdbool.h>

#include "hardware_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int raw[SENSOR_COUNT];
    bool line_detected;
    float position;  // 0-100
} sensor_measurement_t;

void sensor_array_init(void);
sensor_measurement_t sensor_array_read(void);

#ifdef __cplusplus
}
#endif

