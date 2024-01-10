#include "HC89S003AF4.h"
#include "ie.h"

void user_set_interrupt_state(ie_type_t ieType, ie_value_t value)
{
    switch(ieType){
        case IE_TYPE_GLOBAL:{
            EA = value;
            break;
        }

        case IE_TYPE_UART2:{
            ES2 = value;
            break;
        }

        case IE_TYPE_EWDT:{
            EWDT = value;
            break;
        }

        case IE_TYPE_UART1:{
            ES1 = value;
            break;
        }

        case IE_TYPE_TIMER1:{
            ET1 = value;
            break;
        }

        case IE_TYPE_EXTERN1:{
            EX1 = value;
            break;
        }

        case IE_TYPE_TIMER0:{
            ET0 = value;
            break;
        }

        case IE_TYPE_EXTERN0:{
            EX0 = value;
            break;
        }

        case IE_TYPE_EX8_EX15:{
            EX8_15 = value;
            break;
        }

        case IE_TYPE_EX2_EX7:{
            EX2_7 = value;
            break;
        }

        case IE_TYPE_ADC:{
            EADC = value;
            break;
        }

        case IE_TYPE_TIMER5:{
            ET5 = value;
            break;
        }

        case IE_TYPE_IIC:{
            EIIC = value;
            break;
        }

        case IE_TYPE_TIMER4:{
            ET4 = value;
            break;
        }

        case IE_TYPE_TIMER3:{
            ET3 = value;
            break;
        }

        case IE_TYPE_SPI:{
            ESPI = value;
            break;
        }

        default:break;
    }
}