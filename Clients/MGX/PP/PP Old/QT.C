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

#define PNTX(n) Graymap.Pnt[n].x
#define PNTY(n) Graymap.Pnt[n].y
#define QT(i) ( Graymap.Points == 5 ? i : (2*(i))+1 )
#define HILIGHT  QT(0)
#define QUARTER1 QT(1)
#define MIDTONE  QT(2)
#define QUARTER3 QT(3)
#define SHADOW   QT(4)

#define SZX_MARK 10
#define SZY_MARK 10

#define ARED (216+BASE)
#define AGREEN (30+BASE)
#define ABLUE (5+BASE)
#define ACYAN (35+BASE)
#define AMAGENTA (221+BASE)
#define AYELLOW (246+BASE)
#define ONMARK( ix, iy, mark ) \
	( (ix) >= px[mark] && (ix) <= (SZX_MARK + px[mark]) && \
	  (iy) >= py[mark] && (iy) <= (SZY_MARK + py[mark]) )
#define HISTO_X 8
#define HISTO_RY 82
#define HISTO_GY 160
#define HISTO_BY 238
#define HISTO_RTEXTY (HISTO_RY-74)
#define HISTO_GTEXTY (HISTO_GY-74)
#define HISTO_BTEXTY (HISTO_BY-74)
#define HISTO_VALUEY (DEPTH == 1 ? HISTO_BY+MARK_HEIGHT+16-156:HISTO_BY+MARK_HEIGHT+16)
#define HISTO_SY (DEPTH == 1 ? (HISTO_RY+2) : (HISTO_BY+2))
#define HISTO_WIDTH 256
#define HISTO_HEIGHT 75
#define MARK_WIDTH 11
#define MARK_HEIGHT 13
	  
static LPLONG lpRHisto = NULL, lpGHisto = NULL, lpBHisto = NULL;
static long MaxHisto, TotalHisto;
static int HistoValue = 0;
static Pattern grayPat;
static BOOL active;

/************************************************************************/
BOOL HistoInit()
/************************************************************************/
    {
    LPFRAME lpFrame;
    int x, y, i;
    LPTR lpLine;
    RGBS rgb;
    
    lpFrame = frame_set(NULL);
    if (!lpFrame)
        return(FALSE);
    lpRHisto = AllocPtr(256 * sizeof(long));
    lpGHisto = AllocPtr(256 * sizeof(long));
    lpBHisto = AllocPtr(256 * sizeof(long));
    if (!lpRHisto || !lpGHisto || !lpBHisto)
        {
        HistoDone();
        return(FALSE);
        }
    clr(lpRHisto, 256 * sizeof(long));
    clr(lpGHisto, 256 * sizeof(long));
    clr(lpBHisto, 256 * sizeof(long));
    
    AstralWaitCursor();
    for (y = 0; y < lpFrame->Ysize; ++y)
        {
        AstralClockCursor(y, lpFrame->Ysize, TRUE);
        lpLine = CachePtr(0, 0, y, NO);
        for (x = 0; x < lpFrame->Xsize; ++x, lpLine += DEPTH)
            {
            frame_getRGB( lpLine, &rgb );
            ++lpRHisto[rgb.red];
            ++lpGHisto[rgb.green];
            ++lpBHisto[rgb.blue];
            }
        }
    change_cursor(0);
    TotalHisto = (long)lpFrame->Ysize * (long)lpFrame->Xsize;
    MaxHisto = 0;
    for (i = 0; i < 256; ++i)
        {
        if (lpRHisto[i] > MaxHisto)
            MaxHisto = lpRHisto[i];
        if (lpGHisto[i] > MaxHisto)
            MaxHisto = lpGHisto[i];
        if (lpBHisto[i] > MaxHisto)
            MaxHisto = lpBHisto[i];
        }
    for (i = 0; i < 8; ++i)
        grayPat[i] = (i & 1) ? 0x55 : 0xAA;
        
    for (i = 0; i < QTONEPOINTS; ++i)
        QTone.QTmoved[i] = NO;
    MapInit();
    QTone.ActiveMark = 0;
    HistoValue = PNTX(QT(QTone.ActiveMark));
    
    return(TRUE);
    }
    
/************************************************************************/
VOID HistoReset(win)
/************************************************************************/
    WINDOW win;
    {
    int i;
    
    for (i = 0; i < QTONEPOINTS; ++i)
        QTone.QTmoved[i] = NO;
	MapReset();
    HistoValue = PNTX(QT(QTone.ActiveMark));
	AstralHistoPaint(win);
    }
    
/************************************************************************/
BOOL HistoDone()
/************************************************************************/
    {
    if (lpRHisto)
        FreeUp(lpRHisto);
    if (lpGHisto)
        FreeUp(lpGHisto);
    if (lpBHisto)
        FreeUp(lpBHisto);
	MapDone();
    }
    
/************************************************************************/
BOOL DrawHistos(i)
/************************************************************************/
int i;
{
int height, ry, gy, by, y, x;

x = HISTO_X;
ry = HISTO_RY;
gy = HISTO_GY;
by = HISTO_BY;
x += i;

height = ((lpRHisto[255-i] * HISTO_HEIGHT)+(MaxHisto/2)) / MaxHisto;
if (i == HistoValue)
    {
    if (DEPTH == 1)
        AstralForeColor(RED);
    else
        AstralForeColor(ACYAN);
    }
else
    {
    if (DEPTH == 1)
        AstralForeColor(BLACK);
    else
        AstralForeColor(ARED);
    }
MoveTo(x, ry);
LineTo(x, ry - height - 1);
if (i == HistoValue)
    {
    PenPat(&grayPat);
    AstralForeColor(BLACK);
    }
else
    AstralForeColor(WHITE);
if ((ry - height - 1 - 1) >= (ry-HISTO_HEIGHT-1))
    {
    Move(0, -1);
    LineTo(x, ry-HISTO_HEIGHT-1);
    }
PenNormal();
if (DEPTH == 1)
    return;
    
height = ((lpGHisto[255-i] * HISTO_HEIGHT)+(MaxHisto/2)) / MaxHisto;
if (i == HistoValue)
    AstralForeColor(AMAGENTA);
else
    AstralForeColor(AGREEN);
MoveTo(x, gy);
LineTo(x, gy - height - 1);
if (i == HistoValue)
    {
    PenPat(&grayPat);
    AstralForeColor(BLACK);
    }
else
    AstralForeColor(WHITE);
if ((gy - height - 1 - 1) >= (gy-HISTO_HEIGHT-1))
    {
    Move(0, -1);
    LineTo(x, gy-HISTO_HEIGHT-1);
    }
PenNormal();
     
height = ((lpBHisto[255-i] * HISTO_HEIGHT)+(MaxHisto/2)) / MaxHisto;
if (i == HistoValue)
    AstralForeColor(AYELLOW);
else
    AstralForeColor(ABLUE);
MoveTo(x, by);
LineTo(x, by - height - 1);
if (i == HistoValue)
    {
    PenPat(&grayPat);
    AstralForeColor(BLACK);
    }
else
    AstralForeColor(WHITE);
if ((by - height - 1 - 1) >= (by-HISTO_HEIGHT-1))
    {
    Move(0, -1);
    LineTo(x, by-HISTO_HEIGHT-1);
    }
PenNormal();
}

/************************************************************************/
DrawHistoValues( win, HistoValue )
/************************************************************************/
WINDOW win;
int HistoValue;
{
FIXED itemValue;
long left, right;
int i;

AstralForeColor(BLACK);
itemValue = FGET(HistoValue * 100, 255);
DrawStaticFixedNoUnits(win, 0, itemValue, HISTO_VALUEY, 56, HISTO_VALUEY+16, 104);
DrawStaticInt(win, 0, 255-HistoValue, HISTO_VALUEY, 176, HISTO_VALUEY+16, 208);

if (DEPTH == 1)
    AstralForeColor(BLACK);
else
    AstralForeColor(ARED);
itemValue = FGET(lpRHisto[255-HistoValue], TotalHisto) * 100;
DrawStaticFixedNoUnits(win, 0, itemValue, HISTO_RTEXTY, 320, HISTO_RTEXTY+16, 368);
for (left = 0, i = 0; i < HistoValue; ++i)
    left += lpRHisto[255-i];
DrawStaticFixedNoUnits(win, 0, FGET(left, TotalHisto)*100, HISTO_RTEXTY+24, 320, HISTO_RTEXTY+24+16, 368);
for (right = 0, i = 255; i > HistoValue; --i)
    right += lpRHisto[255-i];
DrawStaticFixedNoUnits(win, 0, FGET(right, TotalHisto)*100, HISTO_RTEXTY+48, 320, HISTO_RTEXTY+48+16, 368);
    
if (DEPTH == 1)
    return;
    
AstralForeColor(AGREEN);
itemValue = FGET(lpGHisto[255-HistoValue], TotalHisto) * 100;
DrawStaticFixedNoUnits(win, 0, itemValue, HISTO_GTEXTY, 320, HISTO_GTEXTY+16, 368);
for (left = 0, i = 0; i < HistoValue; ++i)
    left += lpGHisto[255-i];
DrawStaticFixedNoUnits(win, 0, FGET(left, TotalHisto)*100, HISTO_GTEXTY+24, 320, HISTO_GTEXTY+24+16, 368);
for (right = 0, i = 255; i > HistoValue; --i)
    right += lpGHisto[255-i];
DrawStaticFixedNoUnits(win, 0, FGET(right, TotalHisto)*100, HISTO_GTEXTY+48, 320, HISTO_GTEXTY+48+16, 368);
    
AstralForeColor(ABLUE);
itemValue = FGET(lpBHisto[255-HistoValue], TotalHisto) * 100;
DrawStaticFixedNoUnits(win, 0, itemValue, HISTO_BTEXTY, 320, HISTO_BTEXTY+16, 368);
for (left = 0, i = 0; i < HistoValue; ++i)
    left += lpBHisto[255-i];
DrawStaticFixedNoUnits(win, 0, FGET(left, TotalHisto)*100, HISTO_BTEXTY+24, 320, HISTO_BTEXTY+24+16, 368);
for (right = 0, i = 255; i > HistoValue; --i)
    right += lpBHisto[255-i];
DrawStaticFixedNoUnits(win, 0, FGET(right, TotalHisto)*100, HISTO_BTEXTY+48, 320, HISTO_BTEXTY+48+16, 368);
}

/************************************************************************/
DrawQuartertoneMark( win, mark )
/************************************************************************/
WINDOW win;
int mark;
{
int mx, my;

mx = PNTX(QT(mark));
mx -= MARK_WIDTH / 2;
my = 0;
SaveDrawState();
AstralSetCurWindow(win);
MAC_DrawIcon(mx + HISTO_X, my + HISTO_SY, (long)(IDI_QUARTER1 + mark), &QTone.MarkData[mark],
             TRUE, FALSE, WHITE, &QTone.MarkRect[mark], FALSE);
offset_rect(&QTone.MarkRect[mark], mx, my);
QTone.MarkOnImage[mark] = FALSE;
RestoreDrawState();
}

/************************************************************************/
DrawQuartertones( win )
/************************************************************************/
WINDOW win;
{
int mark, mx, my;

for (mark = 0; mark < QTONEPOINTS; ++mark)
    {
    DrawQuartertoneMark(win, mark);
    }
}

/************************************************************************/
VOID ClearAllMarks(win, mark )
/************************************************************************/
    WINDOW win;
    {
    RECT cRect;
    
    cRect.top = HISTO_SY;
    cRect.bottom = cRect.top + MARK_HEIGHT + 1;
    cRect.left = HISTO_X - (MARK_WIDTH/2);
    cRect.right = HISTO_X + HISTO_WIDTH + (MARK_WIDTH/2) + 1;
    EraseRect(&cRect);
    }
    
/************************************************************************/
VOID ClearQuartertoneMark(win, mark )
/************************************************************************/
    WINDOW win;
    int mark;
    {
    int i;
    RCT rect, repair_rect;
    
    repair_rect = QTone.MarkRect[mark];
    offset_rect(&repair_rect, HISTO_X, HISTO_SY);
    ++repair_rect.right; ++repair_rect.bottom;
    EraseRect(&repair_rect);
    for (i = 0; i < QTONEPOINTS; ++i)
        {
        if (i == mark)
            continue;
        if (QTone.MarkRect[i].left > QTone.MarkRect[mark].right) /* to right of mark */
            continue;
        if (QTone.MarkRect[i].right < QTone.MarkRect[mark].left) /* to left of mark */
            continue;
        DrawQuartertoneMark(win, i);
        }
    }
    
/************************************************************************/
AstralHistoPaint( win )
/************************************************************************/
WINDOW win;
{
int i;
RECT cRect;

if (!win)
    return;
    
SaveDrawState();
AstralSetCurWindow(win);
AstralResetForeBack();
PenNormal();

for (i = 0; i < 256; ++i)
    DrawHistos(i);

ClearAllMarks(win);
DrawQuartertones(win);

AstralForeColor(BLACK);
DrawStaticText(win, 0, "Value:", HISTO_VALUEY, 8, HISTO_VALUEY+16, 56);
DrawStaticText(win, 0, "%", HISTO_VALUEY, 104, HISTO_VALUEY+16, 128);
DrawStaticText(win, 0, "Pixel:", HISTO_VALUEY, 130, HISTO_VALUEY+16, 170);
    
if (DEPTH == 1)
    AstralForeColor(BLACK);
else
    AstralForeColor(ARED);
DrawStaticText(win, 0, "Count:", HISTO_RTEXTY, 272, HISTO_RTEXTY+16, 320);
DrawStaticText(win, 0, "%", HISTO_RTEXTY, 368, HISTO_RTEXTY+16, 392);
DrawStaticText(win, 0, "Left:", HISTO_RTEXTY+24, 272, HISTO_RTEXTY+24+16, 368);
DrawStaticText(win, 0, "%", HISTO_RTEXTY+24, 368, HISTO_RTEXTY+24+16, 392);
DrawStaticText(win, 0, "Right:", HISTO_RTEXTY+48, 272, HISTO_RTEXTY+48+16, 368);
DrawStaticText(win, 0, "%", HISTO_RTEXTY+48, 368, HISTO_RTEXTY+48+16, 392);
    
AstralForeColor(BLACK);
MoveTo(HISTO_X, HISTO_RY+1);
LineTo(HISTO_X+HISTO_WIDTH-1, HISTO_RY+1);
if (DEPTH == 1)
    {
    DrawHistoValues(win, HistoValue);
    RestoreDrawState();
    return;
    }

MoveTo(HISTO_X, HISTO_GY+1);
LineTo(HISTO_X+HISTO_WIDTH-1, HISTO_GY+1);
MoveTo(HISTO_X, HISTO_BY+1);
LineTo(HISTO_X+HISTO_WIDTH-1, HISTO_BY+1);

AstralForeColor(AGREEN);
DrawStaticText(win, 0, "Count:", HISTO_GTEXTY, 272, HISTO_GTEXTY+16, 320);
DrawStaticText(win, 0, "%", HISTO_GTEXTY, 368, HISTO_GTEXTY+16, 392);
DrawStaticText(win, 0, "Left:", HISTO_GTEXTY+24, 272, HISTO_GTEXTY+24+16, 368);
DrawStaticText(win, 0, "%", HISTO_GTEXTY+24, 368, HISTO_GTEXTY+24+16, 392);
DrawStaticText(win, 0, "Right:", HISTO_GTEXTY+48, 272, HISTO_GTEXTY+48+16, 368);
DrawStaticText(win, 0, "%", HISTO_GTEXTY+48, 368, HISTO_GTEXTY+48+16, 392);
    
AstralForeColor(ABLUE);
DrawStaticText(win, 0, "Count:", HISTO_BTEXTY, 272, HISTO_BTEXTY+16, 320);
DrawStaticText(win, 0, "%", HISTO_BTEXTY, 368, HISTO_BTEXTY+16, 392);
DrawStaticText(win, 0, "Left:", HISTO_BTEXTY+24, 272, HISTO_BTEXTY+24+16, 368);
DrawStaticText(win, 0, "%", HISTO_BTEXTY+24, 368, HISTO_BTEXTY+24+16, 392);
DrawStaticText(win, 0, "Right:", HISTO_BTEXTY+48, 272, HISTO_BTEXTY+48+16, 368);
DrawStaticText(win, 0, "%", HISTO_BTEXTY+48, 368, HISTO_BTEXTY+48+16, 392);

DrawHistoValues(win, HistoValue);
RestoreDrawState();
}

/************************************************************************/
HistoMouseDown(win, x, y, shift, control, alt)
/************************************************************************/
WINDOW win;
int x, y;
BOOL shift, control, alt;
{
RECT inRect;
int mark;

for (mark = 0; mark < QTONEPOINTS; ++mark)
    {
    inRect = QTone.MarkRect[mark];
    offset_rect(&inRect, HISTO_X, HISTO_SY);
    if (AstralPointInRect(inRect, x, y))
        {
	    MapUndoChange();
        QTone.ActiveMark = mark;
        HistoMouseMove(win, x, y, TRUE, shift, control, alt);
        active = TRUE;
        break;
        }
    }
}

/************************************************************************/
HistoMouseUp(win, x, y, shift, control, alt)
/************************************************************************/
WINDOW win;
int x, y;
BOOL shift, control, alt;
{
active = FALSE;
}

/************************************************************************/
SpaceQuartertones()
/************************************************************************/
    {
    int i, j, num;
    
    /* default values that are evenly spaced */
    if ( !QTone.QTmoved[2] )
	    {
	    if ( !QTone.QTmoved[i = 1] )
		    i = 0;
	    if ( !QTone.QTmoved[j = 3] )
		    j = 4;
		if ((i == 0 && j == 4) || (i == 1 && j == 3)) /* between highlight and shadow */
	        PNTX( MIDTONE ) = ((PNTX( QT(i) ) + PNTX( QT(j)) ) ) / 2;
	    else if (i == 1 && j == 4) /* between 1/4 and shadow */
	        {
	        PNTX( MIDTONE ) = PNTX(QT(i)) + ((((int)PNTX( QT(j) ) - (int)PNTX( QT(i)) ) ) / 3);
	        }
	    else if (i == 0 && j == 3) /* between highlight and 3/4 */
	        PNTX( MIDTONE ) = PNTX(QT(i)) + ((2 * ((int)PNTX( QT(j) ) - (int)PNTX( QT(i)) ) ) / 3);
	  /*  dbg("MIDTONE = %d QT(%d) = %d QT(%d) = %d", PNTX(MIDTONE), i, PNTX(QT(i)),
	    														   j, PNTX(QT(j)));*/
	    }
    if ( !QTone.QTmoved[1] )
	    PNTX( QUARTER1 ) = ( PNTX( HILIGHT ) + PNTX( MIDTONE ) ) / 2;
    if ( !QTone.QTmoved[3] )
	    PNTX( QUARTER3 ) = ( PNTX( MIDTONE ) + PNTX( SHADOW ) ) / 2;

	/* if any points are out of order, place them in order */
	for ( i=1; i<QTONEPOINTS; i++ )
		PNTX( QT(i) ) = max( PNTX( QT(i) ), PNTX( QT(i-1) ) );
    makelut(&Graymap);
    }
		
/************************************************************************/
HistoMouseMove(win, x, y, down, shift, control, alt)
/************************************************************************/
WINDOW win;
int x, y;
BOOL down;
BOOL shift, control, alt;
{
int last_value, i, j;

if (down && active)
    {
    SaveDrawState();
    AstralSetCurWindow(win);
    PenNormal();
    AstralResetForeBack();
    last_value = HistoValue;
    if (x < HISTO_X)
        HistoValue = 0;
    else if (x > (HISTO_X+HISTO_WIDTH-1))
        HistoValue = 255;
    else
        HistoValue = x - HISTO_X;
    PNTX(QT(QTone.ActiveMark)) = HistoValue;
    QTone.QTmoved[QTone.ActiveMark] = TRUE;
    
    SpaceQuartertones();
    makelut(&Graymap);
    MapNewLut();
    HistoValue = PNTX(QT(QTone.ActiveMark));
    DrawHistos(last_value);
    DrawHistos(HistoValue);
    DrawHistoValues(win, HistoValue);
    ClearAllMarks(win);
    for (i = 0; i < QTONEPOINTS;++i)
         if (i != QTone.ActiveMark)
            DrawQuartertoneMark(win, i);
    DrawQuartertoneMark(win, QTone.ActiveMark);
    RestoreDrawState();
    }
}
