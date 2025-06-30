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
-15- Fijar la alarma, deshabilitarla y avanzar el reloj para NO suene.
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

static bool alarm = false;
// Variable para indicar si la alarma ha sonado, se usa en el callback de la alarma
// Se inicializa en false y se cambia a true cuando la alarma suena

/* === Public variable definitions ================================================================================= */

clock_t clock; // Declaramos una variable global de tipo clock_t para el reloj

/* === Private function definitions ================================================================================ */

static void SimulateSeconds(clock_t clock, uint32_t seconds) {
    for (uint32_t i = 0; i < CLOCK_TICK_PER_SECOND * seconds; i++) {
        ClockNewTick(clock);
    }
}


static void AlarmRinging(void) {
    alarm = true;
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

/**
 * @brief Probar get_time con NULL como argumento.
 *
 * Esta prueba verifica que la función ClockGetTime no se rompa al pasar un puntero nulo.
 * La función debe devolver false pero NO romper.
 */
void test_get_time_with_null_pointer(void) {
    clock = Clock_Create(CLOCK_TICK_PER_SECOND); 

    // No seteamos la hora, está inválida por defecto
    bool result = ClockGetTime(clock, NULL);

    // La función debe devolver false pero NO romper
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Tratar de ajustar la hora del reloj con valores invalidos y verificar que no pasa la prueba.
 *
 * Esta prueba verifica que la función ClockSetTime no permita establecer una hora inválida.
 */

void test_set_time_with_invalid_values(void) {
    clock_time_t invalid_time = {
        .time = {
            .seconds = {8, 7},  // 78
            .minutes = {5, 6},  // 65
            .hours = {5, 4}     // 45
        }
    }; //defino un tiempo inválido 45:65:78

    clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    bool result = ClockSetTime(clock, &invalid_time);

    TEST_ASSERT_FALSE(result);
}


/**
 * @brief Hacer una prueba con frecuencia de reloj diferente a 5.
 *
 * Esta prueba verifica que el reloj funcione correctamente con una frecuencia de 10Hz.
 * Se espera que después de 10 ticks, el reloj avance un segundo.
 */
void test_clock_with_different_tick_frequency(void) {
    // Creo el reloj con frecuencia 10 Hz
    clock = Clock_Create(10);

    clock_time_t t0 = {0};  // Seteo hora en 00:00:00
    ClockSetTime(clock, &t0);

    for (int i = 0; i < 10; i++) { // Simulo 10 ticks, debería avanzar 1 segundo
        ClockNewTick(clock);
    }

    clock_time_t result;
    TEST_ASSERT_TRUE(ClockGetTime(clock, &result));

    TEST_ASSERT_EQUAL_UINT8(1, result.bcd[0]); // segundos unidades = 1
    TEST_ASSERT_EQUAL_UINT8(0, result.bcd[1]); // segundos decenas = 0
}

/**
 * @brief Fijar la hora de la alarma y consultarla.
 *
 * Esta prueba verifica que se pueda establecer una hora de alarma y luego recuperarla correctamente.
 */

void test_set_and_get_alarm_time(void) {
    clock_time_t alarm = {
        .time = {
            .hours = {2, 1},    // 12
            .minutes = {5, 4},  // 45
            .seconds = {3, 2}   // 23
        }
    };

    clock_time_t result;

    clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    ClockSetAlarm(clock, &alarm);
    ClockGetAlarm(clock, &result);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarm.bcd, result.bcd, 6);
}


/**
 * @brief prueba mínima para confirmar que ClockAttachAlarmCallback() realmente ejecuta el callback
 * 
 */
void test_alarm_callback_executes(void) {
    clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    alarm = false;
clock_time_t hora = {.bcd = {0, 0, 0, 0, 0, 0}};
    ClockAttachAlarmCallback(clock, AlarmRinging);
    ClockEnableAlarm(clock, true);
    ClockSetAlarm(clock, &hora);
    ClockSetTime(clock, &hora);

    SimulateSeconds(clock, 1);

    TEST_ASSERT_TRUE_MESSAGE(alarm, "La alarma no fue ejecutada aunque la hora coincidia"); 
}

/**
 * @brief Fijar la alarma y avanzar el reloj para que suene.
 *
 * Esta prueba verifica que la alarma suene cuando el tiempo del reloj coincide con la hora de la alarma.
 */

void test_alarm_sounds_when_time_matches(void) {
    clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    ClockAttachAlarmCallback(clock, AlarmRinging);

    clock_time_t alarma = {
        .time = {
            .hours = {1, 2},     // 21
            .minutes = {4, 3},   // 34
            .seconds = {0, 0}    // 00
        }
    };

    clock_time_t inicio = {
        .time = {
            .hours = {1, 2},     // 21
            .minutes = {3, 3},   // 33
            .seconds = {9, 5}    // 59
        }
    };

    ClockSetTime(clock, &inicio);
    ClockSetAlarm(clock, &alarma);
    ClockEnableAlarm(clock, true);

    alarm = false;

    // Simular un segundo: pasar de 59 a 00, y minuto de 24 a 25
    SimulateSeconds(clock, 1);  // 21:34:00

    TEST_ASSERT_TRUE_MESSAGE(alarm, "La alarma no sono cuando debia.");
}

/**
 * @brief Fijar la alarma, deshabilitarla y avanzar el reloj para NO suene.
 *
 * Esta prueba verifica que la alarma no suene si está deshabilitada, incluso si el tiempo del reloj coincide con la hora de la alarma.
 */

void test_alarm_does_not_sound_if_disabled(void) {
    clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    alarm = false;

    clock_time_t hora = {.bcd = {0, 0, 0, 0, 0, 0}}; // 00:00:00

    ClockAttachAlarmCallback(clock, AlarmRinging);
    ClockSetAlarm(clock, &hora);
    ClockEnableAlarm(clock, false);     // Alarma deshabilitada
    ClockSetTime(clock, &hora);         // Ajustar hora al mismo valor

    SimulateSeconds(clock, 1);          // Avanza a la hora de la alarma

    TEST_ASSERT_FALSE_MESSAGE(alarm, "La alarma sonó aunque estaba deshabilitada");
}

/* === End of documentation ========================================================================================*/
