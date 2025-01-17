/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.22                          *
*        Compiled Jul  4 2013, 15:16:01                              *
*        (c) 2013 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "includes.h"
#include  "app.h"
#include "bsp_fsmc_sram.h"
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

// USER START (Optionally insert additional defines)
#define BUFFER_SIZE        0x200/2      /* 512个字节 */
#define WRITE_READ_ADDR    0x80000
#define WRITE_NUM		   0x00
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
extern GUI_FONT     XBF_Font;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSRAM[] = {
  { FRAMEWIN_CreateIndirect, "SRAM", 0, 0, 0, 800, 480, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "read", GUI_ID_TEXT6, 30, 220, 220, 40, 0, 0x64, 0 },
  { MULTIEDIT_CreateIndirect, "read", GUI_ID_MULTIEDIT0, 280, 30, 480, 400, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
static void SRAM_WRITE(uint16_t number,uint32_t startaddr,uint16_t length)
{
	for(;length!=0;length--)
	{
		*(uint16_t *) (Bank1_SRAM3_ADDR + startaddr) = number++;
		startaddr+=2;
	}
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogSRAM(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
	  case WM_DELETE:
		    OS_DEBUG("SRAMapp delete");			
		    OnICON2 = 0;
			app_prevent_refresh = 0;
		    break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'SRAM TEST'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTitleHeight(hItem, 40);
	FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
    FRAMEWIN_SetFont(hItem, GUI_FONT_24B_ASCII);
	FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);

    //
    // Initialization of 'read'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT6);
    TEXT_SetText(hItem, "测试中");
    TEXT_SetFont(hItem, &GUI_FontHZ32);
    TEXT_SetTextColor(hItem, 0x000000FF);
	TEXT_SetTextAlign(hItem,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
  
	//
    // Initialization of 'edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0);
	MULTIEDIT_SetFont(hItem,GUI_FONT_8X18);
	MULTIEDIT_EnableBlink(hItem,500,1);
	MULTIEDIT_SetAutoScrollV(hItem,1);
	MULTIEDIT_SetInsertMode(hItem,1);
	MULTIEDIT_SetWrapWord(hItem);
	MULTIEDIT_SetBkColor(hItem,MULTIEDIT_CI_EDIT,GUI_LIGHTYELLOW);
    // USER START (Optionally insert additional code for further widget initialization)
	//FSMC_SRAM_Init();
    // USER END
    break;
  // USER START (Optionally insert additional message handling)
  case WM_PAINT:	
		GUI_SetBkColor(APPBKCOLOR);
		GUI_SetColor(APPTEXTCOLOR);
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ16);
		GUI_DispStringHCenterAt("外部SRAM读写测试",130,15);
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringHCenterAt("连续向外部SRAM起始地址",130,55);
		GUI_DispStringHCenterAt("为：0x68080000写入256",130,80);
		GUI_DispStringHCenterAt("个以0x00开始自加一的数据",130,105);  
		GUI_DispStringHCenterAt("然后将这些数据读出并比较",130,135); 
  
		GUI_DispStringHCenterAt("读出的数据",500,10);
		break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateSRAM TEST
*/
void OnICON2Clicked(void)
{
	WM_HWIN hWin,hItem;
	uint8_t readbuffer[6]={0};
	uint16_t i=65000,sramflag=0;
	uint16_t RxBuffer[BUFFER_SIZE]={0};
	hWin=GUI_CreateDialogBox(_aDialogCreateSRAM, GUI_COUNTOF(_aDialogCreateSRAM), _cbDialogSRAM, WM_HBKWIN, 0, 0);
	
	GUI_Delay(100);
	OS_DEBUG("SRAMapp create");
	hItem = WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0);
	
	SRAM_WRITE(WRITE_NUM,WRITE_READ_ADDR,BUFFER_SIZE);
	while(i--); //短延时
	 /* 将刚刚写到SRAM里面的数据读出来 */
	FSMC_SRAM_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
	for(i=0;i<BUFFER_SIZE;i++)
	{
		if(RxBuffer[i]!=(WRITE_NUM+i))
		{
			sramflag++;
		}
		sprintf((char *)readbuffer,"0x%02X ",RxBuffer[i]);
		readbuffer[5]='\0';
		MULTIEDIT_AddText(hItem,(const char *)readbuffer);
	}
	if(!sramflag)
		TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6),"SRAM测试通过");
	else
		TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6),"SRAM测试失败");
	while(OnICON2)
	{					 				
		GUI_Delay(10); 				
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
