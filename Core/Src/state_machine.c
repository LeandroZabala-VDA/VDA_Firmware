/*
 * state_machine.c
 *
 *  Created on: Mar 19, 2025
 *      Author: leank
 */

#include "state_machine.h"
#include "main.h"
#include "agc.h"
#include "filters.h"

state_type current_state = STATE_STANDBY_MANUAL;

int8_t state_machine(event_type event, state_type* state) {

	int8_t ret = 0;

	 switch (*state) {
	        case STATE_STANDBY_MANUAL:
	            switch (event) {
	            	case EVENT_RESTART:
	            		*state = STATE_STANDBY_MANUAL;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
	                    blink_ACQ_LED = 0;
	            		break;
	                case EVENT_START:
	                    *state = STATE_MANUAL_OPERATION;
	                    blink_ACQ_LED = 1;
	                    break;
	                case EVENT_STOP:
	                	ret = -1;
	                	break;
	                case EVENT_AGC_ON:
	                	ret = -1;
	                	break;
	                case EVENT_AGC_OFF:
	                	ret = -1;
	                	break;
	                case EVENT_ACQ:
	                	ret = -1;
	                    break;
	            }
	            break;

		        case STATE_STANDBY_AUTO:
		            switch (event) {
						case EVENT_RESTART:
							*state = STATE_STANDBY_MANUAL;
		                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
		                    blink_ACQ_LED = 0;
		                case EVENT_START:
		                    *state = STATE_AUTOMATIC_OPERATION;
		                    blink_ACQ_LED = 1;
		                    break;
		                case EVENT_STOP:
		                	ret = -1;
		                	break;
		                case EVENT_AGC_ON:
		                	ret = -1;
		                	break;
		                case EVENT_AGC_OFF:
		                	ret = -1;
		                	break;
		                case EVENT_ACQ:
		                	ret = -1;
		                    break;
		            }
		            break;

	        case STATE_MANUAL_OPERATION:
	            switch (event) {
	            	case EVENT_RESTART:
	            	    *state = STATE_STANDBY_MANUAL;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
	                    blink_ACQ_LED = 0;
	            	    break;
	                case EVENT_STOP:
	                    *state = STATE_STANDBY_MANUAL;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
	                    blink_ACQ_LED = 0;
	                    break;
	                case EVENT_AGC_ON:
	                    *state = STATE_AUTOMATIC_OPERATION;
	                    AGC_Init();
	                    break;
	                case EVENT_AGC_OFF:
	                	ret = -1;
	                	break;
	                case EVENT_ACQ:
	                    *state = STATE_ACQUIRING_MANUAL;
	                    blink_ACQ_LED = 0;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 1);
	                    break;
	                case EVENT_START:
	                	ret = -1;
	                	break;
	            }
	            break;

	        case STATE_AUTOMATIC_OPERATION:
	            switch (event) {
					case EVENT_RESTART:
						*state = STATE_STANDBY_MANUAL;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
	                    blink_ACQ_LED = 0;
						break;
	                case EVENT_STOP:
	                    *state = STATE_STANDBY_AUTO;
	                    blink_ACQ_LED = 0;
	                    break;
	                case EVENT_ACQ:
	                    *state = STATE_ACQUIRING_AUTO;
	                    blink_ACQ_LED = 0;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 1);
	                    break;
	                case EVENT_START:
	                	ret = -1;
	                	break;
	                case EVENT_AGC_ON:
	                	ret = -1;
	                    break;
	                case EVENT_AGC_OFF:
	                	*state = STATE_MANUAL_OPERATION;
	                	break;
	            }
	            break;

	        case STATE_ACQUIRING_MANUAL:
	            switch (event) {
					case EVENT_RESTART:
						*state = STATE_STANDBY_MANUAL;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
	                    blink_ACQ_LED = 0;
						break;
	                case EVENT_STOP:
	                    *state = STATE_MANUAL_OPERATION;
	                    blink_ACQ_LED = 1;
	                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
	                    break;
	                case EVENT_START:
	                	ret = -1;
	                	break;
	                case EVENT_AGC_ON:
	                	ret = -1;
	                	break;
	                case EVENT_AGC_OFF:
	                	ret = -1;
	                	break;
	                case EVENT_ACQ:
	                	ret = -1;
	                    break;
	            }
	            break;

		        case STATE_ACQUIRING_AUTO:
		            switch (event) {
						case EVENT_RESTART:
							*state = STATE_STANDBY_MANUAL;
		                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
		                    blink_ACQ_LED = 0;
							break;
		                case EVENT_STOP:
		                    *state = STATE_AUTOMATIC_OPERATION;
		                    blink_ACQ_LED = 1;
		                    HAL_GPIO_WritePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin, 0);
		                    break;
		                case EVENT_START:
		                	ret = -1;
		                	break;
		                case EVENT_AGC_ON:
		                	ret = -1;
		                	break;
		                case EVENT_AGC_OFF:
		                	ret = -1;
		                	break;
		                case EVENT_ACQ:
		                	ret = -1;
		                    break;
		            }
		            break;



	        default:
	            // TODO: Manejo de estado desconocido (si es necesario)
	            break;
	    }
	 return ret;
}
