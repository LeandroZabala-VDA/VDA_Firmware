/*
 * serial_handler.c
 *
 *  Created on: Mar 26, 2025
 *      Author: leank
 */
#include "serial_handler.h"
#include "main.h"
#include "state_machine.h"

static UART_HandleTypeDef *uart_handle;
static volatile uint8_t received_byte;
static volatile char rx_buffer[BUFFER_SIZE];
static volatile uint8_t rx_index = 0;

static void process_command(const char *command) {
    if (strcmp(command, "ASTOPZ") == 0) {
        state_machine(EVENT_STOP, &current_state);
    } else if (strcmp(command, "ASTARTZ") == 0) {
    	state_machine(EVENT_START, &current_state);
    } else if (strcmp(command, "AAGCONZ") == 0) {
    	state_machine(EVENT_AGC_ON, &current_state);
    } else if (strcmp(command, "AAGCOFFZ") == 0) {
    	state_machine(EVENT_AGC_OFF, &current_state);
    } else if (strcmp(command, "AACQZ") == 0) {
    	state_machine(EVENT_ACQ, &current_state);
    }else if (strcmp(command, "ARESETZ") == 0) {
        	state_machine(EVENT_RESTART, &current_state);
        }
}

void SerialHandler_Init(UART_HandleTypeDef *huart) {
    uart_handle = huart;
    HAL_UART_Receive_IT(uart_handle, &received_byte, 1);  // Habilita la recepción por interrupción
}

void SerialHandler_SendData(uint16_t data) {
    uint8_t frame[4];
    frame[0] = 'A'; // Inicio de trama
    frame[1] = (uint8_t)(data & 0xFF); // Byte menos significativo
    frame[2] = (uint8_t)((data >> 8) & 0xFF); // Byte más significativo
    frame[3] = 'Z'; // Fin de trama

    HAL_UART_Transmit(uart_handle, frame, sizeof(frame), HAL_MAX_DELAY);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == uart_handle) {
        if (received_byte == 0x7F) {
            programming_mode = 1;
        } else if (received_byte == '\n') {
            rx_buffer[rx_index] = '\0'; // Termina la cadena
            process_command((const char *)rx_buffer);
            rx_index = 0; // Reinicia el índice
        } else if (rx_index < BUFFER_SIZE - 1) {
            rx_buffer[rx_index++] = received_byte;
        } else {
            rx_index = 0; // Resetea en caso de desbordamiento
        }
        HAL_UART_Receive_IT(uart_handle, (uint8_t *)&received_byte, 1);
    }
}


