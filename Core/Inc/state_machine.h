/*
 * state_machine.h
 *
 *  Created on: Mar 19, 2025
 *      Author: leank
 */

#ifndef INC_STATE_MACHINE_H_
#define INC_STATE_MACHINE_H_

#include <stdint.h>

// Event definitions
typedef enum {
    EVENT_START,                       // Start event
    EVENT_STOP,                        // Stop event
    EVENT_AGC_ON,      				   // Run the automatic gain control algoritm
	EVENT_AGC_OFF,
    EVENT_ACQ,           			   // PC is writing data on file
} event_type;

// State definitions
typedef enum {
    STATE_STANDBY,                      // Off state
    STATE_MANUAL_OPERATION,             // Starting test state
    STATE_AUTOMATIC_OPERATION,          // Normal operation state
    STATE_ACQUIRING                		// Error stop state
} state_type;


// Function declaration for event handling
int8_t state_machine(event_type event, state_type* state);
extern state_type current_state;

#endif /* INC_STATE_MACHINE_H_ */
