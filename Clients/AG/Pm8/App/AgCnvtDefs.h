//defs for ag conversion
#include "wmf.h"

CRect rcHalfHFFront(1273, 12522, 11272, 18772);
CRect rcHalfHFIFront(13772, 3772, 23772, 10022);
CRect rcHalfHFIBack(13772, 12522, 23772, 18772);
CRect rcHalfHFBack(1273, 3772, 11272, 10022);

CRect rcHalfVFFront(6919, 2609, 11228, 9505);
CRect rcHalfVFIFront(12952, 2609, 17262, 9505);
CRect rcHalfVFIBack(18986, 2609, 23296, 9505);
CRect rcHalfVFBack(885, 2609, 5195, 9505);

CRect rcQuarterVFFront(1158, 3430, 5418, 9110);
CRect rcQuarterVFIFront(6838, 3430, 11098, 9110);
CRect rcQuarterVFIBack(12517, 3430, 16777, 9110);
CRect rcQuarterVFBack(18197, 3430, 22457, 9110);

CRect rcQuarterHFFront(1492, 4431, 8841, 9944);
CRect rcQuarterHFIFront(11781, 4431, 19131, 9944);
CRect rcQuarterHFIBack(11781, 13251, 19131, 18764);
CRect rcQuarterHFBack(1492, 13251, 8841, 18764);

CRect rcPosterH(21,21,25015, 19336);
CRect rcPosterV(24,24,19339, 25027);

CRect rcPostCardHF(1492,4432,10311, 10312);
CRect rcPostCardHB(11781,4432,20601, 10312);

CRect rcPostCardVF(1492,4432,10311, 10312);
CRect rcPostCardVB(11781,4432,20601, 10312);

CRect rcSquareSticker(0,0, 25019, 25019);
CRect rcRectStickerH(0,0, 25240, 13269);
CRect rcRectStickerV(0,0, 13334, 25212);
CRect rcAddressLbl(127,110, 24962, 8313);

CString strIniPath = "f:\\agsrc\\pmw\\windebug\\FontData.ini";
static double sdDPI;

#pragma pack(1)
struct recordMetaSTRETCHDIB
{
	DWORD dwSize;
	WORD wFunction;
	DWORD dwRop;
	WORD wUsage;
	WORD wSrcYExt;
	WORD wSrcXExt;
	WORD wSrcY;
	WORD wSrcX;
	WORD wDstYExt;
	WORD wDstXExt;
	WORD wDstY;
	WORD wDstX;
	BITMAPINFO BitmapInfo;
	BYTE Bits[1];
};

struct WordPoint
{
	WORD x;
	WORD y;
};

struct recordMetaPOLYPOLYGON
{
	DWORD dwSize;
	WORD wFunction;
	WORD nCount;
	WordPoint Point[1];
};

struct recordMetaPOLYGON
{
	DWORD dwSize;
	WORD wFunction;
	WORD nCount;
	WordPoint Point[1];
};

struct recordMetaCREATEFONTINDIRECT
{
	DWORD dwSize;
	WORD wFunction;
	LOGFONT16 lfFont;
};

struct recordMetaCREATEBRUSHINDIRECT
{
	DWORD dwSize;
	WORD wFunction;
	WORD nStyle;
	COLORREF crColor;
	WORD nHatch;
};

struct recordMetaCREATEPENINDIRECT
{
	DWORD dwSize;
	WORD wFunction;
	WORD nStyle;
	WORD nX;
	WORD nY;
	COLORREF crColor;
};

struct recordMetaTEXTOUT
{
	DWORD dwSize;
	WORD wFunction;
	WORD wCount;
	BYTE String[1];
};

struct recordMetaFONTCOLOR
{
	DWORD dwSize;
	WORD wFunction;
	COLORREF crFontColor;
};

struct recordMetaTEXTALIGN
{
	DWORD dwSize;
	WORD wFunction;
	WORD nVertFlags;
	WORD nHorizFlags;
};

struct recordMetaSETWINDOWEXT
{
	DWORD dwSize;
	WORD wFunction;
	WORD nY;
	WORD nX;
};

#pragma pack()

