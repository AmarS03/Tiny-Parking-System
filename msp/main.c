#include "msp.h"
#include "fsm.h"

//variables
Event_t event = NONE;

State_t curr_state = INIT;

Fsm_t fsm[] = {
               {INIT, init_fn},
               {IDLE, idle_fn},
               {VEHICLE_ENTRY, entry_fn},
               {ENTRY_REFUSED, refuse_fn},
               {ENTRY_ALLOWED, allow_fn},
               {VEHICLE_EXIT, exit_fn},
               {CLOSED, closed_fn}
};

/**
 * main.c
 */
void main(void)
{
    // Stop watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

	//main loop
	while(1) {
	    if (curr_state < STATES_NUM) {
	        fsm[curr_state].state_function();
	    } else {
	        //handle error
	    }
	}
}
