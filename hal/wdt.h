#ifndef _WDT_H_
#define _WDT_H_

typedef enum {
    wdt_disable_rst = 0x00,
    wdt_enable_rst = 0x40,
}wdt_rst_t;

typedef enum {
    wdt_enable_in_idle_mode = 0x00,
    wdt_disable_in_idle_mode = 0x04,
}wdt_mode_t;

typedef enum {
    wdt_period_46ms   = 0x00,//46.55ms
    wdt_period_93ms   = 0x01,//93.06ms
    wdt_period_186ms  = 0x02,//186.18ms
    wdt_period_372ms  = 0x03,//372.36ms
    wdt_period_744ms  = 0x04,//744.73ms
    wdt_period_1489ms = 0x05,//1489.45ms
    wdt_period_2978ms = 0x06,//2978.91ms
    wdt_period_5957ms = 0x07,//5957.82ms
}wdt_period_t;

void wdt_config(wdt_rst_t enable, wdt_mode_t mode, wdt_period_t peroid);
void wdt_enable(bool enable);

#endif
