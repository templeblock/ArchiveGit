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

/************************************************************************/
BOOL WriteShapes(fp, lpMask, fRelative)
/************************************************************************/
int fp;
LPMASK lpMask;
BOOL fRelative;
{
int nShapes, count;
LPPOINT lpPoint;
LPSHAPE lpShape;
int x, y;
TFORM ShapeTForm;
    
nShapes = 0;
lpShape = lpMask->lpHeadShape;
while (lpShape)
    {
    ++nShapes;
    lpShape = lpShape->lpNext;
    }
if (_lwrite(fp, (LPSTR)&nShapes, 2) != 2)
    return(FALSE);
lpShape = lpMask->lpHeadShape;
while (lpShape)
    {
    ShapeTForm = lpShape->tform;
    if (fRelative)
	TInit(&ShapeTForm);
	    
    if (_lwrite(fp,(LPSTR)&lpShape->fCircle,2) != 2)
	return(FALSE);
    if (_lwrite(fp,(LPSTR)&ShapeTForm.ax, 4) != 4)
	return(FALSE);
    if (_lwrite(fp,(LPSTR)&ShapeTForm.ay, 4) != 4)
	return(FALSE);
    if (_lwrite(fp,(LPSTR)&ShapeTForm.bx, 4) != 4)
	return(FALSE);
    if (_lwrite(fp,(LPSTR)&ShapeTForm.by, 4) != 4)
	return(FALSE);
    if (_lwrite(fp,(LPSTR)&ShapeTForm.cx, 4) != 4)
	return(FALSE);
    if (_lwrite(fp,(LPSTR)&ShapeTForm.cy, 4) != 4)
	return(FALSE);
    if (_lwrite(fp,(LPSTR)&lpShape->nPoints, 2) != 2)
	return(FALSE);
    if (fRelative)
	{
	ShapeTForm = lpShape->tform;
	TMove(&ShapeTForm, -lpMask->rMask.left, -lpMask->rMask.top);
	}
    count = lpShape->nPoints;
    lpPoint = lpShape->lpPoints;
    while (--count >= 0)
	{
	if (fRelative && lpPoint->x != BEZIER_MARKER)
	    Transformer(&ShapeTForm, lpPoint, &x, &y);
	else
	    {
	    x = lpPoint->x;
	    y = lpPoint->y;
	    }
		    
	if (_lwrite(fp,(LPSTR)&x, 2) != 2)
	    return(FALSE);
	if (_lwrite(fp,(LPSTR)&y, 2) != 2)
	    return(FALSE);
	++lpPoint;
	}
    lpShape = lpShape->lpNext;
    }
return(TRUE);
}
	
/************************************************************************/
LPSHAPE ReadShapes(fp, fSwap)
/************************************************************************/
int fp;
BOOL fSwap;
{
short count, nPoints[1], i;
LPWORD lpWord;
LPPOINT lpPoint, lpFirstPoint;
LPSHAPE lpShape, lpHeadShape, lpNextShape;
short x, y, nShapes;
TFORM ShapeTForm;
BOOL fCircle;
    
if (_lread(fp,(LPSTR)&nShapes, 2) != 2)
    return(NULL);
if (fSwap) swapw((LPWORD)&nShapes);
if (nShapes <= 0)
    return(NULL);
lpHeadShape = NULL;
for (i = 0; i < nShapes; ++i)
    {
    if (_lread(fp,(LPSTR)&fCircle, 2) != 2)
	return(NULL);
    if (fSwap) swapw((LPWORD)&fCircle);
    if (_lread(fp,(LPSTR)&ShapeTForm.ax, 4) != 4)
	return(NULL);
    if (fSwap) swapl(&ShapeTForm.ax);
    if (_lread(fp,(LPSTR)&ShapeTForm.ay, 4) != 4)
	return(NULL);
    if (fSwap) swapl(&ShapeTForm.ay);
    if (_lread(fp,(LPSTR)&ShapeTForm.bx, 4) != 4)
	return(NULL);
    if (fSwap) swapl(&ShapeTForm.bx);
    if (_lread(fp,(LPSTR)&ShapeTForm.by, 4) != 4)
	return(NULL);
    if (fSwap) swapl(&ShapeTForm.by);
    if (_lread(fp,(LPSTR)&ShapeTForm.cx, 4) != 4)
	return(NULL);
    if (fSwap) swapl(&ShapeTForm.cx);
    if (_lread(fp,(LPSTR)&ShapeTForm.cy, 4) != 4)
	return(NULL);
    if (fSwap) swapl(&ShapeTForm.cy);
    if (_lread(fp,(LPSTR)&nPoints[0], 2) != 2)
 	return(NULL);
    lpWord = (LPWORD)&nPoints[0];
    if (fSwap) 
	swapw(lpWord);
    lpFirstPoint = (LPPOINT)Alloc((long)nPoints[0] * (long)sizeof(POINT));
    if (!lpFirstPoint)
	return(NULL);
    count = nPoints[0];
    lpPoint = lpFirstPoint;
    while (--count >= 0)
	{
	if (_lread(fp,(LPSTR)&lpPoint->x, 2) != 2)
	    return(NULL);
        if (fSwap) swapw((LPWORD)&lpPoint->x);
	if (_lread(fp,(LPSTR)&lpPoint->y, 2) != 2)
	    return(NULL);
        if (fSwap) swapw((LPWORD)&lpPoint->y);
	    ++lpPoint;
	}
    lpShape = BuildShapes(lpFirstPoint, (LPINT)nPoints, 1, fCircle );
    FreeUp((LPTR)lpFirstPoint);
    if (!lpShape)
	{
	FreeUpShapes(lpHeadShape);
	return(NULL);
	}
    lpShape->tform = ShapeTForm;
    /* Link the new shape to the bottom of the list */
    if ( !lpHeadShape )
 	lpHeadShape = lpShape;
    else 
        {
	lpNextShape = lpHeadShape;
	while ( lpNextShape->lpNext )
	    lpNextShape = lpNextShape->lpNext;
	lpNextShape->lpNext = lpShape;
	}
    }
if (lpHeadShape)
    ShapeNewShapes(lpHeadShape);
return(lpHeadShape);
}
	
/************************************************************************/
BOOL WriteMask(szClipName, lpMask, fRelative)
/************************************************************************/
LPMASK lpMask;
LPSTR szClipName;
BOOL fRelative;
{
int fp;
BOOL status = TRUE;
short order;

if (!lpMask)
    return;
if ( (fp = _lcreat(szClipName, 0)) < 0 )
    goto errcreate;
AstralCursor(IDC_WAIT);
if (_lwrite(fp, "MSK", 3) != 3)
    goto errwrite;
order = MYORDER;
if (_lwrite(fp,(LPSTR)&order, 2) != 2)
    goto errwrite;
if (!WriteShapes(fp, lpMask, fRelative))
    goto errwrite;
goto okc;

errcreate:
Message ( IDS_ECREATE, (LPTR)szClipName );
status = FALSE;
goto okc;
errwrite:
Message( IDS_EWRITE, (LPTR)szClipName );
status = FALSE;
okc:	
_lclose( fp );
AstralCursor(NULL);
return(status);
}

/************************************************************************/
LPSHAPE ReadMask(szClipName)
/************************************************************************/
LPSTR szClipName;
{
int fp;
short order;
LPSHAPE lpShape;
char type[4];

lpShape = NULL;
if ( (fp = _lopen(szClipName, OF_READ)) < 0 )
    goto erropen;

AstralCursor(IDC_WAIT);
if (_lread(fp, (LPSTR)type, 3) != 3)
    goto errread;
if (strncmp(type, "MSK", 3) != 0)
    goto errtype;

if (_lread(fp,(LPSTR)&order, 2) != 2)
    goto errread;
lpShape = ReadShapes(fp, order != MYORDER);
if (!lpShape)
    goto errmem;
goto okp;

errtype:
Message( IDS_EBADMASK, (LPTR)szClipName );
goto errc;
errmem:
Message( IDS_EMEMALLOC );
goto errc;
erropen:
Message ( IDS_EOPEN, (LPTR)szClipName );
goto errc;
errread:
Message ( IDS_EREAD, (LPTR)szClipName );
goto errc;
    
errc:	
if (lpShape)
    FreeUpShapes(lpShape);
lpShape = NULL;
okp:    
_lclose(fp);
AstralCursor(NULL);
return(lpShape);
}
