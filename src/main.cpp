#include <Arduino.h>
#include <BluetoothSerial.h>

#include "control_logic.h"
#include "hardware_config.h"
#include "motor_control.h"
#include "sensor_array.h"
#include "status_led.h"

static robot_state_t robot_state;
static bool go_command_active = false;
static BluetoothSerial SerialBT;

static void configure_input(uint8_t pin, bool pullup) {
    pinMode(pin, pullup ? INPUT_PULLUP : INPUT_PULLDOWN);
}

static void init_inputs(void) {
    configure_input(PIN_BUTTON, true);
}

static bool is_button_pressed(void) {
    return digitalRead(PIN_BUTTON) == LOW;
}

static bool go_is_active(void) {
    return go_command_active;
}

static void handle_bluetooth_commands(void) {
    while (SerialBT.available() > 0) {
        const int incoming = SerialBT.read();
        if (incoming == '1') {
            go_command_active = true;
            Serial.println("GO activado via Bluetooth");
            SerialBT.println("GO activado");
        } else if (incoming == '0') {
            go_command_active = false;
            Serial.println("GO desactivado via Bluetooth");
            SerialBT.println("GO desactivado");
        }
    }
}

static int read_button_event(void) {
    if (!is_button_pressed()) {
        return 0;
    }

    status_led_set(true, false, false);
    const uint32_t start_ms = millis();
    while (is_button_pressed()) {
        if (millis() - start_ms >= 1000) {
            status_led_set(true, false, true);
            delay(100);
            status_led_set(false, false, false);
            return 2;
        }
        delay(20);
    }
    status_led_set(false, false, false);
    return 1;
}

static int select_strategy(void) {
    int strategy = 0;
    Serial.println("Selecciona la estrategia con pulsaciones cortas, confirma con pulsacion larga");
    while (true) {
        const int event = read_button_event();
        if (event == 1) {
            strategy = (strategy < 3) ? strategy + 1 : strategy;
            status_led_show_strategy(strategy);
        } else if (event == 2) {
            delay(1000);
            break;
        }
        delay(25);
    }
    if (strategy < 1) {
        strategy = 1;
    }
    Serial.print("Estrategia confirmada: ");
    Serial.println(strategy);
    return strategy;
}

static void minisumo_loop(void) {
    if (go_is_active()) {
        control_logic_execute_start(&robot_state);
        if (!PID_ENABLED) {
            motor_control_stop();
            return;
        }
        const sensor_measurement_t measurement = sensor_array_read();
        const uint32_t now_ms = millis();
        const float base_speed =
            control_logic_base_speed(&robot_state, measurement.raw[1], now_ms);
        const float pid =
            control_logic_pid(&robot_state, measurement.position, KP_DEFAULT, KD_DEFAULT);
        motor_control_drive(base_speed + pid, base_speed - pid);
    } else {
        control_logic_reset(&robot_state);
        motor_control_stop();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Inicializando Bluetooth...");
    if (!SerialBT.begin(BT_DEVICE_NAME)) {// Nombre del dispositivo Bluetooth
        Serial.println("Fallo al iniciar Bluetooth Serial");
    } else {
        Serial.print("Bluetooth listo (nombre: ");
        Serial.print(BT_DEVICE_NAME);
        Serial.println(")");
        SerialBT.println("Conectado a Minisumo. Envia '1' para GO y '0' para detener.");
    }
    status_led_init();
    init_inputs();
    motor_control_init();
    sensor_array_init();

    control_logic_init(&robot_state);

    const int strategy = select_strategy();
    control_logic_set_strategy(&robot_state, strategy);
    status_led_show_strategy(strategy);
    Serial.println("Usa Bluetooth (enviar '1'/'0') para controlar el GO virtual.");
    SerialBT.println("Seleccion completada. Envia '1' para arrancar, '0' para detener.");
}

void loop() {
    handle_bluetooth_commands();
    minisumo_loop();
    delay(CONTROL_LOOP_DELAY_MS);
}
