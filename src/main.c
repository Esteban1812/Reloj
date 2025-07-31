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
#include <string.h>

/* === Macros definitions ====================================================================== */

#define CLOCK_TICK_PER_SECOND 50 // Definimos la frecuencia del reloj

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
static volatile uint32_t milisegundos;       // Variable global para almacenar el tiempo en milisegundos
static clock_time_t new_time;                // Variable para almacenar la nueva hora configurada
static state_t state;                        // Variable para controlar el estado (MODO) del reloj
static uint16_t set_time_pressed_ticks = 0;  // Ticks que F1 estuvo presionado
static uint16_t set_alarm_pressed_ticks = 0; // Ticks que F2 estuvo presionado
static uint8_t hora[4];                      // Hora configurada en formato BCD: [hora_dec, hora_uni, min_dec, min_uni]

/* === Private variable definitions ============================================================ */

static const uint8_t limits_min[] = {5, 9};  // Limites de los minutos
static const uint8_t limits_hour[] = {2, 3}; // Limites de las horas

/* === Private function implementation ========================================================= */
void Change_status(state_t new_state) {
    state = new_state; // Cambia el estado del reloj
    switch (state)     // Dependiendo del estado, se realizan diferentes acciones
    {
    case INITIALIZING:
        DisplayFlashDigits(board->screen, 0, 3, 50); // Parpadeo de todos los dígitos
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
        DisplayFlashPoints(board->screen, 0, 3, 0); // Desactiva el parpadeo de los puntos

        break;
    case SETTING_ALARM_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, 100); // Parpadeo del dígito de los minutos de la alarma
        // Prendo todos los puntos para indicar modo alarma
        ScreenSetPoint(board->screen, 0, true);
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 2, true);
        ScreenSetPoint(board->screen, 3, true);
        DisplayFlashPoints(board->screen, 0, 3, 0); // Desactiva el parpadeo de los puntos
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
        if (num[0] > limite[0] || (num[0] == limite[0] && num[1] > limite[1])) {
            num[0] = 0;
            num[1] = 0;
        }
    } else if (num[0] == limite[0] && num[1] > limite[1]) {
        num[0] = 0;
        num[1] = 0;
    }
}

void DecrementBCD(uint8_t num[2], const uint8_t limite[2]) {
    if (num[1] == 0) {
        if (num[0] == 0) {
            num[0] = limite[0];
            num[1] = limite[1];
        } else {
            num[0]--;
            num[1] = 9;
        }
    } else {
        num[1]--;
    }

    if (num[0] > limite[0] || (num[0] == limite[0] && num[1] > limite[1])) {
        num[0] = limite[0];
        num[1] = limite[1];
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    clock_time_t current_time;
    board = Board_Create();
    DigitalOutput_Deactivate(
        board->buzzer); // el buzzer (led azul de la placa de abajo) se inicia apagado. Este es activo en bajo

    clock = Clock_Create(CLOCK_TICK_PER_SECOND);

    // Configurar SysTick para generar interrupciones cada 1000 ms
    SysTickInt(); // Configura SysTick para 1 segundo

    Change_status(INITIALIZING); // Cambia el estado inicial del reloj

    while (true) {

        switch (state) {
        case INITIALIZING:
            if (DigitalInput_GetIsActive(board->set_time)) {
                set_time_pressed_ticks++;
            } else {
                set_time_pressed_ticks = 0; // Reinicia el contador si F1 no está presionado
            }
            if (set_time_pressed_ticks >= 3000) {   // Si F1 se mantiene presionado por 3 segundo
                ClockGetTime(clock, &current_time); // Obtener hora actual del reloj

                hora[0] = current_time.bcd[5]; // hora decenas
                hora[1] = current_time.bcd[4]; // hora unidades
                hora[2] = current_time.bcd[3]; // minutos decenas
                hora[3] = current_time.bcd[2]; // minutos unidades

                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Mostrar hora actual en pantalla
                Change_status(SETTING_CURRENT_MINUTES);
                set_time_pressed_ticks = 0; // Reinicia el contador
            }

            if (DigitalInput_GetIsActive(board->set_alarm)) {
                set_alarm_pressed_ticks++;
                if (set_alarm_pressed_ticks >= 3000) {  // Si F2 se mantiene presionado por 3 segundo
                    ClockGetTime(clock, &current_time); // Obtener hora actual del reloj

                    hora[0] = current_time.bcd[5]; // hora decenas
                    hora[1] = current_time.bcd[4]; // hora unidades
                    hora[2] = current_time.bcd[3]; // minutos decenas
                    hora[3] = current_time.bcd[2]; // minutos unidades

                    ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Mostrar hora actual en pantalla
                    Change_status(SETTING_ALARM_MINUTES);
                    set_alarm_pressed_ticks = 0; // Reinicia el contador
                }
            }
            break;

        case SHOWING_TIME:
            if (DigitalInput_GetIsActive(board->set_time)) {
                set_time_pressed_ticks++;
                if (set_time_pressed_ticks >= 3000) {   // Si F1 se mantiene presionado por 3 segundo
                    ClockGetTime(clock, &current_time); // Obtener hora actual del reloj

                    hora[0] = current_time.bcd[5]; // hora decenas
                    hora[1] = current_time.bcd[4]; // hora unidades
                    hora[2] = current_time.bcd[3]; // minutos decenas
                    hora[3] = current_time.bcd[2]; // minutos unidades

                    ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Mostrar hora actual en pantalla
                    Change_status(SETTING_CURRENT_MINUTES);
                    set_time_pressed_ticks = 0; // Reinicia el contador
                }
            } else {
                set_time_pressed_ticks = 0; // Reinicia el contador si F1 no está presionado
            }
            if (DigitalInput_GetIsActive(board->set_alarm)) {
                set_alarm_pressed_ticks++;
                if (set_alarm_pressed_ticks >= 3000) {  // Si F2 se mantiene presionado por 3 segundo
                    ClockGetTime(clock, &current_time); // Obtener hora actual del reloj

                    hora[0] = current_time.bcd[5]; // hora decenas
                    hora[1] = current_time.bcd[4]; // hora unidades
                    hora[2] = current_time.bcd[3]; // minutos decenas
                    hora[3] = current_time.bcd[2]; // minutos unidades

                    ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Mostrar hora actual en pantalla
                    Change_status(SETTING_ALARM_MINUTES);
                    set_alarm_pressed_ticks = 0; // Reinicia el contador
                }
            } else {
                set_alarm_pressed_ticks = 0; // Reinicia el contador si F2 no está presionado
            }
            break;

        case SETTING_CURRENT_TIME:
            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(hora, limits_hour);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(hora, limits_hour);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->accept)) {
                new_time.bcd[5] = hora[0];      // hora decenas
                new_time.bcd[4] = hora[1];      // hora unidades
                new_time.bcd[3] = hora[2];      // minutos decenas
                new_time.bcd[2] = hora[3];      // minutos unidades
                new_time.bcd[1] = 0;            // segundos decenas
                new_time.bcd[0] = 0;            // segundos unidades
                ClockSetTime(clock, &new_time); // Configura la hora actual en el reloj
                Change_status(SHOWING_TIME);    // Cambia al estado de mostrar la hora
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME); // Cancela la configuración y vuelve a mostrar la hora
            }
            break;

        case SETTING_CURRENT_MINUTES:
            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(&hora[2], limits_min);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(&hora[2], limits_min);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->accept)) {
                Change_status(SETTING_CURRENT_TIME); // Cambia al estado de mostrar la hora
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME); // Cancela la configuración y vuelve a mostrar la hora
            }
            break;
        case SETTING_ALARM_TIME:
            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(hora, limits_hour);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(hora, limits_hour);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->accept)) {
                new_time.bcd[5] = hora[0];
                new_time.bcd[4] = hora[1];
                new_time.bcd[3] = hora[2];
                new_time.bcd[2] = hora[3];
                new_time.bcd[1] = 0;
                new_time.bcd[0] = 0;

                ClockSetAlarm(clock, &new_time);
                Change_status(SHOWING_TIME);
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME);
            }
            break;
        case SETTING_ALARM_MINUTES:
            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(&hora[2], limits_min);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(&hora[2], limits_min);
                ScreenWriteBCD(board->screen, hora, sizeof(hora)); // Actualiza la pantalla con la nueva hora
            } else if (Digital_WasActivated(board->accept)) {
                Change_status(SETTING_ALARM_TIME);
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME);
            }
            break;
        default:
            break;
        }
    }
}

/* === Interrupt handler implementation ========================================================= */
void SysTick_Handler(void) {
    clock_time_t current_time;
    static bool state_point = false; // Estado del punto decimal entre horas y minutos
    ClockNewTick(clock);
    milisegundos++; // Incrementa el contador de milisegundos
    if (state <= SHOWING_TIME) {

        ClockGetTime(clock, &current_time);
        hora[0] = current_time.bcd[5]; // Hora decenas
        hora[1] = current_time.bcd[4]; // Hora unidades
        hora[2] = current_time.bcd[3]; // Minutos decenas
        hora[3] = current_time.bcd[2]; // Minutos unidades

        if (milisegundos % 1000 == 0) { // Cada segundo
            state_point = !state_point; // Cambia el estado del punto decimal
        }
        ScreenSetPoint(board->screen, 1, state_point); // Punto entre horas y minutos
        ScreenWriteBCD(board->screen, hora, sizeof(hora));
    }

    ScreenRefresh(board->screen);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
