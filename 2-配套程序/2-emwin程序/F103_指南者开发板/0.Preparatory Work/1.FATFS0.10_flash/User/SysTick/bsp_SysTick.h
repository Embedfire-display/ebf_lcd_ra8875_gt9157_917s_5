#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void SysTick_Init(void);
u32 readtime(void);


#endif /* __SYSTICK_H */
