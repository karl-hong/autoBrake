#ifndef _CLOCK_H_
#define _CLOCK_H_
#include "typedef.h"
/* register: CLKCON */
typedef enum {
    CLK_EN_EXTERN = 0x04,
    CLK_EN_INTERNAL = 0x02,
    CLK_EN_MASK = 0x06,
}clkcon_t;

/* rigster: CLKSWR */
typedef enum {
    CLKSEL_INTERNEL_LOW_RC = 0x00,
    CLKSEL_INTERNEL_HIGH_RC = 0x10,
    CLKSEL_EXTERNEL_LOW_OSC = 0x20,
    CLKSEL_EXTERNEL_HIGH_OSC = 0x30,
    CLKSEL_MASK = 0x30,
}clksel_t;

typedef enum {
    /* data */
    RC32M_CLK   = 0x00,
    RC32M_CLK_2 = 0x01,
    RC32M_CLK_4 = 0x02,
    RC32M_CLK_8 = 0x03,//default
    RC32M_CLK_MASK = 0x03,
}rc32m_div_t;

/*
 * 功能：使能时钟
 * 输入参数：CLK_EN_EXTERN(开启外部时钟)、CLK_EN_INTERNAL(开启内部RC)
 */
void clock_enable(clkcon_t clk);

/*
 * 功能：选择时钟源
 * 输入参数：CLKSEL_INTERNEL_LOW_RC-内部低频RC-44K
 *          CLKSEL_INTERNEL_HIGH_RC-内部高频RC-32M
 *          CLKSEL_EXTERNEL_LOW_OSC-外部时钟
 *          CLKSEL_EXTERNEL_HIGH_OSC-外部时钟
 */
void clock_set_source(clksel_t rc);

/*
 * 功能：设置RC32M分频功能（Fosc）
 * 输入参数：RC32M_CLK   - 32M
 *          RC32M_CLK_2 - 16M
 *          RC32M_CLK_4 - 8M
 *          RC32M_CLK_8 - 4M
 */
void clock_set_rc32m_div(rc32m_div_t div);

/*
 * 功能：设置Fcpu
 * 输入参数：0 - Fosc
 *          1 - Fosc
 *          2 - Fosc / 2
 *          ......
 *          255 - Fosc / 255
 */
void clock_set_cpu_div(uint8_t div);

#endif
