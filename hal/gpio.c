#include "HC89S003AF4.h"
#include "gpio.h"


void gpio0_init(gpio_index_t gpiox, gpio_type_t type)
{
    switch(gpiox){
        case GPIO_0:P0M0 = P0M0 & 0xF0 | type;break;
        case GPIO_1:P0M0 = P0M0 & 0x0F | (type << 4);break;
        case GPIO_2:P0M1 = P0M1 & 0xF0 | type;break;
        case GPIO_3:P0M1 = P0M1 & 0x0F | (type << 4);break;
        case GPIO_4:P0M2 = P0M2 & 0xF0 | type;break;
        case GPIO_5:P0M2 = P0M2 & 0x0F | (type << 4);break;
        case GPIO_6:P0M3 = P0M3 & 0xF0 | type;break;
        case GPIO_7:P0M3 = P0M3 & 0x0F | (type << 4);break;
        default:break;
    }
}

void gpio1_init(gpio_index_t gpiox, gpio_type_t type)
{
    switch(gpiox){
        case GPIO_0:P1M0 = P1M0 & 0xF0 | type;break;
        case GPIO_1:P1M0 = P1M0 & 0x0F | (type << 4);break;
        default:break;
    }
}

void gpio2_init(gpio_index_t gpiox, gpio_type_t type)
{
    switch(gpiox){
        case GPIO_0:P2M0 = P2M0 & 0xF0 | type;break;
        case GPIO_1:P2M0 = P2M0 & 0x0F | (type << 4);break;
        case GPIO_2:P2M1 = P2M1 & 0xF0 | type;break;
        case GPIO_3:P2M1 = P2M1 & 0x0F | (type << 4);break;
        case GPIO_4:P2M2 = P2M2 & 0xF0 | type;break;
        case GPIO_5:P2M2 = P2M2 & 0x0F | (type << 4);break;
        case GPIO_6:P2M3 = P2M3 & 0xF0 | type;break;
        case GPIO_7:P2M3 = P2M3 & 0x0F | (type << 4);break;
        default:break;
    }
}