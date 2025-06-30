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

/** @file test_clock.c
 ** @brief Codigo fuente para la gestión de las pruebas del reloj.
 **/

/**
 * Pruebas del reloj:

-1- Al inicializar el reloj está en 00:00 y con hora invalida.
-2- Al ajustar la hora el reloj queda en hora y es válida.
-3- Después de n ciclos de reloj la hora avanza un segundo.
-4- Después de n ciclos de reloj la hora avanza diez segundos.
-5- Después de n ciclos de reloj la hora avanza un minuto.
-6- Después de n ciclos de reloj la hora avanza diez minutos.
-7- Después de n ciclos de reloj la hora avanza una hora.
-8- Después de n ciclos de reloj la hora avanza diez horas.
-9- Después de n ciclos de reloj la hora avanza un día completo.
-10- Probar get_time con NULL como argumento.
-11- Tratar de ajustar la hora del reloj con valores invalidos y verificar que no pasa la prueba.
-12- Hacer una prueba con frecuencia de reloj deferente a 5
-13- Fijar la hora de la alarma y consultarla.
-14- Fijar la alarma y avanzar el reloj para que suene.
-15- Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
-16- Hacer sonar la alarma y posponerla.
-17- Hacer sonar la alarma y cancelarla hasta el otro dia.

 *
 */

/* === Headers files inclusions ==================================================================================== */

#include "unity.h"
#include "clock.h"

/* === Macros definitions ========================================================================================== */

#define CLOCK_TICK_PER_SECOND 5 // Definimos la frecuencia del reloj a 5Hz
#define TEST_ASSERT_TIME(hours_tens, hours_units, minutes_tens, minutes_units, seconds_tens, seconds_units,            \
                         current_time)                                                                                 \
    clock_time_t current_time = {0};                                                                                   \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "El reloj tiene una hora invalida");                  \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, current_time.bcd[0], "Diferencia en las unidades de los segundos"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens, current_time.bcd[1], "Diferencia en las decenas de los segundos");   \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, current_time.bcd[2], "Diferencia en las unidades de los minutos");  \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens, current_time.bcd[3], "Diferencia en las decenas de los minutos");    \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_units, current_time.bcd[4], "Diferencia en las unidades de las horas");      \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_tens, current_time.bcd[5], "Diferencia en las decenas de las horas");

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

clock_t clock; // Declaramos una variable global de tipo clock_t para el reloj

/* === Private function definitions ================================================================================ */

static void SimulateSeconds(clock_t clock, uint32_t seconds) {
    for (uint32_t i = 0; i < CLOCK_TICK_PER_SECOND * seconds; i++) {
        ClockNewTick(clock);
    }
}

/* === Public function implementation ============================================================================== */

void setUp(void) {
    // Esta función se ejecuta antes de cada prueba
    // Aquí puedes inicializar cualquier recurso necesario para las pruebas

    clock = Clock_Create(CLOCK_TICK_PER_SECOND); // Crear el reloj con una frecuencia de 5Hz
}

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
    clock_time_t new_time = {.time = {.hours = {6, 1}, .minutes = {7, 4}, .seconds = {5, 2}}}; // 16:47:25 en BCD
    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TIME(1, 6, 4, 7, 2, 5, current_time); // Verifico que la hora sea 16:47:25
}

/**
 * @brief Después de n ciclos de reloj la hora avanza un segundo.
 *
 */

void test_clock_advance_one_second(void) {
    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 1); // simulo el paso de un segundo

    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1, current_time); // Verifico que la hora sea 00:00:01
}

/**
 * @brief Después de n ciclos de reloj la hora avanza diez segundos.
 *
 */

void test_clock_advance_ten_seconds(void) {
    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 10);                       // simulo el paso de diez segundos
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0, current_time); // Verifico que la hora sea 00:00:10
}

/**
 * @brief Después de n ciclos de reloj la hora avanza un minuto
 *
 */

void test_clock_advance_one_minute(void) {
    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 60);                       // simulo el paso de 60 segundos
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0, current_time); // Verifico que la hora sea 00:01:00
}

/**
 * @brief Después de n ciclos de reloj la hora avanza diez minutos.
 *
 */

void test_clock_advance_ten_minutes(void) {
    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 600);                       // simulo el paso de 600 segundos
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0, current_time); // Verifico que la hora sea 00:10:00
}

/**
 * @brief Después de n ciclos de reloj la hora avanza una hora
 *
 */

void test_clock_advance_one_hour(void) {
    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 3600);                       // simulo el paso de 3600 segundos
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0, current_time); // Verifico que la hora sea 01:00:00
}

/**
 * @brief Después de n ciclos de reloj la hora avanza diez horas
 *
 */

void test_clock_advance_ten_hours(void) {
    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 36000);                       // simulo el paso de 36000 segundos. Diez horas
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0, current_time); // Verifico que la hora sea 10:00:00
}

/**
 * @brief Después de n ciclos de reloj la hora avanza un día completo
 *
 */

void test_clock_advance_one_day(void) {
    // Inicializar el reloj a 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});

    SimulateSeconds(clock, 86400);                       // simulo el paso de 86400 segundos. Un día completo
    TEST_ASSERT_TIME(2, 4, 0, 0, 0, 0, current_time); // Verifico que la hora sea 00:00:00
}
/* === End of documentation ========================================================================================*/
