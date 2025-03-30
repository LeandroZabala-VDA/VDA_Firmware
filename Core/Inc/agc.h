/*
 * AGC.h
 *
 *  Created on: Mar 29, 2025
 *      Author: leank
 */

#ifndef INC_AGC_H_
#define INC_AGC_H_


#include <stdint.h>

#define AGC_BUFFER_SIZE 32  // Tamaño del buffer circular

void AGC_Init(void);
void AGC_AddSample(uint16_t sample);
uint16_t AGC_GetMax(void);

#endif /* INC_AGC_H_ */
