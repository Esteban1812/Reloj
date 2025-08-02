/*********************************************************************************************************************
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * Copyright (c) 2025, Esteban Ignacio Lobo Silva <silvanacho04@gmail.com>
 * Copyright (c) 2025, Laboratorio de Electronica IV, Universidad Nacional de Tucumán, Argentina
 *

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

#ifndef SCREEN_H_
#define SCREEN_H_

/** @file screen.h
 ** @brief Declaraciones del módulo para la gestión de euna pantalla de 7 segmentos 4 digitos multiplexada.
 **
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define SEGMENT_A (1 << 0)
#define SEGMENT_B (1 << 1)
#define SEGMENT_C (1 << 2)
#define SEGMENT_D (1 << 3)
#define SEGMENT_E (1 << 4)
#define SEGMENT_F (1 << 5)
#define SEGMENT_G (1 << 6)
#define SEGMENT_P (1 << 7)

/* === Public data type declarations =============================================================================== */
/** Estructura que representa una pantalla de 7 segmentos multiplexada. */
typedef struct screen_s * screen_t;

typedef void (*digits_turn_off_t)(void);
typedef void (*segments_update_t)(uint8_t value);
typedef void (*digits_turn_on_t)(uint8_t digit);
// Estructura que representa el driver de la pantalla de 7 segmentos multiplexada.
// Contiene punteros a las funciones que manejan los digitos y segmentos de la pantalla.
typedef struct screen_driver_s {
    digits_turn_off_t DigitsTurnOff;
    segments_update_t SegmentsUpdate;
    digits_turn_on_t DigitsTurnOn;
} const * screen_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea una pantalla de 7 segmentos multiplexada.
 *
 * @param digits Numero de digitos de la pantalla (maximo 8).
 * @param driver Puntero a la estructura de driver de pantalla.
 * @return screen_t Puntero a la instancia de la pantalla creada.
 * @note Si el numero de digitos es mayor que 8, se limita a 8.
 */
screen_t ScreenCreate(uint8_t digits, screen_driver_t driver);

/**
 * @brief Escribe un valor en formato BCD en la pantalla.
 *
 * @param screen Puntero al objeto pantalla.
 * @param value Vector de valores BCD a mostrar en la pantalla, cada valor debe estar entre 0 y 9.
 * @param size Tamaño del vector de valores BCD a mostrar en la pantalla.
 * @note Si el tamaño es mayor que el numero de digitos de la pantalla, se limita al numero de digitos.
 */
void ScreenWriteBCD(screen_t screen, uint8_t value[], uint8_t size);

/**
 * @brief Actualiza la pantalla mostrando el digito actual.
 *
 * Esta funcion se debe llamar periodicamente para actualizar la pantalla.
 *
 * @param screen Puntero al objeto pantalla.
 */
void ScreenRefresh(screen_t screen);

/**
 * @brief Funcion para hacer parpadear los digitos de la pantalla.
 *
 * @param display puntero al descriptor de la pantalla
 * @param from posicion del primer digito a parpadear
 * @param to posicion del ultimo digito a parpadear
 * @note Los digitos se numeran desde 0, siendo el primer digito el de la izquierda.
 * @note Si from es mayor que to, no se hace nada.
 * @param frecuency factor de division de la frecuencia de refresco para el parpadeo.
 */
int DisplayFlashDigits(screen_t display, uint8_t from, uint8_t to, uint16_t divisor);

/**
 * @brief Función para hacer parpadear los puntos de los dígitos de la pantalla.
 *
 * @param display puntero al descriptor de la pantalla
 * @param from posicion del primer punto a parpadear
 * @param to posicion del último punto a parpadear
 * @param divisor factor de división de la frecuencia de refresco para el parpadeo.
 */
int DisplayFlashPoints(screen_t display, uint8_t from, uint8_t to, uint16_t divisor);

/**
 * @brief Enciende o apaga el punto decimal de un dígito específico.
 *
 * @param screen Puntero al objeto pantalla.
 * @param digit Índice del dígito (0 a N-1) cuyo punto se quiere modificar.
 * @param state true para encender el punto, false para apagarlo.
 */
void ScreenSetPoint(screen_t screen, uint8_t digit, bool state);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /**  SCREEN_H_ */
