// 新宋体　16x16
// 本字库文件由“uC/GUI汉字库生成器 V1.0”创建
// 作者：黄培雄  E-mail: dialsun@sohu.com

#include "GUI.H"

#ifndef GUI_FLASH
	#define GUI_FLASH
#endif

extern GUI_FLASH const GUI_FONT GUI_FontHZ24;


GUI_FLASH const GUI_CHARINFO GUI_Font24_ASCII_CharInfo[] =
{
    { 12, 12, 2, (void GUI_FLASH *)GUI_FontHZ24_Flash_BaseAddr}
};

GUI_FLASH const GUI_CHARINFO GUI_FontHZ_24_[] =
{
    { 24, 24, 3, (void GUI_FLASH *)GUI_FontHZ24_Flash_BaseAddr},//汉字在FLASH中的偏移量
};

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ_24_Prop_a1 =
{
	0xa1a1,
	0xfefe,//0xf7fe,//
	&GUI_FontHZ_24_[0],
	(void *)0
};

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ_24_Prop_ASC =
{
        0x20,
        0x7F,
        &GUI_Font24_ASCII_CharInfo[0], // address of first character
        (void *)&GUI_FontHZ_24_Prop_a1 // pointer to next GUI_FONT_PROP
};

GUI_FLASH const GUI_FONT GUI_FontHZ24 =
{
	GUI_FONTTYPE_PROP_X, // type of font
	24, // height of font
	24, // space of font y
	1,  // magnification x
	1,  // magnification y
	(void GUI_FLASH *)&GUI_FontHZ_24_Prop_ASC
};



