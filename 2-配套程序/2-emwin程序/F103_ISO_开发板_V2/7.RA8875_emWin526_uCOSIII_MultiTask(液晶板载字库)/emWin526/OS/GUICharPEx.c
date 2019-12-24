/*-------------------------------------------------------------------------

                           外部字库 程序

                              
-------------------------------------------------------------------------*/




#include <string.h>

#include <stddef.h>           /* needed for definition of NULL */
#include "GUI_Private.h"
#include "bsp_ra8875_lcd.h"
#include "GUI.h"
#include "bsp_usart1.h"


void GUIPROP_X_DispChar(U16P c) 
{

	  GUI_COLOR GUI_Color;
	  GUI_COLOR GUI_BkColor;
	  u8 Font;
	  //int OldTextMode;
	
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode;
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp;

    //搜索定位字库数据信息 
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    if (pProp)
    {
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        
        OldDrawMode = LCD_SetDrawMode(DrawMode);
		OldDrawMode = LCD_SetDrawMode(DrawMode);
        /* Fill empty pixel lines */	
        if (GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize) 
		{
					
          int YMag = GUI_pContext->pAFont->YMag;
          int YDist = GUI_pContext->pAFont->YDist * YMag;
          int YSize = GUI_pContext->pAFont->YSize * YMag;
					
          if (DrawMode != LCD_DRAWMODE_TRANS) 
		  {
            LCD_COLOR OldColor = GUI_GetColor();
            GUI_SetColor(GUI_GetBkColor());
            LCD_FillRect(GUI_pContext->DispPosX, 
                         GUI_pContext->DispPosY + YSize, 
                         GUI_pContext->DispPosX + pCharInfo->XSize, 
                         GUI_pContext->DispPosY + YDist);
            GUI_SetColor(OldColor);
          }
        }	
		
		if(OldDrawMode==GUI_DRAWMODE_NORMAL) RA8875_SetTextMode(0);//不穿透
		if(OldDrawMode==GUI_DRAWMODE_TRANS) RA8875_SetTextMode(1);//穿透
		
		if(GUI_pContext->DispPosX+pCharInfo->XDist>800)
		{
			GUI_pContext->DispPosX=0;
			GUI_pContext->DispPosY+=GUI_pContext->pAFont->YDist;
		}
		
		RA8875_SetTextCursor(GUI_pContext->DispPosX,GUI_pContext->DispPosY);
		
		//颜色数据转换
		GUI_Color=GUI_GetColor();
		GUI_BkColor=GUI_GetBkColor();
								

		if(GUI_pContext->pAFont==&GUI_FontHZ16)
		{
			Font=0;
		}			
		else if(GUI_pContext->pAFont==&GUI_FontHZ24)
		{
			Font=1;
		}			
		else if(GUI_pContext->pAFont==&GUI_FontHZ32)
		{
			Font=2;
		}			

		RA8875_SetFrontColor(GUIColorToRGB(GUI_GetColor()));
		RA8875_SetBackColor(GUIColorToRGB(GUI_GetBkColor()));
		RA8875_SetFont(Font,GUI_pContext->pAFont->XMag,GUI_pContext->pAFont->YMag);
		RA8875_DispChar(c);	
//			BTE_SetSourse(GUI_pContext->DispPosX,GUI_pContext->DispPosX,0);
//			BTE_SetDist(GUI_pContext->DispPosX,GUI_pContext->DispPosX,0);
//			BTE_SetArea(Width+GUI_pContext->pAFont->XMag,Width+GUI_pContext->pAFont->YMag);
//			//RA8875_SetFrontColor(GUIColorToRGB(GUI_CYAN)); //取的颜色  
//			RA8875_WriteReg(0x51,0x82);
//			BTE_Start();
//			BTE_Wait();	

		//恢复颜色
		GUI_SetBkColor(GUI_BkColor);
		GUI_SetColor(GUI_Color);
        GUI_pContext->DispPosX += (pCharInfo->XDist * GUI_pContext->pAFont->XMag); //X轴计算
		//printf("pCharInfo->XDist=%d,GUI_pContext->DispPosX=%d\n",pCharInfo->XDist,GUI_pContext->DispPosX);
    }		
}
/*********************************************************************

*

*       GUIPROP_GetCharDistX

*/

int GUIPROP_X_GetCharDistX(U16P c) 
{
    const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_pContext->pAFont->p.pProp;
    
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    return (pProp) ? (pProp->paCharInfo)->XSize * GUI_pContext->pAFont->XMag : 0;
}

