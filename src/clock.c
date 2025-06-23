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
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

clock_t Clock_Create(uint16_t tick_per_second) {
    (void)tick_per_second; // Suppress unused parameter warning, as this is a placeholder for future use
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s)); // Initialize the clock structure to zero
    self->valid = false;
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t * result) {

    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid;
}

bool ClockSetTime(clock_t self, const clock_time_t * new_time) {
    self->valid = true;
    memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    return self->valid;
}

void ClockNewTick(clock_t self) {
    self->clock_ticks++;
    if (self->clock_ticks == 5) {             // Assuming 5 ticks per second
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
                                self->current_time.time.hours[1] = 0; // Reset tens of hours to 0
                            }
                        }
                    }
                }
            }
        }
    }
}
/* === End of documentation ======================================================================================== */
