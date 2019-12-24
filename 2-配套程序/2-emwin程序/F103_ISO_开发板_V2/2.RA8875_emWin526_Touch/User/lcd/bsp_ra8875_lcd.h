#ifndef __BSP_RA8875_LCD_H
#define	__BSP_RA8875_LCD_H

#include "stm32f10x.h"

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

#define Bank1_LCD_RSL    ((u32)0x6C000000)	      //与液晶屏的RS引脚连接的A23为 低电平
#define Bank1_LCD_RSH    ((u32)0x6D000000)       //与液晶屏的RS引脚连接的A23为 高电平// A23 PE2


/*选定LCD指定寄存器*/
#define LCD_WR_RSH(val)      ((*(__IO u16 *) (Bank1_LCD_RSH)) = ((u16)val))   //向RSH地址写入
#define LCD_WR_RSL(val)      ((*(__IO u16 *) (Bank1_LCD_RSL)) = ((u16)(val))) //向RSL地址写入
#define LCD_RD_RSH()         (*(__IO u16 *) (Bank1_LCD_RSH))                   //从RSH地址读出
#define LCD_RD_RSL()         (*(__IO u16 *) (Bank1_LCD_RSL))                   //从RSL地址读出 

/*RA8875 的RS为高电平时表示寄存器，低电平表示数据，与ILI9341相反*/
#define LCD_RA8875_REG(index)            LCD_WR_RSH(index)                    
#define LCD_RA8875_WRITE_DATA(val)       LCD_WR_RSL(val)
#define LCD_RA8875_READ_DATA()           LCD_RD_RSL() 
#define LCD_RA8875_READ_REG()            LCD_RD_RSH() 

void LCD_Init(void);


#endif /* __BSP_RA8875_LCD_H */
