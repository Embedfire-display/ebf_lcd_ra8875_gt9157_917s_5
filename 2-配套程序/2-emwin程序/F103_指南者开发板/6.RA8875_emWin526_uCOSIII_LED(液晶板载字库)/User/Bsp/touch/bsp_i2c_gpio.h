#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

//#include <inttypes.h>

#include "bsp_ra8875_lcd.h"

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */

/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define GTP_I2C_PORT    	    GPIOE			/* GPIO端口 */
#define GTP_I2C_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define GTP_I2C_SCL_PIN		    GPIO_Pin_4			/* 连接到SCL时钟线的GPIO */
#define GTP_I2C_SDA_PIN		    GPIO_Pin_2			/* 连接到SDA数据线的GPIO */

/*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
#define I2C_SCL_1()  GTP_I2C_PORT->BSRR = GTP_I2C_SCL_PIN				/* SCL = 1 */
#define I2C_SCL_0()  GTP_I2C_PORT->BRR = GTP_I2C_SCL_PIN				/* SCL = 0 */

#define I2C_SDA_1()  GTP_I2C_PORT->BSRR = GTP_I2C_SDA_PIN				/* SDA = 1 */
#define I2C_SDA_0()  GTP_I2C_PORT->BRR = GTP_I2C_SDA_PIN				/* SDA = 0 */

#define I2C_SDA_READ()  ((GTP_I2C_PORT->IDR & GTP_I2C_SDA_PIN) != 0)	/* 读SDA口线状态 */

#define GTP_ADDRESS            	0xBA

#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


#define GTP_RST_GPIO_PORT                GPIOE
#define GTP_RST_GPIO_CLK                 RCC_APB2Periph_GPIOE
#define GTP_RST_GPIO_PIN                 GPIO_Pin_0

#define GTP_INT_GPIO_PORT                GPIOE
#define GTP_INT_GPIO_CLK                 RCC_APB2Periph_GPIOE
#define GTP_INT_GPIO_PIN                 GPIO_Pin_3
#define GTP_INT_EXTI_PORTSOURCE          GPIO_PortSourceGPIOE
#define GTP_INT_EXTI_PINSOURCE           GPIO_PinSource3


uint8_t I2C_CheckDevice(uint8_t Address);

uint8_t I2C_ReadBytes( uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead);
uint8_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite);

void I2C_ResetChip(void);
void I2C_Touch_Init(void);




#endif
