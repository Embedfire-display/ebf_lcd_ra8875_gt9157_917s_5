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
 
#include "stm32f10x.h"
#include "bsp_ra8875_lcd.h"
#include "bsp_usart1.h"
#include "GUI.h"
#include "DIALOG.h"

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	  
	uint16_t i;	
	
	/*初始化液晶屏*/
	LCD_Init();
	
	USART1_Config();
	printf("wildfire ISO board emWin test!\n");
	/*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);	
	//初始化emWin 
	GUI_Init();
	GUI_SetBkColor(0);
	GUI_Clear();
	GUI_SetFont(GUI_FONT_24_1);
	 
	/* 显示测试 */
	GUI_DrawGradientV(0, 0, 800-1, 480-1, 0x0000FF, 0x00FFFF);
	GUI_DispStringAt("wildfire ISO board emWin test!",30,10);
	
	
	while (1)
	{
		GUI_DispDecAt(i++,80,30,4);
		if(i>9999)
			i=0; 
	}

  

}

/* ------------------------------------------end of file---------------------------------------- */

