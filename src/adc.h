//
// Created by Gustaw on 29-May-18.
//

#ifndef INC_737_SLAVE_CONTROLLER_ADC_H
#define INC_737_SLAVE_CONTROLLER_ADC_H

#include <avr/io.h>

#define FREQ 1000


class adc {
public:
//    static uint8_t active_channels = 0;
    adc();
    void change_frequency(uint8_t adcsra);
    uint16_t adc_read(uint8_t ch);
    uint16_t read_vcc();
//    static void getChannels(uint8_t* array);
};


#endif //INC_737_SLAVE_CONTROLLER_ADC_H
