#include "my_printf.h"
#include <stdarg.h>
#include <stdio.h>
#include "HC89S003AF4.h"
#include "ie.h"

void UART1_Init(void)
{
    /* 设置P2.4为推挽输出 */
    P2M2 = P2M2 & 0xF0 | 0x08;
    /* 映射TXD 为P24 */
    TXD_MAP = 0x24;

    /* UART1的波特率: 00 T4 */
    BRTSEL = 0x00;

//    P2M0 = P2M0&0x0F|0x80;				      //P21设置为推挽输出
//		TXD_MAP = 0x21;						          //TXD映射P21
//		BRTSEL = 0X00;                      //UART1的波特率:00 T4	


    /* T4工作模式：UART1波特率发生器 */
    T4CON = 0x06;

    /* 设置波特率: 19200 */
    TH4 = 0xFF;
    TL4 = 0x98;

    /* 8位UART, 可变波特率 */
    SCON2 = 0x02;

//    user_set_interrupt_state(IE_TYPE_UART1, IE_ENABLE);
}
 
// 发送一个字符到串口
void UART_SendChar(char ch) 
{
    // 将字符发送到串口
    SBUF = (uint8_t)ch;
    while(!(SCON & 0X02));
    SCON &=~ 0x02;/* 清除发送中断标志位 */  
}

// 发送一个字符串到串口
void UART_SendString(char* str)
{
    while (*str) {
        UART_SendChar(*str++);
    }
}

// 自定义printf函数，仅支持格式化输出 %d 和 %s
void my_printf(const char* format, ...) 
{
    va_list args;
    va_start(args, format);

    while (*format != '\0') {
        if (*format == '%') {
            format++;
            if (*format == 'd') {
                int num = va_arg(args, int);
                // 将数字转换为字符串并发送到串口
                char numStr[12]; // 最多11位数字，再加上结束符
                sprintf(numStr, "%d", num);
                UART_SendString(numStr);
            } else if (*format == 's') {
                char* str = va_arg(args, char*);
                // 直接发送字符串到串口
                UART_SendString(str);
            }
        } else {
            // 发送普通字符
            UART_SendChar(*format);
        }
        format++;
    }

    va_end(args);
}


