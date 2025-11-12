#include <Arduino.h>

extern "C" {
#include "status_led.h"
}

static void configure_led_pin(uint8_t pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void status_led_init(void) {
    configure_led_pin(PIN_LED_R);
    configure_led_pin(PIN_LED_G);
    configure_led_pin(PIN_LED_B);
}

void status_led_set(bool red, bool green, bool blue) {
    digitalWrite(PIN_LED_R, red ? HIGH : LOW);
    digitalWrite(PIN_LED_G, green ? HIGH : LOW);
    digitalWrite(PIN_LED_B, blue ? HIGH : LOW);
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
    delay(800);
    status_led_set(false, false, false);
}
