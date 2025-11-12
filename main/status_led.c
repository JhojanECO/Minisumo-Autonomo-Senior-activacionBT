#include "status_led.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void configure_led_pin(gpio_num_t pin) {
    const gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);
    gpio_set_level(pin, 0);
}

void status_led_init(void) {
    configure_led_pin(PIN_LED_R);
    configure_led_pin(PIN_LED_G);
    configure_led_pin(PIN_LED_B);
}

void status_led_set(bool red, bool green, bool blue) {
    gpio_set_level(PIN_LED_R, red);
    gpio_set_level(PIN_LED_G, green);
    gpio_set_level(PIN_LED_B, blue);
}

void status_led_show_strategy(int strategy) {
    switch (strategy) {
        case 1:
            status_led_set(false, false, true);
            break;
        case 2:
            status_led_set(false, true, false);
            break;
        case 3:
        default:
            status_led_set(true, false, true);
            break;
    }
    vTaskDelay(pdMS_TO_TICKS(800));
    status_led_set(false, false, false);
}

