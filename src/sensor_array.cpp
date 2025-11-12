#include <Arduino.h>

extern "C" {
#include "sensor_array.h"
}

static const uint8_t sensors[SENSOR_COUNT] = {
    PIN_SENSOR_LEFT,
    PIN_SENSOR_CENTER,
    PIN_SENSOR_RIGHT,
};

static int last_valid_edge = -1;
static float last_position = LINE_POSITION_TARGET;

void sensor_array_init(void) {
    analogReadResolution(12);
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        pinMode(sensors[i], INPUT);
    }
}

static float compute_position(sensor_measurement_t *measurement) {
    int64_t avg = 0;
    int sum = 0;
    bool on_line = false;
    int sensors_detected = 0;
    int last_sensor_detected = -1;

    for (int i = 0; i < SENSOR_COUNT; ++i) {
        const int value = measurement->raw[i];
        if (value > SENSOR_KEEP_TRACK_THRESHOLD) {
            on_line = true;
            sensors_detected++;
            last_sensor_detected = i;
        }
        if (value > SENSOR_NOISE_THRESHOLD) {
            avg += (int64_t)value * (i * SENSOR_POSITION_STEP);
            sum += value;
        }
    }

    if (on_line && sum > 0) {
        if (sensors_detected == 1 &&
            (last_sensor_detected == 0 ||
             last_sensor_detected == SENSOR_COUNT - 1)) {
            last_valid_edge = last_sensor_detected;
        }
        last_position = (float)avg / (float)sum;
        measurement->line_detected = true;
        return last_position;
    }

    measurement->line_detected = false;
    if (last_valid_edge == 0) {
        return 0.0f;
    }
    if (last_valid_edge == SENSOR_COUNT - 1) {
        return 100.0f;
    }
    return last_position;
}

sensor_measurement_t sensor_array_read(void) {
    sensor_measurement_t measurement = {0};
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        measurement.raw[i] = analogRead(sensors[i]);
    }
    measurement.position = compute_position(&measurement);
    return measurement;
}
