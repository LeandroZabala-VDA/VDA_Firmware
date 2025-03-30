/*
 * AGC.c
 *
 *  Created on: Mar 29, 2025
 *      Author: leank
 */


#include "agc.h"

static uint16_t buffer[AGC_BUFFER_SIZE];
static uint8_t head = 0;
static uint8_t count = 0;
static uint16_t max_value = 0;        // Máximo actualizado dinámicamente

void AGC_Init(void) {
    for (uint8_t i = 0; i < AGC_BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
    head = 0;
    count = 0;
    max_value = 0;
}

void AGC_AddSample(uint16_t sample) {
    // Guardar el valor antiguo antes de sobrescribirlo
    uint8_t next_pos = (head + 1) % AGC_BUFFER_SIZE;
    uint16_t old_sample = buffer[next_pos];

    // Agregar la nueva muestra
    head = next_pos;
    buffer[head] = sample;

    // Actualizar contador si aún no está lleno el buffer
    if (count < AGC_BUFFER_SIZE) {
        count++;
    }

    // Actualizar el máximo si la nueva muestra es mayor
    if (sample > max_value) {
        max_value = sample;
    }
    // Si el valor eliminado era el máximo actual, necesitamos recalcular
    else if (old_sample == max_value) {
        // Recalcular el máximo escaneando todo el buffer
        max_value = 0; // Asumiendo valores de ADC siempre positivos
        for (uint8_t i = 0; i < count; i++) {
            if (buffer[i] > max_value) {
                max_value = buffer[i];
            }
        }
    }
    // En otro caso, el máximo sigue siendo el mismo
}

uint16_t AGC_GetMax(void) {
    return max_value;
}
