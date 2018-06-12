//
// Created by Gustaw on 29-May-18.
//

#include <Arduino.h>
#include "adc.h"


adc::adc() {
    {
        //as default we use Avcc reference
        ADMUX |= (1<<REFS0);
        //we leave ADLAR 0, not left adjusted
        ADMUX |= (0<<ADLAR);
        //turn of the ADC
        ADCSRA |= (1<<ADEN);
        //use highest prescaler => lowest frequency => highest accuracy so 8M/128 = 62.5k
        ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    }
}

void adc::change_frequency(uint8_t adcsra) {
    ADCSRA |= adcsra;
}

uint16_t adc::adc_read(uint8_t ch) {
    // select the corresponding channel 0~7
    // ANDing with ’7′ will always keep the value
    // of ‘ch’ between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing

    // start single convertion
    // write ’1′ to ADSC
    ADCSRA |= _BV(ADSC);

    // wait for conversion to complete
    // ADSC becomes ’0′ again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));

    return (ADC);
}

long adc::read_vcc() {
    long result;
    //refer to Avcc
    ADMUX |= _BV(REFS0);
    // user 1.1V input channel selection
    ADMUX |= _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2);
    //start conversion
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA,ADSC);
    result = ADCL;
    result |= ADCH << 8;
    result = 1126400L / result; // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
    return result;
}

//void adc::getChannels(uint8_t *array) {
//    for(int i = 0; )
//}
//
