#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

//#include <inttypes.h>

#include "bsp_ra8875_lcd.h"

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */



/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define GTP_I2C_PORT    	    GPIOF			/* GPIO端口 */
#define GTP_I2C_GPIO_CLK 	    RCC_APB2Periph_GPIOF		/* GPIO端口时钟 */
#define GTP_I2C_SCL_PIN		    GPIO_Pin_9			/* 连接到SCL时钟线的GPIO */
#define GTP_I2C_SDA_PIN		    GPIO_Pin_11			/* 连接到SDA数据线的GPIO */

/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#if 0	/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define I2C_SCL_1()  GPIO_SetBits(GTP_I2C_PORT, GTP_I2C_SCL_PIN)		/* SCL = 1 */
	#define I2C_SCL_0()  GPIO_ResetBits(GTP_I2C_PORT, GTP_I2C_SCL_PIN)		/* SCL = 0 */
	
	#define I2C_SDA_1()  GPIO_SetBits(GTP_I2C_PORT, GTP_I2C_SDA_PIN)		/* SDA = 1 */
	#define I2C_SDA_0()  GPIO_ResetBits(GTP_I2C_PORT, GTP_I2C_SDA_PIN)		/* SDA = 0 */
	
	#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GTP_I2C_PORT, GTP_I2C_SDA_PIN)	/* 读SDA口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
    /*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
	#define I2C_SCL_1()  GTP_I2C_PORT->BSRR = GTP_I2C_SCL_PIN				/* SCL = 1 */
	#define I2C_SCL_0()  GTP_I2C_PORT->BRR = GTP_I2C_SCL_PIN				/* SCL = 0 */
	
	#define I2C_SDA_1()  GTP_I2C_PORT->BSRR = GTP_I2C_SDA_PIN				/* SDA = 1 */
	#define I2C_SDA_0()  GTP_I2C_PORT->BRR = GTP_I2C_SDA_PIN				/* SDA = 0 */
	
	#define I2C_SDA_READ()  ((GTP_I2C_PORT->IDR & GTP_I2C_SDA_PIN) != 0)	/* 读SDA口线状态 */
#endif


#define GTP_ADDRESS            0xBA

#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


/* 硬件IIC*/
//#define GTP_I2C                          I2C1
//#define GTP_I2C_CLK                      RCC_APB1Periph_I2C1

//#define GTP_I2C_SCL_PIN                  GPIO_Pin_6                  
//#define GTP_I2C_SCL_GPIO_PORT            GPIOB                       
//#define GTP_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
//#define GTP_I2C_SCL_SOURCE               GPIO_PinSource6
//#define GTP_I2C_SCL_AF                   GPIO_AF_I2C1

//#define GTP_I2C_SDA_PIN                  GPIO_Pin_7                  
//#define GTP_I2C_SDA_GPIO_PORT            GPIOB                     
//#define GTP_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
//#define GTP_I2C_SDA_SOURCE               GPIO_PinSource7
//#define GTP_I2C_SDA_AF                   GPIO_AF_I2C1



#define GTP_RST_GPIO_PORT                GPIOG
#define GTP_RST_GPIO_CLK                 RCC_APB2Periph_GPIOG
#define GTP_RST_GPIO_PIN                 GPIO_Pin_7

#define GTP_INT_GPIO_PORT                GPIOF
#define GTP_INT_GPIO_CLK                 RCC_APB2Periph_GPIOF
#define GTP_INT_GPIO_PIN                 GPIO_Pin_6
#define GTP_INT_EXTI_PORTSOURCE          GPIO_PortSourceGPIOF
#define GTP_INT_EXTI_PINSOURCE           GPIO_PinSource6
#define GTP_INT_EXTI_LINE                EXTI_Line6
#define GTP_INT_EXTI_IRQ                 EXTI9_5_IRQn

#define GTP_IRQHandler                   EXTI9_5_IRQHandler








uint8_t I2C_CheckDevice(uint8_t Address);

uint8_t I2C_ReadBytes( uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead);
uint8_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite);

void I2C_GTP_IRQEnable(void);
void I2C_GTP_IRQDisable(void);
void I2C_ResetChip(void);
void I2C_Touch_Init(void);




#endif
