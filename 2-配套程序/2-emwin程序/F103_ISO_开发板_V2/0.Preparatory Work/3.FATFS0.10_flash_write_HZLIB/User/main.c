#include "stm32f10x.h"
#include "fatfs_flash_spi.h"
#include "bsp_usart1.h"	
#include "ff.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_led.h"

FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT myres;
FIL myfsrc;
UINT mybr;

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

void fatfs_to_flash(void);

int main(void)
{
#if 1			//将Flash文件系统里边HZLIB.bin字库文件写入到外部flash起始地址(4096)、共212KB空间上
	/* USART1 config */
	USART1_Config();
	LED_GPIO_Config();
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	printf("\r\n this is a HZLIB write test demo \r\n");
	fatfs_to_flash();
	while(1);
#elif 0
	LCD_Init();	
	Delay(0XAFFF<<2);
	USART1_Config();
	TM_FATFS_FLASH_SPI_disk_initialize();
	printf("usart init\n");
	LCD_Clear(0, 0, 240, 320, BACKGROUND);

	Lcd_GramScan(1);
	LCD_DispStr(5, 5, (uint8_t *)"fatfs init success", RED); 

	LCD_DispStrCH(50, 50, (uint8_t *)"野火开发板", RED);   
	LCD_DispEnCh(30, 90, "Welcome to use 野火 ISO Stm32 开发板 bigo------",RED);
	
	while(1)
	{
	}
#else
	while(1);
#endif
}

void fatfs_to_flash(void)
{
	uint32_t write_addr=0,j=0;
	uint8_t tempbuf[256]={0};	
	myres=f_mount(&fs,"0:",1);
/****************************************************************************/
#if 1
	write_addr=60*4096;	
	
	printf("1.f_mount_xbf res=%d\n",myres);
	myres = f_open(&myfsrc,"0:youyuan16.xbf",FA_OPEN_EXISTING | FA_READ);
	printf("2.f_open_xbf res=%d\n",myres);
	
	for(j=0;j<317;j++)//擦除扇区，起始位置1900*4096共212KB
	{
		SPI_FLASH_SectorErase(write_addr+j*4096);
	}
	j=0;
	while(myres == FR_OK) 
	{
		myres = f_read( &myfsrc, tempbuf, 256, &mybr);//读取数据	 
		//printf("%d.f_read_xbf res=%d\n",j,myres);
		if(myres!=FR_OK)break;			 //执行错误  
		SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //拷贝数据到外部flash上    
		write_addr+=256;				
		j++;
		if(mybr !=256)break;
	} 
	
	f_close(&myfsrc);
	
	SPI_FLASH_BufferRead(tempbuf,60*4096,100);			//读取数据，打印验证
	printf("readbuf_xbf\n");
	for(j=0;j<100;j++)
		printf("%X",tempbuf[j]);
	printf("\n如果tempbuf不全为 FF ，那么说明字库拷贝成功！！！\n");
	LED1_ON;
#endif
/************************************************************************/
#if 1
	write_addr=4096;	
	
	printf("1.f_mount_bin res=%d\n",myres);
	myres = f_open(&myfsrc,"0:HZLIB.bin",FA_OPEN_EXISTING | FA_READ);

	printf("2.f_open_bin res=%d\n",myres);
	
	for(j=0;j<53;j++)//擦除扇区，起始位置1900*4096共212KB
	{
		SPI_FLASH_SectorErase(write_addr+j*4096);
	}
	j=0;
	while(myres == FR_OK) 
	{
		myres = f_read( &myfsrc, tempbuf, 256, &mybr);//读取数据	 
		//printf("%d.f_read_bin res=%d\n",j,myres);
		if(myres!=FR_OK)break;			 //执行错误  
		SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //拷贝数据到外部flash上    
		write_addr+=256;				
		j++;
		if(mybr !=256)break;
	} 
	
	f_close(&myfsrc);
	
	SPI_FLASH_BufferRead(tempbuf,4096,100);			//读取数据，打印验证
	printf("readbuf_bin\n");
	for(j=0;j<100;j++)
		printf("%X",tempbuf[j]);
	printf("\n如果tempbuf不全为 FF ，那么说明字库拷贝成功！！！\n");
	LED2_ON;
#endif	

/************************************************************************/
#if 1
	write_addr=465*4096;
	printf("1.f_mount_gbk res=%d\n",myres);
	myres = f_open(&myfsrc,"0:UNIGBK.BIN",FA_OPEN_EXISTING | FA_READ);
	printf("2.f_open_gbk res=%d,filesize=%d\n",myres,(int)myfsrc.fsize);
	
	for(j=0;j<43;j++)//擦除扇区，起始位置1900*4096共212KB
	{
		SPI_FLASH_SectorErase(write_addr+j*4096);
	}
	j=0;
	while(myres == FR_OK) 
	{
		myres = f_read( &myfsrc, tempbuf, 256, &mybr);//读取数据	 
		//printf("%d.f_read_gbk res=%d\n",j,myres);
		if(myres!=FR_OK)break;			 //执行错误  
		SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //拷贝数据到外部flash上    
		write_addr+=256;				
		j++;
		if(mybr !=256)break;
	} 
	
	f_close(&myfsrc);
	SPI_FLASH_BufferRead(tempbuf,465*4096+174344/2,100);			//读取数据，打印验证
	printf("readbuf_UNIGBK.BIN\n");
	for(j=0;j<100;j++)
		printf("%X",tempbuf[j]);
	printf("\n如果tempbuf不全为 FF ，那么说明字库拷贝成功！！！\n");
	
#endif	
	LED3_ON;
	myres=f_mount(&fs,"0:",0);
}
/*********************************************END OF FILE**********************/
