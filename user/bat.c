#include "HC89S003AF4.h"
#include "bat.h"
#include "common.h"
#include "gpio.h"
#include "led.h"
#include "my_printf.h"

#define LVDIF_MASK          0x08
#define LVD_VDD_42          0x07
#define LVD_VDD_39          0x06
#define LVD_VDD_36          0x05
#define LVD_VDD_MASK        0x07
#define LVD_ENABLE          0x80

static uint16_t    gBatCnt = 0;
static bat_level_t gCurLevel = BAT_LEVEL_42;
static bat_level_t gVddLevel = BAT_LEVEL_INVALID;
static uint16_t    gAdcValue = 0;
static bool_en     gAdcStart = false;

void lvd_enable(bool_en enable)
{
    if(enable){
        LVDC |= bmLVDEN;
    }else{
        LVDC &= (~bmLVDEN);
    }
}

void lvd_clear_flag(void)
{
    LVDC &= (~bmLVDF);
}

bool lvd_get_flag(void)
{
    return (LVDC & bmLVDF);
}

void lvd_set_level(uint8_t level)
{
    LVDC = LVDC & 0xF8 | level;
}

uint8_t lvd_get_level(void)
{
    return (bmLVDF & bmLVDV);
}

/**
  * @说明  	延时函数
  * @参数  	fui_i : 延时时间
  * @返回值 无
  * @注 	  Fcpu = 16MHz,fui_i = 1时,延时时间约为2us
  */
void Delay_2us(unsigned int fui_i)
{
	while(fui_i--);	
}

void bat_init(void)
{
    BORC = 0xC0;						              //BOR使能，BOR消抖使能，检测电压点1.8V
	//BOR消抖时间计算
	//消抖时间 = BORDBC[7:0] * 8Tcpu + 2Tcpu
	//		   = 255 * 8 * 0.0625us + 2 * 0.0625us
	//		   = 127.625us

	BORDBC = 0xFF;						            //BOR电压检测去抖时间127.625us


    lvd_set_level(LVD_VDD_42);
    lvd_clear_flag();
    lvd_enable(true);
    // LVDC = LVD_ENABLE | LVD_VDD_42;						  //LVD设置
	//	消抖时间 = 	(0xFF + 2) * 1/Fcpu
	//			 =	(0xFF + 2) / 16000000	（当前的CPU时钟）
	//			 =	16.0625us
	LVDDBC = 0xFF;						//设置消抖时间

    // gpio0_init(GPIO_1, GPIO_INPUT_ANALOG_RESERVE);

    // ADCC0 = 0x81;						  //打开ADC转换电源
    // Delay_2us(20);					      //延时20us，确保ADC系统稳定
	// ADCC1 = 0x01;						  //选择外部通道1
	// ADCC2 = 0x4D;						  //转换结果12位数据，数据右对齐，ADC时钟16分频
}

void check_bat_task(void)
{
    static bool stLvdIF;
    static uint8_t stLvdLevel;
    static uint8_t cnt = 0;
    if(!gMainContext.mBatDelay){
        return;
    }
    gMainContext.mBatDelay = false;

//    if(gAdcStart){
//        goto wait_done;
//    }

    gBatCnt ++;
    if(10 > gBatCnt){
        return;
    }
    gBatCnt = 0;

    /* check bat state per 1000ms */
    stLvdIF = lvd_get_flag();

    stLvdLevel = lvd_get_level();
    switch(stLvdLevel){
        case LVD_VDD_42:{
            if(stLvdIF){
                /* vdd lower than 4.2V */
                lvd_clear_flag();
                cnt ++;
                if(cnt >= 5){
                    cnt = 0;
                    gCurLevel = BAT_LEVEL_39;
                    /* check vdd whether lower than 3.9 V*/
                    lvd_set_level(LVD_VDD_39);
                } 
            }else{
                /* vdd higher than 4.2V */
                cnt ++;
                if(cnt >= 10){
                    cnt = 0;
                    gCurLevel = BAT_LEVEL_42;
                }
            }
            break;
        }

        case LVD_VDD_39:{
            if(stLvdIF){
                /* vdd lower than 3.9V */
                lvd_clear_flag();
                cnt ++;
                if(cnt >= 5){
                    cnt = 0;
                    gCurLevel = BAT_LEVEL_36;
                    /* check vdd whether lower than 3.6 V*/
                    lvd_set_level(LVD_VDD_36);
                } 
            }else{
                /* 3.9V < vdd < 4.2V */
                cnt ++;
                if(cnt >= 10){
                    cnt = 0;
                    lvd_set_level(LVD_VDD_42);
                    gCurLevel = BAT_LEVEL_39;
                }
            }
            break;
        }

        case LVD_VDD_36:{
            if(stLvdIF){
                lvd_clear_flag();
                cnt ++;
                if(cnt >= 5){
                    cnt = 0;
                    gCurLevel = BAT_LEVEL_30;
                } 
            }else{
                /* 3.6V < VDD < 3.9V */
                cnt ++;
                if(cnt >= 10){
                    cnt = 0;
                    lvd_set_level(LVD_VDD_39);
                    gCurLevel = BAT_LEVEL_36;
                }
            }
            break;
        }

        default:{
            LVDC = LVD_VDD_42;
            break;
        }
    }

//     ADCC0 |= 0x40;					//启动ADC转换
//     gAdcStart = true;
// wait_done:  
//     while(!(ADCC0&0x20)){
//         //等待ADC转换结束
//         return;
//     }	    
//     ADCC0 &=~ 0x20;					//清除标志位
//     gAdcValue = ADCR;		        //获取ADC的值
//     gAdcStart = false;

//     my_printf("adc value: %d\r\n", gAdcValue);

//     if(gAdcValue >= 3270){
//        gCurLevel = BAT_LEVEL_42;
//     }else if(gAdcValue >= 2950){
//         gCurLevel = BAT_LEVEL_36;
//     }else{
//         gCurLevel = BAT_LEVEL_30;
//     }
}

void bat_led_task(void)
{
    static u8 ledCnt = 0;

    if(!gMainContext.mLedDelay){
        return;
    }

    gMainContext.mLedDelay = false;

    // if(gCurLevel == gVddLevel){
    //     cnt = 0;
    //     return;
    // }

//    ledCnt ++;
//    if(ledCnt < 250){
//        return;
//    }

//    ledCnt = 0;
    // gVddLevel ++;
    // if(gVddLevel >= BAT_LEVEL_INVALID) gVddLevel = 0;
// 
    gVddLevel = gCurLevel;
    my_printf("vdd level: %d\r\n", gVddLevel);
    switch(gVddLevel){
        case BAT_LEVEL_42:{
            user_set_blue_led_state(LED_STATE_ON);
            user_set_red_led_state(LED_STATE_ON);
            break;
        }

        case BAT_LEVEL_39:{
            user_set_blue_led_state(LED_STATE_ON);
            user_set_red_led_state(LED_STATE_OFF);
            break;
        }

        case BAT_LEVEL_36:{
            user_set_blue_led_state(LED_STATE_OFF);
            user_set_red_led_state(LED_STATE_ON);
            break;
        }

        case BAT_LEVEL_30:{
            user_set_blue_led_state(LED_STATE_OFF);
            user_set_red_led_state(LED_STATE_OFF);
            break;
        }

        default:
            break;
    }
}