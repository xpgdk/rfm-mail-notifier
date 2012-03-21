/*
 * cpu.c
 *
 *  Created on: Aug 16, 2011
 *      Author: pf
 */

#include "cpu.h"
#include <msp430.h>

#if 1
#define CALDCO_16MHZ_          0x10F8    /* DCOCTL  Calibration Data for 1MHz */
const_sfrb(CALDCO_16MHZ, CALDCO_16MHZ_);
#define CALBC1_16MHZ_          0x10F9    /* BCSCTL1 Calibration Data for 1MHz */
const_sfrb(CALBC1_16MHZ, CALBC1_16MHZ_);
#endif

#ifndef CPU_FREQ
#define CPU_FREQ 1
#endif

void
cpu_init(void) {
	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

	/* Set proper CPU clock speed */
	DCOCTL = 0;
#if CPU_FREQ == 1
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
#elif CPU_FREQ == 8
    BCSCTL1 = CALBC1_8MHZ
    DCOCTL = CALDCO_8HZ;
#elif CPU_FREQ == 12
    BCSCTL1 = CALBC1_12MHZ
    DCOCTL = CALDCO_12HZ;
#elif CPU_FREQ == 16
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
#else
#error "Unsupported CPU frequency"
#endif
}
