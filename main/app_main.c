#include <stdio.h>

#include "control_logic.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware_config.h"
#include "motor_control.h"
#include "sensor_array.h"
#include "status_led.h"

static const char *TAG = "app_main";

static uint32_t millis(void) {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

static void configure_input(gpio_num_t pin, bool pullup) {
    const gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = pullup ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
}

static void init_inputs(void) {
    configure_input(PIN_BUTTON, true);
    configure_input(PIN_GO, false);
}

static bool is_button_pressed(void) {
    return gpio_get_level(PIN_BUTTON) == 0;
}

static bool go_is_active(void) {
    return gpio_get_level(PIN_GO) == 1;
}

static int read_button_event(void) {
    if (!is_button_pressed()) {
        return 0;
    }

    status_led_set(true, false, false);
    const uint32_t start = millis();
    while (is_button_pressed()) {
        if (millis() - start >= 1000) {
            status_led_set(true, false, true);
            vTaskDelay(pdMS_TO_TICKS(100));
            status_led_set(false, false, false);
            return 2;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    status_led_set(false, false, false);
    return 1;
}

static int select_strategy(void) {
    int strategy = 0;
    ESP_LOGI(TAG, "Selecciona la estrategia con pulsaciones cortas, confirma con pulsación larga");
    while (true) {
        const int event = read_button_event();
        if (event == 1) {
            strategy = (strategy < 3) ? strategy + 1 : strategy;
            status_led_show_strategy(strategy);
        } else if (event == 2) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    if (strategy < 1) {
        strategy = 1;
    }
    ESP_LOGI(TAG, "Estrategia confirmada: %d", strategy);
    return strategy;
}

void app_main(void) {
    status_led_init();
    init_inputs();
    motor_control_init();
    sensor_array_init();

    robot_state_t robot_state;
    control_logic_init(&robot_state);

    const int strategy = select_strategy();
    control_logic_set_strategy(&robot_state, strategy);
    status_led_show_strategy(strategy);

    while (true) {
        if (go_is_active()) {
            control_logic_execute_start(&robot_state);
            const sensor_measurement_t measurement = sensor_array_read();
            const uint32_t now = millis();
            const float base_speed =
                control_logic_base_speed(&robot_state, measurement.raw[1], now);
            const float pid =
                control_logic_pid(&robot_state, measurement.position, KP_DEFAULT, KD_DEFAULT);
            motor_control_drive(base_speed + pid, base_speed - pid);
        } else {
            control_logic_reset(&robot_state);
            motor_control_stop();
        }
        vTaskDelay(pdMS_TO_TICKS(CONTROL_LOOP_DELAY_MS));
    }
}

