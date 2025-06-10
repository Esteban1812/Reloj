/* Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * Copyright (c) 2025, Esteban Ignacio Lobo Silva <nachosilva04.com>
 * Copyright (c) 2025, Laboratorio de Electronica IV, Universidad Nacional de Tucumán, Argentina

 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include <stdbool.h>
#include "bsp.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================= */

int main(void) {
    int divisor = 0;
    uint8_t value[4] = {1, 2, 3, 4};
    Board_t board = Board_Create();

    ScreenWriteBCD(board->screen, value, 4);
    /*
     DisplayFlashDigits(board->screen, 0, 4, 50);

     ScreenSetPoint(board->screen, 4, false);
     ScreenSetPoint(board->screen, 3, false);
     ScreenSetPoint(board->screen, 2, true);
     ScreenSetPoint(board->screen, 1, false);
     DisplayFlashPoints(board->screen, 1, 4, 50);


     */

    while (true) {

        if (!DigitalInput_GetIsActive(board->accept)) {
            DigitalOutput_Activate(board->blue_led);
        } else {
            DigitalOutput_Deactivate(board->blue_led);
        }
        if (!DigitalInput_GetIsActive(board->cancel)) {
            DigitalOutput_Activate(board->red_led);
        } else {
            DigitalOutput_Deactivate(board->red_led);
        }
        if (DigitalInput_GetIsActive(board->increment)) {
            ScreenSetPoint(board->screen, 4, true);
        } else {
            ScreenSetPoint(board->screen, 4, false);
        }
        if (DigitalInput_GetIsActive(board->decrement)) {
            ScreenSetPoint(board->screen, 3, true);
        } else {
            ScreenSetPoint(board->screen, 3, false);
        }
        if (DigitalInput_GetIsActive(board->set_time)) {
            ScreenSetPoint(board->screen, 1, true);
        } else {
            ScreenSetPoint(board->screen, 1, false);
        }

        if (DigitalInput_GetIsActive(board->set_alarm)) {
            ScreenSetPoint(board->screen, 2, true);
        } else {
            ScreenSetPoint(board->screen, 2, false);
        }

        if (Digital_WasActivated(board->increment)) {
            DigitalOutput_Activate(board->buzzer); // CORREGIR PONCHO
        }
        if (Digital_WasActivated(board->decrement)) {
            DigitalOutput_Deactivate(board->buzzer); // CORREGIR PONCHO
        }

        divisor++;
        if (divisor == 100) {
            divisor = 0;
            DigitalOutput_Toggle(board->green_led);
        }
        ScreenRefresh(board->screen);

        for (int delay = 0; delay < 25000; delay++) {
            __asm("NOP");
        }
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
