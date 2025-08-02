/*********************************************************************************************************************
Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * Copyright (c) 2025, Esteban Ignacio Lobo Silva <silvanacho04@gmail.com>
 * Copyright (c) 2025, Laboratorio de Electronica IV, Universidad Nacional de Tucumán, Argentina

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

#ifndef BSP_H_
#define BSP_H_

/** @file bsp.h
 ** @brief Declaraciones del módulo BOARD para la gestión de entradas y salidas digitales.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital.h"
#include <stdbool.h>
#include <stdint.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#include "digital.h"
#include "screen.h"

/* === Public data type declarations =============================================================================== */

//! Estructura que representa las entradas y salidas digitales de la placa.

typedef struct Board_s {
    digital_output_t buzzer;
    digital_output_t red_led;
    digital_output_t green_led;
    digital_output_t blue_led;

    digital_input_t set_time;  // Tecla F1
    digital_input_t set_alarm; // Tecla F2
    digital_input_t decrement; // Tecla F3
    digital_input_t increment; // Tecla F4
    digital_input_t accept;    // Tecla de aceptar
    digital_input_t cancel;    // Tecla de cancelar
    screen_t screen;           // Puntero a la pantalla
} const * Board_t;

/* === Public variable declarations ================================================================================ */

// static volatile uint32_t milisegundos; // Variable global para almacenar el tiempo en milisegundos

/* === Public function declarations ================================================================================ */

/**
 * @brief Inicializa (crea) la placa.
 *
 * @return Board_t Puntero a la instancia de la placa creada.
 * @note Esta funcion debe ser llamada una sola vez al inicio de la aplicacion.
 */

Board_t Board_Create(void);

/**
 * @brief Inicializa el temporizador del sistema.
 * Configura el SysTick para generar interrupciones cada 1 ms.
 */
void SysTickInt(void);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* BSP_H_ */
