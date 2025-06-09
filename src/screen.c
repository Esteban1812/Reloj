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

/** @file screen.c
 ** @brief Codigo fuente del módulo para la gestión de la pantalla de 7 segmentos 4 digitos multiplexada.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "screen.h"
#include "poncho.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif

/* === Private data type declarations ============================================================================== */

struct screen_s {
    uint8_t digits;                   // numero de digitos de la pantalla
    uint8_t value[SCREEN_MAX_DIGITS]; // valores a mostrar en la pantalla
    uint8_t current_digit;            // digito actual que se esta mostrando
    screen_driver_t driver;           // puntero a la estructura de driver de pantalla
    uint8_t flashing_from;            // rango de digitos a parpadear
    uint8_t flashing_to;              // rango de digitos a parpadear
    uint8_t flashing_count;           // contador de parpadeo
    uint16_t flashing_frequency;      // frecuencia de parpadeo en milisegundos
};

/* === Private function declarations =============================================================================== */

static const uint8_t IMAGES[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G              // 9
};

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

screen_t ScreenCreate(uint8_t digits, screen_driver_t driver) {
    screen_t self = malloc(sizeof(struct screen_s));

    if (digits > SCREEN_MAX_DIGITS) {
        digits = SCREEN_MAX_DIGITS;
    }
    if (self != NULL) {
        self->digits = digits;
        self->driver = driver;
        self->current_digit = 0;
        self->flashing_count = 0;
        self->flashing_frequency = 0;
    }
    return self;
}

void ScreenWriteBCD(screen_t screen, uint8_t value[], uint8_t size) {
    memset(screen->value, 0, sizeof(screen->value));
    if (size > screen->digits) {
        size = screen->digits;
    }
    for (uint8_t i = 0; i < size; i++) {
        screen->value[i] = IMAGES[value[i]];
    }
}

void ScreenRefresh(screen_t self) {
    uint8_t segments;

    self->driver->DigitsTurnOff();
    self->current_digit = (self->current_digit + 1) % self->digits;
    
    segments = self->value[self->current_digit];

    if (self->flashing_frequency != 0) {
        if (self->current_digit == 0) {
            self->flashing_count = (self->flashing_count + 1) % (self->flashing_frequency);
        }
        if (self->flashing_count < (self->flashing_frequency / 2)) {
            segments = 0; // Apagar segmentos si el contador de parpadeo esta en la primera mitad del ciclo
        }
    }

    self->driver->SegmentsUpdate(segments);
    self->driver->DigitsTurnOn(self->current_digit);
}

int DisplayFlashDigits(screen_t self, uint8_t from, uint8_t to, uint16_t divisor) {
    int result = 0;
    if ((from > to) || (from >= SCREEN_MAX_DIGITS) || (to >= SCREEN_MAX_DIGITS)) {
        result = -1;
    } else if (!self) {
        result = -1;
    } else {
        self->flashing_from = from;
        self->flashing_to = to;
        self->flashing_frequency = 2 * divisor; // Multiplicamos por 2 para tener en cuenta el tiempo de encendido y apagado
        self->flashing_count = 0;
    }

    return result;
}

/* === End of documentation ======================================================================================== */
