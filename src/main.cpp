//
// Created by Gustaw on 25-May-18.
//

#define TWDR
#include <Arduino.h>
#include <Wire1.h>
#include <MyWire.h>
//#include <Wire.h>

//INPUT PULL-UPS ARE NOT REQUIRED IN SLAVE
#define DEVICE_ID 1
#define FLAG_SETUP 0xFA
#define FLAG_OUT 0xFB
#define FLAG_DATA_READY_TO_SEND 0xFC
#define FLAG_SEND 0xFD
#define SENDING_BUFFER_SIZE 14 // FLAG_SEND, Dev_ID, PINA-D, ANALOG0-7
#define SENDING_BUFFER_SIZE_NO_ANALOG 6
#define MAX_RX_BUFFER_SIZE 32
#define ADC_CHANNELS 8

#define PORTA_POS 1
#define PORTB_POS 2
#define PORTC_POS 3
#define PORTD_POS 4



uint8_t rx_buffer[MAX_RX_BUFFER_SIZE];
uint8_t FLAG_INPUT_READY = 0;
uint8_t FLAG_SETUP_DONE = 0;
uint8_t FLAG_DATA_REQUESTED = 0;

uint8_t FLAG_USE_ANALOG = 0;
uint8_t ANALOG_CHANNELS_ACTIVE = 0;
uint8_t ANALOG_CHANNELS = 0;
uint8_t ANALOG_DATA[ADC_CHANNELS] = {};

uint8_t sending_buffer[SENDING_BUFFER_SIZE];
uint8_t FLAG_DATA_RECEIVED = 0;
uint8_t BYTES_RECEIVED=0;

extern "C"{
    #define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
}

TwoWire Wire = TwoWire(rx_buffer);

void getChannels(uint8_t *arr,uint8_t size)
{
    uint8_t counter = 0;
    for(int i = 0; i<ADC_CHANNELS;i++)
    {
        if(CHECK_BIT(ANALOG_CHANNELS,i))
        {
            arr[counter] = ANALOG_DATA[i];
            counter++;
        }
    }
}

/*
 * FLAG SETUP:
 * -order of bytes is DDRA,PORTA,DDRB,PORTB,DDRC,PORTC,DDRD,PORTD,USE_ANALOG_FLAG,NUMBER_OF_ANALOG,ANALOG_BITMASK FLAG
 * FlAG SETUP with input only
 * -DDRA,DDRB,DDRC
 *
 *
 * */
void handleRecieve()
{

}
void receiveEvent(int howMany) {
    FLAG_DATA_RECEIVED = 1;
    BYTES_RECEIVED = howMany;
}
void requestEvent() {
    if(FLAG_INPUT_READY&&FLAG_SETUP_DONE)
    {
        FLAG_DATA_REQUESTED = 1;
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
    if(FLAG_DATA_REQUESTED) //sending routine
    {
        if(!FLAG_USE_ANALOG)
        {
           uint8_t t_buffer[SENDING_BUFFER_SIZE_NO_ANALOG] = {FLAG_SEND,DEVICE_ID,PINA,PINB,PINC,PIND};
           Wire.write(t_buffer,SENDING_BUFFER_SIZE_NO_ANALOG);
        }
        else
        {
            uint8_t t_buffer[SENDING_BUFFER_SIZE_NO_ANALOG+ANALOG_CHANNELS_ACTIVE] = {FLAG_SEND,DEVICE_ID,PINA,PINB,PINC,PIND};
            uint8_t channels[ANALOG_CHANNELS_ACTIVE] = {};
            getChannels(channels,ANALOG_CHANNELS_ACTIVE);
            for(int i=0;i<ANALOG_CHANNELS_ACTIVE;i++)
            {
                t_buffer[SENDING_BUFFER_SIZE_NO_ANALOG+i] = channels[i] ;
            }
            Wire.write(t_buffer,SENDING_BUFFER_SIZE_NO_ANALOG+ANALOG_CHANNELS_ACTIVE);
        }
        FLAG_DATA_REQUESTED = 0;
    }
    if(FLAG_DATA_RECEIVED)
    {
        uint8_t flag = rx_buffer[0];
        switch (flag)
        {
            case FLAG_SETUP:
                if(BYTES_RECEIVED>=11)
                {
                    DDRA = rx_buffer[1];
                    PORTA = rx_buffer[2];

                    DDRB = rx_buffer[3];
                    PORTB = rx_buffer[4];

                    DDRC = rx_buffer[5];
                    PORTC = rx_buffer[6];

                    DDRD = rx_buffer[7];
                    PORTD = rx_buffer[8];

                    FLAG_USE_ANALOG = rx_buffer[9];
                    if(FLAG_USE_ANALOG)
                    {
                        ANALOG_CHANNELS_ACTIVE = rx_buffer[10];
                        ANALOG_CHANNELS= rx_buffer[11];
                    }

                    FLAG_SETUP_DONE = 1;
                }
                break;
            case FLAG_DATA_READY_TO_SEND:
                FLAG_INPUT_READY = 1;
                break;
            case FLAG_OUT:
                PORTA = rx_buffer[PORTA_POS];
                PORTB = rx_buffer[PORTB_POS];
                PORTC = rx_buffer[PORTC_POS];
                PORTD = rx_buffer[PORTD_POS];
                break;
            default:
                break;
        }
        FLAG_DATA_RECEIVED = 0;
        BYTES_RECEIVED = 0;
    }
}

