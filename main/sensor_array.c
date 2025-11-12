#include "sensor_array.h"

#include "driver/adc.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "sensor_array";

typedef struct {
    gpio_num_t pin;
    adc1_channel_t channel;
} sensor_adc_t;

static const sensor_adc_t sensors[SENSOR_COUNT] = {
    {PIN_SENSOR_LEFT, ADC1_CHANNEL_5},
    {PIN_SENSOR_CENTER, ADC1_CHANNEL_7},
    {PIN_SENSOR_RIGHT, ADC1_CHANNEL_4},
};

static int last_valid_edge = -1;
static float last_position = LINE_POSITION_TARGET;

void sensor_array_init(void) {
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    for (int i = 0; i < SENSOR_COUNT; ++i) {
        ESP_ERROR_CHECK(
            adc1_config_channel_atten(sensors[i].channel, ADC_ATTEN_DB_11));
    }
    ESP_LOGI(TAG, "Sensor array initialized");
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
        measurement.raw[i] = adc1_get_raw(sensors[i].channel);
    }
    measurement.position = compute_position(&measurement);
    return measurement;
}

