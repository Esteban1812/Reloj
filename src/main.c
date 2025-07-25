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

typedef enum {               // Estados del reloj:
    INITIALIZING,            // (0) Estado inicial, el reloj no ha sido configurado
    SHOWING_TIME,            // (1) Estado en el que se muestra la hora actual
    SETTING_CURRENT_TIME,    // (2) Estado en el que se configura la hora actual
    SETTING_CURRENT_MINUTES, // (3) Estado en el que se configura los minutos actuales
    SETTING_ALARM_TIME,      // (4) Estado en el que se configura la hora de la alarma
    SETTING_ALARM_MINUTES,   // (5) Estado en el que se configura los minutos de la alarma
} state_t;
/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

static Board_t board; // variable global para la placa
static clock_t clock; // variable global para el reloj
static state_t state; // Variable para controlar el estado (MODO) del reloj

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
void Change_status(state_t new_state) {
    state = new_state; // Cambia el estado del reloj
    switch (state)     // Dependiendo del estado, se realizan diferentes acciones
    {
    case INITIALIZING:
        DisplayFlashDigits(board->screen, 0, 3, CLOCK_TICK_PER_SECOND); // Parpadeo de todos los dígitos del display
        break;
    case SHOWING_TIME:
        DisplayFlashDigits(board->screen, 0, 0, 0); // Muestra la hora actual sin parpadeo
        break;
    case SETTING_CURRENT_TIME:
        DisplayFlashDigits(board->screen, 0, 1, CLOCK_TICK_PER_SECOND); // Parpadeo del dígito de las horas
        break;
    case SETTING_CURRENT_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, CLOCK_TICK_PER_SECOND); // Parpadeo del dígito de los minutos
        break;
    case SETTING_ALARM_TIME:
        DisplayFlashDigits(board->screen, 0, 1, CLOCK_TICK_PER_SECOND); // Parpadeo del dígito de las horas de la alarma
        // enciende los puntos de los dígitos
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 2, true);
        ScreenSetPoint(board->screen, 3, true);
        ScreenSetPoint(board->screen, 4, true);
        break;
    case SETTING_ALARM_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3,
                           CLOCK_TICK_PER_SECOND); // Parpadeo del dígito de los minutos de la alarma
        // enciende los puntos de los dígitos
        ScreenSetPoint(board->screen, 0, true);
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 3, true);
        ScreenSetPoint(board->screen, 4, true);
        break;
    default:
        break;
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    uint8_t hora[4];           // Array para almacenar la hora en formato BCD
    clock_time_t current_time; // Variable para almacenar la hora actual

    board = Board_Create();
    clock = Clock_Create(CLOCK_TICK_PER_SECOND);

    // Configurar SysTick para generar interrupciones cada 1000 ms
    SysTickInt(10000);           // Configura SysTick para 1 segundo
    Change_status(INITIALIZING); // Cambia el estado inicial del reloj

    while (true) {

        if (!DigitalInput_GetIsActive(board->accept)) {
            if (state == SETTING_CURRENT_MINUTES) {
                Change_status(SETTING_CURRENT_TIME); // Cambia al estado de configuración de la hora actual
            } else if (state == SETTING_CURRENT_TIME) {
                ClockSetTime(clock, &current_time); // Configura la hora actual en el reloj
                Change_status(SHOWING_TIME);        // Cambia al estado de mostrar la hora actual
            }

            if (!DigitalInput_GetIsActive(board->cancel)) {
                if (ClockGetTime(clock, &current_time)) {
                    Change_status(SHOWING_TIME); // Cambia al estado de mostrar la hora actual
                } else {
                    Change_status(INITIALIZING); // Si no hay hora configurada, vuelve al estado inicial
                }
            }


            if (DigitalInput_GetIsActive(board->set_time)) {
                Change_status(SETTING_CURRENT_TIME); // Cambia al estado de configuración de la hora actual
                ClockGetTime(clock, &current_time);  // Obtiene la hora actual del reloj
                hora[0] = current_time.bcd[5];       // Hora decenas
                hora[1] = current_time.bcd[4];       // Hora unidades
                hora[2] = current_time.bcd[3];       // Minutos decenas
                hora[3] = current_time.bcd[2];       // Minutos unidades
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Muestra la hora actual en el display
            }

            if (DigitalInput_GetIsActive(board->set_alarm)) {
                Change_status(SETTING_ALARM_TIME);   // Cambia al estado de configuración de la hora de la alarma
                ClockGetAlarm(clock, &current_time); // Obtiene la hora de la alarma del reloj
                hora[0] = current_time.bcd[5];       // Hora decenas
                hora[1] = current_time.bcd[4];       // Hora unidades
                hora[2] = current_time.bcd[3];       // Minutos decenas
                hora[3] = current_time.bcd[2];       // Minutos unidades
            }
            }
        }
        return 0;
    }
    
    /* === Interrupt handler implementation ========================================================= */
    void SysTick_Handler(void) {
        static bool state_point = false; // Variable para controlar la visibilidad del punto central
        static uint16_t tick_count = 0;
        uint8_t hora[4];
        clock_time_t current_time;
    
        tick_count++;
    
        ClockNewTick(clock); // Incrementa el reloj
    
        if (tick_count >= CLOCK_TICK_PER_SECOND) {
            tick_count = 0;
            state_point = !state_point;
            if (state_point) {
                ScreenSetPoint(board->screen, 2, true); // Enciende el punto central
            } else {
                ScreenSetPoint(board->screen, 2, false); // Apaga el punto central
            }
    
            if (state <= SHOWING_TIME) {
    
                // Mostrar hora en display
                
                ClockGetTime(clock, &current_time); // Obtiene la hora actual del reloj
                hora[0] = current_time.bcd[5]; // Hora decenas
                hora[1] = current_time.bcd[4]; // Hora unidades
                hora[2] = current_time.bcd[3]; // Minutos decenas
                hora[3] = current_time.bcd[2]; // Minutos unidades
                ScreenWriteBCD(board->screen, hora, sizeof(hora));
            }
        }
    
        ScreenRefresh(board->screen);
    }
    
    /* === End of documentation ==================================================================== */
    
    /** @} End of module definition for doxygen */
