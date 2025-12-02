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

} Event_t;

typedef enum {
    INIT,
    IDLE,
    VEHICLE_ENTRY,
    ENTRY_REFUSED,
    ENTRY_ALLOWED,
    VEHICLE_EXIT,
    CLOSED,
    STATES_NUM
} State_t;


typedef struct {
    State_t state;
    void (*state_function)(void);
} Fsm_t;


#endif /* FSM_H_ */
