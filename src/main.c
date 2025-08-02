/* Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * Copyright (c) 2025, Esteban Ignacio Lobo Silva <silvanacho04@gmail.com>
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

#define CLOCK_TICK_PER_SECOND 1000     // Definimos la frecuencia del reloj
#define LONG_PRESS_TIME_MS    3000  // cantidad de tick (milisegundos) que defino para considerar una presion larga
#define TIMEOUT_MS            30000 // 30 segundos de timeout para configuración

/* === Private data type declarations ========================================================== */

/*
 * @brief Estados del reloj
 * Estos estados definen el modo de operación del reloj.
 */
typedef enum {
    INITIALIZING,            // (0) Estado inicial, el reloj no ha sido configurado
    SHOWING_TIME,            // (1) Estado en el que se muestra la hour actual
    SETTING_CURRENT_TIME,    // (2) Estado en el que se configura la hour actual
    SETTING_CURRENT_MINUTES, // (3) Estado en el que se configura los minutos actuales
    SETTING_ALARM_TIME,      // (4) Estado en el que se configura la hour de la alarma
    SETTING_ALARM_MINUTES,   // (5) Estado en el que se configura los minutos de la alarma
} state_t;

/**
 * @brief Estructura para manejar el estado de una pulsación larga de un botón.
 * Esta estructura almacena información sobre si el botón está presionado,
 * si ya se ha procesado la pulsación larga y el tiempo en que comenzó la pulsación.
 */
typedef struct {
    bool is_pressed;           // Indica si el botón está siendo presionado
    bool already_processed;    // Evita múltiples detecciones
    uint32_t press_start_time; // Marca el instante en que empezó la pulsación
} key_long_status_t;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Cambia el estado del reloj y actualiza la pantalla.
 *
 * @param new_state El nuevo estado al que se cambiará el reloj.
 * @note Esta función actualiza la pantalla según el nuevo estado.
 */
static void Change_status(state_t new_state);

/**
 * @brief Incrementa un número en formato BCD (Binary-Coded Decimal) y lo ajusta al límite.
 *
 * @param num Puntero al número BCD a incrementar.
 * @param limite Puntero al límite BCD que no debe excederse.
 */
static void IncrementBCD(uint8_t num[2], const uint8_t limite[2]);

/**
 * @brief Decrementa un número en formato BCD (Binary-Coded Decimal) y lo ajusta al límite.
 *
 * @param num Puntero al número BCD a decrementar.
 * @param limite Puntero al límite BCD que no debe excederse.
 */
static void DecrementBCD(uint8_t num[2], const uint8_t limite[2]);

/**
 * @brief Callback para la alarma.
 * Esta función se ejecuta cuando la alarma suena.
 */
void AlarmCallback(void);

/**
 * @brief Verifica si se ha mantenido presionado un botón por un tiempo largo.
 *
 * @param key Puntero al botón a verificar.
 * @param status Puntero a la estructura que maneja el estado de la pulsación larga.
 * @param delay_ms Tiempo en milisegundos para considerar una pulsación larga.
 * @return true si se detecta una pulsación larga, false en caso contrario.
 */
static bool CheckLongPress(digital_input_t key, key_long_status_t * status, uint32_t delay_ms);

/**
 * @brief Verifica si ha pasado un tiempo de espera desde la última interacción.
 *
 * @return true si ha pasado el tiempo de espera, false en caso contrario.
 */
static bool CheckTimeout(void);

/**
 * @brief Actualiza el tiempo de la última tecla presionada.
 * Esta función actualiza la variable global que almacena el tiempo de la última tecla presionada.
 */
static void UpdateLastKeyTime(void);

/**
 * @brief Convierte la hora en formato BCD a un arreglo de 4 bytes.
 *
 * @param time Puntero a la estructura clock_time_t que contiene la hora.
 * @param hour Arreglo de 4 bytes donde se almacenará la hora en formato BCD.
 */
static void Time_Bcd(clock_time_t * time, uint8_t hour[]);

/**
 * @brief Convierte un arreglo de 4 bytes en formato BCD a la estructura clock_time_t.
 *
 * @param time Puntero a la estructura clock_time_t donde se almacenará la hora.
 * @param hour Arreglo de 4 bytes que contiene la hora en formato BCD.
 */
static void Bcd_Time(clock_time_t * time, uint8_t hour[]);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

static Board_t board;                      // variable global para la placa
static clock_t clock;                      // variable global para el reloj
static volatile uint32_t milisegundos = 0; // Variable global para almacenar el tiempo en milisegundos
static uint32_t last_key_time = 0;         // Marca el último tiempo en que se presionó una tecla
static state_t state;                      // Variable para controlar el estado (MODO) del reloj
static uint8_t hour[4];                    // Hora configurada en formato BCD: [hora_dec, hora_uni, min_dec, min_uni]

static key_long_status_t set_time_long_press = {
    0}; // Estructura para manejar la presión larga del botón de configuración de hora
static key_long_status_t set_alarm_long_press = {
    0}; // Estructura para manejar la presión larga del botón de configuración de alarma

static const uint8_t limits_min[] = {5, 9};  // Limites de los minutos
static const uint8_t limits_hour[] = {2, 3}; // Limites de las horas

/* === Private function implementation ========================================================= */
static void Change_status(state_t new_state) {
    state = new_state;
    last_key_time = milisegundos;
    switch (state) {
    case INITIALIZING:
        memset(hour, 0, sizeof(hour));
        ScreenWriteBCD(board->screen, hour, sizeof(hour));
        DisplayFlashDigits(board->screen, 0, 3, 50);
        ScreenSetPoint(board->screen, 1, true);
        DisplayFlashPoints(board->screen, 1, 1, 50);
        break;
    case SHOWING_TIME:
        DisplayFlashDigits(board->screen, 0, 0, 0);
        ScreenSetPoint(board->screen, 0, false);
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 2, false);
        ScreenSetPoint(board->screen, 3, false);
        DisplayFlashPoints(board->screen, 0, 3, 0);
        break;
    case SETTING_CURRENT_TIME:
        DisplayFlashDigits(board->screen, 0, 1, 100);
        ScreenSetPoint(board->screen, 1, false);
        break;
    case SETTING_CURRENT_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, 100);
        ScreenSetPoint(board->screen, 1, false);
        break;
    case SETTING_ALARM_TIME:
        DisplayFlashDigits(board->screen, 0, 1, 100);
        ScreenSetPoint(board->screen, 0, true);
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 2, true);
        ScreenSetPoint(board->screen, 3, true);
        DisplayFlashPoints(board->screen, 0, 3, 0);
        break;
    case SETTING_ALARM_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, 100);
        ScreenSetPoint(board->screen, 0, true);
        ScreenSetPoint(board->screen, 1, true);
        ScreenSetPoint(board->screen, 2, true);
        ScreenSetPoint(board->screen, 3, true);
        DisplayFlashPoints(board->screen, 0, 3, 0);
        break;
    default:
        break;
    }
}

static void IncrementBCD(uint8_t num[2], const uint8_t limite[2]) {
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

static void DecrementBCD(uint8_t num[2], const uint8_t limite[2]) {
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

void AlarmCallback(void) {
    DigitalOutput_Deactivate(board->blue_led); // activo en bajo
}

static bool CheckLongPress(digital_input_t key, key_long_status_t * status, uint32_t delay_ms) {
    const uint32_t debounce = 50;
    if (DigitalInput_GetIsActive(key)) {
        if (!status->is_pressed) {
            status->is_pressed = true;
            status->press_start_time = milisegundos;
            status->already_processed = false;
        } else if (!status->already_processed) {
            uint32_t held = milisegundos - status->press_start_time;
            if (held >= delay_ms + debounce) {
                status->already_processed = true;
                return true;
            }
        }
    } else {
        status->is_pressed = false;
        status->already_processed = false;
        status->press_start_time = 0;
    }

    return false;
}

static bool CheckTimeout(void) {
    return (milisegundos - last_key_time) > TIMEOUT_MS;
}

static void UpdateLastKeyTime(void) {

    last_key_time = milisegundos;
}

static void Time_Bcd(clock_time_t * time, uint8_t hour[]) {
    if (time && hour) {
        hour[0] = time->bcd[5];
        hour[1] = time->bcd[4];
        hour[2] = time->bcd[3];
        hour[3] = time->bcd[2];
    }
}

static void Bcd_Time(clock_time_t * time, uint8_t hour[]) {
    if (time && hour) {
        time->bcd[5] = hour[0];
        time->bcd[4] = hour[1];
        time->bcd[3] = hour[2];
        time->bcd[2] = hour[3];
        time->bcd[1] = 0;
        time->bcd[0] = 0;
    }
}

/* === Public function implementation ========================================================= */

int main(void) {
    clock_time_t current_time, alarm_time;
    board = Board_Create();

    clock = Clock_Create(CLOCK_TICK_PER_SECOND);
    ClockAttachAlarmCallback(clock, AlarmCallback);
    SysTickInt();

    Change_status(INITIALIZING);

    while (true) {

        switch (state) {
        case INITIALIZING:

            if (CheckLongPress(board->set_time, &set_time_long_press, LONG_PRESS_TIME_MS)) {
                Change_status(SETTING_CURRENT_MINUTES);
            }
            if (CheckLongPress(board->set_alarm, &set_alarm_long_press, LONG_PRESS_TIME_MS)) {
                Change_status(SETTING_ALARM_MINUTES);
            }
            break;

        case SHOWING_TIME:
            ScreenSetPoint(board->screen, 3, ClockIsAlarmEnabled(clock));

            if (ClockIsAlarmRinging(clock)) {
                if (Digital_WasActivated(board->accept)) {
                    ClockPostponeAlarm(clock, 5);
                    DigitalOutput_Activate(board->blue_led);
                    UpdateLastKeyTime();
                } else if (Digital_WasActivated(board->cancel)) {
                    DigitalOutput_Activate(board->blue_led);
                    ClockPostponeAlarmUntilTomorrow(clock);
                    UpdateLastKeyTime();
                }
            } else if (ClockGetAlarm(clock, &alarm_time)) {

                if (Digital_WasActivated(board->accept)) {
                    ClockEnableAlarm(clock, true);
                    UpdateLastKeyTime();
                } else if (Digital_WasActivated(board->cancel)) {
                    ClockEnableAlarm(clock, false);
                    UpdateLastKeyTime();
                }
            }

            if (CheckLongPress(board->set_time, &set_time_long_press, LONG_PRESS_TIME_MS)) {
                Change_status(SETTING_CURRENT_MINUTES);
            }

            if (CheckLongPress(board->set_alarm, &set_alarm_long_press, LONG_PRESS_TIME_MS)) {

                if (ClockGetAlarm(clock, &alarm_time)) {
                    Time_Bcd(&alarm_time, hour);
                } else {
                    ClockGetTime(clock, &current_time);
                    Time_Bcd(&current_time, hour);
                }
                ScreenWriteBCD(board->screen, hour, sizeof(hour));
                Change_status(SETTING_ALARM_MINUTES);
            }
            break;

        case SETTING_CURRENT_TIME:

            if (CheckTimeout()) {
                Change_status(SHOWING_TIME);
                break;
            }
            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(hour, limits_hour);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(hour, limits_hour);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->accept)) {
                Bcd_Time(&current_time, hour);
                ClockSetTime(clock, &current_time);
                Change_status(SHOWING_TIME);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME);
                UpdateLastKeyTime();
            }

            ScreenWriteBCD(board->screen, hour, sizeof(hour));
            break;

        case SETTING_CURRENT_MINUTES:
            if (CheckTimeout()) {
                Change_status(SHOWING_TIME);
                break;
            }
            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(&hour[2], limits_min);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(&hour[2], limits_min);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->accept)) {
                Change_status(SETTING_CURRENT_TIME);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME);
                UpdateLastKeyTime();
            }
            ScreenWriteBCD(board->screen, hour, sizeof(hour));
            break;

        case SETTING_ALARM_TIME:
            if (CheckTimeout()) {
                Change_status(SHOWING_TIME);
                break;
            }
            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(hour, limits_hour);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(hour, limits_hour);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->accept)) {
                Bcd_Time(&alarm_time, hour);
                ClockSetAlarm(clock, &alarm_time);
                ClockEnableAlarm(clock, true);
                Change_status(SHOWING_TIME);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME);
                UpdateLastKeyTime();
            }
            ScreenWriteBCD(board->screen, hour, sizeof(hour));
            break;

        case SETTING_ALARM_MINUTES:
            if (CheckTimeout()) {
                Change_status(SHOWING_TIME);
                break;
            }

            if (Digital_WasActivated(board->increment)) {
                IncrementBCD(&hour[2], limits_min);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->decrement)) {
                DecrementBCD(&hour[2], limits_min);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->accept)) {
                Change_status(SETTING_ALARM_TIME);
                UpdateLastKeyTime();
            } else if (Digital_WasActivated(board->cancel)) {
                Change_status(SHOWING_TIME);
                UpdateLastKeyTime();
            }
            ScreenWriteBCD(board->screen, hour, sizeof(hour));
            break;
        default:
            break;
        }
    }
}

/* === Interrupt handler implementation ========================================================= */
void SysTick_Handler(void) {
    clock_time_t current_time;
    static bool state_point = false;
    ClockNewTick(clock);
    milisegundos++;

    if (state == SHOWING_TIME) {

        ClockGetTime(clock, &current_time);
        Time_Bcd(&current_time, hour);

        if (milisegundos % 1000 == 0) {
            state_point = !state_point;
        }
        ScreenSetPoint(board->screen, 1, state_point);
        ScreenWriteBCD(board->screen, hour, sizeof(hour));
    }

    ScreenRefresh(board->screen);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
