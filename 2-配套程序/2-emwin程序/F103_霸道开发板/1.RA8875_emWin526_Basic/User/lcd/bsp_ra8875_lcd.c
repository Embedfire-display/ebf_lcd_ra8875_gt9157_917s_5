/**
  ******************************************************************************
  * @file    bsp_ra8875_lcd.c
  * @author  参考armfly液晶驱动
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ra8875电容屏驱动函数
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "bsp_ra8875_lcd.h"



void LCD_Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

 /**
  * @brief  初始化控制LcD的IO
  * @param  无
  * @retval 无
  */
void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能FSMC时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* 使能FSMC对应相应管脚时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE 
	                          | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG 
	                          | RCC_APB2Periph_GPIOF , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* 配置LCD复位控制管脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    

    
    /* 配置FSMC相对应的数据线,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10*/	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
		/* 配置FSMC相对应的控制线
		 * PD4-FSMC_NOE   :LCD-RD
		 * PD5-FSMC_NWE   :LCD-WR
		 * PG12-FSMC_NE4  :LCD-CS
		 * PE2-FSMC_A23   :LCD-DC
		*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
    GPIO_Init(GPIOG, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_Init(GPIOE, &GPIO_InitStructure);  
    

    /* 配置LCD忙碌查询管脚*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

}

 /**
  * @brief  LCD  FSMC 模式配置
  * @param  无
  * @retval 无
  */
void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //地址建立时间
    p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FSMC_DataSetupTime = 0x05;		 //数据建立时间
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;
    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // 一般使用模式B来控制LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
//	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	/* 注意旧库无这个成员 */
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;  
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* 使能 FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}


void RA8875_WriteReg(uint8_t RegAddr, uint8_t RegValue)
{
  LCD_RA8875_REG (RegAddr);
  LCD_RA8875_WRITE_DATA(RegValue);
}

void RA8875_Config(void)
{

		/* 初始化PLL.  晶振频率为25M */
		LCD_RA8875_REG(0x88);
		LCD_Delay(10);		/* 短暂延迟 */
		LCD_RA8875_WRITE_DATA(12);	/* PLLDIVM [7] = 0 ;  PLLDIVN [4:0] = 10 */

	  LCD_Delay(10000);
  

		LCD_RA8875_REG(0x89);
		LCD_Delay(10);		/* 延迟1us */
		LCD_RA8875_WRITE_DATA(2);	/* PLLDIVK[2:0] = 2, 除以4 */

  

		/* RA8875 的内部系统频率 (SYS_CLK) 是结合振荡电路及PLL 电路所产生，频率计算公式如下 :
		  SYS_CLK = FIN * ( PLLDIVN [4:0] +1 ) / ((PLLDIVM+1 ) * ( 2^PLLDIVK [2:0] ))
		          = 25M * (12 + 1) / ((0 + 1) * (2 ^ 2))
				  = 81.25MHz
		*/

		/* REG[88h]或REG[89h]被设定后，为保证PLL 输出稳定，须等待一段「锁频时间」(< 100us)。*/
	    LCD_Delay(10000);
      
      /*
			配置系统控制寄存器。 中文pdf 第18页:

			bit3:2 色彩深度设定 (Color Depth Setting)
				00b : 8-bpp 的通用TFT 接口， i.e. 256 色。
				1xb : 16-bpp 的通用TFT 接口， i.e. 65K 色。	 【选这个】

			bit1:0 MCUIF 选择
				00b : 8-位MCU 接口。
				1xb : 16-位MCU 接口。 【选这个】
		*/
    
    RA8875_WriteReg(0x10, (1 <<3 ) | (1 << 1));	/* 配置16位MCU并口，65K色 */
  
  

		/* REG[04h] Pixel Clock Setting Register (PCSR)
			bit7  PCLK Inversion
				0 : PDAT 是在PCLK 正缘上升 (Rising Edge) 时被取样。
				1 : PDAT 是在PCLK 负缘下降 (Falling Edge) 时被取样。
			bit1:0 PCLK 频率周期设定
				Pixel Clock ,PCLK 频率周期设定。
				00b: PCLK 频率周期= 系统频率周期。
				01b: PCLK 频率周期= 2 倍的系统频率周期。
				10b: PCLK 频率周期= 4 倍的系统频率周期。
				11b: PCLK 频率周期= 8 倍的系统频率周期。
		*/
	    RA8875_WriteReg(0x04, 0x81);
	    LCD_Delay(10000);
      
      
     
		/* OTD9960 & OTA7001 设置 */
		RA8875_WriteReg(0x14, 0x63);
		RA8875_WriteReg(0x15, 0x00);
		RA8875_WriteReg(0x16, 0x03);
		RA8875_WriteReg(0x17, 0x03);
		RA8875_WriteReg(0x18, 0x0B);
		RA8875_WriteReg(0x19, 0xDF);
		RA8875_WriteReg(0x1A, 0x01);
		RA8875_WriteReg(0x1B, 0x1F);
		RA8875_WriteReg(0x1C, 0x00);
		RA8875_WriteReg(0x1D, 0x16);
		RA8875_WriteReg(0x1E, 0x00);
		RA8875_WriteReg(0x1F, 0x01);
    
                       

    
    /* 设置TFT面板的 DISP  引脚为高，使能面板. 野火TFT模块的DISP引脚连接到RA8875芯片的GP0X脚 */
    RA8875_WriteReg(0xC7, 0x01);	/* DISP = 1 */

    /* LCD 显示/关闭讯号 (LCD Display on) */
    RA8875_WriteReg(0x01, 0x80);

    /* 	REG[40h] Memory Write Control Register 0 (MWCR0)

      Bit 7	显示模式设定
        0 : 绘图模式。
        1 : 文字模式。

      Bit 6	文字写入光标/内存写入光标设定
        0 : 设定文字/内存写入光标为不显示。
        1 : 设定文字/内存写入光标为显示。

      Bit 5	文字写入光标/内存写入光标闪烁设定
        0 : 游标不闪烁。
        1 : 游标闪烁。

      Bit 4   NA

      Bit 3-2  绘图模式时的内存写入方向
        00b : 左 -> 右，然后上 -> 下。
        01b : 右 -> 左，然后上 -> 下。
        10b : 上 -> 下，然后左 -> 右。
        11b : 下 -> 上，然后左 -> 右。

      Bit 1 	内存写入光标自动增加功能设定
        0 : 当内存写入时光标位置自动加一。
        1 : 当内存写入时光标位置不会自动加一。

      Bit 0 内存读取光标自动增加功能设定
        0 : 当内存读取时光标位置自动加一。
        1 : 当内存读取时光标位置不会自动加一。
    */
    RA8875_WriteReg(0x40, 0x00);	/* 选择绘图模式 */
	//RA8875_WriteReg(0x20, 0x04);	

    /* 	REG[41h] Memory Write Control Register1 (MWCR1)
      写入目的位置，选择图层1
    */
    RA8875_WriteReg(0x41, 0x01);	/* 选择绘图模式, 目的为CGRAM */
}



/**
 * @brief  LCD 软件复位
 * @param  无
 * @retval 无
 */
void LCD_Rst(void)
{			
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);	 //低电平复位
    LCD_Delay(0xAFFff); 					   
    GPIO_SetBits(GPIOB, GPIO_Pin_1);		 	 
    LCD_Delay(0xAFFff); 	
}


/**
 * @brief  lcd初始化，如果要用到lcd，一定要调用这个函数
 * @param  无
 * @retval 无
 */
void LCD_Init(void)
{
	LCD_GPIO_Config();
	LCD_FSMC_Config();
	
	LCD_Rst();
  
    RA8875_Config();  
}

/*****************************************END OF FILE***************************/

