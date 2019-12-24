/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   5.0寸液晶显示测试实验
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
#include "bsp_led.h"
#include "palette.h"

#include "ff.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_bmp.h"


FIL fnew;													/* file objects */
FATFS fs[2];													/* Work area (file system object) for logical drives */
FRESULT res_sd; 
UINT br, bw;            					/* File R/W count */


void Delay(__IO u32 nCount); 
void RA8875_Test(void);
void RA8875_Palette(void);
void RA8875_Show_BMP(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	  
  
  LED_GPIO_Config();
  
   
  /*初始化液晶屏*/
  LCD_Init();	
  
  /*必须先初始化FSMC(液晶)，再初始化串口*/
  USART1_Config(); 
  
  /* Sdio Interrupt Config */
  NVIC_Configuration();
  
  /*液晶图形函数显示测试*/
  
  LCD_INFO("LCD chip ID = 0x%x", RA8875_ReadID());  

  RA8875_ClrScr(CL_BLACK);	/* 清屏，显示全黑 */


  RA8875_SetFrontColor(CL_WHITE);
  
  RA8875_SetFont( RA_FONT_32,  10,  0);

  RA8875_DispStr(200,240,"野火5.0寸液晶屏测试程序...");
  
  Delay(0xFFFFFF);

    
  RA8875_Test();
  
  RA8875_Show_BMP();
  
  RA8875_Palette();
  

  while(1);
  

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
  
  RA8875_SetFont( RA_FONT_32,  10,  0);

  RA8875_DispStr(200,240,"液晶显示测试...");
  
  Delay(0xFFFFFF);
  
  
  RA8875_ClrScr(CL_BLACK);	/* 清屏，显示全黑 */

  RA8875_SetFrontColor(CL_WHITE);
  
  RA8875_SetFont( RA_FONT_24,  10,  0);

  RA8875_DispStr(0,0,"野火5.0寸液晶屏参数:");
  RA8875_DispStr(0,30,"分辨率：800x480 像素");
  RA8875_DispStr(0,60,"电容触摸屏：支持5点触控");
  RA8875_DispStr(0,90,"屏幕自带中文字库，自带绘图引擎，可超高速描绘各种2D图形");
  RA8875_DispStr(0,120,"液晶控制器采用8080接口通讯，16位数据线，可直接采用FSMC驱动");
  RA8875_DispStr(0,150,"触摸屏控制器采用IIC接口通讯");


  /* 画直线 */
  RA8875_SetFrontColor(CL_BLUE);
  
  
  RA8875_SetFont( RA_FONT_32,  10,  0);
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

/* 显示bmp图片测试 */
void RA8875_Show_BMP(void)
{
  
  RA8875_ClrScr(CL_BLACK);	/* 清屏，显示全黑 */

  RA8875_SetFrontColor(CL_WHITE);
  
  RA8875_SetFont( RA_FONT_32,  10,  0);

  RA8875_DispStr(200,240,"显示BMP图片测试...");
  Delay(0xFFFFFF);    

  res_sd = f_mount(&fs[1],"1:",1); 
   
  	if(res_sd !=FR_OK)
		{
      RA8875_ClrScr(CL_BLACK);	/* 清屏，显示全黑 */

      RA8875_SetFont( RA_FONT_24,  10,  0);

      RA8875_SetFrontColor(CL_RED);
      RA8875_DispStr(0,100,"SD卡文件系统挂载失败，请给开发板接入SD卡再进行测试..");
      
      RA8875_SetFrontColor(CL_WHITE);
      RA8875_DispStr(0,200,"测试说明：");
      RA8875_DispStr(0,250,"1.本测试需要给开发板接入保存有特定名称BMP图片的SD卡");
      RA8875_DispStr(0,290,"2.在电脑上把本例程工程目录下的“SD卡备份文件”文件夹中的内容复制到SD卡，然后把SD卡插入到开发板即可");
      
      RA8875_SetFrontColor(CL_BLUE);
      RA8875_DispStr(0,360,"3.本测试跟SD卡及其内容有关，不通过并不表示液晶屏有问题，只要触摸画板程序正常运行则表示液晶屏正常。");
      
      RA8875_DispStr(0,430,"即将进入触摸画板测试程序...");
      Delay(0x1FFFFFF);    
		
    }
    else
    {
      
        /* 整屏清为白色 */
     RA8875_ClrScr(CL_WHITE); 
     Delay(0xFFFFF);      

      Lcd_show_bmp(0,0,"1:test_picture/tree.bmp"); 
      Delay(0xFFFFFF);     

      Lcd_show_bmp(0,0,"1:test_picture/moutain.bmp"); 
      Delay(0xFFFFFF);
      
      Lcd_show_bmp(0,0,"1:test_picture/car.bmp"); 
      Delay(0xFFFFFF);    

      Lcd_show_bmp(0,0,"1:test_picture/lake.bmp"); 
      Delay(0xFFFFFF);     
   
      Lcd_show_bmp(0,0,"1:test_picture/couple.bmp"); 
      Delay(0xFFFFFF);     
      
      Lcd_show_bmp(0,0,"1:test_picture/sea.bmp"); 
      Delay(0xFFFFFF);  

      Lcd_show_bmp(0,0,"1:test_picture/army.bmp"); 
      Delay(0xFFFFFF);   

    }
}


/*触摸画板测试*/
void RA8875_Palette(void)
{
      
  RA8875_ClrScr(CL_BLACK);	/* 清屏，显示全黑 */

  RA8875_SetFrontColor(CL_WHITE);
  
  RA8875_SetFont( RA_FONT_32,  10,  0);

  RA8875_DispStr(200,240,"触摸屏测试...");
  
  Delay(0xFFFFFF);
  
    /* 初始化触摸屏 */
  GTP_Init_Panel();       

  /*初始化画板应用*/
  Palette_Init();

}

/* ------------------------------------------end of file---------------------------------------- */

