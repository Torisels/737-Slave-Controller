//
// Created by Gustaw on 29-May-18.
//

#ifndef INC_737_SLAVE_CONTROLLER_ADC_H
#define INC_737_SLAVE_CONTROLLER_ADC_H

#include <avr/io.h>

#define FREQ 1000


class adc {
public:
    adc();
    void change_frequency(uint8_t adcsra);
    uint16_t adc_read(uint8_t ch);
    long read_vcc();
};


#endif //INC_737_SLAVE_CONTROLLER_ADC_H
