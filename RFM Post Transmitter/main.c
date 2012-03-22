/*
 * main.cc
 *
 *  Created on: Mar 18, 2012
 *      Author: pf
 */

// Use HW SPI for communication with RFM-module
#define USE_HW_SPI

#include <msp430.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "rfm.h"
#include "cpu.h"

#include "adc.h"

#define PACKET_BAT_LEVEL 	0xF1
// Two bytes value, indicating current voltage

#define PACKET_ACK			0xF2
#define PACKET_SIGNAL		0xF3

static bool measure = true;
static bool signal = false;

int main(void) {
	cpu_init();
	P1IFG = 0x00;

	/* Clock configuration */
	BCSCTL1 &= ~(XTS); // Low frequency mode
	BCSCTL1 &= ~(DIVA0|DIVA1);
	BCSCTL1 |= DIVA_3;
	BCSCTL3 &= ~(LFXT1S0|LFXT1S1);
	BCSCTL3 |= LFXT1S_2;

	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL;
	IE1 |= WDTIE;

	adc_init();

	P1REN &= ~BIT1;

	/* Let the RF12 module power up completely first */
	delayMs(1000);

	__enable_interrupt();

	/* Initialize RFM-module */
	rf12_initialize(2, RF12_433MHZ, 33);

	//uart_puts("H\n");
	char payload[10];
	unsigned int adc_value;

	while (true) {
		if (rf12_recvDone() && rf12_crc == 0) {
			//uart_puts("GA\n");
		}

		/*if (uart_getc(&c)) {
			uart_putc(c);
		}*/

		if (measure) {
			adc_sensor_read(INCH_1);
			measure = false;
		}

		if( adc_read_result(&adc_value)){
			if (rf12_canSend()) {
				//uart_puts("S:");
				adc_value *=  5;
				itoa(adc_value/1023, payload+1, 10);
				int l = strlen(payload+1);
				payload[l+1] = '.';
				itoa((adc_value%1023)/102, payload+2+l,10);
				//uart_puts(buf);
				//uart_putc('\n');
				payload[0] = PACKET_BAT_LEVEL;
				/*payload[1] = adc_value & 0xFF;
				payload[2] = (adc_value >> 8) & 0xFF;*/
				rf12_sendStart(0, payload, 10);
			}
		}

		/* Power down */
		// Only power down, if there is nothing to do
		if( !adc_result_ready && rxstate == TXIDLE) {
			rf12_control(RF_SLEEP_MODE);
			//rf12_sleep(100);
			__bis_SR_register(LPM3_bits | GIE);
			rf12_control(RF_IDLE_MODE);
		}
	}

	return 0;
}

void __attribute__((interrupt(PORT1_VECTOR)))
PORT1_ISR(void) {
	__bic_SR_register_on_exit(LPM3_bits);
	if (P1IFG & IRQ) {
		rf12_interrupt();
	}
	/*if (P1IFG & RXD) {
		uart_recv_int();
	}*/
	P1IFG = 0x00;
}

void __attribute__((interrupt(WDT_VECTOR)))
WATCHDOG_ISR(void) {
	measure = true;
	__bic_SR_register_on_exit(LPM3_bits);
}
