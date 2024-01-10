#ifndef _GPIO_H_
#define _GPIO_H_
#include "typedef.h"

typedef enum {
    GPIO_INPUT_WITHOUT_SMT    = 0x00,
    GPIO_INPUT_PD_WITHOUT_SMT = 0x01,
    GPIO_INPUT_PU_WITHOUT_SMT = 0x02,
    GPIO_INPUT_ANALOG         = 0x03,
    GPIO_INPUT_WITH_SMT       = 0x04,
    GPIO_INPUT_PD_WITH_SMT    = 0x05,
    GPIO_INPUT_PU_WITH_SMT    = 0x06,
    GPIO_INPUT_ANALOG_RESERVE = 0x07,
    GPIO_OUTPUT_PP            = 0x08,
    GPIO_OUTPUT_OD            = 0x09,
    GPIO_OUTPUt_OD_PU         = 0x0A,
    GPIO_OUTPUT_PP_RESERVE    = 0x0B,
}gpio_type_t;

typedef enum {
    GPIO_0 = 0,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    GPIO_4,
    GPIO_5,
    GPIO_6,
    GPIO_7,
}gpio_index_t;

void gpio0_init(gpio_index_t gpiox, gpio_type_t type);
void gpio1_init(gpio_index_t gpiox, gpio_type_t type);
void gpio2_init(gpio_index_t gpiox, gpio_type_t type);

#endif
