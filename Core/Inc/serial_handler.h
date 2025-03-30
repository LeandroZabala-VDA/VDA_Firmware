/*
 * serial_handler.h
 *
 *  Created on: Mar 26, 2025
 *      Author: leank
 */

#ifndef INC_SERIAL_HANDLER_H_
#define INC_SERIAL_HANDLER_H_

#include "main.h"

#define BUFFER_SIZE 16

void SerialHandler_Init(UART_HandleTypeDef *huart);
void SerialHandler_SendData(uint16_t data);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

extern uint8_t programming_mode;
#endif /* INC_SERIAL_HANDLER_H_ */
