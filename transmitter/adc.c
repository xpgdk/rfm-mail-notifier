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
#include "adc.h"

#include <msp430.h>
#include "cpu.h"

void __attribute__((interrupt(ADC10_VECTOR))) ADC10_ISR(void);

bool adc_result_ready;

void
adc_init(void) {
  ADC10CTL0 = SREF_1 | ADC10SHT_3 | REF2_5V | REFON | ADC10IE | ADC10SR | ADC10ON;
  ADC10CTL1 = ADC10DIV_3;
  ADC10AE0 = 0;

  adc_result_ready = false;
}

bool
adc_read_result(unsigned int *res) {
  if( adc_result_ready == true) {
    adc_result_ready = false;
    *res = ADC10MEM;
    return true;
  }
  return false;
}

void
adc_sensor_read(unsigned int channel) {
  adc_result_ready = false;
  if( channel < INCH_8) {
    ADC10AE0 = 1 << ((channel & 0xF000) >> 12);
  } else {
    ADC10AE0 = 0;
  }

  ADC10CTL1 &= ~INCH_15;
  ADC10CTL1 |= channel;

  ADC10CTL0 |= ENC | ADC10SC;
}

void __attribute__((interrupt(ADC10_VECTOR)))
  ADC10_ISR(void) {
    ADC10CTL0 &= ~(ENC);
    adc_result_ready = true;
    __bic_SR_register_on_exit(LPM3_bits);
  }
