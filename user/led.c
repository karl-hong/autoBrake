#include "HC89S003AF4.h"
#include "led.h"
#include "gpio.h"

void led_init(void)
{
    /* pwm: p0.4*/
    gpio0_init(GPIO_4, GPIO_OUTPUT_PP);
    PWM3_MAP = 0x04;
    PWM3P = 0xff;
    PWM3D = 0x00;
    PWM3C = 0x92;

    /* 蓝色指示灯：P0.0 */
    gpio0_init(GPIO_0, GPIO_OUTPUT_PP);
    P0_0 = 1;

    /* 红色指示灯: P1.0 */
    gpio1_init(GPIO_0, GPIO_OUTPUT_PP);
    P1_0 = 1;
}

void user_set_brake_led_level(led_levet_t level)
{
    switch(level){
        case LED_LEVEL_OFF:{
            PWM3D = 0x00;
            break;
        }

        case LED_LEVEL_LOW:{
            PWM3D = 0x10;
            break;
        }

        case LED_LEVEL_HIGH:{
            PWM3D = 0x55;
            break;
        }

        default:
            break;
    }
}

void user_set_blue_led_state(led_state_t state)
{
    if(LED_STATE_OFF == state){
        P0_0 = 1;
    }else{
        P0_0 = 0;
    }
}

void user_set_red_led_state(led_state_t state)
{
    if(LED_STATE_OFF == state){
        P1_0 = 1;
    }else{
        P1_0 = 0;
    }
}