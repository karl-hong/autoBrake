//#define	ALLOCATE_EXTERN
#include "HC89S003AF4.h"
#include "wdt.h"


void wdt_config(wdt_rst_t enable, wdt_mode_t mode, wdt_period_t peroid)
{
    WDTC = enable | mode | peroid;
}

void wdt_enable(bool enable)
{
    if(enable){
        WDTCCR = 0xff;
    }else{
        WDTCCR = 0x00;
    }   
}