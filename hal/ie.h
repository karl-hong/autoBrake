#ifndef _IE_H_
#define _IE_H_

typedef enum {
    IE_TYPE_GLOBAL,
    IE_TYPE_UART2,
    IE_TYPE_EWDT,
    IE_TYPE_UART1,
    IE_TYPE_TIMER1,
    IE_TYPE_EXTERN1,
    IE_TYPE_TIMER0,
    IE_TYPE_EXTERN0,
    IE_TYPE_EX8_EX15,
    IE_TYPE_EX2_EX7,
    IE_TYPE_ADC,
    IE_TYPE_TIMER5,
    IE_TYPE_IIC,
    IE_TYPE_TIMER4,
    IE_TYPE_TIMER3,
    IE_TYPE_SPI,
}ie_type_t;

typedef enum {
    IE_DISABLE = 0,
    IE_ENABLE  = 1,
}ie_value_t;

void user_set_interrupt_state(ie_type_t ieType, ie_value_t value);

#endif
