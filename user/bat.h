#ifndef _BAT_H_
#define _BAT_H_

typedef enum {
    BAT_LEVEL_42    = 0,//VDD >= 4.2V
    BAT_LEVEL_39    = 1,//VDD >= 3.9V
    BAT_LEVEL_36    = 2,//VDD >= 3.6V
    BAT_LEVEL_30    = 3,//VDD >= 3.0V
    BAT_LEVEL_INVALID,
}bat_level_t;

void bat_init(void);
void check_bat_task(void);
void bat_led_task(void);

#endif
