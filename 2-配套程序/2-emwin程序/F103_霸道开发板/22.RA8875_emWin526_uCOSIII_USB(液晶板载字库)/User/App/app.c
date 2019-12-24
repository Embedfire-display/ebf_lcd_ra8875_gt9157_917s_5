/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   5.0寸电容液晶触摸画板实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "includes.h"

/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/
#define ICONVIEW_Width     70   /* 控件ICONVIEW的宽 */  
#define ICONVIEW_Height    70   /* 控件ICONVIEW的高，包括图标的Y方向间距 */  
#define ICONVIEW_XSpace    30   /* 控件ICONVIEW的X方向间距 */ 
#define ICONVIEW_YSpace    20   /* 控件ICONVIEW的Y方向间距 */ 
#define ICONVIEW_XPos	   10    /* 控件ICONVIEW的X方向调整距离*/
#define BOTWIN_YSpace	   60   /* 屏幕下方部分窗口Y方向间距*/
#define MIDWIN_yPos		   40   /* 中间窗口Y方向起始位置即顶部文字可用高度*/
#define MIDWIN_xPos		   0 	/* 中间窗口Y方向起始位置*/

/*
*********************************************************************************************************
*                                      变量
*********************************************************************************************************
*/ 
extern GUI_CONST_STORAGE GUI_BITMAP bmled;
extern GUI_CONST_STORAGE GUI_BITMAP bmbutton;
extern GUI_CONST_STORAGE GUI_BITMAP bmsram;
extern GUI_CONST_STORAGE GUI_BITMAP bmCAN;

extern GUI_CONST_STORAGE GUI_BITMAP bmflash;
extern GUI_CONST_STORAGE GUI_BITMAP bmadc;
extern GUI_CONST_STORAGE GUI_BITMAP bmWeather;
extern GUI_CONST_STORAGE GUI_BITMAP bmWeather2;

extern GUI_CONST_STORAGE GUI_BITMAP bmbln;
extern GUI_CONST_STORAGE GUI_BITMAP bmclock;
extern GUI_CONST_STORAGE GUI_BITMAP bmmusic;
extern GUI_CONST_STORAGE GUI_BITMAP bmcamera;

extern GUI_CONST_STORAGE GUI_BITMAP bmPhone;
extern GUI_CONST_STORAGE GUI_BITMAP bmmessage;
extern GUI_CONST_STORAGE GUI_BITMAP bmnote;
extern GUI_CONST_STORAGE GUI_BITMAP bmusb;

extern GUI_CONST_STORAGE GUI_BITMAP bmcalculator;
extern GUI_CONST_STORAGE GUI_BITMAP bmpicture;
extern GUI_CONST_STORAGE GUI_BITMAP bmbrowser;

/*时间结构体*/
extern struct rtc_time systmtime;
extern char TimeDisplay;
//GUI_XBF_DATA XBF_Data;
//GUI_FONT     XBF_Font;

uint8_t OnICON0  = 0;   /* ICONVIEW控件按下的标志，0表示未按下，1表示按下 */
uint8_t OnICON1  = 0;
uint8_t OnICON2  = 0;
uint8_t OnICON3  = 0;
uint8_t OnICON4  = 0;
uint8_t OnICON5  = 0;
uint8_t OnICON6  = 0;
uint8_t OnICON7  = 0;
uint8_t OnICON8  = 0;
uint8_t OnICON9  = 0;
uint8_t OnICON10  = 0;
uint8_t OnICON11  = 0;
uint8_t OnICON12  = 0;
uint8_t OnICON13  = 0;
uint8_t OnICON14  = 0;
uint8_t OnICON15  = 0;
uint8_t OnICON16  = 0;
uint8_t OnICON17  = 0;
uint8_t OnICON18  = 0;


/* 用于桌面ICONVIEW图标的创建 */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

typedef struct WIN_PARA{			//窗口使用到的用户定义参数，方便在回调函数中使用
	int xSizeLCD, ySizeLCD;			//LCD屏幕尺寸
	int xPosWin,  yPosWin;			//窗口的起始位置
	int xSizeWin, ySizeWin;			//窗口尺寸	
	int xSizeBM,  ySizeBM;
	int ySizeBotWin;
	WM_HWIN hWinBot;				//控制窗口的句柄（底部）
	WM_HWIN hWinMid;				//主显示窗的句柄（中部）	
}WIN_PARA;

WIN_PARA WinPara;			//用户使用的窗口额外数据
uint8_t app_prevent_refresh = 0;  /* 这个参数比较重要，用于防止ICON控件点击下时就触发回调函数的WM_PAINT消息*/

/* 用于桌面ICONVIEW0图标的创建 */
static const BITMAP_ITEM _aBitmapItem[] = 
{
	{&bmled,  	 	"LED"			},
	{&bmbutton,  	"KEY"			},  
	{&bmsram,      	"SRAM"			},
	{&bmCAN,  	 	"CAN"			},

	{&bmflash,    	"EEPROM" 		},
	{&bmadc,  	 	"ADC"			},
	{&bmWeather,   	"DS18B20" 		},
	{&bmWeather2,  	"TEMP" 			},

	{&bmbln, 		"Breathing"		},  
	{&bmclock,     	"Clock" 		},
//	{&bmmusic,     	"Music" 		},
//	{&bmcamera,    	"Camera"		},

	{&bmPhone,     	"Phone" 		},
	{&bmmessage,  	"Message"		},
	{&bmnote,      	"Note" 			},
	{&bmusb,  	   	"USB"			},

//	{&bmcalculator,	"Calculator" 	},
//	{&bmpicture,   	"Picture" 		},
//	{&bmbrowser,   	"Browser"		},
};




//void OnICON0Clicked(void)  {printf("OnICON0Clicked\n");}
//void OnICON1Clicked(void)  {printf("OnICON1Clicked\n");}
//void OnICON2Clicked(void)  {printf("OnICON2Clicked\n");}
//void OnICON3Clicked(void)  {printf("OnICON3Clicked\n");}

//void OnICON4Clicked(void)  {printf("OnICON4Clicked\n");}
//void OnICON5Clicked(void)  {printf("OnICON5Clicked\n");}
//void OnICON6Clicked(void)  {printf("OnICON6Clicked\n");}
//void OnICON7Clicked(void)  {printf("OnICON7Clicked\n");}

//void OnICON8Clicked(void)  {printf("OnICON8Clicked\n");}
//void OnICON9Clicked(void)  {printf("OnICON9Clicked\n");}
//void OnICON10Clicked(void) {printf("OnICON10Clicked\n");}
//void OnICON11Clicked(void) {printf("OnICON11Clicked\n");}

//void OnICON12Clicked(void) {printf("OnICON12Clicked\n");}
//void OnICON13Clicked(void) {printf("OnICON13Clicked\n");}
void OnICON14Clicked(void) {printf("OnICON14Clicked\n");}
void OnICON15Clicked(void) {printf("OnICON15Clicked\n");}

void OnICON16Clicked(void) {printf("OnICON16Clicked\n");}
void OnICON17Clicked(void) {printf("OnICON17Clicked\n");}
void OnICON18Clicked(void) {printf("OnICON18Clicked\n");}
	
/*
*********************************************************************************************************
*	函 数 名: _cbBkWindow
*	功能说明: 桌面窗口的回调函数 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	char text_buffer[20]={0};
	WM_HWIN hText;
	switch (pMsg->MsgId) 
	{
		case WM_TIMER://处理时间显示的信息，每秒钟修改一次时间
			if(!TimeDisplay || app_prevent_refresh)
			{
				WM_RestartTimer(pMsg->Data.v, 1000);
				break;
			}
			/* 获取text句柄 */      
			hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
			
			/* 转换rtc值至北京时间 */
			RTC_TimeCovr(&systmtime);
  
			/* 转换成字符串 */
			sprintf(text_buffer,"%02d:%02d:%02d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
			/* 输出时间 */
			TEXT_SetText(hText,text_buffer);
			WM_RestartTimer(pMsg->Data.v, 1000);
		break;
		/* 重绘消息*/
		case WM_PAINT:		
				GUI_SetBkColor(GUI_LIGHTCYAN+0x60);
				GUI_Clear();
//				GUI_SetFont(&GUI_FontHZ32);
//				GUI_SetColor(GUI_RED);
//				GUI_DispStringHCenterAt("APP图标显示",400,300);			
				ICONVIEW_SetSel(WM_GetDialogItem(WinPara.hWinMid, GUI_ID_ICONVIEW0),-1);
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}
/*
*********************************************************************************************************
*	函 数 名: _cbMidWin
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbMidWin(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
			switch (Id) 
			{
				case GUI_ID_ICONVIEW0:
					switch (NCode) 
					{
						/* ICON控件点击消息 */
						case WM_NOTIFICATION_CLICKED:
							app_prevent_refresh = 1;
							break;
						
						/* ICON控件释放消息 */
						case WM_NOTIFICATION_RELEASED: 
							
							/* 打开相应选项 */
							switch(ICONVIEW_GetSel(pMsg->hWinSrc))
							{
								
								/* Phone *******************************************************************/
								case 0:	
									OnICON0 = 1;
									//printf("this is the NO.1 ICON\n");
									OnICON0Clicked();			
									break;	
								
								/* Message ***********************************************************************/
								case 1:
									OnICON1 = 1;
									//printf("this is the NO.2 ICON\n");
									OnICON1Clicked();
									break;
								
								/* Note *********************************************************************/
								case 2:
									OnICON2 = 1;
									//printf("this is the NO.3 ICON\n");
									OnICON2Clicked();	
									break;
								
								/* Ethernet *******************************************************************/
								case 3:
									OnICON3 = 1;
									//printf("this is the NO.4 ICON\n");
									OnICON3Clicked();
									break;
								
								/* LED  ******************************************************************/
								case 4:	
									OnICON4 = 1;
									OnICON4Clicked();			
									break;	
								
								/* KEY ***********************************************************************/
								case 5:
									OnICON5 = 1;
									OnICON5Clicked();
									break;
								
								/* SRAM *********************************************************************/
								case 6:
									OnICON6 = 1;
									OnICON6Clicked();	
									break;
								
								/* CAN ********************************************************************/
								case 7:
									OnICON7 = 1;
									OnICON7Clicked();
									break;
								
								/* EEPROM **********************************************************************/
								case 8:
									OnICON8 = 1;
									OnICON8Clicked();
									break;
								
								/* ADC ********************************************************************/
								case 9:
									OnICON9 = 1;
									OnICON9Clicked();
									break;
								
								/* DS18B20 ******************************************************************/
								case 10:					
									OnICON10 = 1;
									OnICON10Clicked();
									break;
								
								/* TEMP *******************************************************************/
								case 11:
									OnICON11 = 1;
									OnICON11Clicked();
									break;
								
								/* Breathing ******************************************************************/
								case 12:
									OnICON12 = 1;
									OnICON12Clicked();
									break;
								 
								 /* Clock ******************************************************************/
								case 13:
									OnICON13 = 1;
									OnICON13Clicked();
									break;
								 
								 /* FM ******************************************************************/
								case 14:
									OnICON14 = 1;
									OnICON14Clicked();
									break;
								 
								/* Music *******************************************************************/
								case 15:
									OnICON15 = 1;
									OnICON15Clicked();
									break;
								/* Music *******************************************************************/
								case 16:
									OnICON16 = 1;
									OnICON16Clicked();
									break;
								/* Music *******************************************************************/
								case 17:
									OnICON17 = 1;
									OnICON17Clicked();
									break;
								/* Music *******************************************************************/
								case 18:
									OnICON18 = 1;
									OnICON18Clicked();
									break;
								}
							 break;
						}
					break;
			}
			break;
			
		/* 重绘消息*/
		case WM_PAINT:
			{
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
			}			
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/**
  * @brief  _cbBotWin,控制栏回调函数
  * @param  none
  * @retval none
  */
static void _cbBotWin(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) 
	{			 		 
		case WM_PAINT:	                             //重绘背景	
			GUI_SetBkColor(GUI_LIGHTCYAN+0x30);
			GUI_Clear();
			GUI_SetFont(&GUI_FontHZ24);//GUI_SetFont(GUI_FONT_32B_ASCII);
			GUI_SetColor(GUI_BLUE);
			GUI_DispStringHCenterAt("淘宝:http://fire-stm32.taobao.com",200,25);	
			GUI_DispStringHCenterAt("论坛:http://www.firebbs.cn",600,25);	
		break;
		default:		
			WM_DefaultProc(pMsg);
		break;		 
	}
}

/**
  * @brief  CreateTopWin，创建顶部的窗口
  * @param  none
  * @retval none
  */
static void CreateTopWin(void)
{
	WM_HWIN hText;
	/* 顶部的 "wildfire"文本 */
	hText = TEXT_CreateEx(0, 5, 120 , 30, WM_HBKWIN, WM_CF_SHOW, GUI_TA_LEFT|TEXT_CF_VCENTER, GUI_ID_TEXT0, "");
	TEXT_SetFont(hText, &GUI_FontHZ24);
	TEXT_SetText(hText,"野火5寸电容屏");
	TEXT_SetTextColor(hText,GUI_BLUE);
	
	/* 状态栏的时间显示文本 */
	hText = TEXT_CreateEx(WinPara.xSizeLCD/2-50,5,100,30,WM_HBKWIN,WM_CF_SHOW,GUI_TA_HCENTER|TEXT_CF_VCENTER,GUI_ID_TEXT1,"11:56:00");
	//TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,GUI_DARKGRAY);
	TEXT_SetFont(hText,GUI_FONT_24B_ASCII);
}
/**
  * @brief  CreateMidWin，创建中间的窗口
  * @param  none
  * @retval none
  */
static void CreateMidWin(void)
{
	uint8_t i=0;
	WM_HWIN MIDWIN;
	WinPara.hWinMid= WM_CreateWindowAsChild(
											WinPara.xPosWin,											
											WinPara.yPosWin,
											WinPara.xSizeWin,
											WinPara.ySizeWin,	
											WM_HBKWIN, 
											WM_CF_SHOW|WM_CF_MEMDEV,
											_cbMidWin, 
											sizeof(WIN_PARA *)
											);
/*-------------------------------------------------------------------------------------*/
					/*在指定位置创建指定尺寸的ICONVIEW1 小工具*/
					MIDWIN=ICONVIEW_CreateEx(
											 ICONVIEW_XPos, 					/* 小工具的最左像素（在父坐标中）*/
											 0, 								/* 小工具的最上像素（在父坐标中）*/
											 WinPara.xSizeLCD-ICONVIEW_XPos,	/* 小工具的水平尺寸（单位：像素）*/
											 3*(WinPara.ySizeBM+35), 			/* 小工具的垂直尺寸（单位：像素）*/
											 WinPara.hWinMid, 				    /* 父窗口的句柄。如果为0 ，则新小工具将成为桌面（顶级窗口）的子窗口 */
											 WM_CF_SHOW|WM_CF_HASTRANS,       /* 窗口创建标记。为使小工具立即可见，通常使用 WM_CF_SHOW */ 
											 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* 默认是0，如果不够现实可设置增减垂直滚动条 */
											 GUI_ID_ICONVIEW0, 			        /* 小工具的窗口ID */
											 ICONVIEW_Width, 				    /* 图标的水平尺寸 */
											 ICONVIEW_Height+20);				/* 图标的垂直尺寸 */
											 
		/* 向ICONVIEW 小工具添加新图标 */
		for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) 
		{
			ICONVIEW_AddBitmapItem(MIDWIN, _aBitmapItem[i].pBitmap,_aBitmapItem[i].pText);
			ICONVIEW_SetTextColor(MIDWIN,i,GUI_DARKGRAY);
		}	
		/* 设置小工具的背景色 32 位颜色值的前8 位可用于alpha混合处理效果*/
		ICONVIEW_SetBkColor(MIDWIN, ICONVIEW_CI_SEL, GUI_WHITE | 0x80000000);
		/* 设置字体 */
		ICONVIEW_SetFont(MIDWIN, GUI_FONT_16B_ASCII);
		/* 设置初始选择的图标为 -1 (表示尚未选择)*/
		ICONVIEW_SetSel(MIDWIN,-1);
		/* 设置图标在x 或y 方向上的间距。*/
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_X, ICONVIEW_XSpace);
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_Y, ICONVIEW_YSpace);
	//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
		/* 设置对齐方式 在5.22版本中最新加入的 */
		ICONVIEW_SetIconAlign(MIDWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
		
/*-------------------------------------------------------------------------------------*/
}
/**
  * @brief  CreateBotWin，创建底部的窗口
  * @param  none
  * @retval none
  */
static void CreateBotWin(void)
{
	WinPara.hWinBot= WM_CreateWindowAsChild(
											0,											
											480-60,	//底部位置
											800,
											60,	//底部剩余宽度
											WM_HBKWIN, 
											WM_CF_SHOW,
											_cbBotWin, 
											0
											);
}
/*
*********************************************************************************************************
*
*       _cbGetData
*
* Function description
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*********************************************************************************************************
*/
//static int _cbGetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
//{
//	SPI_FLASH_BufferRead(pBuffer,Offset,NumBytes);
//	return 0;
//}

//static void _CreateXBF(void) 
//{
//	//
//	// Create XBF font
//	//
//	GUI_XBF_CreateFont(&XBF_Font,       // Pointer to GUI_FONT structure in RAM
//					   &XBF_Data,         // Pointer to GUI_XBF_DATA structure in RAM
//					   GUI_XBF_TYPE_PROP_EXT, 		// Font type to be created
//					   _cbGetData,        // Pointer to callback function
//					   0);        // Pointer to be passed to GetData function
//}
/*
*********************************************************************************************************
*	函 数 名: emWinMainApp
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void emWinMainApp(void)
{
	app_prevent_refresh = 0;
	//准备建立3个窗口，以下是使用到的用户定义参数，方便在回调函数中使用
	WinPara.xSizeLCD = LCD_GetXSize();				//LCD屏幕尺寸
	WinPara.ySizeLCD = LCD_GetYSize();				//LCD屏幕尺寸
	WinPara.xSizeBM  = ICONVIEW_Width;				//图标宽度
	WinPara.ySizeBM  = ICONVIEW_Height;				//图标高度
	WinPara.ySizeBotWin=BOTWIN_YSpace;				//界面下方窗口高度
	WinPara.xPosWin	 = MIDWIN_xPos;							//窗口的起始位置
	WinPara.yPosWin  = MIDWIN_yPos;							//窗口的起始位置
	WinPara.xSizeWin = WinPara.xSizeLCD;						//窗口尺寸
	WinPara.ySizeWin = WinPara.ySizeLCD-WinPara.ySizeBotWin-WinPara.yPosWin;	//窗口尺寸
	
	/***************************设置皮肤色*****************************/
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	/***************************设置默认字体**********************************/
//	_CreateXBF();
//	GUI_UC_SetEncodeUTF8();
//  GUI_SetDefaultFont(&GUI_FontHZ32);
	/*************************************************************************/
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
	WM_CreateTimer(WM_HBKWIN, 0, 1000, 0);
	/* 创建三个窗口 状态栏、控制栏、主窗口*/
	CreateMidWin();
	CreateTopWin();	
	CreateBotWin();
	//WM_InvalidateWindow(WM_HBKWIN);
	while(1)
	{
		GUI_Delay(5);
	}
}
/*************************** End of file ****************************/
