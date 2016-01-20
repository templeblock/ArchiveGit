/* led.h */
#ifndef _led_h
#define _led_h

typedef struct LEDchar 
{
  char Code ;
  char BitCode[7];
}LEDCHAR;


typedef struct LEDdisplay 
{
	short nLines;
	short nCharsPerLine; 
	short AspectRatio;
	short DisplayWidth;
	short DisplayHeight;
	short Red;
	short Green;
	short Blue;
	char **LineStrings;
}LEDdisplay;

typedef enum BarEncoding
{
	BELinear = 0,
	BELog
}BarEncoding;

int DefineLedDisplay(HWND hwnd, short x, short y, LEDdisplay *whichDisplay);
int RefreshLedDisplay(HWND hwnd, LEDdisplay *whichDisplay);
//int RefreshLedDisplayLine(HWND hwnd, LEDdisplay *whichDisplay,short whichLine);
int DrawLedChar(HDC hdc, short x, short y, short width, short height, char WhichChar,short red,short green, short blue);
 void DrawRGB_BarGraph(	HWND hwnd, LEDdisplay *whichDisplay, double Red, double Green, double Blue,BarEncoding whichCode);

#endif // _led_h