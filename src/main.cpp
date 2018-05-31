//
// Created by Gustaw on 25-May-18.
//
#include <Arduino.h>
#include <Wire.h>
#include <MyWire.h>

//INPUT PULL-UPS ARE NOT REQUIRED IN SLAVE
#define DEVICE_ID 1
#define FLAG_SETUP 0xFA
#define FLAG_OUT 0xFB
#define FLAG_IN 0xFC
#define SENDING_BUFFER_SIZE 6
#define MAX_BUFFER_SIZE 64

#define PORTA_POS 1
#define PORTB_POS 2
#define PORTC_POS 3
#define PORTD_POS 4


int current_lenght=0;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t FLAG_INPUT_READY = 0;
uint8_t FLAG_SETUP_DONE = 0;





/*
 * FLAG SETUP:
 * -order of bytes is DDRA,PORTA,DDRB,PORTB,DDRC,PORTC,DDRD,PORTD
 * FlAG SETUP with input only
 * -DDRA,DDRB,DDRC
 *
 *
 * */
void handleRecieve()
{
    uint8_t Flag = buffer[0];
    switch (Flag)
    {
        case FLAG_SETUP:
            if(current_lenght>=8)
            {
                DDRA = buffer[1];
                PORTA = buffer[2];

                DDRB = buffer[3];
                PORTB = buffer[4];

                DDRC = buffer[5];
                PORTC = buffer[6];

                DDRD = buffer[7];
                PORTD = buffer[8];
                FLAG_SETUP_DONE = 1;
            }
            break;
        case FLAG_IN:
            FLAG_INPUT_READY = 1;
            break;
        case FLAG_OUT:
            PORTA = buffer[PORTA_POS];
            PORTB = buffer[PORTB_POS];
            PORTC = buffer[PORTC_POS];
            PORTD = buffer[PORTD_POS];
            break;
    }
        current_lenght = 0;
}
void receiveEvent(int howMany) {
    uint8_t buf[howMany] = {};
    int a = 0;
    while ((a = Wire.available())) {
        buf[howMany-a] = Wire.read();
}
    current_lenght = howMany;
    memmove(buffer,buf,sizeof(buf));
    handleRecieve();
}
void requestEvent() {
    if(FLAG_INPUT_READY&&FLAG_SETUP_DONE)
    {
        uint8_t dev_id = DEVICE_ID;
        uint8_t temp_buffer[SENDING_BUFFER_SIZE] = {FLAG_IN,dev_id,PINA,PINB,PINC,PIND};
        Wire.write(temp_buffer,SENDING_BUFFER_SIZE);
        FLAG_INPUT_READY = 0;
    }
}

void setup()
{
    Wire.begin(DEVICE_ID);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
}

void loop()
{
    delay(2);
}

