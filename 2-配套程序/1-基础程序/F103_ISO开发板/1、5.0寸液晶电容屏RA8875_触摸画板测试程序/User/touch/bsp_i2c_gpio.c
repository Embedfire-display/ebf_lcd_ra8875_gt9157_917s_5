/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  fire 
  * @version v1.0
  * @date    2015-xx-xx
  * @brief   本文件专用于驱动GT9xx.h触摸屏芯片，使用模拟IIC
  *           用gpio模拟I2C总线, 适用于STM32系列CPU。
  *          该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
  *   
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
#include "bsp_i2c_gpio.h"
#include "gt9xx.h"

/***模拟IIC通用函数****/


static void I2C_GPIO_Config(void);
static void I2C_Start(void);
static void I2C_Stop(void);
static void I2C_SendByte(uint8_t ucByte);
static uint8_t I2C_ReadByte(void);
static uint8_t I2C_WaitAck(void);
static void I2C_Ack(void);
static void I2C_NAck(void);


/**
  * @brief   I2C_Delay I2C总线位延迟，最快400KHz
  * @param   无
  * @retval  无
  */
static void I2C_Delay(void)
{
	uint8_t i;

	/*　
    可使用逻辑分析仪来观察IIC通讯的频率
		CPU主频72MHz时，在内部Flash运行, MDK工程不优化
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
        
	*/
	for (i = 0; i < 10; i++);
}


/**
  * @brief   Delay 复位触屏芯片使用的延迟函数
  * @param   无
  * @retval  无
  */
static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief   I2C_Start CPU发起I2C总线启动信号
  * @param   无
  * @retval  无
  */
void I2C_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	I2C_SDA_1();
	I2C_SCL_1();
	I2C_Delay();
	I2C_SDA_0();
	I2C_Delay();
	I2C_SCL_0();
	I2C_Delay();
}


/**
  * @brief   I2C_Stop CPU发起I2C总线停止信号
  * @param   无
  * @retval  无
  */
void I2C_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2C_SDA_0();
	I2C_SCL_1();
	I2C_Delay();
	I2C_SDA_1();
}





/**
  * @brief   I2C_Ack 产生一个ACK信号
  * @param   无
  * @retval  无
  */
static uint8_t I2C_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* CPU释放SDA总线 */
	I2C_Delay();
	I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	I2C_Delay();
	if (I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	I2C_Delay();
	return re;
}


/**
  * @brief   I2C_Ack 产生一个ACK信号
  * @param   无
  * @retval  无
  */
static void I2C_Ack(void)
{
	I2C_SDA_0();	/* CPU驱动SDA = 0 */
	I2C_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	I2C_Delay();
	I2C_SCL_0();
	I2C_Delay();
	I2C_SDA_1();	/* CPU释放SDA总线 */
}



/**
  * @brief   I2C_NAck 产生1个NACK信号
  * @param   无
  * @retval  无
  */
static void I2C_NAck(void)
{
	I2C_SDA_1();	/* CPU驱动SDA = 1 */
	I2C_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	I2C_Delay();
	I2C_SCL_0();
	I2C_Delay();	
}


/**
  * @brief   I2C_SendByte CPU向总线设备发送8bit数据
  * @param   等待发送的字节
  * @retval  无
  */
static void I2C_SendByte(uint8_t ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		I2C_Delay();
		I2C_SCL_1();
		I2C_Delay();	
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); // 释放总线
		}
		ucByte <<= 1;	/* 左移一个bit */
		I2C_Delay();
	}
}



/**
  * @brief   I2C_ReadByte CPU从I2C总线设备读取8bit数据
  * @param   无
  * @retval  读到的数据
  */
static uint8_t I2C_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		I2C_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		I2C_Delay();
	}
	return value;
}




/**
  * @brief  I2C_CheckDevice 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
  * @param  从设备地地址
  * @retval 返回值 0 表示正确， 返回1表示未探测到
  */
uint8_t I2C_CheckDevice(uint8_t Address)
{
	uint8_t ucAck;

	I2C_GPIO_Config();		/* 配置GPIO */
	
	I2C_Start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	I2C_SendByte(Address | I2C_WR);
	ucAck = I2C_WaitAck();	/* 检测设备的ACK应答 */

	I2C_Stop();			/* 发送停止信号 */

	return ucAck;
}

/*****GT9xx触屏芯片专用函数*****/

/**
  * @brief   使用IIC读取数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:存放由从机读取的数据的缓冲区指针
  *		@arg NumByteToRead:读取的数据长度
  * @retval  无
  */
uint8_t I2C_ReadBytes( uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
	uint16_t i;
	
	
	/* 第1步：发起I2C总线启动信号 */
	I2C_Start();

	/* 第2步：发送读设备地址，即将读取数据*/
	I2C_SendByte(ClientAddr|I2C_RD);
	
	/* 第3步：等待ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}	
	
	/* 第4步：循环读取数据 */
	for (i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = I2C_ReadByte();	/* 读1个字节 */     
		
		/* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
		if (i != NumByteToRead - 1)
		{
			I2C_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
		}
		else
		{
			I2C_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
		}  
	}
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 0;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();

	return 0xff;
}

/**
  * @brief   使用IIC写入数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:缓冲区指针
  *   @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint8_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
	uint16_t i;
  
  /* 第1步：发起I2C总线启动信号 */
  I2C_Start();

  /* 第2步：发送写设备地址，即将写入数据 */
  I2C_SendByte(ClientAddr);
  
  /* 第3步：等待ACK */
  if (I2C_WaitAck() != 0)
  {
    goto cmd_fail;	/*器件无应答 */
  }
  
	for (i = 0; i < NumByteToWrite; i++)
	{
	
		/* 第4步：开始写入数据 */
		I2C_SendByte(pBuffer[i]);
	
		/* 第5步：等待ACK */
		if (I2C_WaitAck() != 0)
		{
			goto cmd_fail;	/* 器件无应答 */
		}

	}
	
	/* 命令执行成功，发送I2C总线停止信号 */
	I2C_Stop();
	return 0;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();
  
	return 0xff;
}





/**
  * @brief   I2C_GPIO_Config 初始化控制触屏芯片的GPIO IIC RST INT
  * @param   无
  * @retval  无
  */
static void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(GTP_I2C_GPIO_CLK|GTP_RST_GPIO_CLK|GTP_INT_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = GTP_I2C_SCL_PIN | GTP_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(GTP_I2C_PORT, &GPIO_InitStructure);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	I2C_Stop();  
 
  /*!< Configure RST */   
  GPIO_InitStructure.GPIO_Pin = GTP_RST_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GTP_RST_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure INT */   
  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);

}




/**
  * @brief  使能GT91xx中断
  * @param  无
  * @retval 无
  */
void I2C_GTP_IRQEnable(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;  
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;  
  /*!< Configure INT */   
  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);  
  
  /* 连接 EXTI 中断源 到INT 引脚 */
  GPIO_EXTILineConfig(GTP_INT_EXTI_PORTSOURCE, GTP_INT_EXTI_PINSOURCE);

  /* 选择 EXTI 中断源 */
  EXTI_InitStructure.EXTI_Line = GTP_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);  
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = GTP_INT_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}


/**
  * @brief  禁止GT91xx中断
  * @param  无
  * @retval 无
  */
void I2C_GTP_IRQDisable(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  /*!< Configure INT */
  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);

  /* 连接 EXTI 中断源 到INT 引脚 */
  GPIO_EXTILineConfig(GTP_INT_EXTI_PORTSOURCE, GTP_INT_EXTI_PINSOURCE);

  /* 选择 EXTI 中断源 */
  EXTI_InitStructure.EXTI_Line = GTP_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = GTP_INT_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

}




/**
  * @brief  对GT91xx芯片进行复位
  * @param  无
  * @retval 无
  */
void I2C_ResetChip(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;

	  /*!< Configure INT */
	  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);

	  /*初始化GT9157,rst为高电平，int为低电平，则gt9157的设备地址被配置为0xBA*/

	  /*复位为低电平，为初始化做准备*/
    GPIO_ResetBits (GTP_INT_GPIO_PORT,GTP_INT_GPIO_PIN);
	  GPIO_ResetBits (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN);
	  Delay(0x0FFFFF);

	  /*拉高一段时间，进行初始化*/
	  GPIO_SetBits (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN);
	  Delay(0x0FFFFF);

	  /*把INT引脚设置为浮空输入模式*/
	  /*!< Configure INT */
	  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
}


/**
  * @brief  I2C 外设(GT91xx)初始化
  * @param  无
  * @retval 无
  */
void I2C_Touch_Init(void)
{
  I2C_GPIO_Config(); 
 
  /*必须要复位芯片以设置IIC地址*/
  I2C_ResetChip();

  I2C_GTP_IRQEnable();
}



/* ------------------------------------------end of file---------------------------------------- */

