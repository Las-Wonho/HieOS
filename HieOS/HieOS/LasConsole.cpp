#include"LasConsole.h"
namespace LasConsole {
	static ConsoleColor _Color;
	static ConsoleColor _Text;
	static ConsoleColor _backGroundColor;

	static uint _xPos;
	static uint _yPos;

	static ushort* _pVideoMemory; //Just a pointer to video memory
	static unsigned int _ScreenHeight;
	static unsigned int _ScreenWidth;
	static unsigned short _VideoCardType;
}
void LasConsole::init() {
	//컬러인지 흑백인지 선택
	char c = (*(unsigned short*)0x410 & 0x30);

	if (c == 0x30)
	{
		_pVideoMemory = (unsigned short*)0xb0000;
		_VideoCardType = VGA_MONO_CRT_ADDRESS;	// 흑백
	}
	else
	{
		_pVideoMemory = (unsigned short*)0xb8000;
		_VideoCardType = VGA_COLOR_CRT_ADDRESS;	// 컬러
	}

	_xPos = 0;
	_yPos = 0;

	_ScreenHeight = 30;
	_ScreenWidth = 80;

	_backGroundColor = ConsoleColor::White;
	_Text = ConsoleColor::Black;
	_Color = ConsoleColor::Cyan;
}

void LasConsole::move_cursor(unsigned int x, unsigned int y) {

	unsigned short Offset = (unsigned short)((y*_ScreenWidth) + (x - 1));

	OutPort(_VideoCardType, VGA_CRT_CURSOR_H_LOCATION);
	OutPort(_VideoCardType + 1, Offset >> 8);
	OutPort(_VideoCardType, VGA_CRT_CURSOR_L_LOCATION);
	OutPort(_VideoCardType + 1, (Offset << 8) >> 8);
}