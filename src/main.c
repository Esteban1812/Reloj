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
#include <stddef.h>

/* === Macros definitions ====================================================================== */

#define CLOCK_TICK_PER_SECOND 1000 // Definimos la frecuencia del reloj

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

static Board_t board;                        // variable global para la placa
static clock_t clock;                        // variable global para el reloj
static state_t state;                        // Variable para controlar el estado (MODO) del reloj
static uint16_t set_time_pressed_ticks = 0;  // Ticks que F1 estuvo presionado
static uint16_t set_alarm_pressed_ticks = 0; // Ticks que F2 estuvo presionado

/* === Private variable definitions ============================================================ */

static const uint8_t limits_min[] = {5, 9};  // Limites de los minutos
static const uint8_t limits_hour[] = {2, 3}; // Limites de las horas

/* === Private function implementation ========================================================= */
void Change_status(state_t new_state) {
    state = new_state; // Cambia el estado del reloj
    ScreenSetPoint(board->screen, 0, false);
    ScreenSetPoint(board->screen, 1, false);
    ScreenSetPoint(board->screen, 2, false);
    ScreenSetPoint(board->screen, 3, false);
    switch (state) // Dependiendo del estado, se realizan diferentes acciones
    {
    case INITIALIZING:
        DisplayFlashDigits(board->screen, 0, 3, 50); // Parpadeo de todos los dígitos
        ScreenSetPoint(board->screen, 1, true);
        break;

    case SHOWING_TIME:
        DisplayFlashDigits(board->screen, 0, 0, 0); // Muestra la hora actual sin parpadeo
        break;
    case SETTING_CURRENT_TIME:
        DisplayFlashDigits(board->screen, 0, 1, 100); // Parpadeo del dígito de las horas
        break;
    case SETTING_CURRENT_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, 100); // Parpadeo del dígito de los minutos
        break;
    case SETTING_ALARM_TIME:
        DisplayFlashDigits(board->screen, 0, 1, 100); // Parpadeo del dígito de las horas de la alarma
                                                      // Prendo todos los puntos para indicar modo alarma
        ScreenSetPoint(board->screen, 0, true);
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 2, true);
        ScreenSetPoint(board->screen, 3, true);
        break;
    case SETTING_ALARM_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, 100); // Parpadeo del dígito de los minutos de la alarma
        // Prendo todos los puntos para indicar modo alarma

        ScreenSetPoint(board->screen, 0, true);
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 2, true);
        ScreenSetPoint(board->screen, 3, true);
        break;
    default:
        break;
    }
}

void IncrementBCD(uint8_t num[2], const uint8_t limite[2]) {
    // Incrementa el valor BCD y lo ajusta al límite
    num[1]++;
    if (num[1] > 9) {
        num[1] = 0;
        num[0]++;
        if ((num[0] > limite[0]) && (num[1] > limite[1])) {
            num[0] = 0; // Reinicia a cero si se supera el límite
            num[1] = 0; // Reinicia a cero si se supera el límite
        }
    }
}

void DecrementBCD(uint8_t num[2], const uint8_t limite[2]) {
    // Decrementa el valor BCD y lo ajusta al límite
    num[1]--;
    if (num[1] > 9) {
        num[1] = 9;
        num[0]--;
        if ((num[0] < limite[0]) && (num[1] < limite[1])) {
            num[0] = limite[0]; // Ajusta al límite inferior
            num[1] = limite[1]; // Ajusta al límite inferior
        }
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    uint8_t input[4];          // Variable para almacenar la hora en formato BCD
    clock_time_t current_time; // Variable para almacenar la hora actual

    board = Board_Create();
    clock = Clock_Create(CLOCK_TICK_PER_SECOND);

    // Configurar SysTick para generar interrupciones cada 1000 ms
    SysTickInt(1000);            // Configura SysTick para 1 segundo
    Change_status(INITIALIZING); // Cambia el estado inicial del reloj

    while (true) {

        if (Digital_WasActivated(board->accept)) {

            if (state == SETTING_CURRENT_MINUTES) {
                Change_status(SETTING_CURRENT_TIME);
            } else if (state == SETTING_CURRENT_TIME) {
                ClockSetTime(clock, &current_time);
                Change_status(SHOWING_TIME);
            } else if (state == SETTING_ALARM_MINUTES) {
                Change_status(SETTING_ALARM_TIME);
            } else if (state == SETTING_ALARM_TIME) {
                ClockSetAlarm(clock, &current_time);
                Change_status(SHOWING_TIME);
            }
        }

        /*
                    if (Digital_WasActivated(board->cancel)) {
                        if (ClockGetTime(clock, &current_time)) {
                            Change_status(SHOWING_TIME); // Cambia al estado de mostrar la hora actual
                        } else {
                            Change_status(INITIALIZING); // Si no hay hora configurada, vuelve al estado inicial
                        }
                    }
                }

                                if (Digital_WasActivated(board->increment)) {
                                    if (state == SETTING_CURRENT_MINUTES) {
                                        IncrementBCD(&hora[2], limits_min); // Incrementa los minutos
                                    } else if (state == SETTING_CURRENT_TIME) {
                                        IncrementBCD(&hora[0], limits_hour); // Incrementa las horas
                                    } else if (state == SETTING_ALARM_MINUTES) {
                                        IncrementBCD(&hora[2], limits_min); // Incrementa los minutos de la alarma
                                    } else if (state == SETTING_ALARM_TIME) {
                                        IncrementBCD(&hora[0], limits_hour); // Incrementa las horas de la alarma
                                    }
                                    ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Muestra la hora actualizada en
           el display
                                }

                                if (Digital_WasActivated(board->decrement)) {
                                    if (state == SETTING_CURRENT_MINUTES) {
                                        DecrementBCD(&hora[2], limits_min); // Decrementa los minutos
                                    } else if (state == SETTING_CURRENT_TIME) {
                                        DecrementBCD(&hora[0], limits_hour); // Decrementa las horas
                                    } else if (state == SETTING_ALARM_MINUTES) {
                                        DecrementBCD(&hora[2], limits_min); // Decrementa los minutos de la alarma
                                    } else if (state == SETTING_ALARM_TIME) {
                                        DecrementBCD(&hora[0], limits_hour); // Decrementa las horas de la alarma
                                    }
                                    ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Muestra la hora actualizada en
           el display
                                }

                                if (Digital_WasActivated(board->set_alarm)) {
                                    Change_status(SETTING_ALARM_TIME);   // Cambia al estado de configuración de la hora
           de la alarma ClockGetAlarm(clock, &current_time); // Obtiene la hora de la alarma del reloj hora[0] =
                       current_time.bcd[5];       // Hora decenas hora[1] = current_time.bcd[4];       // Hora unidades
           hora[2] = current_time.bcd[3];       // Minutos decenas hora[3] = current_time.bcd[2];       // Minutos
           unidades
                                }*/
    }

    return 0;
}

/* === Interrupt handler implementation ========================================================= */
void SysTick_Handler(void) {
    uint8_t count_ticks = 0;  // Contador de ticks para el parpadeo
    bool state_point = false; // Estado del punto central
    uint8_t hora[4];
    clock_time_t current_time;

    count_ticks++; // Incrementa el contador de ticks
    ClockNewTick(clock);

    if (DigitalInput_GetIsActive(board->set_time)) {
        set_time_pressed_ticks++;
    } else {
        set_time_pressed_ticks = 0;
    }
    if (set_time_pressed_ticks >= 3000) { // Si F1 fue presionado por más de 3 segundos
        set_time_pressed_ticks = 0;
        if (state <= SHOWING_TIME) {
            Change_status(SETTING_CURRENT_MINUTES); // Entrar al modo de ajuste de minutos actuales
            ClockGetTime(clock, &current_time);
            hora[0] = current_time.bcd[5]; // Hora dec
            hora[1] = current_time.bcd[4]; // Hora uni
            hora[2] = current_time.bcd[3]; // Min dec
            hora[3] = current_time.bcd[2]; // Min uni
            ScreenWriteBCD(board->screen, hora, sizeof(hora));
        }
    }

    if (DigitalInput_GetIsActive(board->set_alarm)) {
        set_alarm_pressed_ticks++;
    } else {
        set_alarm_pressed_ticks = 0;
    }
    if (set_alarm_pressed_ticks >= 3000) { // Si F2 fue presionado por más de 3 segundos
        set_alarm_pressed_ticks = 0;
        if (state <= SHOWING_TIME) {
            Change_status(SETTING_ALARM_MINUTES); // Entrar al modo de ajuste de minutos de la alarma
            ClockGetAlarm(clock, &current_time);
            hora[0] = current_time.bcd[5]; // Hora dec
            hora[1] = current_time.bcd[4]; // Hora uni
            hora[2] = current_time.bcd[3]; // Min dec
            hora[3] = current_time.bcd[2]; // Min uni
            ScreenWriteBCD(board->screen, hora, sizeof(hora));
        }
    }

    if (state <= SHOWING_TIME) {
        if (count_ticks >= CLOCK_TICK_PER_SECOND) {
            count_ticks = 0;
            state_point = !state_point;
        }
    }
    ScreenSetPoint(board->screen, 1, state_point);
    ScreenRefresh(board->screen);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
