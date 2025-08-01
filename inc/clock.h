/*********************************************************************************************************************
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * Copyright (c) 2025, Esteban Ignacio Lobo Silva <nachosilva04.com>
 * Copyright (c) 2025, Laboratorio de Electronica IV, Universidad Nacional de Tucumán, Argentina
 *

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

#ifndef CLOCK_H_
#define CLOCK_H_

/** @file clock.h
 ** @brief Declaraciones del módulo para la gestión de la hora (00.00.00).
 **
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>
#include "stdint.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

typedef union {
    struct {
        uint8_t seconds[2]; // [0] unidad, [1] decena
        uint8_t minutes[2]; // [0] unidad, [1] decena
        uint8_t hours[2];   // [0] unidad, [1] decena
    } time;
    uint8_t bcd[6]; // Representación BCD de la hora: [0] seg. unidad, [1] seg. decena, [2] min. unidad, [3] min.
                    // decena, [4] hora unidad, [5] hora decena
} clock_time_t;

typedef struct clock_s * clock_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea una instancia del reloj.
 *
 * @note Esta función debe ser llamada antes de usar cualquier otra función del reloj.
 * @param tick_per_second Frecuencia del reloj en ticks por segundo (por ejemplo, 5 para 5Hz).
 *                        cantidad de pulsos que recibe para contar un segundo.
 * @return Devuelve un puntero a la instancia del reloj.
 */

clock_t Clock_Create(uint16_t tick_per_second);

/**
 * @brief Copia la hora actual (si es válida) en la estructura result.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @param current_time Estructura que contiene la hora a ajustar.
 * @return Devuelve true si la hora está “en hora” (o sea, si el reloj fue ajustado con ClockSetTime()).
 * @return Devuelve false si la hora aún no fue ajustada (es inválida).
 */

bool ClockGetTime(clock_t self, clock_time_t * result);

/**
 * @brief Ajusta la hora del reloj a un nuevo valor.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @param new_time Estructura que contiene la nueva hora a establecer.
 * @return bool true si la hora se ajustó correctamente, false si no.
 */

bool ClockSetTime(clock_t self, const clock_time_t * new_time);

/**
 * @brief Funcion para contar un tick del reloj y actualizar la hora.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @note Esta función debe ser llamada periódicamente para simular el avance del tiempo.
 */

void ClockNewTick(clock_t self);

/**
 * @brief Funcion para obtener la hora de la alarma. y el estado de la alarma.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @param alarm Estructura que contiene la hora de la alarma a establecer.
 * @return Devuelve true si la alarma está configurada, false si no.
 * @note La alarma se puede consultar posteriormente con ClockGetAlarm().
 */
bool ClockGetAlarm(clock_t self, clock_time_t * alarm);

/**
 * @brief setea la hora de la alarma del reloj.
 *
 * @param self puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @param alarm estructura que contiene la hora de la alarma a establecer.
 * @note La alarma se puede consultar posteriormente con ClockGetAlarm().
 */

void ClockSetAlarm(clock_t self, const clock_time_t * alarm);

/**
 * @brief Habilita o deshabilita la alarma del reloj.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @param enable true para habilitar la alarma, false para deshabilitarla.
 */

void ClockEnableAlarm(clock_t self, bool enable);

/**
 * @brief Adjunta un callback que se ejecutará cuando la alarma se active.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @param callback Puntero a la función que se llamará cuando la alarma se active.
 * @note El callback debe ser una función sin parámetros y sin valor de retorno.
 */

void ClockAttachAlarmCallback(clock_t self, void (*callback)(void));

/**
 * @brief Pospone la alarma por un número específico de minutos.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @param minutos Número de minutos para posponer la alarma.
 * @note La alarma se pospondrá hasta el siguiente día si se excede el tiempo actual.
 */

void ClockPostponeAlarm(clock_t self, uint32_t minutos);

/**
 * @brief Verifica si la alarma está habilitada.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @return true si la alarma está habilitada, false en caso contrario.
 */

bool ClockIsAlarmEnabled(clock_t self);

/**
 * @brief Verifica si la alarma está pospuesta.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @return true si la alarma está pospuesta, false en caso contrario.
 */

bool ClockIsAlarmPostponed(clock_t self);

/**
 * @brief Verifica si la alarma ha sonado hoy.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @return true si la alarma ha sonado hoy, false en caso contrario.
 */

bool ClockIsAlarmSoundedToday(clock_t self);


/**
 * @brief Verifica si la alarma está sonando actualmente.
 *
 * @param self Puntero a la instancia del reloj devuelta por la función Clock_Create.
 * @return true si la alarma está sonando, false en caso contrario.
 */
bool ClockIsAlarmRinging(clock_t self);


/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /**  CLOCK_H_ */
