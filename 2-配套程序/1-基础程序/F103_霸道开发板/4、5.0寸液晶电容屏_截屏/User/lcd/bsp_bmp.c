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



/**********************************************************
 * 函数名：Screen_shot
 * 描述  ：截取LCD指定位置  指定宽高的像素 保存为24位真彩色bmp格式图片
 * 输入  : 	x								---水平位置 
 *					y								---竖直位置  
 *					Width						---水平宽度   
 *					Height					---竖直高度  	
 *					filename				---文件名
 * 输出  ：	0 		---成功
 *  				-1 		---失败
 *	    		8			---文件已存在
 * 举例  ：Screen_shot(0, 0, 320, 240, "/myScreen");-----全屏截图
 * 注意  ：x范围[0,319]  y范围[0,239]  Width范围[0,320-x]  Height范围[0,240-y]
 *					如果文件已存在,将直接返回	
 **************************************************************/ 
/*
 * bmp文件头有54个字节，其中前14个字节是文件头信息，后40个字节是位图信息头信息
 * bmp文件头之后就是具体的像素信息
 * 0x42 0x4d :bm
 * 54        :实际位图数据的偏移字节数
 * 40        :位图信息头结构体的长度
 * 1         :平面数为1
 * 24        :24bit真彩色
 */
int Screen_Shot(unsigned short int x,
                  unsigned short int y,
                  unsigned short int Width,
                  unsigned short int Height,
                  unsigned char *filename)
{
	/* bmp  文件头 54个字节 */
	unsigned char header[54] =
	{
		0x42, 0x4d, 0, 0, 0, 0, 
		0, 0, 0, 0, 54, 0, 
		0, 0, 40,0, 0, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 1, 0, 24, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 
		0, 0, 0
	};
	
	int i;
	int j;
	long file_size;     
	long width;
	long height;
	unsigned char r,g,b;	
	unsigned char tmp_name[30];
	unsigned int mybw;
	unsigned int read_data;
	char kk[4]={0,0,0,0};
	
	/* 宽*高 +补充的字节 + 头部信息 */
	file_size = (long)Width * (long)Height * 3 + Height*(Width%4) + 54;		

	/* 文件大小 4个字节 */
	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
	
	/* 位图宽 4个字节 */
	width=Width;	
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) &0x000000ff;
	header[20] = (width >> 16) &0x000000ff;
	header[21] = (width >> 24) &0x000000ff;
	
	/* 位图高 4个字节 */
	height = Height;
	header[22] = height &0x000000ff;
	header[23] = (height >> 8) &0x000000ff;
	header[24] = (height >> 16) &0x000000ff;
	header[25] = (height >> 24) &0x000000ff;
	
	/* 将filename 按照一定的格式拷贝到 tmp_name */
	sprintf((char*)tmp_name,"1:%s.bmp",filename);
	
//	/* 注册一个工作区，逻辑号为0 */
//	f_mount(0, &bmpfs[0]);

   /* BMP图片数据写入方向 */
  RA8875_SetReadDirection(0,0);
	
	/* 新建一个文件 */
	bmpres = f_open( &bmpfsrc , (char*)tmp_name, FA_CREATE_ALWAYS | FA_WRITE );
	
	/* 新建文件之后要先关闭再打开才能写入 */
	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)tmp_name,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK || bmpres ==FR_EXIST )
	{    
		/* 将预先定义好的bmp头部信息写进文件里面 */
		bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		
		
		/* 下面是将指定窗口的数据读出来写到文件里面去 */
		for(i=0; i<Height; i++)					
		{
     /* bmp文件储存数据是倒序的，这里定位bmp文件位置，使读出的数据是顺序的*/
//     f_lseek(&bmpfsrc,bitHead.bfOffBits+l_width*(height-(i+1)*rowNum));        //一次读取rowNum行
      #if 0
			if( !(Width%4) )				/* 刚好是4字节对齐 */
			{
       
        
				for(j=0; j<Width; j++)  
				{					
					read_data = RA8875_GetPixel(x+j, y+i);				
					
					r =  GETR_FROM_RGB16(read_data);
					g =  GETG_FROM_RGB16(read_data);
					b =  GETB_FROM_RGB16(read_data);

					bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
				}
			}
			else
			{
				for(j=0;j<Width;j++)
				{					
					read_data = RA8875_GetPixel(x+i,y+j );
					
					r =  GETR_FROM_RGB16(read_data);
					g =  GETG_FROM_RGB16(read_data);
					b =  GETB_FROM_RGB16(read_data);

					bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
				}
				/* 不是4字节对齐则需要补齐 */	
				bmpres = f_write(&bmpfsrc, kk,sizeof(unsigned char)*(Width%4), &mybw);
			}

      #else
        if( !(Width%4) )				/* 刚好是4字节对齐 */
			{
       
        
				for(j=0; j<Width; j++)  
				{					
					read_data = RA8875_GetPixel(x+j, y+i);				
					
					r =  GETR_FROM_RGB16(read_data);
					g =  GETG_FROM_RGB16(read_data);
					b =  GETB_FROM_RGB16(read_data);

					bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
				}
			}
			else
			{
				for(j=0;j<Width;j++)
				{					
					read_data = RA8875_GetPixel(x+i,y+j );
					
					r =  GETR_FROM_RGB16(read_data);
					g =  GETG_FROM_RGB16(read_data);
					b =  GETB_FROM_RGB16(read_data);

					bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
					bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
				}
				/* 不是4字节对齐则需要补齐 */	
				bmpres = f_write(&bmpfsrc, kk,sizeof(unsigned char)*(Width%4), &mybw);
			}

      #endif      
		}/* 截屏完毕 */

		f_close(&bmpfsrc); 
    
      RA8875_SetReadDirection(0,0);
		return 0;
	}
	else if ( bmpres == FR_EXIST )  //如果文件已经存在
	{
    
    printf("exist");
    
      RA8875_SetReadDirection(0,0);
		return FR_EXIST;	 					//8
	}
	else/* 截屏失败 */
	{
        printf("error");
  RA8875_SetReadDirection(0,0);
		return -1;
	}    
}


/************************end of file******************/
