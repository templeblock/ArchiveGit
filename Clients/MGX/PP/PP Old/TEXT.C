// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

#define THRESH	127
#define EXTRA_X 5
#define EXTRA_Y 5

static unsigned rowBytes;
static LPTR lpChar;
static RECT osRect;
static int width, height;
static LPSHAPE lpTextShapes;
static HDC hTextDC;
static BMI TextBitmap;

/************************************************************************/
BOOL InTextRegion(v)
/************************************************************************/
    long v;
    {
    return(v != 0);
    }
    
/************************************************************************/
BOOL PutTextPixel(x, y, Pixel)
/************************************************************************/
    int x, y;
    long Pixel;
    {
    LPTR lpData;

    lpData = lpChar + ((height-y-1) * (long)rowBytes) + x;
    *lpData = Pixel;
    return(TRUE);
    }
    
/************************************************************************/
BOOL GetTextPixel(x, y, lpPixel)
/************************************************************************/
    int x, y;
    LPLONG lpPixel;
    {
    LPTR lpData;
    
    if (x < osRect.left || x > osRect.right ||
        y < osRect.top || y > osRect.bottom)
        return(FALSE);
    
    lpData = lpChar + ((height-y-1) * (long)rowBytes) + x;
    *lpPixel = *lpData;
    return(TRUE);
    }
    
/************************************************************************/
VOID DIB2Contone(lpIn, lpOut, width, height, rowbytes)
/************************************************************************/
LPTR lpIn;
LPTR lpOut;
int width;
int height;
int rowbytes;
{
LPTR lpPixel;
int y, iCount;
BYTE byte, bit;

for (y = 0; y < height; ++y)
	{
	lpPixel = lpIn;
	iCount = width;
	bit = 1;
	while (--iCount >= 0)
		{
		if ( --bit == 0 )
			{
			byte = (*lpPixel++);
			bit = 8;
			}
		if ( byte > THRESH )
			*lpOut++ = 1;
		else	
			*lpOut++ = 0;
		byte <<= 1;
		}
	lpIn += rowbytes;
	}
}

/************************************************************************/
int VectorizeCharacters(lpString, lpFirstPoint, lpFirstCode, dx, dy, max_points, npoints)
/************************************************************************/
LPSTR lpString;
LPPOINT lpFirstPoint;
LPTR lpFirstCode;
int dx, dy;
int max_points;
LPINT npoints;
{
LPTR lpPixel, lpBits, lpLine;
int px, py, i, num_points, startx, starty, lines;
LPPOINT lpPoint, lpInPoint, lpOutPoint;
POINT last;
LPTR lpCode, lpInCode, lpOutCode;
BYTE pix1, pix2, lastCode;
int status;
BOOL GotVert, GotHorz;
long lCount;
HBITMAP hTextBitmap, hOldBitmap;
DWORD dwSize, dwCount;
TEXTMETRIC metrics;
int extrax, extray;

GetTextMetrics(hTextDC, &metrics);
extrax = metrics.tmOverhang;
dbg("tmOverhang = %d", metrics.tmOverhang);
if (extrax < EXTRA_X)
	extrax = EXTRA_X;
extray = EXTRA_Y;
dbg("extrax = %d extray = %d", extrax, extray);
dwSize = GetTextExtent(hTextDC, lpString, lstrlen(lpString));
height = HIWORD(dwSize) + (2*extray);;
width = LOWORD(dwSize) + (2*extrax);
dbg("height = %d width = %d", height, width);

dbg("CreateBitmap");dbg(NULL);
hTextBitmap = CreateBitmap(width, height, 1, 1, NULL);
if (!hTextBitmap)
    return(1);
dbg("SelectObject");dbg(NULL);
hOldBitmap = SelectObject(hTextDC, hTextBitmap);
osRect.top = osRect.left = 0;
osRect.right = width - 1;
osRect.bottom = height - 1;
dbg("FillRect");dbg(NULL);
osRect.right++; osRect.bottom++;
FillRect(hTextDC, (LPRECT)&osRect, Window.hWhiteBrush);
osRect.right--; osRect.bottom--;
dbg("ColorText");dbg(NULL);
ColorText(hTextDC, extrax, extray, lpString, lstrlen(lpString), RGB(0, 0, 0));
dbg("Select Old Bitmap");dbg(NULL);
SelectObject(hTextDC, hOldBitmap);

rowBytes = 4 * ((((width)) + 3) / 4); // DIB packing
lpChar = Alloc((long)rowBytes * (long)height);
if (!lpChar)
	{
	DeleteObject(hTextBitmap);
	return(3);
	}
TextBitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
TextBitmap.bmiHeader.biWidth = width;
TextBitmap.bmiHeader.biHeight = height;
TextBitmap.bmiHeader.biPlanes = 1;
TextBitmap.bmiHeader.biBitCount = 8;
TextBitmap.bmiHeader.biCompression = BI_RGB;
TextBitmap.bmiHeader.biSizeImage = (long)rowBytes * (long)height;
TextBitmap.bmiHeader.biXPelsPerMeter = 0;
TextBitmap.bmiHeader.biYPelsPerMeter = 0;
TextBitmap.bmiHeader.biClrUsed = 256;
TextBitmap.bmiHeader.biClrImportant = 0;
for (i = 0; i < 256; ++i)
    {
    TextBitmap.bmi.Colors[i].rgbRed = i;
    TextBitmap.bmi.Colors[i].rgbGreen = i;
    TextBitmap.bmi.Colors[i].rgbBlue = i;
    }

dbg("GetDIBits");dbg(NULL);
lines = GetDIBits(hTextDC, hTextBitmap, 0, height, lpChar, (LPBITMAPINFO)&TextBitmap, DIB_RGB_COLORS);
dbg("DeleteObject");dbg(NULL);
DeleteObject(hTextBitmap);
dbg("biBitCount = %d", TextBitmap.bmiHeader.biBitCount);dbg(NULL);
if (lines != height)
    {
    FreeUp(lpChar);
    Print("GetDIBits only processed %d lines", lines);
    return(2);
    }
lCount = (long)rowBytes * (long)height;
lpPixel = lpChar;
while (--lCount >= 0)
    {
    if (*lpPixel)
        *lpPixel++ = 0;
    else
        *lpPixel++ = 1;
    }
for (py = 0; py < height; ++py)
	{
    	lpPixel = lpChar + ((long)py * (long)rowBytes);
	lpPixel += (width-1);
	if (*lpPixel)
		dbg("last pixel in line %d set = %d", py, (int)*lpPixel);
	}
dbg("doing tracing");dbg(NULL);
*npoints = 0;
status = 0;
for (py = 0; py < height; ++py)
    {
    lpPixel = lpChar + ((height-py-1) * (long)rowBytes);
    pix1 = *lpPixel;
    ++lpPixel;
    for (px = 0; px < width-1; ++px)
        {
        pix2 = *lpPixel;
        if (pix1 == 0 && pix2 == 1)
            {
            lpPoint = lpFirstPoint + *npoints;
            lpCode = lpFirstCode + *npoints;
            status = Tracer(px+1, py, osRect, GetTextPixel, PutTextPixel, 
                            InTextRegion, lpPoint, lpCode, max_points, 
                            FALSE, &num_points);
    	    if (status == 0 && FALSE)
		{
        	status = TraceHoles(osRect, GetTextPixel, PutTextPixel, InTextRegion, lpPoint, lpCode, max_points, FALSE, &num_points);
		}
            *npoints += num_points;
            max_points -= num_points;
            if (status != 0)
        	break;
            }
       	else if (pix1 == 1 && pix2 == 0)
            {
            lpPoint = lpFirstPoint + *npoints;
            lpCode = lpFirstCode + *npoints;
            status = Tracer(px, py, osRect, GetTextPixel, PutTextPixel, 
                           InTextRegion, lpPoint, lpCode, max_points, 
                                    FALSE, &num_points);
    	    if (status == 0 && FALSE)
		{
        	status = TraceHoles(osRect, GetTextPixel, PutTextPixel, InTextRegion, lpPoint, lpCode, max_points, FALSE, &num_points);
		}
	    *npoints += num_points;
            max_points -= num_points;
            if (status != 0)
        	break;
            }
       	pix1 = pix2;
        ++lpPixel;
        }
    if (status != 0)
        break;
    }
dbg("done tracing");dbg(NULL);
FreeUp(lpChar);

lpInPoint = lpOutPoint = lpFirstPoint;
lpInCode = lpOutCode = lpFirstCode;
num_points = 0;
last.x = last.y = -30000;
GotHorz = FALSE;
GotVert = FALSE;
dx -= extrax;
dy -= extray;
for (i = 0; i < *npoints; ++i)
    {
    if ((GotVert && (lpInPoint->x != startx || *lpInCode == 8)) ||
        (GotHorz && (lpInPoint->y != starty || *lpInCode == 8)))
        {
        lpOutPoint->x = last.x + dx;
        lpOutPoint->y = last.y + dy;
        *lpOutCode = lastCode;
        ++lpOutPoint;
        ++lpOutCode;
        ++num_points;
        GotVert = GotHorz = FALSE;
        last.x = last.y = -30000;
        }
    if (lpInPoint->y == last.y)
        {
        if (!GotHorz)
            {
            starty = lpInPoint->y;
            GotHorz = TRUE;
            }
        last = *lpInPoint;
        lastCode = *lpInCode;
        }
    else if (lpInPoint->x == last.x)
        {
        if (!GotVert)
            {
            startx = lpInPoint->x;
            GotVert = TRUE;
            }
        last = *lpInPoint;
        lastCode = *lpInCode;
        }
    else
        {
        last = *lpInPoint;
        lastCode = *lpInCode;
        lpOutPoint->x = lpInPoint->x + dx;
        lpOutPoint->y = lpInPoint->y + dy;
        *lpOutCode = *lpInCode;
        ++lpOutPoint;
        ++lpOutCode;
        ++num_points;
        }
    ++lpInPoint;
    ++lpInCode;
    }
if (GotVert || GotHorz)
    {
    lpOutPoint->x = last.x + dx;
    lpOutPoint->y = last.y + dy;
    *lpOutCode = lastCode;
    ++num_points;
    }
*npoints = num_points;
return(status);
}
    
/************************************************************************/
BOOL DoText(lpTextRect)
/************************************************************************/
LPRECT lpTextRect;
{
int max_points, npoints, num_points, width, height;
LPPOINT lpFirstPoint = NULL, lpPoint;
LPTR lpCode, lpFirstCode;
BOOL first = TRUE, fRefineSize;
LPINT lpNumPoints;
int i, nShapes;
int status;
LPMASK lpMask;
HDC hDC;
HFONT hTextFont, hOldFont;
int iTextHeight;
DWORD dwSize;

RemoveMask();
max_points = 5000;
while (lpFirstPoint == NULL)
    {
    lpFirstPoint = (LPPOINT)Alloc((long)max_points * (long)(sizeof(APOINT)+sizeof(BYTE)));
    if (!lpFirstPoint)
        {
        max_points /= 2;
        if (max_points < 3)
            break;
        }
    }
if (max_points < 3)
    {
    Print("not enough memory available for text tool");
    return(NO);
    }
lpFirstCode = (LPTR)(lpFirstPoint + max_points);

AstralCursor(IDC_WAIT);
hDC = CreateIC("DISPLAY", NULL, NULL, NULL);
hTextDC = CreateCompatibleDC(hDC);
if (!hTextDC)
    {
    FreeUp((LPTR)lpFirstPoint);
    return(NO);
    }

if ( abs( lpTextRect->right - lpTextRect->left ) <= SMALL_MOVEMENT &&
     abs( lpTextRect->bottom - lpTextRect->top ) <= SMALL_MOVEMENT )
	{
	iTextHeight = ((long)Text.Size * lpImage->yres ) / 72L;
	fRefineSize = NO;
	}
else	{
	iTextHeight = 0;
	fRefineSize = YES;
	}

Display2File( &lpTextRect->left, &lpTextRect->top );
Display2File( &lpTextRect->right, &lpTextRect->bottom );
if ( iTextHeight <= 0 )
	iTextHeight = RectHeight(lpTextRect);

dbg("iTextHeight = %d", iTextHeight);
hTextFont = CreateFont(/* Height */ iTextHeight,
		 	/* Width */ 0,
			/* Escapement */ 0,
			/* Orientation */ 0,
			/* Weight */ ( Text.Weight ? FW_BOLD : FW_NORMAL ),
			/* Italic */ Text.Italic,
			/* UnderLine */ Text.Underline,
			/* StrikeOut */ Text.Strikeout,
			/* CharSet */ Text.CharSet[Text.nFont],
			/* Output Precis */ OUT_CHARACTER_PRECIS,
			/* Clip Precis */ CLIP_DEFAULT_PRECIS,
			/* Quality */ PROOF_QUALITY,
			/* PitchAndFamily */ Text.PitchAndFamily[Text.nFont],
			/* Face */ Text.FontList[Text.nFont] );
dbg("Face = %ls", (LPTR)Text.FontList[Text.nFont]);
if (!hTextFont)
    {
    DeleteDC(hTextDC);
    FreeUp((LPTR)lpFirstPoint);
    return(NO);
    }
hOldFont = SelectObject(hTextDC, hTextFont);
if ( fRefineSize )
    {
    dwSize = GetTextExtent(hTextDC, Text.TextString, lstrlen(Text.TextString));
    height = RectHeight(lpTextRect);
    width = RectWidth(lpTextRect);
    dbg("height = %d width = %d", height, width);
    ScaleToFit(&width, &height, LOWORD(dwSize), HIWORD(dwSize));
    dbg("height = %d width = %d", height, width);
    iTextHeight = height;
    SelectObject(hTextDC, hOldFont);
    DeleteObject(hTextFont);
    hTextFont = CreateFont(/* Height */ iTextHeight,
		 	/* Width */ 0,
			/* Escapement */ 0,
			/* Orientation */ 0,
			/* Weight */ ( Text.Weight ? FW_BOLD : FW_NORMAL ),
			/* Italic */ Text.Italic,
			/* UnderLine */ Text.Underline,
			/* StrikeOut */ Text.Strikeout,
			/* CharSet */ Text.CharSet[Text.nFont],
			/* Output Precis */ OUT_CHARACTER_PRECIS,
			/* Clip Precis */ CLIP_DEFAULT_PRECIS,
			/* Quality */ PROOF_QUALITY,
			/* PitchAndFamily */ Text.PitchAndFamily[Text.nFont],
			/* Face */ Text.FontList[Text.nFont] );
    if (!hTextFont)
	{
	DeleteDC(hTextDC);
	FreeUp((LPTR)lpFirstPoint);
	return(NO);
	}
    hOldFont = SelectObject(hTextDC, hTextFont);
    }

lpPoint = lpFirstPoint;
lpCode = lpFirstCode;
status = VectorizeCharacters((LPSTR)Text.TextString, lpPoint, lpCode, lpTextRect->left, lpTextRect->top, max_points, (LPINT)&npoints);

num_points = npoints;

SelectObject(hTextDC, hOldFont);
DeleteObject(hTextFont);
DeleteDC(hTextDC);
if (status != 0 || !num_points)
    {
    FreeUp((LPTR)lpFirstPoint);
    return(NO);
    }
lpCode = lpFirstCode;
for (i = 0, nShapes = 0; i < num_points; ++i, ++lpCode)
    if (*lpCode == 8)
	 ++nShapes;
lpNumPoints = (LPINT)Alloc((long)sizeof(int)*(long)nShapes);
if (!lpNumPoints)
    {
    FreeUp((LPTR)lpFirstPoint);
    return(NO);
    }
lpCode = lpFirstCode;
npoints = 0;
for (i = 0, nShapes = 0; i < num_points; ++i, ++lpCode)
    {
    if (*lpCode == 8 && npoints)
	 {
	 lpNumPoints[nShapes] = npoints;
	 ++nShapes;
	 npoints = 1;
	 }
    else
	 ++npoints;
    }
lpNumPoints[nShapes] = npoints;
++nShapes;
lpMask = mask_create(NULL, lpFirstPoint, lpNumPoints, nShapes, NO );
if (!lpMask)
    MessageBeep(0);
else
    mask_link(lpMask, WRITE_MASK);
//TintFill( &Palette.ActiveRGB );
//lpImage->dirty = IDS_UNDOTEXT;
BuildMarquee();
Marquee(YES);
FreeUp((LPTR)lpNumPoints);
FreeUp((LPTR)lpFirstPoint);
return(YES);
}

#ifdef UNUSED
/************************************************************************/
STATICFCN PW_STATUS text_done(win, x, y)
/************************************************************************/
    WINDOW win;
    int x, y;
    {
    LPFRAME lpFrame;
    LPMASK lpMask;
    
    if (text_active)
        {
        Ctrl.ModalFunction = 0;
        AstralSetDocumentCursor(win);
        ShapeTransformDone();
        lpFrame = frame_set(NULL);
        if (lpFrame)
            {
            lpMask = BuildEdges(lpFrame->WriteMask, lpTextShapes);
			if ( lpMask )
				mask_link( lpMask, WRITE_MASK );
			else
    			beep();
			BuildMarquee();
            if (DoMask(DoTint))
                lpImage->dirty |= DIRTY_TEXT;
            DrawApplyButton();
    	    GrayMaskMenus();
    	    DrawInsideOutside();
			DisplayMarquee(img_win, TRUE);
            }
        }
    return(PW_DONE);
    }
    
/************************************************************************/
STATICFCN PW_STATUS text_mouse_down(win, x, y, shift, control, alt)
/************************************************************************/
    WINDOW win;
    int x, y;
    BOOLEAN shift, control, alt;
    {
    if (text_active)
        {
        ShapeTransformMouseDown(win, x, y, shift, control, alt);
        return(PW_ACTIVE);
        }
    return(PW_DONE);
    }
    
/************************************************************************/
STATICFCN PW_STATUS text_mouse_up(win, x, y, shift, control, alt)
/************************************************************************/
    WINDOW win;
    int x, y;
    BOOLEAN shift, control, alt;
    {
    if (text_active)
        return(PW_ACTIVE);
    return(PW_DONE);
    }
   
/************************************************************************/
STATICFCN PW_STATUS text_mouse_move(win, x, y, down, shift, control, alt)
/************************************************************************/
    WINDOW win;
    int x, y;
    BOOLEAN down;
    BOOLEAN shift, control, alt;
    {
    if (text_active)
        {
        ShapeTransformMouseMove(win, x, y, down, shift, control, alt);
        return(PW_ACTIVE);
        }
    return(PW_DONE);
    }

#endif
