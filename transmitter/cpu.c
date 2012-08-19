/*
 * Copyright (c) 2012 Paul Fleischer <paul@xpg.dk>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
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
