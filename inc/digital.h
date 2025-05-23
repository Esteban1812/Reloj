/*********************************************************************************************************************
Copyright (c) Año, Nombre y Apellido del autor <correo@ejemplo.com>

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

#ifndef DIGITAL_H_
#define DIGITAL_H_

/** @file digital.h
 ** @brief Declaraciones del módulo para la gestión de entradas y salidas digitales.
 **
 ** Esta bibloteca puede usar memoria estática o dinámica. La memoria estática se define en el archivo de configuración
 *@anchor "config.h".
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

typedef enum digital_state_e {
    DIGITAL_INPUT_WAS_DEACTIVATED = -1,
    DIGITAL_INPUT_WAS_ACTIVATED = 1,
    DIGITAL_INPUT_WAS_CHANGED = 0,
} digital_state_t;

//! Estructura que representa una salida digital.
typedef struct digital_output_s * digital_output_t;
//! Estructura que representa una entrada digital.
typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion para crear una salida digital.
 * Esta funcion crea un objeto de la clase salida digital. La locacion de memoria depende
 * de la @ref configuracion de la bibloteca "config.h".
 *
 * @param gpio puerto de la salida digital
 * @param bit pin de la salida digital
 * @return digital_output_t
 */
digital_output_t DigitalOutput_Create(uint8_t gpio, uint8_t bit);

/**
 * @brief
 * @param self
 */
void DigitalOutput_Activate(digital_output_t self);

/**
 * @brief
 * @param self
 */
void DigitalOutput_Deactivate(digital_output_t self);

/**
 * @brief
 *
 * @param self
 */
void DigitalOutput_Toggle(digital_output_t self);

/**
 * @brief funcion para crear una entrada digital.
 *
 * @param gpio
 * @param bit
 * @param inverted
 * @note Si inverted es verdadero, la entrada digital se considera activa cuando el pin es bajo.
 * @note Si inverted es falso, la entrada digital se considera activa cuando el pin es alto.
 * @return digital_input_t Puntero a la instancia de la entrada digital creada.
 */

digital_input_t DigitalInput_Create(uint8_t gpio, uint8_t bit, bool inverted);

/**
 * @brief Funcion para leer el estado de la entrada digital.
 *
 * @param self puntero a la instancia de la entrada digital devuelta por la funcion DigitalInput_Create
 * @return bool true si la entrada digital es activa, false si no lo es.
 */

bool DigitalInput_GetIsActive(digital_input_t input);

/**
 * @brief Indica si la entrada digital ha cambiado de estado.
 *
 * @param self puntero a la instancia de la entrada digital devuelta por la funcion DigitalInput_Create
 * @return enum digital_state_t -1 si la entrada digital ha cambiado de estado a bajo, 1 si ha cambiado a alto, 0 si no ha cambiado.
 * @note Esta funcion debe ser llamada en cada ciclo de la aplicacion para detectar cambios de estado.
 */

enum digital_state_e Digital_WasChanged(digital_input_t self);

/**
 * @brief Indica si la entrada digital ha sido activada.
 *
 * devuelve true si la entrada digital ha sido activada, false si no lo ha sido.
 * 
 * @param self puntero a la instancia de la entrada digital devuelta por la funcion DigitalInput_Create
 * @return bool true si la entrada digital ha sido activada, false si no lo ha sido.
 */

bool Digital_WasActivated(digital_input_t self);

/**
 * @brief Indica si la entrada digital ha sido desactivada.
 *
 * devuelve true si la entrada digital ha sido desactivada, false si no lo ha sido.
 * 
 * @param self puntero a la instancia de la entrada digital devuelta por la funcion DigitalInput_Create
 * @return bool true si la entrada digital ha sido desactivada, false si no lo ha sido.
 */

bool Digitalt_WasDeactivated(digital_input_t self);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /**  DIGITAL_H_ */
