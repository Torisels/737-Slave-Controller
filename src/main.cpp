//
// Created by Gustaw on 25-May-18.
//

#include <Arduino.h>
#include <Wire1.h>
#include "adc.h"


//INPUT PULL-UPS ARE NOT REQUIRED IN SLAVE

//CONSTANTS
#define DEVICE_ID 0
#define FLAG_SETUP 0xFA
#define FLAG_OUT 0xFB
#define FLAG_SEND 0xFD
#define USE_ANALOG_FLAG 0xFE
#define FLAG_SETUP_DONE 0xCF
//SENDING BUFFER SIZES
#define SENDING_BUFFER_SIZE 14 // FLAG_SEND, Dev_ID, PINA-D, ANALOG0-7
#define SENDING_BUFFER_SIZE_NO_ANALOG 6
#define SENDING_BUFFER_VCC_SIZE 2
//RX BUFFER
#define MAX_RX_BUFFER_SIZE 32
//ADC
#define ADC_CHANNELS 8
//POSITIONS todo: Add all postiions
#define PORTA_POS 1
#define PORTB_POS 2
#define PORTC_POS 3
#define PORTD_POS 4

//FLAGS
uint8_t FLAGS = 0;
#define INPUT_READY (1<<0)
#define SETUP_DONE (1<<1)
#define DATA_REQUESTED (1<<2)
#define FLAG_DATA_RECEIVED (1<<3)
#define USE_ANALOG (1<<4)

//BUFFERS
uint8_t rx_buffer[MAX_RX_BUFFER_SIZE];
//uint8_t sending_buffer[SENDING_BUFFER_SIZE];
//ANALOG DYNAMIC MEMORY
uint8_t ANALOG_CHANNELS_ACTIVE = 0;
uint8_t ANALOG_CHANNEL_BIT_MASK = 0;
uint16_t ANALOG_DATA[ADC_CHANNELS] = {};
uint8_t ADC_RESULT_L = 0;
uint8_t ADC_RESULT_H = 0;



uint8_t BYTES_RECEIVED=0;


extern "C"{
    #define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
}

TwoWire Wire = TwoWire(rx_buffer);
adc ad = adc();
//void getChannels(uint8_t *arr,uint8_t size)
//{
//    uint8_t counter = 0;
//    for(int i = 0; i<ADC_CHANNELS;i++)
//    {
//        if(CHECK_BIT(ANALOG_CHANNEL_BIT_MASK,i))
//        {
//            arr[counter] = ANALOG_DATA[i];
//            counter++;
//        }
//    }
//}

/*
 * FLAG SETUP:
 * -order of bytes is DDRA,PORTA,DDRB,PORTB,DDRC,PORTC,DDRD,PORTD,USE_ANALOG_FLAG,NUMBER_OF_ANALOG,ANALOG_BITMASK FLAG
 * FlAG SETUP with input only
 * -DDRA,DDRB,DDRC
 *
 *
 * DATA SEND ROUTINE:
 * FLAG, DEVICE_ID, PINA-D,VCC_H,VCC_L,ADC0-7_H_L
 *
 * */
void receiveEvent(int howMany) {
    FLAGS |=FLAG_DATA_RECEIVED;
    BYTES_RECEIVED = (uint8_t) howMany;
}
void requestEvent() {
//    if(/*(FLAGS & INPUT_READY)&&*/(FLAGS & SETUP_DONE))
//    {
        FLAGS |= DATA_REQUESTED;
    //}
}
void setup()
{
    Wire.begin(DEVICE_ID);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
}

void loop()
{
    if(FLAGS & USE_ANALOG)//analog voltage routine
    {
        uint16_t vcc = ad.read_vcc();
        ADC_RESULT_L = (uint8_t)vcc;
        ADC_RESULT_H = (uint8_t)(vcc>>8);

        int counter = 0;
        for (uint8_t i=0;i<ADC_CHANNELS;i++)
        {
            if(CHECK_BIT(ANALOG_CHANNEL_BIT_MASK,i))
            {
                ANALOG_DATA[counter] = ad.adc_read(i);
                counter++;
            }
        }
    }
    if(FLAGS & FLAG_DATA_RECEIVED)//receiving routine
    {
        uint8_t flag = rx_buffer[0];
        switch (flag)
        {
            case FLAG_SETUP:
                if(BYTES_RECEIVED>=10)
                {
                    DDRA = rx_buffer[1];
                    PORTA = rx_buffer[2];

                    DDRB = rx_buffer[3];
                    PORTB = rx_buffer[4];

                    DDRC = rx_buffer[5];
                    PORTC = rx_buffer[6];

                    DDRD = rx_buffer[7];
                    PORTD = rx_buffer[8];

                    uint8_t analog_flag = rx_buffer[9];
                    if(analog_flag==USE_ANALOG_FLAG)
                    {
                        FLAGS |= USE_ANALOG;
                        ANALOG_CHANNELS_ACTIVE = rx_buffer[10];
                        ANALOG_CHANNEL_BIT_MASK = rx_buffer[11];
                    }

                    FLAGS |= SETUP_DONE;
                }
                break;
//            case FLAG_DATA_READY_TO_SEND:
//                FLAGS |= INPUT_READY;
//                break;
            case FLAG_OUT:
                PORTA = rx_buffer[PORTA_POS];
                PORTB = rx_buffer[PORTB_POS];
                PORTC = rx_buffer[PORTC_POS];
                PORTD = rx_buffer[PORTD_POS];
                break;
            default:
                break;
        }
        FLAGS &= ~FLAG_DATA_RECEIVED;
        BYTES_RECEIVED = 0;
    }
    if(FLAGS & DATA_REQUESTED) //sending routine
    {
        if(FLAGS & SETUP_DONE)
        {
            Wire.write(FLAG_SETUP_DONE,1);
            FLAGS &= ~SETUP_DONE;
        }
        else{
            if(0==(FLAGS&USE_ANALOG))
            {
                uint8_t t_buffer[SENDING_BUFFER_SIZE_NO_ANALOG] = {FLAG_SEND,DEVICE_ID,PINA,PINB,PINC,PIND};
                Wire.write(t_buffer,SENDING_BUFFER_SIZE_NO_ANALOG);
            }
            else
            {
                uint8_t t_buffer[SENDING_BUFFER_SIZE_NO_ANALOG+SENDING_BUFFER_VCC_SIZE+ANALOG_CHANNELS_ACTIVE] = {FLAG_SEND,DEVICE_ID,PINA,PINB,PINC,PIND,ADC_RESULT_H,ADC_RESULT_L};
                uint8_t counter = 0;
                for(int i=0;i<ANALOG_CHANNELS_ACTIVE;i++)
                {
                    t_buffer[SENDING_BUFFER_SIZE_NO_ANALOG+SENDING_BUFFER_VCC_SIZE+counter] = (uint8_t)ANALOG_DATA[i]>>8;
                    counter++;
                    t_buffer[SENDING_BUFFER_SIZE_NO_ANALOG+SENDING_BUFFER_VCC_SIZE+i] = (uint8_t)ANALOG_DATA[i];
                    counter++;
                }

                Wire.write(t_buffer,SENDING_BUFFER_SIZE_NO_ANALOG+SENDING_BUFFER_VCC_SIZE+ANALOG_CHANNELS_ACTIVE);
            }
            FLAGS &= ~DATA_REQUESTED ;
        }
    }
}

