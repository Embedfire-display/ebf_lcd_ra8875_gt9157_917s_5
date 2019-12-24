/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试5.0寸lcd显示英文字符
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO-mini STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_ra8875_lcd.h"
#include "bsp_usart1.h"


void Delay(__IO u32 nCount); 
void RA8875_Test(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{  
  /*初始化液晶屏*/
  LCD_Init();	
  
  /*必须先初始化FSMC(液晶)，再初始化串口*/
  USART1_Config();   
 
  
  printf("id =%x ",RA8875_ReadID());
  
  
	while( 1 )
  {
    RA8875_Test();
    }
}



void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

/*用于测试各种RA8875的函数*/
void RA8875_Test(void)
{
  RA8875_ClrScr(CL_BLACK);	/* 清屏，显示全黑 */

  RA8875_SetFrontColor(CL_WHITE);
  
  RA8875_SetFont( RA_FONT_24,  0,  0);

  RA8875_DispStr(0,0,"野火5.0寸液晶屏参数:");
  RA8875_DispStr(0,30,"分辨率：800x480 像素");
  RA8875_DispStr(0,60,"电容触摸屏：支持5点触控");
  RA8875_DispStr(0,90,"屏幕自带中文字库，自带绘图引擎，可超高速描绘各种2D图形");
  RA8875_DispStr(0,120,"液晶控制器采用8080接口通讯，16位数据线，可直接采用FSMC驱动");
  RA8875_DispStr(0,150,"触摸屏控制器采用IIC接口通讯");


  /* 画直线 */
  RA8875_SetFrontColor(CL_BLUE);
  
  
  RA8875_SetFont( RA_FONT_32,  0,  0);
  RA8875_DispStr(10,200,"画线：");
  
  RA8875_DrawLine(50,250,750,250,CL_RED);
  
  RA8875_DrawLine(50,300,750,300,CL_RED);
  
  RA8875_DrawLine(400,250,200,400,CL_GREEN);
  
  RA8875_DrawLine(600,250,250,400,CL_GREEN);
  
  Delay(0xFFFFFF);
  
  RA8875_FillRect(0,200,LCD_HEIGHT-200,LCD_WIDTH,CL_BLACK);
  

  
  /*画矩形*/
  RA8875_SetFrontColor(CL_BLUE);
  RA8875_DispStr(10,200,"画矩形：");
  
  RA8875_DrawRect(200,250,200,100,CL_RED);
  RA8875_DrawRect(350,250,200,50,CL_GREEN);
  RA8875_DrawRect(200,350,50,200,CL_BLUE);
  
  Delay(0xFFFFFF);
  
  RA8875_FillRect(0,200,LCD_HEIGHT-200,LCD_WIDTH,CL_BLACK);

  

  /*填充矩形*/
  RA8875_SetFrontColor(CL_BLUE);
  RA8875_DispStr(10,200,"填充矩形:");
 
  RA8875_FillRect(200,250,200,100,CL_RED);
  RA8875_FillRect(350,250,200,50,CL_GREEN);
  RA8875_FillRect(200,350,50,200,CL_BLUE);
  
  Delay(0xFFFFFF);
  
  RA8875_FillRect(0,200,LCD_HEIGHT-200,LCD_WIDTH,CL_BLACK);

  /* 画圆 */
  RA8875_SetFrontColor(CL_BLUE);
  RA8875_DispStr(10,200,"画圆:");
  
  RA8875_DrawCircle(200,350,50,CL_RED);
  RA8875_DrawCircle(350,350,75,CL_GREEN);
  
  Delay(0xFFFFFF);
  
  RA8875_FillRect(0,200,LCD_HEIGHT-200,LCD_WIDTH,CL_BLACK);


  /*填充圆*/
  RA8875_SetFrontColor(CL_BLUE);
  RA8875_DispStr(10,200,"填充圆：");
  
  RA8875_FillCircle(300,350,50,CL_RED);
  RA8875_FillCircle(450,350,75,CL_GREEN);
  
  Delay(0xFFFFFF);
  
  RA8875_FillRect(0,200,LCD_HEIGHT-200,LCD_WIDTH,CL_BLACK);


}

/*********************************************END OF FILE**********************/

