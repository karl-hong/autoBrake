#ifndef _LED_H_
#define _LED_H_

typedef enum {
    LED_LEVEL_OFF = 0,
    LED_LEVEL_LOW,
    LED_LEVEL_HIGH,
}led_levet_t;

typedef enum {
    LED_STATE_OFF,
    LED_STATE_ON,
}led_state_t;

void led_init(void);
void user_set_brake_led_level(led_levet_t level);
void user_set_blue_led_state(led_state_t state);
void user_set_red_led_state(led_state_t state);

#endif
