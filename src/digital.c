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

/** @file digitales.c
 ** @brief Codigo fuente del módulo para la gestión de entradas y salidas digitales.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "chip.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/*! Estructura que representa una salida digital*/
struct digital_output_s {
    uint8_t port; /*!< Puerto de la salida digital */
    uint8_t pin;  /*!< Pin de la salida digital */
};

/*! Estructura que representa una entrada digital*/
struct digital_input_s {
    uint8_t port;    /*!< Puerto de la entrada digital */
    uint8_t pin;     /*!< Pin de la entrada digital */
    bool inverted;   /*!< logica de entrada digital */
    bool last_state; /*!< Estado anterior de la entrada digital */
};

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

digital_output_t DigitalOutput_Create(uint8_t port, uint8_t pin) {
    digital_output_t self = malloc(sizeof(struct digital_output_s));
    if (self != NULL) {
        self->port = port;
        self->pin = pin;
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, true);
    }
    return self;
}

void DigitalOutput_Activate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, true);
}

void DigitalOutput_Deactivate(digital_output_t self) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, self->port, self->pin, false);
}

void DigitalOutput_Toggle(digital_output_t self) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->port, self->pin);
}

/*-----------------------------------------------------------------------------------*/

digital_input_t DigitalInput_Create(uint8_t gpio, uint8_t bit, bool inverted) {
    digital_input_t self = malloc(sizeof(struct digital_input_s));
    if (self != NULL) {
        self->port = gpio;
        self->pin = bit;
        self->inverted = inverted;

        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->port, self->pin, false);

        self->last_state = DigitalInput_GetIsActive(self);
    }
    return self;
}

bool DigitalInput_GetIsActive(digital_input_t self) {
    bool state = Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->port, self->pin) != 0; // llamar a la funcion del fabricante y comparar con 1

    if (self->inverted) {
        state = !state;
    }
    return state;
}

digital_state_t Digital_WasChanged(digital_input_t self) {

    digital_state_t result = DIGITAL_INPUT_WAS_CHANGED;

    bool state = DigitalInput_GetIsActive(self);

    if (state && self->last_state) {
        result = DIGITAL_INPUT_WAS_ACTIVATED;
    } else if (!state && !self->last_state) {
        result = DIGITAL_INPUT_WAS_DEACTIVATED;
    }
    self->last_state = state;
    return result;
}

bool Digital_WasActivated(digital_input_t self) {
    return DIGITAL_INPUT_WAS_ACTIVATED == Digital_WasChanged(self);
}

bool Digitalt_WasDeactivated(digital_input_t self) {
    return DIGITAL_INPUT_WAS_DEACTIVATED == Digital_WasChanged(self);
}

/* === End of documentation ======================================================================================== */
