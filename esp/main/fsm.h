/*
 * fsm.h
 *
 * This file contains the declaration
 * of the finite state machine, its
 * states and the events handled by
 * the system
 *
 *  Created on: Dec 2, 2025
 *      Author: mirco
 */

#ifndef FSM_H_
#define FSM_H_

typedef enum {
    VALID_WEIGHT_DETECTED,
    EXIT_DETECTED,
    REMOTE_OPEN,
    PLATE_RECOGNIZED,
    PLATE_REFUSED,
    NONE
} Event_t;

typedef enum {
    INIT,
    IDLE,
    VEHICLE_ENTRY,
    ENTRY_REFUSED,
    ENTRY_ALLOWED,
    VEHICLE_EXIT,
    STATES_NUM
} State_t;


void fsm_handle_event(Event_t event);

void fsm_run_state_function();


#endif /* FSM_H_ */
