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

/** @file plantilla.c
 ** @brief Plantilla para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

struct clock_s {
    uint16_t clock_ticks;         // Contador de ticks del reloj, se incrementa cada vez que se llama a ClockNewTick
    uint16_t ticks_per_second;    // Cantidad de ticks por segundo, por ejemplo 5 para 5Hz
    clock_time_t current_time;    // Hora actual del reloj
    clock_time_t alarm_time;      // Hora de la alarma
    clock_time_t postponed_until; // Hora hasta la que se pospone la alarma
    bool alarm_postponed;         // Indica si la alarma está pospuesta
    bool alarm_enabled;           // Indica si la alarma está habilitada
    bool valid;                   // Indica si la hora actual es válida (ha sido ajustada con ClockSetTime)
    bool alarm_sounded_today;     // Indica si la alarma ha sonado hoy
    void (*alarm_callback)(void); // Puntero a la función de callback para la alarma
};

/* === Private function declarations =============================================================================== */

/** @brief Verifica si la hora es válida (00:00:00 a 23:59:59).
 * @param time Puntero a la estructura clock_time_t que contiene la hora a verificar.
 * @return Devuelve true si la hora es válida, false en caso contrario.
 */

static bool ClockIsValidTime(const clock_time_t * time);

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

static bool ClockIsValidTime(const clock_time_t * time) {
    if (!time)
        return false;

    uint8_t s = time->time.seconds[1] * 10 + time->time.seconds[0];
    uint8_t m = time->time.minutes[1] * 10 + time->time.minutes[0];
    uint8_t h = time->time.hours[1] * 10 + time->time.hours[0];
    // Esto reconstruye el número decimal de cada campo y verifica:
    return (s <= 59 && m <= 59 && h <= 23);
}
/* === Public function implementation ============================================================================== */

clock_t Clock_Create(uint16_t tick_per_second) {
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s)); // Initialize the clock structure to zero
    self->ticks_per_second = tick_per_second;
    self->valid = false;
    self->alarm_postponed = false;
    self->alarm_enabled = false;
    self->alarm_sounded_today = false;
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t * result) {

    if (!self || !result) {
        return false;
    }

    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid; // Devuelve true si la hora es válida, false si no ha sido ajustada
}

bool ClockSetTime(clock_t self, const clock_time_t * new_time) {

    self->valid = ClockIsValidTime(new_time);
    memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    return self->valid;
}

void ClockNewTick(clock_t self) {
    if (!self || !self->valid)
        return;

    self->clock_ticks++;

    if (self->clock_ticks >= self->ticks_per_second) {
        self->clock_ticks = 0;

        // Verificar si estamos justo en 23:59:59
        if (self->current_time.time.hours[1] == 2 && self->current_time.time.hours[0] == 3 &&
            self->current_time.time.minutes[1] == 5 && self->current_time.time.minutes[0] == 9 &&
            self->current_time.time.seconds[1] == 5 && self->current_time.time.seconds[0] == 9) {

            memset(&self->current_time, 0, sizeof(clock_time_t));
            self->alarm_sounded_today = false;
            self->valid = true;
            return;
        }

        // Si no estamos en 23:59:59, avanzar normalmente
        self->current_time.time.seconds[0]++;
        if (self->current_time.time.seconds[0] > 9) {
            self->current_time.time.seconds[0] = 0;
            self->current_time.time.seconds[1]++;
            if (self->current_time.time.seconds[1] > 5) {
                self->current_time.time.seconds[1] = 0;
                self->current_time.time.minutes[0]++;
                if (self->current_time.time.minutes[0] > 9) {
                    self->current_time.time.minutes[0] = 0;
                    self->current_time.time.minutes[1]++;
                    if (self->current_time.time.minutes[1] > 5) {
                        self->current_time.time.minutes[1] = 0;
                        self->current_time.time.hours[0]++;
                        if (self->current_time.time.hours[0] > 9) {
                            self->current_time.time.hours[0] = 0;
                            self->current_time.time.hours[1]++;
                            // No se necesita rollover extra acá, lo manejamos antes
                        }
                    }
                }
            }
        }
    }
    // Revalidar hora (en caso de manipulación externa)
    self->valid = ClockIsValidTime(&self->current_time);

    // Cancelar alarma pospuesta si corresponde
    if (self->alarm_postponed && memcmp(&self->current_time, &self->postponed_until, sizeof(clock_time_t)) >= 0) {
        self->alarm_postponed = false;
    }

    // Ejecutar alarma si corresponde
    if (self->alarm_enabled && self->alarm_callback && !self->alarm_postponed && !self->alarm_sounded_today &&
        memcmp(&self->alarm_time, &self->current_time, sizeof(clock_time_t)) == 0) {
        self->alarm_callback();
        self->alarm_sounded_today = true;
    }
}

void ClockSetAlarm(clock_t self, const clock_time_t * alarm) {
    if (!self || !alarm)
        return;
    memcpy(&self->alarm_time, alarm, sizeof(clock_time_t));
}

void ClockGetAlarm(clock_t self, clock_time_t * alarm) {
    if (!self || !alarm)
        return;
    memcpy(alarm, &self->alarm_time, sizeof(clock_time_t));
}

void ClockEnableAlarm(clock_t self, bool enable) {
    if (self) {
        self->alarm_enabled = enable;
    }
}

void ClockAttachAlarmCallback(clock_t self, void (*callback)(void)) {
    if (self) {
        self->alarm_callback = callback;
    }
}

void ClockPostponeAlarm(clock_t self, uint8_t minutos) {
    if (!self)
        return;

    self->alarm_postponed = true;

    self->postponed_until = self->current_time;

    // Sumar minutos en BCD (simplificado)
    self->postponed_until.time.minutes[0] += minutos;
    while (self->postponed_until.time.minutes[0] > 9) {
        self->postponed_until.time.minutes[0] -= 10;
        self->postponed_until.time.minutes[1]++;
    }
    if (self->postponed_until.time.minutes[1] > 5) {
        self->postponed_until.time.minutes[1] = 0;
        self->postponed_until.time.hours[0]++;
    }
    if (self->postponed_until.time.hours[0] > 9) {
        self->postponed_until.time.hours[0] = 0;
        self->postponed_until.time.hours[1]++;
    }
    if (self->postponed_until.time.hours[1] > 2 ||
        (self->postponed_until.time.hours[1] == 2 && self->postponed_until.time.hours[0] > 3)) {
        self->postponed_until.time.hours[1] = 0;
        self->postponed_until.time.hours[0] = 0;
    }
}

/* === End of documentation ======================================================================================== */
