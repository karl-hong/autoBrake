#ifndef _MY_PRINTF_H_
#define _MY_PRINTF_H_
#include "typedef.h"

void UART1_Init(void);
void my_printf(const char* format, ...);
void UART_SendChar(char ch);
#endif
