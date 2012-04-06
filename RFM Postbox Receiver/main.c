/*
 * main.cc
 *
 *  Created on: Mar 18, 2012
 *      Author: pf
 */

// Use HW SPI for communication with RFM-module
#define USE_HW_SPI

#include <stdbool.h>
#include "config.h"
#include "rfm.h"
#include "cpu.h"
#include "uart.h"

#define PACKET_BAT_LEVEL 	0xF1
#define PACKET_ACK			0xF2
#define PACKET_SIGNAL		0xF3

extern volatile bool isReceiving;

int main(void) {
	cpu_init();
	P1IFG = 0x00;
	uart_init();

	delayMs(1000);

	__enable_interrupt();

	/* Initialize RFM-module */
	rf12_initialize(2, RF12_433MHZ, 33);

	uint8_t c = 0;
	uart_puts("Hello\n");
	uint8_t payload[1];

	bool sendAck = false;
	while (true) {
		if (rf12_recvDone() && rf12_crc == 0) {
			switch(rf12_data[0]) {
			case PACKET_BAT_LEVEL:
				uart_puts("BATTERY LEVEL: ");
				uart_puts(rf12_data+1);
				break;
			case PACKET_SIGNAL:
				uart_puts("SIGNAL: ");
				uart_puts(rf12_data+1);
				break;
			default:
				uart_puts("UNKNOWN");
				break;
			}
			uart_putc('\n');

			/* Next, send back a response, to let the other end know
			 * that we are hearing it loud and clear */
			//sendAck = true;
		}
/*		if (sendAck == true && rf12_canSend()) {
			sendAck = false;
			payload[0] = PACKET_ACK;
			rf12_sendStart(0, payload, 1);
		}*/

		if (uart_getc(&c)) {
			if( c == 'I' ) {
				uart_putc('O');
				uart_putc('\n');
			}
			//sendAck = true;
		}

		/* Power down */
		// Only power down, if there is nothing to do
		if( !isReceiving)
			__bis_SR_register(LPM3_bits | GIE);
	}

	return 0;
}

void __attribute__((interrupt(PORT1_VECTOR)))
PORT1_ISR(void) {
	__bic_SR_register_on_exit(LPM3_bits);
	if (P1IFG & IRQ) {
		rf12_interrupt();
	}
	if (P1IFG & RXD) {
		uart_recv_int();
	}
	P1IFG = 0x00;
}
