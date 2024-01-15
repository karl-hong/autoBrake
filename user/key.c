#include "HC89S003AF4.h"
#include "key.h"
#include "gpio.h"
#include "common.h"
#include "led.h"
#include "my_printf.h"

#define KEY_MASK        0x00
#define KEY_VALUE_PWR   0x01
#define KEY_VALUE_GAP   0x02
#define KEY_VALUE_TIM   0x04

static bool_en gKeyPress     = false;
static bool_en gKeyPressed   = false;
static bool_en gKeyLongPress = false;
static uint8_t gLastKeyValue = KEY_MASK;
static uint16_t gKeyCnt = 0;


static void doPowerKeyShortPressEvent(void)
{
    static uint8_t state = 0;
    state = !state;
    if(state){
        user_set_brake_led_level(LED_LEVEL_LOW);
    }else{
        user_set_brake_led_level(LED_LEVEL_OFF);
    }
}

static void doGapKeyShortPressEvent(void)
{

}

static void doTimKeyShortPressEVent(void)
{

}

static void doPowerKeyLongPressEvent(void)
{

}

static void doGapKeyLongPressEvent(void)
{

}

static void doTimKeyLongPressEVent(void)
{

}

static void onLongKeyPressed(uint8_t keyValue)
{
    if(KEY_VALUE_PWR & keyValue){
        doPowerKeyLongPressEvent();
    }else if(KEY_VALUE_GAP & keyValue){
        doGapKeyLongPressEvent();
    }else if(KEY_VALUE_TIM & keyValue){
        doTimKeyLongPressEVent();
    }
}

static void onShortKeyPressed(uint8_t keyValue)
{
    if(KEY_VALUE_PWR & keyValue){
        doPowerKeyShortPressEvent();
    }else if(KEY_VALUE_GAP & keyValue){
        doGapKeyShortPressEvent();
    }else if(KEY_VALUE_TIM & keyValue){
       doTimKeyShortPressEVent(); 
    }
}

void key_init(void)
{
    /* 电源键：P2.5 */
//    gpio2_init(GPIO_5, GPIO_INPUT_PU_WITHOUT_SMT);
	
		P2M2 = P2M2 & 0X0F | 0X60;
    /* Gsensor 阈值设置键: P0.2 */
    gpio0_init(GPIO_2, GPIO_INPUT_PU_WITHOUT_SMT);

    /* Gsensor 滤波时间设置键: P2.3 */
    gpio2_init(GPIO_3, GPIO_INPUT_PU_WITHOUT_SMT);
}

void key_task(void)
{
    uint8_t stKeyValue;
	
	char value = P2_5;

    if(gMainContext.mKeyDelay == false){
        return;
    }
    gMainContext.mKeyDelay = false;

    stKeyValue = P2_5;
		my_printf("p2_5: %d\r\n", P2_5);
	stKeyValue |= (P0_2 ? 0x02 : 0x00);
	stKeyValue |= (P2_3 ? 0x04 : 0x00);

    

    if(KEY_MASK == stKeyValue){
        /* no key pressed */
        gKeyPress = false;
        return;
    }

    if(gKeyPress){
        /* key pressing */
        if(gLastKeyValue == stKeyValue){
            gKeyCnt ++;
            if(gKeyCnt >= 2000 && !gKeyLongPress){
                gKeyLongPress = true;
                onLongKeyPressed(stKeyValue);
            }else if(gKeyCnt >= 30 && !gKeyPressed){
                gKeyPressed = true;
            }
        }else{
            /* release key */
            if(!gKeyLongPress && gKeyPressed){
                onShortKeyPressed(stKeyValue);
            }
            gKeyPress = false;
        }
    }else{
        /* key value changed */
        if(gLastKeyValue != stKeyValue){
            gLastKeyValue = stKeyValue;
            gKeyPress = true;
            gKeyPressed = false;
            gKeyLongPress = false;
            gKeyCnt = 0;
        }
    }
		
}