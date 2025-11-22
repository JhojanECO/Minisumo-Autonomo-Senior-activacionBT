#pragma once

#include <stdint.h>

// Motor pins
#define PIN_MOTOR_A_IN1 18    // Motor A IN1
#define PIN_MOTOR_A_IN2 5     // Motor A IN2
#define PIN_MOTOR_B_IN1 19    // Motor B IN1
#define PIN_MOTOR_B_IN2 21    // Motor B IN2

// Sensor pins
#define PIN_SENSOR_LEFT 33    // Sensor izquierdo
#define PIN_SENSOR_CENTER 35  // Sensor centro
#define PIN_SENSOR_RIGHT 32   // Sensor derecho

// IO pins
#define PIN_LED_R 12   // LED rojo
#define PIN_LED_G 14   // LED verde
#define PIN_LED_B 13   // LED azul
#define PIN_BUTTON 0   // Boton de seleccion

// Bluetooth
#define BT_DEVICE_NAME "Hugoautonomo"  // Nombre visible en Bluetooth

// Arranque y PID
#define PID_ENABLED 1  // 1 = usa PID despues del giro inicial, 0 = solo hace el giro y se detiene

#define START1_SPEED_LEFT 150.0f    // Estrategia 1: rueda izquierda (positivo avanza, negativo retrocede)
#define START1_SPEED_RIGHT -150.0f  // Estrategia 1: rueda derecha
#define START1_DURATION_MS 160      // Estrategia 1: tiempo del giro en ms

#define START2_SPEED_LEFT -150.0f   // Estrategia 2: rueda izquierda
#define START2_SPEED_RIGHT 150.0f   // Estrategia 2: rueda derecha
#define START2_DURATION_MS 160      // Estrategia 2: tiempo del giro en ms

#define START3_RANDOM_DIRECTION 1        // Estrategia 3: 1 = elige direccion aleatoria, 0 = usa los valores CW
#define START3_SPEED_LEFT_CW 150.0f      // Estrategia 3 horario: rueda izquierda
#define START3_SPEED_RIGHT_CW -150.0f    // Estrategia 3 horario: rueda derecha
#define START3_SPEED_LEFT_CCW -150.0f    // Estrategia 3 antihorario: rueda izquierda
#define START3_SPEED_RIGHT_CCW 150.0f    // Estrategia 3 antihorario: rueda derecha
#define START3_DURATION_MS 260           // Estrategia 3: tiempo del giro en ms

// Sensores y lineas
#define SENSOR_COUNT 3                    // Numero de sensores
#define LINE_POSITION_TARGET 50.0f        // Posicion objetivo de la linea (0-100)
#define SENSOR_KEEP_TRACK_THRESHOLD 600   // Valor para considerar que sigue viendo la linea
#define SENSOR_NOISE_THRESHOLD 600        // Valor minimo para filtrar ruido
#define SENSOR_POSITION_STEP 50           // Paso entre sensores para calcular posicion
#define SENSOR_SPEED_BOOST_THRESHOLD 3000 // Valor central para activar turbo
#define SENSOR_SPEED_BOOST_DURATION_MS 1000   // Tiempo necesario en el umbral para turbo

// Velocidades
#define BASE_SPEED_DEFAULT 150.0f  // Velocidad base normal
#define BASE_SPEED_BOOST 250.0f    // Velocidad base con turbo

// PID y lazo de control
#define CONTROL_LOOP_DELAY_MS 10  // Retardo del lazo principal en ms
#define KP_DEFAULT 1.5f           // Ganancia proporcional del PID
#define KD_DEFAULT 0.2f           // Ganancia derivativa del PID
