#include "includes.h"
#include  "app.h"
#include "sim900a.h"
#include "bsp_usart2.h"
/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/ 
#define ID_BUTTON              (GUI_ID_USER + 0)

#define BUTTON_WHITH   100
#define BUTTON_HIGHT   65
#define BUTTON_SpaceX  (10+BUTTON_WHITH) 
#define BUTTON_SpaceY  (10+BUTTON_HIGHT)
#define TEXTLENGTH   	25

/*
*********************************************************************************************************
*                                      变量
*********************************************************************************************************
*/ 
extern GUI_CONST_STORAGE GUI_BITMAP bmcall;
extern GUI_CONST_STORAGE GUI_BITMAP bmdelete;

static WM_HWIN hKey,hBack,hText;

static uint8_t 		s_Key;
static char 		text[TEXTLENGTH]={"112"};
static uint8_t 		IsCall=0;//1:calling  0:waiting
static uint8_t		textoff=0;

typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;   /* 按钮对应的字符 */
} BUTTON;

static const BUTTON ButtonData[] = 
{

	/* 第1排按钮 */
	{ 5,                   5, 				 	BUTTON_WHITH, 		BUTTON_HIGHT, "1"  },
	{ 5+BUTTON_SpaceX,     5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "2"  },
	{ 5+BUTTON_SpaceX*2,   5,  					BUTTON_WHITH, 		BUTTON_HIGHT, "3"  },
	/* 第2排按钮 */
	{ 5,   				   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "4"  },	
	{ 5+BUTTON_SpaceX,     5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "5"  },
	{ 5+BUTTON_SpaceX*2,   5+BUTTON_SpaceY,  	BUTTON_WHITH, 		BUTTON_HIGHT, "6"  },
	/* 第3排按钮 */
	{ 5,   				   5+BUTTON_SpaceY*2,  	BUTTON_WHITH, 		BUTTON_HIGHT, "7"  },
	{ 5+BUTTON_SpaceX,     5+BUTTON_SpaceY*2,  	BUTTON_WHITH, 		BUTTON_HIGHT, "8"  },	
	{ 5+BUTTON_SpaceX*2,   5+BUTTON_SpaceY*2,  	BUTTON_WHITH, 		BUTTON_HIGHT, "9"  },
	/* 第4排按钮 */
	{ 5,     			   5+BUTTON_SpaceY*3,  	BUTTON_WHITH, 		BUTTON_HIGHT, "*"  },
	{ 5+BUTTON_SpaceX,     5+BUTTON_SpaceY*3,  	BUTTON_WHITH, 		BUTTON_HIGHT, "0"  },
	{ 5+BUTTON_SpaceX*2,   5+BUTTON_SpaceY*3,  	BUTTON_WHITH, 		BUTTON_HIGHT, "#"  }
};

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreatePhone[] = {
	{ FRAMEWIN_CreateIndirect, "Phone",0, 0, 0, 800, 480, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "number", GUI_ID_TEXT0, 250, 5, 300, 50, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "calling", GUI_ID_TEXT1, 300, 60, 200, 40, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "back", GUI_ID_BUTTON0, 550, 15, 24, 24, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "call", GUI_ID_BUTTON1, 373,380, 48, 48, 0, 0x0, 0 },
	// USER START (Optionally insert additional widgets)
	// USER END
};
/*
*********************************************************************************************************
*	函 数 名: _cbKeyPad
*	功能说明: 回调函数
*	形    参：pMsg  指针参数
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbDialogPhone(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int        Id;
	int        NCode;
	switch (pMsg->MsgId) 
	{
		case WM_DELETE:
		OS_DEBUG("Phonerapp delete\n");
		OnICON10 = 0;
		textoff=0;
		text[0]='\0';
		IsCall=0;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);	
		USART_Cmd(USART2, DISABLE);
		app_prevent_refresh = 0;
		break;
		case WM_INIT_DIALOG:
			//
			// Initialization of 'Phone'
			//
			hItem = pMsg->hWin;
			FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
			FRAMEWIN_SetFont(hItem, GUI_FONT_32B_ASCII);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
			FRAMEWIN_SetTitleHeight(hItem, 40);
			//
			// Initialization of 'number'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
			hText=hItem;
			TEXT_SetTextColor(hItem, GUI_BLACK);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
			TEXT_SetText(hItem, "112");
			//
			// Initialization of 'calling'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
			TEXT_SetTextColor(hItem, GUI_BLACK);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hItem, &GUI_FontHZ32);
			TEXT_SetText(hItem, "正在呼叫...");
			WM_HideWindow(hItem);
			//
			// Initialization of 'back'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
			hBack=hItem;
			BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmdelete,0,0);			
			BUTTON_SetText(hItem,"");
			//WM_HideWindow(hBack);
			//
			// Initialization of 'call'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
			BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmcall,0,0);
			BUTTON_SetText(hItem,"");
			WM_BringToTop(hItem);
			// USER START (Optionally insert additional code for further widget initialization)
			// USER END
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) {
				case GUI_ID_BUTTON0: // Notifications sent by 'back'
				  switch(NCode) {
					  case WM_NOTIFICATION_CLICKED:
						// USER START (Optionally insert code for reacting on notification message)
						textoff--;
						if(textoff==255)
						{
							textoff=0;
						}
						// USER END
						break;
					  case WM_NOTIFICATION_RELEASED:
						// USER START (Optionally insert code for reacting on notification message)
						text[textoff]='\0';
						TEXT_SetText(hText,text);
						//printf("text:%s\n",text);
						if(textoff==0)WM_HideWindow(hBack);
						// USER END
						break;
					  // USER START (Optionally insert additional code for further notification handling)
					  // USER END
				  }
				  break;
				case GUI_ID_BUTTON1: // Notifications sent by 'call'
				  switch(NCode) {
					  case WM_NOTIFICATION_CLICKED:
						// USER START (Optionally insert code for reacting on notification message)
						IsCall=1-IsCall;
						if(IsCall)
						{
							WM_HideWindow(hKey);
							WM_HideWindow(hBack);
							WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1));
						}
						else
						{
							WM_ShowWindow(hKey);
							//WM_ShowWindow(hBack);
							WM_HideWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1));
							textoff=0;
							text[0]='\0';
							TEXT_SetText(hText,text);
						}
						// USER END
						break;
					  case WM_NOTIFICATION_RELEASED:
						// USER START (Optionally insert code for reacting on notification message)
						if(IsCall)
						{
							//拨打电话号码
							sim900a_call(text);
						}
						else
						{
							/*挂电话*/
							SIM900A_HANGOFF();	
						}
						// USER END
						break;
					  // USER START (Optionally insert additional code for further notification handling)
					  // USER END
				  }
				  break;
				  // USER START (Optionally insert additional code for further Ids)
				// USER END
			}
			break;
		/* 绘制背景 */	
		case WM_PAINT:	
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			break;
		default:
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _cbKey(WM_MESSAGE * pMsg)
{
	WM_HWIN hButton;
	uint8_t i;
	int        Id;
	int        NCode;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			// USER START (Optionally insert additional code for further widget initialization)
			/* 创建所需的按钮 */
			for (i = 0; i < GUI_COUNTOF(ButtonData); i++) 
			{
				hButton = BUTTON_CreateEx(ButtonData[i].xPos, ButtonData[i].yPos, ButtonData[i].xSize, ButtonData[i].ySize, 
				WM_GetClientWindow(pMsg->hWin), WM_CF_SHOW, 0, ID_BUTTON + i);
				BUTTON_SetFont(hButton, &GUI_Font32B_ASCII);
				BUTTON_SetText(hButton, ButtonData[i].acLabel);
				BUTTON_SetTextAlign(hButton,GUI_TA_HCENTER|GUI_TA_VCENTER);
				BUTTON_SetFocussable(hButton, 0);
			}
			// USER END
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) 
			{
				case WM_NOTIFICATION_CLICKED:
					// USER START (Optionally insert code for reacting on notification message)
					LED1_ON;
					WM_ShowWindow(hBack);
					// USER END
				break;
				case WM_NOTIFICATION_RELEASED:					
					// USER START (Optionally insert code for reacting on notification message)
					LED1_OFF;
					s_Key = ButtonData[Id - ID_BUTTON].acLabel[0];
					text[textoff]=s_Key;
					text[textoff+1]='\0';
					TEXT_SetText(hText,text);
					//printf("text:%s\n",text);
					textoff++;
					// USER END
				break;
			    // USER START (Optionally insert additional code for further notification handling)
			    // USER END
			}
			break;
		/* 绘制背景 */	
		case WM_PAINT:	
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			break;
		default:
		WM_DefaultProc(pMsg);
		break;
	}
}
/*
*********************************************************************************************************
*	函 数 名: MainAPP
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void OnICON10Clicked(void)
{
	WM_HWIN hWin;
	OS_DEBUG("Phoneapp create\n");
	hWin=GUI_CreateDialogBox(_aDialogCreatePhone, GUI_COUNTOF(_aDialogCreatePhone), _cbDialogPhone, WM_HBKWIN, 0, 0);
	hKey=WM_CreateWindowAsChild(230,100,330,300,hWin,WM_CF_SHOW,_cbKey,0);
	WM_BringToTop(hKey);
	
	/* 初始化并检测模块 */
	if (sim900a_init()!= 0)
	{
		GUI_MessageBox("\r\n No detected SIM900A module! \r\n","error",GUI_MESSAGEBOX_CF_MOVEABLE);
	}
	
	while(OnICON10)
	{
		GUI_Delay(10);
	}
}
