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
	EVENT_RESTART,					   // Restart the state Machine
    EVENT_START,                       // Start event
    EVENT_STOP,                        // Stop event
    EVENT_AGC_ON,      				   // Run the automatic gain control algoritm
	EVENT_AGC_OFF,
    EVENT_ACQ,           			   // PC is writing data on file
} event_type;

// State definitions
typedef enum {
    STATE_STANDBY_MANUAL,               // Off state
	STATE_STANDBY_AUTO,               	// Off state
    STATE_MANUAL_OPERATION,             // Manual Gain control
    STATE_AUTOMATIC_OPERATION,          // AGC Algoritm gain control
    STATE_ACQUIRING_AUTO,          		// Acquiring with gain set by AGC algoritm
	STATE_ACQUIRING_MANUAL				// Acquiring with gain set by potentiometer

} state_type;


// Function declaration for event handling
int8_t state_machine(event_type event, state_type* state);
extern state_type current_state;
extern uint8_t blink_ACQ_LED;

#endif /* INC_STATE_MACHINE_H_ */
