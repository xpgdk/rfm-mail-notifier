#include "adc.h"

#include <msp430.h>
#include "cpu.h"

void __attribute__((interrupt(ADC10_VECTOR))) ADC10_ISR(void);

static bool adc_result_ready;

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
    __bic_SR_register_on_exit(CPUOFF);
  }
