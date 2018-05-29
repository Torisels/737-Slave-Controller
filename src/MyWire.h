//
// Created by Gustaw on 25-May-18.
//

#ifndef INC_737_SLAVE_CONTROLLER_MYWIRE_H
#define INC_737_SLAVE_CONTROLLER_MYWIRE_H

#endif //INC_737_SLAVE_CONTROLLER_MYWIRE_H


#define DEVICE_ID 1
#define FLAG_SETUP 0xFA
#define FLAG_OUT 0xFB
#define FLAG_INPUT 0xFC
#define SENDING_BUFFER_SIZE 6
#define MAX_BUFFER_SIZE 64

#define PORTA_POS 1
#define PORTB_POS 2
#define PORTC_POS 3
#define PORTD_POS 4

void handle_receive_data();
