/*********************************************************************************************************************
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * Copyright (c) 2025, Esteban Ignacio Lobo Silva <nachosilva04.com>
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

/** @file test_reloj.c
 ** @brief Codigo fuente para la gestión de las pruebas del reloj.
 **/

/**
 * Pruebas del reloj:

- Despues de n ciclos de reloj la hora avanza un segundo, diez segundos, un minuto, diez minutos, una hora, diez horas y
un día completo.
- Al inicializar el reloj está en 00:00 y con hora invalida.
- Al ajustar la hora el reloj queda en hora y es válida.
- Después de n ciclos de reloj la hora avanza un segundo, diez segundos, un minutos, diez minutos, una hora, diez horas
y un día completo.
- Fijar la hora de la alarma y consultarla.
- Fijar la alarma y avanzar el reloj para que suene.
- Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
- Hacer sonar la alarma y posponerla.
- Hacer sonar la alarma y cancelarla hasta el otro dia.
- Probar get_time con NULL como argumento.
- Tratar de ajustar la hora del reloj con valores invalidos y verificar que no pasa la prueba.
- Hacer una prueba con frecuencia de reloj deferente a 5
 *
 */

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */

#define CLOCK_TICK_PER_SECOND 5 // Definimos la frecuencia del reloj a 5Hz

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

void SimulateSeconds(clock_t clock, uint8_t seconds) {
    for (uint8_t i = 0; i < CLOCK_TICK_PER_SECOND * seconds; i++) {
        ClockNewTick(clock);
    }
}

/* === Public function implementation ============================================================================== */

/**
 * @brief Al inicializar el reloj está en 00:00 y con hora invalida.
 *
 */
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {
        .bcd = {5, 5, 5, 5, 5, 5}
        // coloco una Hora invalida para hacer que la prueba falle, y no pase de suerte como me paso
    };

    clock_t clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

/**
 * @brief Al ajustar la hora del reloj con valores correctos, queda en hora y es válida.
 * primero creo una hora valida(con valores correctos), luego la ajusto y verifico que quede en 00:00:00
 */

void test_set_up_and_adjust_with_valid_time(void) {
    clock_time_t new_time = {.time = {.seconds = {2, 5}, .minutes = {4, 7}, .hours = {1, 6}}};
    clock_time_t current_time = {0};
    clock_t clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TRUE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(new_time.bcd, current_time.bcd, 6);
}

/**
 * @brief Después de n ciclos de reloj la hora avanza un segundo, diez segundos, un minuto, diez minutos, una hora,
 * diez horas y un día completo.
 *
 */

void test_clock_advance_one_second(void) {
    clock_time_t current_time = {0};
    static const clock_time_t espected_value = {.time = {.seconds = {1, 0}, .minutes = {0, 0}, .hours = {0, 0}}};

    clock_t clock = Clock_Create(CLOCK_TICK_PER_SECOND); // Crear el reloj con una frecuencia de 5Hz

    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 1); // simulo el paso de un segundo
    ClockGetTime(clock, &current_time);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(espected_value.bcd, current_time.bcd, 6);

    // Avanzar un segundo   
}
/* === End of documentation ======================================================================================== */
