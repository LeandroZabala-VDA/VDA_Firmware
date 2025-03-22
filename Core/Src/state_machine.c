/*
 * state_machine.c
 *
 *  Created on: Mar 19, 2025
 *      Author: leank
 */

#include "state_machine.h"
#include "main.h"


int8_t state_machine(event_type event, state_type* state) {

	int8_t ret = 0;

	 switch (*state) {
	        case STATE_STANDBY:
	            switch (event) {
	                case EVENT_START:
	                    *state = STATE_MANUAL_OPERATION;
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
	                case EVENT_STOP:
	                    *state = STATE_STANDBY;
	                    break;
	                case EVENT_AGC_ON:
	                    *state = STATE_AUTOMATIC_OPERATION;
	                    break;
	                case EVENT_AGC_OFF:
	                	ret = -1;
	                	break;
	                case EVENT_ACQ:
	                    *state = STATE_ACQUIRING;
	                    break;
	                case EVENT_START:
	                	ret = -1;
	                	break;
	            }
	            break;

	        case STATE_AUTOMATIC_OPERATION:
	            switch (event) {
	                case EVENT_STOP:
	                    *state = STATE_STANDBY;
	                    break;
	                case EVENT_ACQ:
	                    *state = STATE_ACQUIRING;
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

	        case STATE_ACQUIRING:
	            switch (event) {
	                case EVENT_STOP:
	                    *state = STATE_STANDBY;
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
