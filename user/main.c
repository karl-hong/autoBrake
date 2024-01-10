#define	ALLOCATE_EXTERN
#include "HC89S003AF4.h"
#include "clock.h"
#include "my_printf.h"
#include <stdio.h>
#include "typedef.h"
#include "gpio.h"
#include "led.h"
#include "ie.h"
#include "common.h"
#include "key.h"
#include "bat.h"

uint16_t cnt = 0;
main_context_t gMainContext;

void Delay_ms(unsigned int fui_i);
void timer0_init(void);

void main(void)
{
     char test = 0;
	WDTCCR = 0x00;						//关闭看门狗
     /* init clock */
     clock_enable(CLK_EN_INTERNAL);
     clock_set_source(CLKSEL_INTERNEL_HIGH_RC);
     clock_set_rc32m_div(RC32M_CLK_2);//Fosc = 16M
     clock_set_cpu_div(1);//Fcpu = Fosc，由于串口需要Fosc = Fcpu

     /* init uart1 */
     UART1_Init();
     /* led init */
     led_init();
     /* timer init */
     timer0_init();
     /* key init */
     key_init();
     /* check bat init */
     bat_init();

     /* enable interrupt */
     user_set_interrupt_state(IE_TYPE_GLOBAL, IE_ENABLE);

     while(1){
          // // key_task();
          check_bat_task();
          bat_led_task();
     }

}


void Delay_ms(unsigned int fui_i)
{
	unsigned int fui_j;
	for(;fui_i > 0;fui_i --)
	for(fui_j = 1596;fui_j > 0;fui_j --);
}

void timer0_init(void)
{
  TCON1 = 0x00;						  //Tx0定时器时钟为Fosc
	TMOD = 0x00;						  //16位重装载定时器/计数器

	//Tim0计算时间 	= (65536 - 0xFACB) * (1 / (Fosc /Timer分频系数))
	//				= 1333 / (16000000 / 12)
	//				= 1 ms

	//定时1ms
	//反推初值 	= 65536 - ((1/1000) / (1/(Fosc / Timer分频系数)))
	//		   	= 65536 - ((1/1000) / (1/(16000000 / 12)))
	//			= 65536 - 1333
	//			= 0xFACB
	TH0 = 0xFA;
	TL0 = 0xCB;							  //T0定时时间1ms
	IE |= 0x02;							  //打开T0中断
	TCON |= 0x10;						  //使能T0
}

/***************************************************************************************
  * @说明  	T0中断服务函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void TIMER0_Rpt(void) interrupt TIMER0_VECTOR
{ /* 1ms */
	gMainContext.mKeyDelay = true;
     gMainContext.mBatDelay = true;
     gMainContext.mLedDelay = true;
}
