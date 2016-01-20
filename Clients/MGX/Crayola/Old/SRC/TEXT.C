//®PL1¯®FD1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

static HFONT TextNewFont(int, int, BYTE, LFIXED, BOOL);
static BOOL TextAddChar(WORD nVKey);
static BOOL TextCheckBufSize(WORD wBytes);
static BOOL TextAddByte(BYTE bValue);
static BYTE TextExtractStyle(void);
static void TextDeleteChar(BOOL bLast);
static void TextRedrawProc(HDC hDC, LPRECT lpRect );
static void TextDisplayLine(HWND hWnd, HDC hDC, BOOL on, BYTE bSingleChar, BOOL fDisplay, BOOL fExtentsOnly, LFIXED DispRate, BOOL fAllowScaling);
static void TextDisplayChar(HDC hDC, char bChar, LPTEXTMETRIC lpTM,
			LPTFORM lpTForm, BOOL fExtentsOnly, BOOL fDisplay);
static int TextGetAngle(int nFont);
static BOOL TextApply(HWND hWnd);
static LPFRAME TextGetBitmapBits(HDC hDC, HBITMAP hBitmap, int iWidth, int iHeight);
static BOOL TextFill(LPFRAME lpBits, LPRECT lpRect );
static BOOL TextCreateObject(LPFRAME lpBits, LPRECT lpRect );
static void TextDataProc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );
static void TextMaskProc(int y, int left, int right, LPTR lpMsk );
static LPFRAME TextAntiAlias(LPFRAME lpBits, int iRasterWidth,
			int iRasterHeight, int iWidth, int iHeight);
static BOOL TextCheckRotate(BOOL fCanRotate, int nFont);
static void SetBackSlashBits(LPTR lp, int w, int h, WORD wBytes);
static void SetSlashBits(LPTR lp, int w, int h, WORD wBytes);
static void SetBit(LPTR lp, int x);

#define TEXT_BUF_SIZE 1024

#define TEXT_ITALIC	0x01
#define TEXT_UNDERLINE  0x02
#define TEXT_STRIKEOUT	0x04
#define TEXT_BOLD	0x08

#define TEXT_FONT	0x80
#define TEXT_SIZE	0x81
#define TEXT_STYLE	0x83

#define ANTI_ALIAS_FACTOR 2

#define OPACITY Text.TextOpacity // Was Text.Opacity
#define MERGEMODE Text.TextMergeMode // Was Text.Opacity

static LPTR lpTextBuf;
static LPTR lpTextEnd;
static LPTR lpText;
static int nCharsDrawn;
static long lBufSize;
static POINT StartPos;
static POINT CurrentPos;
static RECT rExtent;
static COLORINFO TextColor;
static TFORM tform;
static BOOL fNoAdd;
static BOOL fCanRotate;
static int iWidth;
static long lRet;
static HBITMAP hbm;
static LPDISPLAYHOOK lpLastDisplayHook;

/************************************************************************/
BOOL TextBegin(HWND hWnd, int x, int y)
/************************************************************************/
{
HDC hDC;
int caps;

StartPos.x = x; StartPos.y = y;
Display2File((LPINT)&StartPos.x, (LPINT)&StartPos.y);

lBufSize = TEXT_BUF_SIZE;
if (!(lpTextBuf = Alloc(lBufSize)))
	{
	TextEnd(hWnd, YES);
	return(FALSE);
	}
lpTextEnd = lpTextBuf + lBufSize;
lpText = lpTextBuf;

hDC = GetDC(hWnd);
caps = GetDeviceCaps(hDC, TEXTCAPS);
ReleaseDC(hWnd, hDC);
fCanRotate = (caps & TC_CR_ANY) ? TRUE : FALSE;
lpLastDisplayHook = SetDisplayHook(hWnd, TextRedrawProc);

GetActiveColorFromType(FrameType(ImgGetBaseEditFrame(lpImage)), &TextColor);
TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);

return(TRUE);
}

/************************************************************************/
void TextEnd(HWND hWnd, BOOL fCancel)
/************************************************************************/
{
TextDisplayLine(hWnd, NULL, OFF, 0, YES, NO, 0, YES);
TextDisplayCaret(NULL, NULL);
SetDisplayHook(hWnd, lpLastDisplayHook);
lpLastDisplayHook = NULL;
if (!fCancel && lpText != lpTextBuf)
	{
	AstralCursor(IDC_WAIT);
	if (!TextApply(hWnd))
		Message(IDS_EMEMALLOC);
	AstralCursor(NULL);
	}
if (lpTextBuf)
	{
	FreeUp(lpTextBuf);
	lpTextBuf = NULL;
	}
}

/************************************************************************/
void TextAnchor(HWND hWnd)
/************************************************************************/
{
POINT SavePos;

if (lpText != lpTextBuf)
	{
	SavePos = CurrentPos;
	TextApply(hWnd);
	StartPos = SavePos;
	Display2File((LPINT)&StartPos.x, (LPINT)&StartPos.y);
	lpText = lpTextBuf;
	GetActiveColorFromType(FrameType(ImgGetBaseEditFrame(lpImage)), &TextColor);
	TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);
	}
}

/************************************************************************/
void TextMove(HWND hWnd, int x, int y)
/************************************************************************/
{
TextDisplayLine(hWnd, NULL, OFF, 0, YES, NO, 0, YES);
StartPos.x = x; StartPos.y = y;
Display2File((LPINT)&StartPos.x, (LPINT)&StartPos.y);
TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);
}

/************************************************************************/
void TextOffset(HWND hWnd, int x, int y)
/************************************************************************/
{
RECT ClientRect;

TextDisplayLine(hWnd, NULL, OFF, 0, YES, NO, 0, YES);
GetClientRect( hWnd, &ClientRect );
StartPos.x += x; StartPos.y += y;
if ( x <= ClientRect.left )		x = ClientRect.left + 1;
if ( x >= ClientRect.right )	x = ClientRect.right - 1;
if ( y <= ClientRect.top )		y = ClientRect.top + 1;
if ( y >= ClientRect.bottom )	y = ClientRect.bottom - 1;
Display2File((LPINT)&StartPos.x, (LPINT)&StartPos.y);
TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);
}

/************************************************************************/
void TextRMove(HWND hWnd, int dx, int dy)
/************************************************************************/
{
int	x, y;

x = bound(StartPos.x + dx, 0, lpImage->npix-1);
y = bound(StartPos.y + dy, 0, lpImage->nlin-1);
if (x == StartPos.x && y == StartPos.y)
	MessageBeep(0);
else
	{
	TextDisplayLine(hWnd, NULL, OFF, 0, YES, NO, 0, YES);
	StartPos.x = x; StartPos.y = y;
	TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);
	}
}

/************************************************************************/
void TextKeystroke(HWND hWnd, WORD nVKey)
/************************************************************************/
{
if (nVKey == VK_ESCAPE)
	{
	TextDeleteAllChars(hWnd);
	return;
	}

if ( nVKey == VK_BACK )
	{
	TextDisplayLine(hWnd, NULL, OFF, 0, YES, NO, 0, YES);
	TextDeleteChar(YES);
	TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);
	return;
	}

if (fNoAdd)
	{
	MessageBeep(0);
	return;
	}

if (nVKey == VK_TAB)
	nVKey = VK_SPACE;

if (TextAddChar(nVKey))
	TextDisplayLine(hWnd, NULL, ON, *(lpText-1), YES, NO, 0, YES);
else
	Message(IDS_EMEMALLOC);
}

/************************************************************************/
void TextFontChanged(HWND hWnd)
/************************************************************************/
{
TextDisplayLine(hWnd, NULL, OFF, 0, YES, NO, 0, YES);
TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);
}

/************************************************************************/
void TextDisplayCaret(HWND hWnd, HDC hInDC)
/************************************************************************/
{
HDC	hDC;
TEXTMETRIC tm;
HFONT	hFont, hOldFont;
int	cx, cy, iAngle;
LPTR	lpBits;
WORD    wBytes;
int	w, h;
BOOL	fSlash, fBackSlash;

DestroyCaret();
if (hbm)
	{
	DeleteObject(hbm);
	hbm = NULL;
	}
if (!hWnd)
	return;

if (!(hDC = hInDC))
	hDC = GetDC(hWnd);
if (!hDC)
	return;

hFont = TextNewFont(Text.nFont, Text.Size, TextExtractStyle(),
		lpImage->lpDisplay->DispRate, YES);
if (hFont)
	hOldFont = (HFONT)SelectObject(hDC, hFont);

GetTextMetrics(hDC, &tm);
Transformer(&tform, &CurrentPos, &cx, &cy);
Display2File(&cx, &cy);
fNoAdd = NO;
File2Display(&cx, &cy);

iAngle = TextGetAngle(Text.nFont);
while ( iAngle < 0 ) iAngle += 360;
while ( iAngle > 360 ) iAngle -= 360;

fSlash = fBackSlash = NO;
if (iAngle > 337 || iAngle <= 22)
	{ // Vertical
	w = 2;
	h = tm.tmHeight;
	cy = cy-tm.tmAscent+1;
	}
else if (iAngle > 22 && iAngle <= 67)
	{ // slash
	w = h = lsqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
	cy = cy-h+1;
	fSlash = YES;
	}
else if (iAngle > 67 && iAngle <= 112)
	{ // Horizontal
	w = tm.tmHeight;
	h = 2;
	cx = cx-tm.tmDescent+1;
	}
else if (iAngle > 112 && iAngle <= 157)
	{ // backslash
	w = h = lsqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
	cx = cx-tm.tmDescent+1;
	fBackSlash = YES;
	}
else if (iAngle > 157 && iAngle <= 202)
	{ // Vertical flipped
	w = 2;
	h = tm.tmHeight;
	cx = cx - 1;
	cy = cy-tm.tmDescent+1;
	}
else if (iAngle > 202 && iAngle <= 247)
	{ // slash
	w = h = lsqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
	cx = cx - 1;
	cy = cy-tm.tmDescent+1;
	fSlash = YES;
	}
else if (iAngle > 247 && iAngle <= 292)
	{ // Horizontal flipped
	w = tm.tmHeight;
	h = 2;
	cx = cx-tm.tmAscent+1;
	cy = cy - 1;
	}
else // if (iAngle > 292 && iAngle <= 337)
	{ // backslash
	w = h = lsqrt(((long)tm.tmHeight*(long)tm.tmHeight)/2L);
	cx = cx-tm.tmAscent+1;
	cy = cy - 1;
	fBackSlash = YES;
	}
wBytes = (w + 7) / 8;
if (wBytes & 1)
	++wBytes;
if (lpBits = Alloc((long)wBytes * h))
	{
	if (fSlash)
		SetSlashBits(lpBits, w, h, wBytes);
	else if (fBackSlash)
		SetBackSlashBits(lpBits, w, h, wBytes);
	else
		set(lpBits, wBytes*h, 255);
	hbm = CreateBitmap(w, h, 1, 1, lpBits);
	FreeUp(lpBits);
	}
CreateCaret(hWnd, hbm, w, h);
SetCaretPos(cx, cy);
ShowCaret(hWnd);

if (hFont)
	{
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
	}

if (!hInDC)
	ReleaseDC(hWnd, hDC);
}

/************************************************************************/
BOOL TextCanRotate(HWND hWnd, int nFont)
/************************************************************************/
{
HDC	hDC;
int	caps;
BOOL    fCanRotate;

hDC = GetDC(hWnd);
caps = GetDeviceCaps(hDC, TEXTCAPS);
ReleaseDC(hWnd, hDC);
fCanRotate = (caps & TC_CR_ANY) ? TRUE : FALSE;
return(TextCheckRotate(fCanRotate, nFont));
}

/************************************************************************/
static BOOL TextCheckRotate(BOOL fCanRotate, int nFont)
/************************************************************************/
{
return(fCanRotate || (Text.lpFontList[nFont].FontType & TRUETYPE_FONTTYPE));
}


static LPFRAME lpTextBits;
static RECT rBits;

/************************************************************************/
static BOOL TextApply(HWND hWnd)
/************************************************************************/
{
HDC	hTextDC;
int	iWidth, iHeight, iRasterWidth, iRasterHeight;
HBITMAP hOldBitmap, hBitmap;
RECT rBitmap, rApply, rRaster;
LPFRAME	lpBits;
int	ScaleFactor;
BOOL bRet;

if ( !lpImage )
	return( NO );

// Draw text at final size
ScaleFactor = 1;
TextDisplayLine(hWnd, NULL, ON, 0, NO, YES, FGET(ScaleFactor, 1), YES);

// Transform (rotate) extent rectangle
TransformRect(&tform, &rExtent, &rApply);

// inflate rectangle for anti-aliasing
if (Text.AntiAlias)
	InflateRect(&rApply, 2*ScaleFactor, 2*ScaleFactor);

// get width of destination bitmap
iWidth = RectWidth(&rApply);
iHeight = RectHeight(&rApply);

// make sure final bitmap has some size
if (iWidth <= 4 || iHeight <= 4)
	return(FALSE);

// Setup anti-alias scale factor if necessary and get extent info
if (Text.AntiAlias == 1) // scaling up, SmartSize Down
	{
	ScaleFactor = ANTI_ALIAS_FACTOR;
	TextDisplayLine(hWnd, NULL, ON, 0, NO, YES, FGET(ScaleFactor, 1), NO);
	}

// Transform (rotate) extent rectangle
TransformRect(&tform, &rExtent, &rRaster);

// inflate rectangle for anti-aliasing
if (Text.AntiAlias)
	InflateRect(&rRaster, 2*ScaleFactor, 2*ScaleFactor);

// get width of destination rasterization bitmap
iRasterWidth = RectWidth(&rRaster);
iRasterHeight = RectHeight(&rRaster);

// create memory DC and bitmap
hTextDC = CreateCompatibleDC(NULL);
if (!hTextDC)
	return(FALSE);
if ( !(hBitmap = CreateBitmap(iRasterWidth, iRasterHeight, 1, 1, NULL)) )
	{
	DeleteDC(hTextDC);
	return(FALSE);
	}
hOldBitmap = (HBITMAP)SelectObject(hTextDC, hBitmap);

// Clear out the background
rBitmap.top = rBitmap.left = 0;
rBitmap.right = iRasterWidth;
rBitmap.bottom = iRasterHeight;
FillRect(hTextDC, (LPRECT)&rBitmap, (HBRUSH)GetStockObject(BLACK_BRUSH) );

// Draw the text
StartPos.x = StartPos.x - rRaster.left;
StartPos.y = StartPos.y - rRaster.top;
TextDisplayLine(hWnd, hTextDC, ON, 0, NO, NO, FGET(ScaleFactor, 1),
		Text.AntiAlias == 1 ? NO : YES);
SelectObject(hTextDC, hOldBitmap);

// Get bits from the bitmap
lpBits = TextGetBitmapBits(hTextDC, hBitmap, iRasterWidth, iRasterHeight);

// Free everything and clean up
DeleteObject(hBitmap);
DeleteDC(hTextDC);
if (!lpBits)
	return(FALSE);

// Anti-Alias text if requested (i.e. - get rid of jaggies)
// Mode 1: scale up and SmartSize down
// Mode 2: smooth at final size
if (Text.AntiAlias)
	{
	if (!(lpBits = TextAntiAlias(lpBits, iRasterWidth, iRasterHeight,
				iWidth, iHeight)))
		{
		FrameClose(lpBits);
		return(FALSE);
		}
	}

if (Control.MultipleObjects)
	bRet = TextCreateObject(lpBits, &rApply);
else
	bRet = TextFill(lpBits, &rApply);

return( bRet );
}

/************************************************************************/
static BOOL TextFill(LPFRAME lpBits, LPRECT lpRect )
/************************************************************************/
{
ENGINE Engine;

// Apply text to hi-res image objects
lpTextBits = lpBits;
rBits = *lpRect;
SetEngineColor(&Engine, &TextColor, OPACITY, MERGEMODE);
Engine.lpEditRect = lpRect;
Engine.lpMaskProc = TextMaskProc;
LineEngineSelObj(lpImage, &Engine, IDS_UNDOTEXT);
FrameClose(lpBits);
return(TRUE);
}

/************************************************************************/
static BOOL TextCreateObject(LPFRAME lpBits, LPRECT lpRect )
/************************************************************************/
{
LPALPHA lpAlpha;
LPTR lpData, lpBitsData, lpAlphaData;
LPFRAME lpAlphaFrame, lpDataFrame;
LPOBJECT lpTextObject;
int dx, dy, y;
RECT rAlpha;

// Create a mask for the text bits
lpAlpha = MaskCreate(lpBits, 0, 0, NO, Control.NoUndo);
if (!lpAlpha)
	{
	FrameClose(lpBits);
	return(FALSE);
	}

// get minimum bound rectangle for text bits
// this is necessary because of the unavailability of
// correct extent information in windows
MaskRectUpdate(lpAlpha, &rAlpha);

// create an alpha frame the size of the minimum bounding rectangle
lpAlphaFrame = FrameOpen(FrameType(lpBits),
						RectWidth(&rAlpha),
						RectHeight(&rAlpha),
						FrameResolution(lpBits));
if (!lpAlphaFrame)
	{
	MaskClose(lpAlpha);
	return(FALSE);
	}

// copy text bits into alpha frame
dx = FrameXSize(lpAlphaFrame);
dy = FrameYSize(lpAlphaFrame);
for (y = 0; y < dy; ++y)
	{
	lpBitsData = FramePointer(lpBits, rAlpha.left, y+rAlpha.top, NO);
	lpAlphaData = FramePointer(lpAlphaFrame, 0, y, YES);
	if (lpBitsData && lpAlphaData)
		copy(lpBitsData, lpAlphaData, dx);
	}

// bag text bits alpha and create one base on new alpha frame
MaskClose(lpAlpha);
lpAlpha = MaskCreate(lpAlphaFrame, 0, 0, NO, Control.NoUndo);
if (!lpAlpha)
	{
	FrameClose(lpAlphaFrame);
	return(FALSE);
	}

// create a data frame and fill with the active color
lpDataFrame = FrameOpen(FrameType(ImgGetBaseEditFrame(lpImage)),
						FrameXSize(lpAlphaFrame),
						FrameYSize(lpAlphaFrame),
						FrameResolution(lpAlphaFrame));
if (!lpDataFrame)
	{
	MaskClose(lpAlpha);
	return(FALSE);
	}

dx = FrameXSize(lpDataFrame);
dy = FrameYSize(lpDataFrame);
for (y = 0; y < dy; ++y)
	{
	if (lpData = FramePointer(lpDataFrame, 0, y, YES))
		LoadColor(lpDataFrame, lpData, dx, &TextColor);
	}

// create and add text object to object list
OffsetRect(&rAlpha, lpRect->left, lpRect->top);
lpTextObject = ObjCreateFromFrame( ST_PERMANENT, lpDataFrame, lpAlpha,
								&rAlpha, Control.NoUndo );
if (!lpTextObject)
	{
	MaskClose(lpAlpha);
	FrameClose(lpDataFrame);
	return(FALSE);
	}

// set opacity and merge mode for this object
lpTextObject->Opacity = OPACITY;
lpTextObject->MergeMode = MERGEMODE;

// select our new object
ImgAddNewObject(lpImage, lpTextObject);

ImgEditInit( lpImage, ET_OBJECT, UT_DELETEOBJECTS, ImgGetBase(lpImage));
lpTextObject->fUndoDeleted = YES;
ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDOTEXT, NULL);

return(TRUE);
}

/************************************************************************/
static void TextRedrawProc(HDC hDC, LPRECT lpRect )
/************************************************************************/
{
TextDisplayLine(lpImage->hWnd, hDC, ON, 0, YES, NO, 0, YES);
if ( lpLastDisplayHook )
	(*lpLastDisplayHook)( (HDC)hDC, (LPRECT)lpRect );
}

/************************************************************************/
static void TextDisplayLine(HWND hWnd, HDC hInDC, BOOL on, BYTE bSingleChar,
			 BOOL fDisplay, BOOL fExtentsOnly, LFIXED DispRate,
			 BOOL fAllowScaling)
/************************************************************************/
{
LPTR lpChar;
BYTE bChar;
HDC	hDC;
int	oldMode, oldAlign, iAngle;
DWORD oldColor;
HFONT hFont, hOldFont;
TEXTMETRIC tm;
RECT rInvalid;
RGBS rgb;

if (fDisplay && !fExtentsOnly)
	{
	// Hide the text caret
	HideCaret(hWnd);
	}

// Undraw the text
if (!on)
	{
	// only undraw if we have drawn before
	if (nCharsDrawn)
		{ // unplug the text redraw proc
		SetDisplayHook(hWnd, lpLastDisplayHook);
		++rExtent.right; ++rExtent.bottom;
		TransformRect(&tform, &rExtent, &rInvalid);
		InvalidateRect(hWnd, &rInvalid, FALSE);
		// force undraw to happen now
		AstralUpdateWindow(hWnd);
		// install the redraw proc
		lpLastDisplayHook = SetDisplayHook(hWnd, TextRedrawProc);
		nCharsDrawn = 0; // indicate no drawing done
		}
	return;
	}

// get a new DC if one not passed in
if (!(hDC = hInDC))
	hDC = GetDC(hWnd);
if (!hDC)
	{
	Message(IDS_EMEMALLOC);
	return;
	}

// create font for rendering text
if (!DispRate)
	DispRate = (fDisplay ? lpImage->lpDisplay->DispRate : UNITY);
if (!(hFont = TextNewFont(Text.nFont, Text.Size, TextExtractStyle(), DispRate,
			fAllowScaling)))
	{
	Message(IDS_EMEMALLOC);
	if (!hInDC)
		ReleaseDC(hWnd, hDC);
	return;
	}

// setup all text parameters
if (fDisplay)
	{
	if ( !bSingleChar && lpText != lpTextBuf )
		GetActiveColorFromType(FrameType(ImgGetBaseEditFrame(lpImage)), &TextColor);
	if (FrameDepth(ImgGetBaseEditFrame(lpImage)) <= 1)
		rgb.red = rgb.green = rgb.blue = TextColor.gray;
	else
		rgb = TextColor.rgb;
	if ( (rgb.red + rgb.green + rgb.blue) == 3*255 ) // pure white
		{ rgb.red = 204; rgb.green = 204; rgb.blue = 204; }
	oldColor = SetTextColor(hDC, RGB(rgb.red, rgb.green, rgb.blue));
	}
else
	oldColor = SetTextColor(hDC, RGB(255, 255, 255));
oldAlign = SetTextAlign(hDC, TA_LEFT|TA_BASELINE|TA_NOUPDATECP);
oldMode = SetBkMode(hDC, TRANSPARENT);
hOldFont = (HFONT)SelectObject(hDC, hFont);

// get metrics for rendering text
GetTextMetrics(hDC, &tm);

if (bSingleChar)
	TextDisplayChar(hDC, bSingleChar, &tm, &tform, fExtentsOnly,
			 fDisplay);
else
	{
	// get pointer to start of buffer
	lpChar = lpTextBuf;

	// Convert start position to display coordinates
	CurrentPos = StartPos;
	if (fDisplay)
		File2Display((LPINT)&CurrentPos.x, (LPINT)&CurrentPos.y);

	// if device can rotate or it's a TrueType font, do rotation
	TInit(&tform);
	iAngle = TextGetAngle(Text.nFont);
	if (iAngle)
		{
		TMove(&tform, -CurrentPos.x, -CurrentPos.y);
		TRotate(&tform, iAngle, iAngle);
		TMove(&tform, CurrentPos.x, CurrentPos.y);
		}

	// initialize extent values
	rExtent.left = rExtent.right = CurrentPos.x;
	rExtent.top = 32767;
	rExtent.bottom = -32767;

	// reset to no characters draw and process text
	nCharsDrawn = 0;
	while (lpChar < lpText)
		{
		bChar = *lpChar++;
		TextDisplayChar(hDC, bChar, &tm, &tform, fExtentsOnly,
				 fDisplay);
		}
	}

// turn back on text caret
if (fDisplay && !fExtentsOnly)
	TextDisplayCaret(hWnd, hDC);

// reset all text parameters
SelectObject(hDC, hOldFont);
DeleteObject(hFont);
SetTextAlign(hDC, oldAlign);
SetBkMode(hDC, oldMode);
SetTextColor(hDC, oldColor);
if (!hInDC)
	ReleaseDC(hWnd, hDC);
}

/************************************************************************/
void TextDisplayChar(HDC hDC, char bChar, LPTEXTMETRIC lpTM,
			LPTFORM lpTForm, BOOL fExtentsOnly, BOOL fDisplay)
/************************************************************************/
{
int	iLeftExtent, iRightExtent, iCharWidth, iCharHeight, y;
ABC	abc;
int	cx, cy, slop;
RECT	rect, rOldClip;
SIZE	size;

if (bChar == VK_RETURN)
	{
	y = CurrentPos.y + lpTM->tmHeight + lpTM->tmExternalLeading;
	// Convert start position to display coordinates
	CurrentPos = StartPos;
	if (fDisplay)
		File2Display((LPINT)&CurrentPos.x, (LPINT)&CurrentPos.y);
	CurrentPos.y = y;
	return;
	}
else if (bChar < lpTM->tmFirstChar || bChar > lpTM->tmLastChar)
	bChar = lpTM->tmDefaultChar;

// If is 3.1 and is a TrueType character use real extent info
if ( GetCharABCWidths(hDC, (WORD)bChar, (WORD)bChar, &abc) )
	{
	iLeftExtent = abc.abcA;
	iCharWidth = abc.abcB;
	iRightExtent = abc.abcC;
	}
else // use bogus 3.0 extent information
	{
	iLeftExtent = 0;
	GetCharWidth(hDC, (WORD)bChar, (WORD)bChar, &iCharWidth);
	// italic and no overhang usually means Windows has
	// fucked up the character extents, so add in our
	// own compensation
	slop = max(lpTM->tmHeight/2, lpTM->tmOverhang);
	iRightExtent = -slop;
	iCharWidth -= iRightExtent;
	iLeftExtent = -slop;
	iCharWidth -= iLeftExtent;
	}
// Display character
if (!fExtentsOnly)
	{
	// make sure ExtTextOut is clipped to DispRect
	Transformer(lpTForm, &CurrentPos, &cx, &cy);
	if (fDisplay)
		{ // make sure ExtTextOut is clipped to DispRect
		rect = lpImage->lpDisplay->DispRect;
		SelectClipRect(hDC, &rect, &rOldClip);
		++rect.right; ++rect.bottom;
		ExtTextOut(hDC, cx, cy, ETO_CLIPPED, &rect, &bChar, 1, NULL);
		SelectClipRect(hDC, &rOldClip, NULL);
		}
	else
		TextOut(hDC, cx, cy, (LPSTR)&bChar, 1);
	}

// Get extents of character
GetTextExtentPoint(hDC, &bChar, 1, &size);
iCharHeight = size.cy;

// Advance current position by left extent (can be negative)
CurrentPos.x += iLeftExtent;
if (CurrentPos.x < rExtent.left) // check left extent
	rExtent.left = CurrentPos.x;
// Advance current position by character width
CurrentPos.x += iCharWidth;
if (CurrentPos.x > rExtent.right) // check right extent
	rExtent.right = CurrentPos.x;
// Advance current position by right extent (can be negative)
CurrentPos.x += iRightExtent;
if (CurrentPos.x > rExtent.right) // check right extent
	rExtent.right = CurrentPos.x;
// check top and bottom extents
y = CurrentPos.y - lpTM->tmAscent;
if (y < rExtent.top)
	rExtent.top = y;
y = CurrentPos.y + lpTM->tmDescent;
if (iCharHeight > lpTM->tmHeight)
	y += (iCharHeight-lpTM->tmHeight);
if (lpTM->tmUnderlined)
	y += lpTM->tmDescent;
if (y > rExtent.bottom)
	rExtent.bottom = y;
++nCharsDrawn;
}

/************************************************************************/
static HFONT TextNewFont(int nFont, int Size, BYTE Style, LFIXED DispRate,
			BOOL fAllowScaling)
/************************************************************************/
{
int iTextHeight, iAngle, iWeight, yres;
HFONT hFont;
BYTE Italic, UnderLine, StrikeOut, Quality;

iAngle = TextGetAngle(nFont);
yres = FrameResolution(ImgGetBaseEditFrame(lpImage));
iTextHeight = ((long)Size * yres ) / 72L;
iTextHeight = FMUL(iTextHeight, DispRate);
iTextHeight = bound(iTextHeight, 0, 16384);
iWeight = ( Style & TEXT_BOLD ? FW_BOLD : FW_NORMAL );
Italic = (BYTE)((Style & TEXT_ITALIC)?TRUE:FALSE);
UnderLine = (BYTE)((Style & TEXT_UNDERLINE)?TRUE:FALSE);
StrikeOut = (BYTE)((Style & TEXT_STRIKEOUT)?TRUE:FALSE);
Quality = (BYTE)(fAllowScaling ? DRAFT_QUALITY : PROOF_QUALITY);
hFont = CreateFont(
		/* Height */ -iTextHeight,
		/* Width */ 0,
		/* Escapement */ -(iAngle*10),
		/* Orientation */ 0,
		/* Weight */ iWeight,
		/* Italic */ Italic,
		/* UnderLine */ UnderLine,
		/* StrikeOut */ StrikeOut,
		/* CharSet */ Text.lpFontList[nFont].fdCharSet,
		/* Output Precis */ OUT_CHARACTER_PRECIS,
		/* Clip Precis */ CLIP_DEFAULT_PRECIS,
		/* Quality */ Quality,
		/* PitchAndFamily */ Text.lpFontList[nFont].fdPitchAndFamily,
		/* Face */ Text.lpFontList[nFont].fdFaceName );

return(hFont);
}

/************************************************************************/
static int TextGetAngle(int nFont)
/************************************************************************/
{
if (TextCheckRotate(fCanRotate, nFont))
	return(Text.Angle);
else
	return(0);
}

/************************************************************************/
static BOOL TextAddChar(WORD nVKey)
/************************************************************************/
{
if (!TextCheckBufSize(1))
	return(FALSE);
TextAddByte(LOBYTE(nVKey));
return(TRUE);
}

/************************************************************************/
static BOOL TextCheckBufSize(WORD wBytes)
/************************************************************************/
{
LPTR lpNewBuf;

if ((lpText + wBytes) <= lpTextEnd)
	return(TRUE);
lBufSize += TEXT_BUF_SIZE;
if (!(lpNewBuf = AllocExtend(lpTextBuf, lBufSize)))
	return(FALSE);
lpText = lpNewBuf + (lpText-lpTextBuf);
lpTextBuf = lpNewBuf;
lpTextEnd = lpTextBuf + lBufSize;
return TRUE;
}

/************************************************************************/
static BOOL TextAddByte(BYTE bValue)
/************************************************************************/
{
*lpText++ = bValue;
return(TRUE);
}

/************************************************************************/
static BYTE TextExtractStyle()
/************************************************************************/
{
BYTE	NewStyle = 0;

if (Text.Italic)
	NewStyle |= TEXT_ITALIC;
if (Text.Underline)
	NewStyle |= TEXT_UNDERLINE;
if (Text.Strikeout)
	NewStyle |= TEXT_STRIKEOUT;
if (Text.Weight)
	NewStyle |= TEXT_BOLD;
return(NewStyle);
}

/************************************************************************/
static void TextDeleteAllChars( HWND hWnd )
/************************************************************************/
{
TextDisplayLine(hWnd, NULL, OFF, 0, YES, NO, 0, YES);
lpText = lpTextBuf;
TextDisplayLine(hWnd, NULL, ON, 0, YES, NO, 0, YES);
}

/************************************************************************/
static void TextDeleteChar(BOOL bLast)
/************************************************************************/
{
if (lpText > lpTextBuf)
	--lpText;
else
	MessageBeep(0);
}

/************************************************************************/
LPFRAME TextGetBitmapBits(HDC hDC, HBITMAP hBitmap, int iWidth, int iHeight)
/************************************************************************/
{
LPTR lpDIBits, lpOut;
LPFRAME lpBits;
long lSize;
int	y;
BMI TextBitmap;
int nLines;

if (!(lpBits = FrameOpen(FDT_GRAYSCALE, iWidth, iHeight,
					FrameResolution(ImgGetBaseEditFrame(lpImage)))))
	return(NULL);
lSize = (iWidth + 7) / 8;
lSize = 4 * ((lSize + 3) / 4); // DIB packing
if (!(lpDIBits = Alloc(lSize)))
	{
	FrameClose(lpBits);
	return(NULL);
	}
// Setup the DIB structure
TextBitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
TextBitmap.bmiHeader.biWidth = iWidth;
TextBitmap.bmiHeader.biHeight = iHeight;
TextBitmap.bmiHeader.biPlanes = 1;
TextBitmap.bmiHeader.biBitCount = 1;
TextBitmap.bmiHeader.biCompression = BI_RGB;
TextBitmap.bmiHeader.biSizeImage = 0; //lSize;
TextBitmap.bmiHeader.biXPelsPerMeter = 0;
TextBitmap.bmiHeader.biYPelsPerMeter = 0;
TextBitmap.bmiHeader.biClrUsed = 2;
TextBitmap.bmiHeader.biClrImportant = 0;

TextBitmap.bmi.Colors[0].rgbRed   = 0;
TextBitmap.bmi.Colors[0].rgbGreen = 0;
TextBitmap.bmi.Colors[0].rgbBlue  = 0;

TextBitmap.bmi.Colors[1].rgbRed   = 255;
TextBitmap.bmi.Colors[1].rgbGreen = 255;
TextBitmap.bmi.Colors[1].rgbBlue  = 255;

for (y = 0; y < iHeight; ++y)
	{
	lpOut = FramePointer(lpBits, 0, y, YES);
	if (!lpOut)
		continue;
	nLines = GetDIBits(hDC, hBitmap, iHeight-y-1, 1, lpDIBits,
				(LPBITMAPINFO)&TextBitmap, DIB_RGB_COLORS);
	if (!nLines)
		continue;
	la2con(lpDIBits, iWidth, lpOut, YES); 
	}
FreeUp(lpDIBits);
return(lpBits);
}

/************************************************************************/

static void TextDataProc(
	int  y, 
	int  left,
	int  right,
	LPTR lpDst,
	LPTR lpSrc,
	int depth )
{
	register WORD sb, db;
	int      iCount;
	LPTR     lpTextMask;
	int      x;

	x      = left - rBits.left;
	iCount = right - left + 1;

	lpTextMask = FramePointer(lpTextBits, x, y-rBits.top, NO);
	if (!lpTextMask)
		return;

	switch(depth)
	{
		case 1 :
			while (--iCount >= 0)
			{
				sb = *lpTextMask++;
				if (sb > 127)
					++sb;
				db = 256-sb;

				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
			}
		break;

		case 3 :
			while (--iCount >= 0)
			{
				sb = *lpTextMask++;
				if (sb > 127)
					++sb;
				db = 256-sb;

				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
			}
		break;

		case 4 :
			while (--iCount >= 0)
			{
				sb = *lpTextMask++;
				if (sb > 127)
					++sb;
				db = 256-sb;

				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
				*lpSrc = (((*lpDst++) * (256-sb)) + ((*lpSrc++) * sb))>>8;
			}
		break;
	}

	return;
}

/************************************************************************/

static void TextMaskProc(int y, int left, int right, LPTR lpMsk)
{
int      iCount;
LPTR     lpTextMask;

lpTextMask = FramePointer(lpTextBits, left-rBits.left, y-rBits.top, NO);
if (lpTextMask)
	{
	iCount = right - left + 1;
	ScaleDataBuf8(lpMsk, iCount, lpTextMask);
	}
}

/************************************************************************/
LPFRAME TextAntiAlias(LPFRAME lpRasterBits, int iRasterWidth, int iRasterHeight,
		int iWidth, int iHeight)
/************************************************************************/
{
#define MAXVALUES 17
BOOL ret, bCursorEnable, bMessageEnable;
int	iValues[MAXVALUES], iEdgeValue, iCenterValue;
LPFRAME lpBits;

if (iRasterWidth != iWidth && iRasterHeight != iHeight)
	{
//	bCursorEnable = AstralCursorEnable(NO);
	bMessageEnable = MessageEnable(NO);
	lpBits = DoSmartSize(lpRasterBits, iWidth,
					iHeight, FrameResolution(lpRasterBits));
//	AstralCursorEnable(bCursorEnable);
	MessageEnable(bMessageEnable);
	FrameClose(lpRasterBits);
	return(lpBits);
	}
iCenterValue = 600;
if (iCenterValue == 1000)
	return(lpRasterBits);
iEdgeValue = (1000-iCenterValue)/8;
iValues[0] = 1;
iValues[1] = iEdgeValue;
iValues[2] = iEdgeValue;
iValues[3] = iEdgeValue;
iValues[4] = iEdgeValue;
iValues[5] = iCenterValue;
iValues[6] = iEdgeValue;
iValues[7] = iEdgeValue;
iValues[8] = iEdgeValue;
iValues[9] = iEdgeValue;
iValues[10] = 0;
iValues[11] = 0;
iValues[12] = 0;
iValues[13] = 0;
iValues[14] = 255;
iValues[15] = 0;
iValues[16] = 0;

bCursorEnable = AstralCursorEnable(NO);
bMessageEnable = MessageEnable(NO);
ret = ConvolveData( lpRasterBits, iValues );
AstralCursorEnable(bCursorEnable);
MessageEnable(bMessageEnable);
if (!ret)
	{
	FrameClose(lpRasterBits);
	return(NULL);
	}
return( lpRasterBits );
}

/************************************************************************/
static void SetBit(LPTR lp, int x)
/************************************************************************/
{
BYTE	bitmask;

lp += (x/8);
bitmask = 0x80;
if (x)
	bitmask >>= (x%8);
*lp |= bitmask;
}

/************************************************************************/
static void SetBackSlashBits(LPTR lp, int w, int h, WORD wBytes)
/************************************************************************/
{
int	x, iPixels, px;

clr(lp, h*wBytes);
x = -1;
while (--h >= 0)
	{
	px = x;
	iPixels = 3;
	while (--iPixels >= 0)
		{
		if (px >= 0 && px < w)
			SetBit(lp, px);
		++px;
		}
	++x;
	lp += wBytes;
	}
}

/************************************************************************/
static void SetSlashBits(LPTR lp, int w, int h, WORD wBytes)
/************************************************************************/
{
int	x, iPixels, px;

clr(lp, h*wBytes);
x = w - 2;
while (--h >= 0)
	{
	px = x;
	iPixels = 3;
	while (--iPixels >= 0)
		{
		if (px >= 0 && px < w)
			SetBit(lp, px);
		++px;
		}
	--x;
	lp += wBytes;
	}
}
