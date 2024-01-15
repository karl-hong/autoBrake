#ifndef _COMMON_H_
#define _COMMON_H_
#include "typedef.h"

typedef struct {
    bool_en mSysOn;
    bool_en mKeyDelay;
		bool_en mBatDelay;
    bool_en mLedDelay;
		bool_en mSc7a20Delay;
}main_context_t;

extern main_context_t gMainContext;

#endif
