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
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "gt9xx.h"
#include "bsp_i2c_gpio.h"
#include "palette.h"
#include "bsp_led.h"


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	  
  
  LED_GPIO_Config();
  
  LED2_ON;
   
  /*初始化液晶屏*/
  LCD_Init();	
  
  /*必须先初始化FSMC(液晶)，再初始化串口*/
  USART1_Config(); 

  LCD_INFO("LCD chip ID = 0x%x", RA8875_ReadID());  
  

  /* 初始化触摸屏 */
  GTP_Init_Panel();       

  /*初始化画板应用*/
  Palette_Init();

  while(1);
  

}

/* ------------------------------------------end of file---------------------------------------- */

