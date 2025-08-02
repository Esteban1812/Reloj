/*********************************************************************************************************************
* Facultad de Ciencias Exactas y Tecnología
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

/** @file clock.c
 ** @brief Codigo fuente del módulo CLOCK para la gestión de la hora (00.00.00) y alarma.
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
    bool valid_alarm;             // Indica si hay una alarma configurada
    bool alarm_sounded_today;     // Indica si la alarma ha sonado hoy
    bool alarm_cancelled_today;   // Indica si la alarma fue cancelada hoy
    bool alarm_is_sounding;       // Indica si la alarma está sonando actualmente
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
    memset(self, 0, sizeof(struct clock_s)); // inicializa el reloj en cero
    self->ticks_per_second = tick_per_second;
    self->valid = false;
    self->alarm_postponed = false;
    self->alarm_enabled = false;
    self->alarm_sounded_today = false;
    self->alarm_cancelled_today = false;
    self->alarm_is_sounding = false; // Inicializa el estado de alarma sonando
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

    if (!self || !new_time || !ClockIsValidTime(new_time)) {
        self->valid = false; // Marca el reloj como no válido si la hora es incorrecta
    } else {
        self->valid = true; // Marca el reloj como válido si la hora es correcta
        memcpy(&self->current_time, new_time, sizeof(clock_time_t)); // Copia la nueva hora al reloj
    }
    return self->valid;
}

void ClockNewTick(clock_t self) {
    if (!self || !self->valid)
        return;

    self->clock_ticks++;

    if (self->clock_ticks >= self->ticks_per_second) {
        self->clock_ticks = 0;

        // Si estamos justo en 23:59:59 → reiniciar
        if (self->current_time.time.hours[1] == 2 && self->current_time.time.hours[0] == 3 &&
            self->current_time.time.minutes[1] == 5 && self->current_time.time.minutes[0] == 9 &&
            self->current_time.time.seconds[1] == 5 && self->current_time.time.seconds[0] == 9) {
            self->alarm_cancelled_today = false;

            memset(&self->current_time, 0, sizeof(clock_time_t));
            self->alarm_sounded_today = false;
            self->valid = true;
            return;
        }

        // Incrementar el tiempo en BCD
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
                        }
                    }
                }
            }
        }
    }

    self->valid = ClockIsValidTime(&self->current_time);

    // Cancelar posponer si llegó la hora
    if (self->alarm_postponed) {
        if (memcmp(&self->current_time, &self->postponed_until, sizeof(clock_time_t)) == 0) {
            self->alarm_postponed = false;
            self->alarm_sounded_today = true; // <- clave: evita que se dispare nuevamente más adelante
            self->alarm_is_sounding = true;   // Indica que la alarma está sonando
            // Ejecutar el callback de alarma si está habilitado
            if (self->alarm_enabled && self->alarm_callback) {
                self->alarm_callback();
            }
        }
        return; // <- clave: si estaba pospuesta, no seguir ejecutando
    }

    // Ejecutar alarma solo si no está pospuesta
    if (self->alarm_enabled && self->alarm_callback && !self->alarm_postponed && !self->alarm_sounded_today &&
        !self->alarm_cancelled_today && memcmp(&self->alarm_time, &self->current_time, sizeof(clock_time_t)) == 0) {

        self->alarm_callback();
        self->alarm_sounded_today = true;
        self->alarm_is_sounding = true; // Indica que la alarma está sonando
    }
}

void ClockSetAlarm(clock_t self, const clock_time_t * alarm) {
    if (!ClockIsValidTime(alarm) && self) {
        self->valid_alarm = false;
    }
    memcpy(&self->alarm_time, alarm, sizeof(clock_time_t));
    self->alarm_time.time.seconds[0] = 0;
    self->alarm_time.time.seconds[1] = 0;
    self->alarm_enabled = true;
    self->alarm_sounded_today = false;
    self->alarm_is_sounding = false;
    self->valid_alarm = true;
}

bool ClockGetAlarm(clock_t self, clock_time_t * alarm) {
    if (!self || !alarm) {
        return false;
    }
    memcpy(alarm, &self->alarm_time, sizeof(clock_time_t));
    return self->valid_alarm;
}

void ClockEnableAlarm(clock_t self, bool enable) {
    if (self) {
        self->alarm_enabled = enable;
    }
    if (!enable) {
        self->alarm_postponed = false;     // Desactiva la alarma pospuesta al deshabilitar la alarma
        self->alarm_sounded_today = false; // Resetea el estado de alarma sonando hoy
        self->alarm_is_sounding = false;   // Resetea el estado de alarma sonando
    }
}

void ClockAttachAlarmCallback(clock_t self, void (*callback)(void)) {
    if (self) {
        self->alarm_callback = callback;
    }
}

void ClockPostponeAlarm(clock_t self, uint32_t minutos) {
    if (!self || !self->valid || minutos == 0)
        return;

    clock_time_t * base = self->alarm_postponed ? &self->postponed_until : &self->current_time;
    self->postponed_until = *base;

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

    self->alarm_postponed = true;
    self->alarm_sounded_today = false;
    self->alarm_is_sounding = false;
}

void ClockPostponeAlarmUntilTomorrow(clock_t self) {
    if (!self || !self->valid)
        return;
    self->postponed_until = self->alarm_time;
    // sumar un día en horas BCD
    self->postponed_until.time.hours[1]++;
    if (self->postponed_until.time.hours[1] > 2 ||
        (self->postponed_until.time.hours[1] == 2 && self->postponed_until.time.hours[0] > 3)) {
        self->postponed_until.time.hours[1] = 0;
        self->postponed_until.time.hours[0] = 0;
    }
    self->alarm_postponed = true;
    self->alarm_enabled = true;
    self->alarm_sounded_today = false;
    self->alarm_is_sounding = false;
}

bool ClockIsAlarmEnabled(clock_t self) {
    return self->alarm_enabled;
}
bool ClockIsAlarmPostponed(clock_t self) {
    return self->alarm_postponed;
}
bool ClockIsAlarmSoundedToday(clock_t self) {
    return self->alarm_sounded_today;
}

bool ClockIsAlarmRinging(clock_t self) {
    return self->alarm_is_sounding;
}

/* === End of documentation ======================================================================================== */
