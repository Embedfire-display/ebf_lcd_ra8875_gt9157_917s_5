/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   5.0寸电容液晶触摸画板实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "includes.h"

extern void CreateTouch(void);

void emWinMainApp(void)
{
	/* 显示测试 */
	GUI_DispStringAt("wildfire ISO board emWin test!",50,10);
	GUI_DrawGradientV(100, 50, 750, 470, 0x0000FF, 0x00FFFF);
	CreateTouch();
}
/*************************** End of file ****************************/
