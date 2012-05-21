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
#define PACKET_ACK			0xF2
#define PACKET_SIGNAL		0xF3

/*
 * How often do we measure and send out battery voltages
 * Watchdog timer is running at a frequency of 21s.
 */
#define COUNT_RESET_VALUE 	1029; // Fire once every 6th hour

static bool measure = true;
static bool sendSignal = false;
static unsigned int counter  = COUNT_RESET_VALUE;

int main(void) {
	cpu_init();

	/* Clock configuration. CPU is running at ~1MHz */
	BCSCTL1 &= ~(XTS); // Low frequency mode
	BCSCTL1 &= ~(DIVA0 | DIVA1);
	BCSCTL1 |= DIVA_3; // ACLK = 12kHz / 8 = 1500Hz
	BCSCTL3 &= ~(LFXT1S0 | LFXT1S1);
	BCSCTL3 |= LFXT1S_2;

	/* Watchdog timer running off ACLK with a divider of 32768 =>
	 * 	~0.045Hz ~ 21s */
	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL;
	IE1 |= WDTIE;

	adc_init();

	P1REN |= BIT0; /* Enable pull-up/down*/
	P1IFG &= ~BIT0;
	P1IE |= BIT0; // Enable interrupt for BIT2
	P1DIR &= ~BIT0; /* Input */

	P1OUT |= BIT0; /* Pull-up*/

	/* Let the RF12 module power up completely first */
	delayMs(1000);
	__enable_interrupt();

	/* Initialize RFM-module */
	rf12_initialize(2, RF12_433MHZ, 33);

	char payload[10];
	unsigned int adc_value;

	while (true) {

		rf12_recvDone();

		if (measure) {
			adc_sensor_read(INCH_2);
			measure = false;
			counter = COUNT_RESET_VALUE;
		}

		if (adc_read_result(&adc_value)) {
			ADC10CTL0 &= ~(REFON | ADC10ON);
			if (rf12_canSend()) {
				adc_value *= 50;
				adc_value /= 1023;
				//itoa(adc_value / 1023, payload + 1, 10);

				payload[1] = (adc_value >> 8) & 0xFF;
				payload[2] = (adc_value) & 0xFF;

				if( sendSignal ) {
					payload[0] = PACKET_SIGNAL;
				} else {
					payload[0] = PACKET_BAT_LEVEL;
				}
				rf12_sendStart(0, payload, 3);
			}
			sendSignal = false;
		} /*else if (sendSignal) {
			if (rf12_canSend()) {
				payload[0] = PACKET_SIGNAL;
				rf12_sendStart(0, payload, 1);
				measure = true;
				ADC10CTL0 |= (REFON | ADC10ON);
			}
			sendSignal = false;
		}*/

		/* Power down */
		// Only power down, if there is nothing to do
		if (!adc_result_ready && !measure && !sendSignal) {
			__bis_SR_register(LPM3_bits | GIE);
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
	if (P1IFG & BIT0) {
		P1IE &= ~BIT0; // Disable interrupt for BIT2
		sendSignal = true;
		measure = true;
		ADC10CTL0 |= (REFON | ADC10ON);
	}
	P1IFG = 0x00;
}

void __attribute__((interrupt(WDT_VECTOR)))
WATCHDOG_ISR(void) {
	if (counter == 0) {
		measure = true;
		ADC10CTL0 |= (REFON | ADC10ON);
		__bic_SR_register_on_exit(LPM3_bits);
	} else {
		counter--;
	}

	// Enable interrupt for BIT2, but clear interrupt flag first
	P1IFG &= ~BIT0;
	P1IE |= BIT0;
}
