#include "bsp_bmp.h"
#include "ff.h"
#include "bsp_ra8875_lcd.h"
#include<stdio.h>

#define RGB24TORGB16(R,G,B) ((unsigned short int)((((R)>>3)<<11) | (((G)>>2)<<5)	| ((B)>>3)))


/*
* M_FileAlignAdj宏，用于优化文件系统读取大于512字节数据时出现的地址对齐问题
* 参考帖子：http://www.amobbs.com/thread-5488537-1-1.html
* 每次读的接收指针ptr在m_FileIo_Buf[]的前4Byte之间移动，经过f_read()之后，DMA接收SD Sector数据时一定是(u32 *)地址。
*/
#define M_FileAlignAdj(pFile)       (4 - (512 - ((uint32_t)((pFile)->fptr) & 0x01FF)) & 0x0003)  


#define BMP_BUFF_SIZE  1024*2*10      //图片数据缓冲区大小  

__align(4) BYTE ColorData[BMP_BUFF_SIZE];      //接收缓冲区
BYTE * pColorData = (BYTE *)ColorData;					/* 用于指向缓冲区的指针，配合M_FileAlignAdj宏使用 */

FATFS bmpfs; 
FIL bmpfsrc; 
FRESULT bmpres;

/* 如果不需要打印bmp相关的提示信息,将printf注释掉即可
 * 如要用printf()，需将串口驱动文件包含进来
 */
#define BMP_DEBUG_PRINTF(FORMAT,...)  //printf(FORMAT,##__VA_ARGS__)	 

/* 打印BMP文件的头信息，用于调试 */
static void showBmpHead(BITMAPFILEHEADER* pBmpHead)
{
    BMP_DEBUG_PRINTF("位图文件头:\r\n");
    BMP_DEBUG_PRINTF("文件大小:%d\r\n",(*pBmpHead).bfSize);
    BMP_DEBUG_PRINTF("保留字:%d\r\n",(*pBmpHead).bfReserved1);
    BMP_DEBUG_PRINTF("保留字:%d\r\n",(*pBmpHead).bfReserved2);
    BMP_DEBUG_PRINTF("实际位图数据的偏移字节数:%d\r\n",(*pBmpHead).bfOffBits);
		BMP_DEBUG_PRINTF("\r\n");	
}

/* 打印BMP文件的头信息，用于调试 */
static void showBmpInforHead(tagBITMAPINFOHEADER* pBmpInforHead)
{
    BMP_DEBUG_PRINTF("位图信息头:\r\n");
    BMP_DEBUG_PRINTF("结构体的长度:%d\r\n",(*pBmpInforHead).biSize);
    BMP_DEBUG_PRINTF("位图宽:%d\r\n",(*pBmpInforHead).biWidth);
    BMP_DEBUG_PRINTF("位图高:%d\r\n",(*pBmpInforHead).biHeight);
    BMP_DEBUG_PRINTF("biPlanes平面数:%d\r\n",(*pBmpInforHead).biPlanes);
    BMP_DEBUG_PRINTF("biBitCount采用颜色位数:%d\r\n",(*pBmpInforHead).biBitCount);
    BMP_DEBUG_PRINTF("压缩方式:%d\r\n",(*pBmpInforHead).biCompression);
    BMP_DEBUG_PRINTF("biSizeImage实际位图数据占用的字节数:%d\r\n",(*pBmpInforHead).biSizeImage);
    BMP_DEBUG_PRINTF("X方向分辨率:%d\r\n",(*pBmpInforHead).biXPelsPerMeter);
    BMP_DEBUG_PRINTF("Y方向分辨率:%d\r\n",(*pBmpInforHead).biYPelsPerMeter);
    BMP_DEBUG_PRINTF("使用的颜色数:%d\r\n",(*pBmpInforHead).biClrUsed);
    BMP_DEBUG_PRINTF("重要颜色数:%d\r\n",(*pBmpInforHead).biClrImportant);
		BMP_DEBUG_PRINTF("\r\n");
}

/*
 * 显示bmp图片, 24位真彩色
 * 图片宽度和高度根据图片大小而定
 */
void Lcd_show_bmp(unsigned short int x, unsigned short int y, char *pic_name)
{
  
	int i, j ;
	int width, height, l_width;
  
	BYTE red,green,blue;
	BITMAPFILEHEADER bitHead;
	BITMAPINFOHEADER bitInfoHead;
	WORD fileType;

	unsigned int read_num;
	unsigned char tmp_name[20];
	sprintf((char*)tmp_name,"%s",pic_name);

	bmpres = f_open(&bmpfsrc,(char *)tmp_name, FA_OPEN_EXISTING|FA_READ);	
/*-------------------------------------------------------------------------------------------------------*/
	if(bmpres == FR_OK)
	{
		BMP_DEBUG_PRINTF("Open file success\r\n");

		/* 读取文件头信息  两个字节*/         
		f_read(&bmpfsrc,&fileType,sizeof(WORD),&read_num);     

		/* 判断是不是bmp文件 "BM"*/
		if(fileType != 0x4d42)
		{
			BMP_DEBUG_PRINTF("file is not .bmp file!\r\n");
			return;
		}
		else
		{
			BMP_DEBUG_PRINTF("Ok this is .bmp file\r\n");	
		}        

		/* 读取BMP文件头信息*/
		f_read(&bmpfsrc,&bitHead,sizeof(tagBITMAPFILEHEADER),&read_num);        
		showBmpHead(&bitHead);

		/* 读取位图信息头信息 */
		f_read(&bmpfsrc,&bitInfoHead,sizeof(BITMAPINFOHEADER),&read_num);        
		showBmpInforHead(&bitInfoHead);
	}    
	else
	{
		BMP_DEBUG_PRINTF("file open fail!\r\n");
		return;
	}    
/*-------------------------------------------------------------------------------------------------------*/
	width = bitInfoHead.biWidth;  
 
  /* bmp图像的高为负数表示图像存储方向是从上到下，为正数表示从下到上 */
  height = (bitInfoHead.biHeight>0 ? bitInfoHead.biHeight:-bitInfoHead.biHeight);
  
 
  l_width = WIDTHBYTES(width* bitInfoHead.biBitCount);	


	if(l_width > 800*480*3)
	{
		BMP_DEBUG_PRINTF("\n SORRY, PIC IS TOO BIG (<=800)\n");
		return;
	}
	
  
  /* 设置图片的位置和大小， 即设置显示窗口 因为y方向反转了，需要重新计算y方向坐标*/
	RA8875_SetDispWin(x, y, height, width);
  
   /* BMP图片数据写入方向 */
  RA8875_SetWriteDirection(0,0);

  LCD_RA8875_REG(0x02); 		/* 准备读写显存 */
	
	/* 判断是否是24bit 或16bits 图片 */
	if(bitInfoHead.biBitCount == 24 || bitInfoHead.biBitCount == 16)
	{
    uint8_t rowNum = BMP_BUFF_SIZE/l_width  ;     //每次读取的行数（一次读多行可加快图片显示速度）
    uint8_t readCount = height/rowNum;             //按rowNum读取的次数  
    uint8_t restRow = height%rowNum;                //不足rowNum行，剩余的行数 
    
    uint8_t *ptr;                                     //显示时用于调整buff顺序的指针
    uint8_t row;                                      //显示时的行计数
    
  
    for(i=0;i< readCount ||(restRow && i == readCount); i++)  //当i=readcound 且restRow非零时，读取剩余不足rownum行的数据
		{	 
      
      if(bitInfoHead.biHeight>0)                    //若储存数据方向为正向
      {
        if(i != readCount)                             
        {
          /* bmp文件储存数据是倒序的，这里定位bmp文件位置，使读出的数据是顺序的*/
          f_lseek(&bmpfsrc,bitHead.bfOffBits+l_width*(height-(i+1)*rowNum));        //一次读取rowNum行
        }
        else        
        {        
          //最后一次，一次读取restRow行
          f_lseek(&bmpfsrc,bitHead.bfOffBits+l_width*(height-readCount*rowNum-restRow));  
          
          /*把rowNum值修改成restRow，即将最后一次读数据*/
          rowNum = restRow;           
        }
      }
      
      pColorData = (BYTE *)ColorData;  
      pColorData += M_FileAlignAdj(&bmpfsrc);           //4字节地址对齐优化，可提高数据读取速度，不经过底层的memcpy搬运
      
      /* 一次读取rowNum行或restRow行数据(最后一次时读取restRow) */
      f_read(&bmpfsrc,pColorData,l_width*rowNum,&read_num); 

      /* 24位图像输出 */
      if(bitInfoHead.biBitCount == 24)
      {
        for(row=0;row<rowNum;row++)
        {
          if(bitInfoHead.biHeight>0)
            ptr=pColorData+l_width*(rowNum-(row+1));       //重新定向，使显示数据按顺序排列
          else
            ptr=pColorData+l_width*row;
            
          for(j=0; j<l_width;j+=3) 											   //一行有效信息
          {           
            red = ptr[j+2];
            green = ptr[j+1];
            blue = 	ptr[j];
            LCD_RA8875_WRITE_DATA(RGB24TORGB16(red,green,blue)); //写入LCD-GRAM 
          } 
         }
      }
      else if(bitInfoHead.biBitCount == 16) /*16位图像输出*/
      {           
        for(row=0;row<rowNum;row++)
        {
          if(bitInfoHead.biHeight>0)
            ptr=pColorData+l_width*(rowNum-(row+1));       //重新定向，使显示数据按顺序排列
          else
            ptr=pColorData+l_width*row;
          
        for(j=0; j<l_width; j++) 											   //一行有效信息
        {          
          LCD_RA8875_WRITE_DATA(ptr[j]|ptr[++j]<<8);   //写入LCD-GRAM      
        } 
        }
      }      
    }

       
	}    
	else 
	{        
		BMP_DEBUG_PRINTF("SORRY, THIS  IS NOT A 24 NOR A 16 BITS  PIC");
		return ;
	}
	f_close(&bmpfsrc);    
  
  	/* 退出窗口绘图模式 */
	RA8875_QuitWinMode();
  


}


/************************end of file******************/
