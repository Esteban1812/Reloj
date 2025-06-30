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

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

struct clock_s {
    uint16_t clock_ticks;
    clock_time_t current_time;
    bool valid;
    uint16_t ticks_per_second;
    clock_time_t alarm_time;
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
    if (!time) return false;

    uint8_t s = time->time.seconds[1] * 10 + time->time.seconds[0];
    uint8_t m = time->time.minutes[1] * 10 + time->time.minutes[0];
    uint8_t h = time->time.hours[1]   * 10 + time->time.hours[0];
//Esto reconstruye el número decimal de cada campo y verifica:
    return (s <= 59 && m <= 59 && h <= 23);
}
/* === Public function implementation ============================================================================== */

clock_t Clock_Create(uint16_t tick_per_second) {
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s)); // Initialize the clock structure to zero
    self->ticks_per_second = tick_per_second;
    self->valid = false;
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t * result) {

    if (!self || !result) {
        return false;
    }

    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid;
}

bool ClockSetTime(clock_t self, const clock_time_t * new_time) {
    if (!self || !ClockIsValidTime(new_time)) {
        return false;
    }

    self->valid = true;
    memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    return self->valid;
}

void ClockNewTick(clock_t self) {
    self->clock_ticks++;
    if (self->clock_ticks >= self->ticks_per_second) {             // Assuming 5 ticks per second
        self->clock_ticks = 0;                // Reset ticks after 5 ticks (1 second)
        self->current_time.time.seconds[0]++; // Increment seconds
        if (self->current_time.time.seconds[0] > 9) {
            self->current_time.time.seconds[0] = 0; // Reset seconds to 0
            self->current_time.time.seconds[1]++;   // Increment tens of seconds
            if (self->current_time.time.seconds[1] > 5) {
                self->current_time.time.seconds[1] = 0; // Reset tens of seconds to 0
                self->current_time.time.minutes[0]++;   // Increment minutes
                if (self->current_time.time.minutes[0] > 9) {
                    self->current_time.time.minutes[0] = 0; // Reset minutes to 0
                    self->current_time.time.minutes[1]++;   // Increment tens of minutes
                    if (self->current_time.time.minutes[1] > 5) {
                        self->current_time.time.minutes[1] = 0; // Reset tens of minutes to 0
                        self->current_time.time.hours[0]++;     // Increment hours
                        if (self->current_time.time.hours[0] > 9) {
                            self->current_time.time.hours[0] = 0; // Reset hours to 0
                            self->current_time.time.hours[1]++;   // Increment tens of hours
                            if (self->current_time.time.hours[1] > 2 ||
                                (self->current_time.time.hours[1] == 2 && self->current_time.time.hours[0] > 3)) {
                                memset(&self->current_time.time, 0,
                                       sizeof(self->current_time.time)); // Reset time to 00:00:00
                                // self->valid = false; // Invalidate the clock time if it exceeds 23:59:59
                            }
                        }
                    }
                }
            }
        }
    }
}

void ClockSetAlarm(clock_t self, const clock_time_t * alarm) {
    if (!self || !alarm) return;
    memcpy(&self->alarm_time, alarm, sizeof(clock_time_t));
}

void ClockGetAlarm(clock_t self, clock_time_t * alarm) {
    if (!self || !alarm) return;
    memcpy(alarm, &self->alarm_time, sizeof(clock_time_t));
}
/* === End of documentation ======================================================================================== */
