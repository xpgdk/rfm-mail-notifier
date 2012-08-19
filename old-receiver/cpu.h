/*
 * cpu.h
 *
 *  Created on: Mar 18, 2012
 *      Author: pf
 */

#ifndef CPU_H_
#define CPU_H_


void delayMs(int ms);

void cpu_init(void);

/* Spends 3 * n cycles */
__inline__ static void delay_cycles(register unsigned int n) {
        __asm__ __volatile__ (
                        "1: \n"
                        " dec   %[n] \n"
                        " jne   1b \n"
                        : [n] "+r"(n));
}

#endif /* CPU_H_ */
