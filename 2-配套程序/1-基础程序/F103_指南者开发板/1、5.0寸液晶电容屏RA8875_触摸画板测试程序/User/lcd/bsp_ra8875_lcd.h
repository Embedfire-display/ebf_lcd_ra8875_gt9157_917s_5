#ifndef __BSP_RA8875_LCD_H
#define	__BSP_RA8875_LCD_H

#include "stm32f10x.h"
#include "bsp_usart1.h"

/***************************************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM4 连接 TFT，地址范围为0X6C00 0000 ~ 0X6FFF FFFF
FSMC_A23 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X6C00 0000
RAM基地址 = 0X6D00 0000 = 0X6C00 0000+2^23*2 = 0X6C00 0000 + 0X100 0000 = 0X6D00 0000
当选择不同的地址线时，地址要重新计算  
****************************************************************************************/

//RA8875的RS高电平表示寄存器，低电平表示数据，与ILI9341相反

#define Bank1_LCD_RSL    ((u32)0x60000000)	      //与液晶屏的RS引脚连接的A16为 低电平
#define Bank1_LCD_RSH    ((u32)0x60020000)       //与液晶屏的RS引脚连接的A16为 高电平// A16 PD11

/*选定LCD指定寄存器*/
#define LCD_WR_RSH(val)      ((*(__IO u16 *) (Bank1_LCD_RSH)) = ((u16)val))   //向RSH地址写入
#define LCD_WR_RSL(val)      ((*(__IO u16 *) (Bank1_LCD_RSL)) = ((u16)(val))) //向RSL地址写入
#define LCD_RD_RSH()         (*(__IO u16 *) (Bank1_LCD_RSH))                   //从RSH地址读出
#define LCD_RD_RSL()         (*(__IO u16 *) (Bank1_LCD_RSL))                   //从RSL地址读出 

/* ILI9341 的RS为低电平时表示寄存器，高电平表示数据 */
#define LCD_ILI9341_CMD(index)            LCD_WR_RSL(index)
#define LCD_ILI9341_Parameter(val)	       LCD_WR_RSH(val)
#define LCD_ILI9341_READ_DATA()           LCD_RD_RSH()

/*RA8875 的RS为高电平时表示寄存器，低电平表示数据，与ILI9341相反*/
#define LCD_RA8875_REG(index)            LCD_WR_RSH(index)                    
#define LCD_RA8875_WRITE_DATA(val)      LCD_WR_RSL(val)
#define LCD_RA8875_READ_DATA()           LCD_RD_RSL() 
#define LCD_RA8875_READ_STATUS()          LCD_RD_RSH()
#define LCD_RA8875_RAM_ADDR               Bank1_LCD_RSL


#define LCD_WIDTH     800
#define LCD_HEIGHT    480


  
#define LCD_DEBUG_ON          1
#define LCD_DEBUG_ARRAY_ON   1
#define LCD_DEBUG_FUNC_ON    1
   
   
// Log define
#define LCD_INFO(fmt,arg...)           printf("<<-LCD-INFO->> "fmt"\n",##arg)
#define LCD_ERROR(fmt,arg...)          printf("<<-LCD-ERROR->> "fmt"\n",##arg)
#define LCD_DEBUG(fmt,arg...)          do{\
                                         if(LCD_DEBUG_ON)\
                                         printf("<<-LCD-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)
#define LCD_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(LCD_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-LCD-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)
#define LCD_DEBUG_FUNC()               do{\
                                         if(LCD_DEBUG_FUNC_ON)\
                                         printf("<<-LCD-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)


/* 背景光控制 */
#define BRIGHT_MAX		    255
#define BRIGHT_MIN		    0
#define BRIGHT_DEFAULT	200
#define BRIGHT_STEP		  5

/*
	LCD 颜色代码，CL_是Color的简写
	16Bit由高位至低位， RRRR RGGG GGGB BBBB

	下面的RGB 宏将24位的RGB值转换为16位格式。
	启动windows的画笔程序，点击编辑颜色，选择自定义颜色，可以获得的RGB值。

	推荐使用迷你取色器软件获得你看到的界面颜色。
*/
/*这里把B和R的数据线交换了*/
#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* 将8位R,G,B转化为 16位RGB565格式 */
#define RGB565_R(x)  ((x >> 8) & 0xF8)
#define RGB565_G(x)  ((x >> 3) & 0xFC)
#define RGB565_B(x)  ((x << 3) & 0xF8)

enum
{
	CL_WHITE    = RGB(255,255,255),	/* 白色 */
	CL_BLACK    = RGB(  0,  0,  0),	/* 黑色 */
	CL_RED      = RGB(255,	0,  0),	/* 红色 */
	CL_GREEN    = RGB(  0,255,  0),	/* 绿色 */
	CL_BLUE     = RGB(  0,	0,255),	/* 蓝色 */
	CL_YELLOW   = RGB(255,255,  0),	/* 黄色 */

	CL_GREY    = RGB( 98, 98, 98), 	/* 深灰色 */
	CL_GREY1		= RGB( 150, 150, 150), 	/* 浅灰色 */
	CL_GREY2		= RGB( 180, 180, 180), 	/* 浅灰色 */
	CL_GREY3		= RGB( 200, 200, 200), 	/* 最浅灰色 */
	CL_GREY4		= RGB( 230, 230, 230), 	/* 最浅灰色 */

	CL_BUTTON_GREY	= RGB( 220, 220, 220), /* WINDOWS 按钮表面灰色 */

	CL_MAGENTA      = 0xF81F,	/* 红紫色，洋红色 */
	CL_CYAN         = 0x7FFF,	/* 蓝绿色，青色 */

	CL_BLUE1        = RGB(  0,  0, 240),		/* 深蓝色 */
	CL_BLUE2        = RGB(  0,  0, 128),		/* 深蓝色 */
	CL_BLUE3        = RGB(  68, 68, 255),		/* 浅蓝色1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* 浅蓝色1 */

	/* UI 界面 Windows控件常用色 */
	CL_BTN_FACE		  = RGB(236, 233, 216),	/* 按钮表面颜色(灰) */
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* 分组框主线颜色 */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* 分组框阴影线颜色 */


	CL_MASK			    = 0x9999	/* 颜色掩码，用于文字背景透明 */
};


enum
{
	RA_FONT_16 = 0,		/* RA8875 字体 16点阵 */
	RA_FONT_24 = 1,		/* RA8875 字体 24点阵 */
	RA_FONT_32 = 2		/* RA8875 字体 32点阵 */
};

/* 文字放大参数 */
enum
{
	RA_SIZE_X1 = 0,		/* 原始大小 */
	RA_SIZE_X2 = 1,		/* 放大2倍 */
	RA_SIZE_X3 = 2,		/* 放大3倍 */
	RA_SIZE_X4 = 3		/* 放大4倍 */
};


void LCD_Init(void);

uint16_t RA8875_ReadID(void);

void Display_ON(void);
void Display_OFF(void);
void RA8875_SetBackLight(uint8_t _bright);
void RA8875_SetCursor(uint16_t _usX, uint16_t _usY);
void RA8875_SetReadCursor(uint16_t _usX, uint16_t _usY);
void RA8875_SetTextCursor(uint16_t _usX, uint16_t _usY);
void RA8875_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth);
void RA8875_QuitWinMode(void);
void RA8875_SetWriteDirection(uint8_t horizontal,uint16_t vertical);
void RA8875_Config(void);
void RA8875_DispAscii(uint16_t _usX, uint16_t _usY, char *_ptr);
void RA8875_ClrScr(uint16_t _usColor);
void RA8875_SetBackLight(uint8_t _bright);

void RA8875_SetFrontColor(uint16_t _usColor);
void RA8875_SetBackColor(uint16_t _usColor);
void RA8875_SetActiveWindow(uint16_t _usX1,uint16_t _usY1 ,uint16_t _usX2 ,uint16_t _usY2);
void RA8875_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
void RA8875_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
void RA8875_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void RA8875_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void RA8875_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void RA8875_FillCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void RA8875_SetFont(uint8_t _ucFontType, uint8_t _ucLineSpace, uint8_t _ucCharSpace);
void RA8875_SetTextZoom(uint8_t _ucHSize, uint8_t _ucVSize);
void RA8875_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
void RA8875_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr);



#endif /* __BSP_RA8875_LCD_H */
