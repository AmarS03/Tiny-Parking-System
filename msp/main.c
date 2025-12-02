#include "msp.h"


/**
 * main.c
 */
void main(void)
{
    // Stop watchdog timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
}
