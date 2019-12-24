// 新宋体　16x16
// 本字库文件由“uC/GUI汉字库生成器 V1.0”创建
// 作者：黄培雄  E-mail: dialsun@sohu.com

#include "GUI.H"

#ifndef GUI_FLASH
	#define GUI_FLASH
#endif

extern GUI_FLASH const GUI_FONT GUI_FontHZ16;



static GUI_FLASH const GUI_CHARINFO GUI_FontHZ_16_CharInfo[] =
{
	{ 8, 8, 1, (void GUI_FLASH *)GUI_FontHZ16_Flash_BaseAddr},
};


GUI_FLASH const GUI_CHARINFO GUI_FontHZ_16_[] =
{
    { 16, 16, 2, (void GUI_FLASH *)GUI_FontHZ16_Flash_BaseAddr},//汉字在FLASH中的偏移量
};


GUI_FLASH const GUI_FONT_PROP GUI_FontHZ_16_Prop_a1 =
{
	0xa1a1,
	0xfefe,//0xf7fe,//
	&GUI_FontHZ_16_[0],
	(void *)0
};


GUI_FLASH const GUI_FONT_PROP GUI_FontHZ_16_Prop_ASC =
{
	0x20,
	0x7F,
	&GUI_FontHZ_16_CharInfo[0], // address of first character
	(void *)&GUI_FontHZ_16_Prop_a1 // pointer to next GUI_FONT_PROP
};


GUI_FLASH const GUI_FONT GUI_FontHZ16 =
{
	GUI_FONTTYPE_PROP_X, // type of font
	16, // height of font
	16, // space of font y
	1,  // magnification x
                                                                	1,  // magnification y
	(void GUI_FLASH *)&GUI_FontHZ_16_Prop_ASC
};



