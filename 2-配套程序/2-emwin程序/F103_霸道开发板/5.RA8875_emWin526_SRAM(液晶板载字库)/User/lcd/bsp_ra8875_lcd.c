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
#include "GUI.h"
#include "bsp_usart1.h"

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
    
    
    p.FSMC_AddressSetupTime = 0x01;	 //地址建立时间
    p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FSMC_DataSetupTime = 0x03;		 //数据建立时间
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

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_ReadReg
*	功能说明: 读取LCD控制器的寄存器的值。
*	形    参:  
*			SPFD5420_Reg ：寄存器地址;
*			SPFD5420_RegValue : 寄存器值
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t RA8875_ReadReg(uint16_t RegAddr)
{
	/* Write 16-bit Index (then Read Reg) */
	LCD_RA8875_REG (RegAddr);
	/* Read 16-bit Reg */
	return (LCD_RA8875_READ_DATA());
}

void RA8875_WriteReg(uint8_t RegAddr, uint8_t RegValue)
{
  LCD_RA8875_REG (RegAddr);
  LCD_RA8875_WRITE_DATA(RegValue);
}

uint8_t RA8875_ReadStatus(void)
{
	return  LCD_RA8875_READ_STATUS();
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_SetDispWin
*	功能说明: 设置显示窗口，进入窗口显示模式。在窗口显示模式，连续写显存，光标会自动在设定窗口内进行递增
*	形    参:
*		_usX : 水平坐标
*		_usY : 垂直坐标
*		_usHeight: 窗口高度
*		_usWidth : 窗口宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{

	uint16_t usTemp;

	/* 坐标系统示意图： （横屏）
			 -----------------------------
			|(0,0)                        |
			|     --------->              |
			|         |                   |
			|         |                   |
			|         |                   |
			|         V                   |
			|     --------->              |
			|                    (800,480)|
			 -----------------------------

		左上角是坐标零点, 扫描方向，先从左到右，再从上到下。

		如果需要做竖屏方式，你需要进行物理坐标和逻辑坐标的转换
	*/

	RA8875_WriteReg(0x30, _usX);
    RA8875_WriteReg(0x31, _usX >> 8);

	RA8875_WriteReg(0x32, _usY);
    RA8875_WriteReg(0x33, _usY >> 8);

	usTemp = _usWidth + _usX - 1;
	RA8875_WriteReg(0x34, usTemp);
    RA8875_WriteReg(0x35, usTemp >> 8);

	usTemp = _usHeight + _usY - 1;
	RA8875_WriteReg(0x36, usTemp);
    RA8875_WriteReg(0x37, usTemp >> 8);

	RA8875_SetCursor(_usX, _usY);
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
	RA8875_SetDispWin(0, 0, 480, 800);
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

/*************** emWin ***********************/

u16 GUIColorToRGB(GUI_COLOR color)
{
	u32 Color_R;
	u32 Color_G;
	u32 Color_B;
	u32 tempcolor;
	tempcolor=color;
	Color_B=(tempcolor&0x00f80000)>>19;//取高5BIT
	Color_G=(tempcolor&0x0000fc00)>>5;
	Color_R=(tempcolor&0x000000f8)<<8;
	
	return((u16)((Color_R)|(Color_G)|Color_B));
}
/*
*********************************************************************************************************
*	函 数 名: RA8875_SetFrontColor
*	功能说明: 设定前景色
*	形    参: 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetFrontColor(uint16_t _usColor)
{
	//s_ucRA8875Busy = 1;
	RA8875_WriteReg(0x63, (_usColor & 0xF800) >> 11);	/* R5  */
	RA8875_WriteReg(0x64, (_usColor & 0x07E0) >> 5);	/* G6 */
	RA8875_WriteReg(0x65, (_usColor & 0x001F));			/* B5 */
	//s_ucRA8875Busy = 0;
}
/*
*********************************************************************************************************
*	函 数 名: RA8875_SetBackColor
*	功能说明: 设定背景色
*	形    参: 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetBackColor(uint16_t _usColor)
{
	//s_ucRA8875Busy = 1;
	RA8875_WriteReg(0x60, (_usColor & 0xF800) >> 11);	/* R5  */
	RA8875_WriteReg(0x61, (_usColor & 0x07E0) >> 5);	/* G6 */
	RA8875_WriteReg(0x62, (_usColor & 0x001F));			/* B5 */
	//s_ucRA8875Busy = 0;
}


/*
*********************************************************************************************************
*	函 数 名: RA8875_SetFont
*	功能说明: 文本模式，设置文字字体、行距和字距
*	形    参：
*			_ucFontType : 字体类型: RA_FONT_16, RA_FONT_24, RA_FONT_32
*			_ucLineSpace : 行距，像素单位 (0-31)
*			_ucCharSpace : 字距，像素单位 (0-63)
*
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetFont(uint8_t _ucFontType, uint8_t _ucLineSpace, uint8_t _ucCharSpace)
{
	/*
		[29H]在文字模式下，用来设定文字间的行距 (单位: 像素) 。
		只有低5个bit有效，0-31
	*/
	//s_ucRA8875Busy = 1;
	if (_ucLineSpace >31)
	{
		_ucLineSpace = 31;
	}
	RA8875_WriteReg(0x29, _ucLineSpace);

	/*
		[2EH] 设置字符间距（像素单位，0-63），和字体（16*16，24*24，32*32）
	*/
	if (_ucCharSpace > 63)
	{
		_ucCharSpace = 63;
	}
	if (_ucFontType > RA_FONT_32)
	{
		_ucFontType = RA_FONT_16;
	}
	RA8875_WriteReg(0x2E, (_ucCharSpace & 0x3F) | (_ucFontType << 6));
	//s_ucRA8875Busy = 0;
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_DispStr
*	功能说明: 显示字符串，字符点阵来自于RA8875外接的字库芯片，支持汉字.
*			文字颜色、背景颜色、是否通透由其他的函数进行设置
*			void RA8875_SetFrontColor(uint16_t _usColor);
*			void RA8875_SetBackColor(uint16_t _usColor);
*	形    参：_usX, _usY : 显示位置（坐标）
*			 _ptr : AascII字符串，已0结束
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_DispChar(U16P c)
{
//	u8 i=0;
	unsigned char ch[2];
	ch[0]=(c>>8)&0x00ff;
	ch[1]=c&0xff;
	/*
		RA8875 透过使用 — 上海集通公司 (Genitop Inc) 外部串行式字体内存 (Font ROM)，可支持各样
		的文字写入到DDRAM 中。RA8875 与上海集通公司 (Genitop Inc) 兼容的产品包含 :
		GT21L16TW/GT21H16T1W 、GT23L16U2W 、GT23L24T3Y/GT23H24T3Y 、GT23L24M1Z 、
		及 GT23L32S4W/GT23H32S4W。这些字体包含16x16, 24x24, 32x32 点 (Dot) 与不同的字

		野火RA8875驱动板集成的字库芯片为 集通字库芯片_GT23l32S4W

		GT23L32S4W是一款内含11X12点阵、15X16点、24X24点阵、32X32点阵的汉字库芯片，支持GB2312
		国标汉字（含有国家信标委合法授权）及SCII字符。排列格式为横置横排。用户通过字符内码，利用本手
		册提供的方法计算出该字符点阵在芯片中的地址，可从该地址连续读出字符点阵信息。
	*/

	/* 设置文本显示位置，注意文本模式的写入光标和图形模式的写入光标是不同的寄存器 */
//	RA8875_SetTextCursor(100, 100);

	RA8875_WriteReg(0x40, (1 << 7));	/* 设置为文本模式 */

	/*
		Serial Flash/ROM 频率频率设定
			0xb: SFCL 频率 = 系统频率频率(当DMA 为致能状态，并且色彩深度为256 色，则SFCL 频率
				固定为=系统频率频率/ 2)
			10b: SFCL 频率 =系统频率频率/ 2
			11b: SFCL 频率 =系统频率频率/ 4

		安富莱驱动板系统频率为 68MHz

		GT23L32S4W的访问速度：SPI 时钟频率：20MHz(max.)

		因此需要设置 4 分频, 17MHz
	*/
	RA8875_WriteReg(0x06, (3 << 0));	/* 设置为文本模式 */

	/* 选择外部字体ROM */
	RA8875_WriteReg(0x21, (0 << 7) | (1 << 5));

	/* 05H  REG[05h] Serial Flash/ROM Configuration Register (SROC)
		7	Serial Flash/ROM I/F # 选择
				0:选择Serial Flash/ROM 0 接口。
				1:选择Serial Flash/ROM 1 接口。
		6	Serial Flash/ROM 寻址模式
				0: 24 位寻址模式。
				此位必须设为0。
		5	Serial Flash/ROM 波形模式
				0: 波形模式 0。
				1: 波形模式 3。
		4-3	Serial Flash /ROM 读取周期 (Read Cycle)
			00b: 4 bus ?? 无空周期 (No Dummy Cycle)。
			01b: 5 bus ??1 byte 空周期。
			1Xb: 6 bus ??2 byte 空周期。
		2	Serial Flash /ROM 存取模式 (Access Mode)
			0: 字型模式 。
			1: DMA 模式。
		1-0	Serial Flash /ROM I/F Data Latch 选择模式
			0Xb: 单一模式。
			10b: 双倍模式0。
			11b: 双倍模式1。
	*/
	RA8875_WriteReg(0x05, (0 << 7) | (0 << 6) | (1 << 5) | (1 << 3) | (0 << 2) | (0 << 1));

	/*
		设置外部字体芯片型号为 GT23L32S4W, 编码为GB2312,

		Bit1:0 决定ASCII字符的格式：
			0 = NORMAL		 [笔画细, 和汉字顶部对齐]
			1 = Arial		 [笔画粗，和汉字底部对齐]
			2 = Roman		 [笔画细, 和汉字底部对齐]
			3 = Bold		 [乱码,不可用]
	 */
	RA8875_WriteReg(0x2F, (4 << 5) | (0 << 2) | (0 << 0));

	
	LCD_RA8875_REG(0x02); 			/* 用于设定RA8875 进入内存(DDRAM或CGRAM)读取/写入模式 */
	//printf("rx1=%x,rx2=%x\n",RA8875_ReadReg(0x2a),RA8875_ReadReg(0x2b));
	if(ch[0])
	{
		LCD_RA8875_WRITE_DATA(ch[0]);
		while ((RA8875_ReadStatus() & 0x80) == 0x80);
	}
	LCD_RA8875_WRITE_DATA(ch[1]);
	while ((RA8875_ReadStatus() & 0x80) == 0x80);
//	/* 开始循环处理字符 */
//	for(i=0;i<2;i++)
//	{
//		//RA8875_WriteData("我");
//		//ch[0]="我";
//		LCD_RA8875_WRITE_DATA(ch[i]);
//		while ((RA8875_ReadStatus() & 0x80) == 0x80);

//		//i++;
//	}
	//s_ucRA8875Busy = 0;

	RA8875_WriteReg(0x40, (0 << 7));	/* 还原为图形模式 */
	while ((RA8875_ReadStatus() & 0x80) == 0x80);
	
	while((RA8875_ReadReg(0xf0)&0x01)==0x01) ;
}

void RA8875_SetTextMode(uint8_t Mode)
{
	/*
		pdf 第22页		[22H]
		bit7 用于对齐，一般不用，缺省设0
		bit6 用于通透，一般不用，缺省设0
		bit4 用于旋转90读，一般不用，缺省设0
		bit3-2 水平放大倍数
		bit1-0 垂直放大倍数
	*/
	RA8875_WriteReg(0x22, (Mode<<6));
}
/*
*********************************************************************************************************
*	函 数 名: RA8875_SetTextCursor
*	功能说明: 设置文本写入光标位置
*	形    参：_usX : X坐标; _usY: Y坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetTextCursor(uint16_t _usX, uint16_t _usY)
{
	/* 设置内存读光标的坐标 */
	RA8875_WriteReg(0x2A, _usX);
	RA8875_WriteReg(0x2B, _usX >> 8);
	RA8875_WriteReg(0x2C, _usY);
	RA8875_WriteReg(0x2D, _usY >> 8);
}

/*
*********************************************************************************************************
*	函 数 名: RA8875_SetCursor
*	功能说明: 设置写显存的光标位置（图形模式）
*	形    参:  _usX : X坐标; _usY: Y坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void RA8875_SetCursor(uint16_t _usX, uint16_t _usY)
{
	/* 设置内存写光标的坐标 【注意0x80-83 是光标图形的坐标】 */
	RA8875_WriteReg(0x46, _usX);
	RA8875_WriteReg(0x47, _usX >> 8);
	RA8875_WriteReg(0x48, _usY);
	RA8875_WriteReg(0x49, _usY >> 8);
}
/*
********************************************************************
*  特别说明：第一次使用，根据自己的屏，修正一下触摸部分，下面有两个函数
*  一个是用于修正触摸的，一个是用于官方DEMO演示的。
********************************************************************
*/
/*
*********************************************************************************************************
*	函 数 名: BTE_Start
*	功能说明: 启动BTE操作
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BTE_Start(void)
{
	//s_ucRA8875Busy = 1;
	/* RA8875_WriteReg(0x50, 0x80);  不能使用这个函数，因为内部已经操作了 s_ucRA8875Busy 标志 */
	LCD_RA8875_REG(0x50);	/* 设置寄存器地址 */
	LCD_RA8875_WRITE_DATA(0x80);	/* 写入寄存器值 */
}
/*
*********************************************************************************************************
*	函 数 名: BTE_Wait
*	功能说明: 等待BTE操作结束
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BTE_Wait(void)
{
	while ((RA8875_ReadStatus() & 0x40) == 0x40);
	//s_ucRA8875Busy = 0;
}

void BTE_SetSourse(U16 x0,U16 y0,U16 Layer)
{
	RA8875_WriteReg(0x54,x0);
	RA8875_WriteReg(0x55,x0>>8);
	RA8875_WriteReg(0x56,y0);
	if(Layer==1)
			RA8875_WriteReg(0x57,(y0>>8)|0x80);
	else
			RA8875_WriteReg(0x57,(y0>>8)&0x7f);
	
}
void BTE_SetDist(U16 x0,U16 y0,U16 Layer)
{
	RA8875_WriteReg(0x58,x0);
	RA8875_WriteReg(0x59,x0>>8);
	RA8875_WriteReg(0x5A,y0);
	if(Layer==1)
			RA8875_WriteReg(0x5B,(y0>>8)|0x80);
	else
			RA8875_WriteReg(0x5B,(y0>>8)&0x7f);
	
}

void BTE_SetArea(U16 x0,U16 y0)
{
	RA8875_WriteReg(0x5C,x0);
	RA8875_WriteReg(0x5D,x0>>8);
	RA8875_WriteReg(0x5E,y0);
	RA8875_WriteReg(0x5F,y0>>8);
}




//static U8 FontDataBuf[128];
void BTE_DRAW(U16 x,U16 y,U16 ch,U8 u_Font)
{
	RA8875_SetFrontColor(GUIColorToRGB(GUI_GetColor()));
	RA8875_SetBackColor(GUIColorToRGB(GUI_GetBkColor()));

	RA8875_SetFont(u_Font,1,1);
	RA8875_DispChar(ch);	
	BTE_SetSourse(x,y,0);
	BTE_SetDist(x,y,0);
	BTE_SetArea(32,32);
	//RA8875_SetFrontColor(GUIColorToRGB(GUI_CYAN)); //取的颜色  
	RA8875_WriteReg(0x51,0x82);
	BTE_Start();
	BTE_Wait();	
}

/*****************************************END OF FILE***************************/

