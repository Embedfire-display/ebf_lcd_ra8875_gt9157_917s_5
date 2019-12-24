#include "stm32f10x.h"
#include "fatfs_flash_spi.h"
#include "bsp_usart1.h"	
#include "ff.h"

FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res_flash; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[1024]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "欢迎使用野火STM32-V2 ISO开发板  今天是个好日子，新建文件系统测试文件";


void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
int main(void)
{
		TM_FATFS_FLASH_SPI_disk_initialize();
		SPI_FLASH_BulkErase();
		/* USART1 config */
		USART1_Config();
		printf("\r\n this is a fatfs test demo \r\n");
/*************************   flash 文件系统   *********************************************/
		/* Register work area for each volume (Always succeeds regardless of disk status) */
		res_flash = f_mount(&fs,"0:",1);
		printf("\r\n f_mount res_flash=%d \r\n",res_flash);

		if(res_flash ==FR_NO_FILESYSTEM)
		{
			res_flash=f_mkfs("0:",0,4096);
			printf("\r\nmkfs res_flash=%d",res_flash);
			res_flash = f_mount(&fs,"0:",0);
			res_flash = f_mount(&fs,"0:",1);
		}
/**************************  flash   *****************************************/		
		res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
		 
		if ( res_flash == FR_OK )
		{
			res_flash = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
			f_close(&fnew);      
		}
		res_flash = f_open(&fnew, "0:flashnewfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
		res_flash = f_read(&fnew, buffer, sizeof(buffer), &br); 
		printf("\r\n %s ", buffer);
		/* Close open files */
		f_close(&fnew);	 
		while(1)
		{
			
		}
}

/*********************************************END OF FILE**********************/
