//#define	ALLOCATE_EXTERN
#include "HC89S003AF4.h"
#include "clock.h"

/*
 * 功能：使能时钟
 * 输入参数：CLK_EN_EXTERN(开启外部时钟)、CLK_EN_INTERNAL(开启内部RC)
 */
void clock_enable(clkcon_t clk)
{
    CLKCON = clk;    
}

/*
 * 功能：选择时钟源
 * 输入参数：CLKSEL_INTERNEL_LOW_RC-内部低频RC-44K
 *          CLKSEL_INTERNEL_HIGH_RC-内部高频RC-32M
 *          CLKSEL_EXTERNEL_LOW_OSC-外部时钟
 *          CLKSEL_EXTERNEL_HIGH_OSC-外部时钟
 */
void clock_set_source(clksel_t rc)
{
    CLKSWR &= (~CLKSEL_MASK);
    CLKSWR |= rc; 
}

/*
 * 功能：设置RC32M分频功能（Fosc）
 * 输入参数：RC32M_CLK   - 32M
 *          RC32M_CLK_2 - 16M
 *          RC32M_CLK_4 - 8M
 *          RC32M_CLK_8 - 4M
 */
void clock_set_rc32m_div(rc32m_div_t div)
{
    CLKSWR &= (~RC32M_CLK_MASK);
    CLKSWR |= div;    
}

/*
 * 功能：设置Fcpu
 * 输入参数：0 - Fosc
 *          1 - Fosc
 *          2 - Fosc / 2
 *          ......
 *          255 - Fosc / 255
 */
void clock_set_cpu_div(uint8_t div)
{
    CLKDIV = div;
}
