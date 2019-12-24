#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include <inttypes.h>

#define I2C_WR_EE	0		/* Ð´¿ØÖÆbit */
#define I2C_RD_EE	1		/* ¶Á¿ØÖÆbit */

void i2c_Start_EE(void);
void i2c_Stop_EE(void);
void i2c_SendByte_EE(uint8_t _ucByte);
uint8_t i2c_ReadByte_EE(void);
uint8_t i2c_WaitAck_EE(void);
void i2c_Ack_EE(void);
void i2c_NAck_EE(void);
uint8_t i2c_CheckDevice_EE(uint8_t _Address);

#endif
