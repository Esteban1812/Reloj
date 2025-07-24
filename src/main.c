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
#include "screen.h"
#include "clock.h"
/* === Macros definitions ====================================================================== */

#define CLOCK_TICK_PER_SECOND 10000 // Definimos la frecuencia del reloj

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

static Board_t board; // variable global para la placa
static clock_t clock; // variable global para el reloj
static bool state_point = false; // Variable para controlar la visibilidad del punto central


/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
int main(void) {
    clock_time_t hora_inicial = {.bcd = {0, 0, 8, 2, 9, 1}}; // Hora inicial en formato BCD (00:00:00)
    clock_time_t hora_actual; // Variable para almacenar la hora actual del reloj
    uint8_t hora[4]; // Inicializamos el array de valores a mostrar en la pantalla
    board = Board_Create();
    clock = Clock_Create(CLOCK_TICK_PER_SECOND);

ClockSetTime(clock, &hora_inicial); // Ajustar la hora inicial del reloj


    // Configurar SysTick para generar interrupciones cada 1000 ms
    SysTickInt(10000); // Configura SysTick para 1 segundo

    while (true) {

        if (!DigitalInput_GetIsActive(board->accept)) {
           
        }
        if (!DigitalInput_GetIsActive(board->cancel)) {
            
        }
        if (DigitalInput_GetIsActive(board->increment)) {
            
        }
        if (DigitalInput_GetIsActive(board->decrement)) {
            
        }
        if (DigitalInput_GetIsActive(board->set_time)) {
            
        }

        if (DigitalInput_GetIsActive(board->set_alarm)) {
            
        }
            
// Mostrar hora en display
    ClockGetTime(clock, &hora_actual);

    hora[0] = hora_actual.bcd[5]; // Hora decenas
    hora[1] = hora_actual.bcd[4]; // Hora unidades
    hora[2] = hora_actual.bcd[3]; // Minutos decenas
    hora[3] = hora_actual.bcd[2]; // Minutos unidades

    ScreenWriteBCD(board->screen, hora, sizeof(hora));       // Escribo hora

        
        // Mostrar hora en display
        


    }
}

void SysTick_Handler(void) {
    // Aquí manejo el evento del SysTick. manejo acciones periódicas

static uint16_t tick_count = 0;

    tick_count++;

    if (tick_count >= CLOCK_TICK_PER_SECOND) {
        tick_count = 0;
        state_point = !state_point;  // Cambia el estado del punto cada segundo
    }
    ScreenSetPoint(board->screen, 2, state_point);           // Apago/Prendo punto
    ClockNewTick(clock); // Incremento el reloj
    ScreenRefresh(board->screen);                            // Refresco pantalla

}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
