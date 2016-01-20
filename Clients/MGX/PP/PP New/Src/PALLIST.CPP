// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include <ctype.h>
#include "id.h"

LOCAL WORD GetIndex(LPTSTR lpLabels);
LOCAL void SetIndex(LPTSTR lpLabels, WORD Index);

/************************************************************************/
LOCAL WORD GetIndex(LPTSTR lpLabels)
/************************************************************************/
{
BYTE lo, hi;
WORD Index;

lo = *(lpLabels+2);
hi = *(lpLabels+3);
Index = MAKEWORD(lo, hi);
return(Index);
}

/************************************************************************/
LOCAL void SetIndex(LPTSTR lpLabels, WORD Index)
/************************************************************************/
{
	BYTE lo, hi;

	lo = LOBYTE(Index);
	hi = HIBYTE(Index);
	*(lpLabels+2) = lo;
	*(lpLabels+3) = hi;
}

/************************************************************************/
LPPALETTE Palette_New( LPTSTR lpPaletteName )
/************************************************************************/
{
LPPALETTE lpPalette;

lpPalette = (LPPALETTE)Alloc((long)sizeof(PALETTE));
if (!lpPalette)
	return(NULL);
clr((LPTR)lpPalette, sizeof(PALETTE));
lstrcpy(lpPalette->szName, lpPaletteName);
return(lpPalette);
}

/************************************************************************/
LPPALETTE Palette_Link( LPPALETTE lpHeadPalette, LPPALETTE lpPalette )
/************************************************************************/
{
LPPALETTE lpNextPalette;

if (lpHeadPalette)
	{
	lpNextPalette = lpHeadPalette;
	while (lpNextPalette->lpNext)
		lpNextPalette = lpNextPalette->lpNext;
	lpNextPalette->lpNext = lpPalette;
	}
else
	lpHeadPalette = lpPalette;
return(lpHeadPalette);
}

/************************************************************************/
void Palette_FreeUp( LPPALETTE lpPalette )
/************************************************************************/
{
LPPALETTE lpNextPalette;

while (lpPalette)
	{
	lpNextPalette = lpPalette->lpNext;
	if (lpPalette->lpColorInfo)
		FreeUp((LPTR)lpPalette->lpColorInfo);
	if (lpPalette->lpLabels)
		FreeUp((LPTR)lpPalette->lpLabels);
	FreeUp((LPTR)lpPalette);
	lpPalette = lpNextPalette;
	}
}

/************************************************************************/
LPPALETTE Palette_Delete( LPPALETTE lpHeadPalette, int iPalette )
/************************************************************************/
{
LPPALETTE lpNextPalette, lpPrevPalette;
int	nPalettes;

lpNextPalette = lpHeadPalette;
lpPrevPalette = NULL;
nPalettes = 0;
while (lpNextPalette)
	{
	if (nPalettes == iPalette)
		{
		if (lpPrevPalette)
			lpPrevPalette->lpNext = lpNextPalette->lpNext;
		else
			lpHeadPalette = lpNextPalette->lpNext;
		if (lpNextPalette->lpColorInfo)
			FreeUp((LPTR)lpNextPalette->lpColorInfo);
		if (lpNextPalette->lpLabels)
			FreeUp(lpNextPalette->lpLabels);
		FreeUp((LPTR)lpNextPalette);
		break;
		}
	++nPalettes;
	lpPrevPalette = lpNextPalette;
	lpNextPalette = lpNextPalette->lpNext;
	}
return(lpHeadPalette);
}

/************************************************************************/
int Palette_GetCount( LPPALETTE lpHeadPalette )
/************************************************************************/
{
int		nPalettes = 0;
LPPALETTE	lpNextPalette;

lpNextPalette = lpHeadPalette;
while (lpNextPalette)
	{
	++nPalettes;
	lpNextPalette = lpNextPalette->lpNext;
	}
return(nPalettes);
}

/************************************************************************/
LPPALETTE Palette_Get( LPPALETTE lpHeadPalette, LPTSTR lpPaletteName, int index )
/************************************************************************/
{
LPPALETTE lpNextPalette;
int		nPalette;

nPalette = 0;
lpNextPalette = lpHeadPalette;
while (lpNextPalette)
	{
	if (lpPaletteName)
		{
		if (StringsEqual(lpNextPalette->szName, lpPaletteName))
			return(lpNextPalette);
		}
	else
		{
		if (index == nPalette)
			return(lpNextPalette);
		}
	++nPalette;
	lpNextPalette = lpNextPalette->lpNext;
	}
return(NULL);
}

/************************************************************************/
BOOL Palette_GetName( LPPALETTE lpHeadPalette, int iPalette, LPTSTR lpPaletteName )
/************************************************************************/
{
LPPALETTE lpPalette;

lstrcpy(lpPaletteName, _T(""));
lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	lstrcpy(lpPaletteName, lpPalette->szName);
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
int Palette_GetNum( LPPALETTE lpHeadPalette, LPTSTR lpPaletteName,
                        LPPALETTE lpPalette )
/************************************************************************/
{
LPPALETTE lpNextPalette;
int		nPalette;

nPalette = 0;
lpNextPalette = lpHeadPalette;
while (lpNextPalette)
	{
	if (lpPaletteName)
		{
		if (StringsEqual(lpNextPalette->szName, lpPaletteName))
			return(nPalette);
		}
	else if (lpPalette)
		{
		if (lpPalette == lpNextPalette)
			return(nPalette);
		}
	++nPalette;
	lpNextPalette = lpNextPalette->lpNext;
	}
return(-1);
}

/************************************************************************/
BOOL Palette_Rename( LPPALETTE lpHeadPalette, int iPalette, LPTSTR lpNewName )
/************************************************************************/
{
LPPALETTE lpPalette;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	lstrcpy(lpPalette->szName, lpNewName);
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_MoveEntry( LPPALETTE lpHeadPalette, int iPalette, int iOldEntry,
                           int iNewEntry )
/************************************************************************/
{
STRING		szLabel;
COLORINFO  ColorInfo;

if (!Palette_GetEntryLabel(lpHeadPalette,iPalette, iOldEntry, szLabel))
	return(FALSE);
if (!Palette_GetEntryColor(lpHeadPalette,iPalette, iOldEntry, &ColorInfo))
	return(FALSE);
if (!Palette_DeleteEntry(lpHeadPalette, iPalette, iOldEntry))
	return(FALSE);
if (!Palette_InsertEntry(lpHeadPalette, iPalette, iNewEntry, &ColorInfo))
	return(FALSE);
if (!Palette_SetEntryLabel(lpHeadPalette, iPalette, iNewEntry, szLabel))
	return(FALSE);
return(TRUE);
}

/************************************************************************/
BOOL Palette_InsertEntry( LPPALETTE lpHeadPalette, int iPalette, int iBefore,
                              LPCOLORINFO lpColorInfo )
/************************************************************************/
{
LPPALETTE lpPalette;
LPCOLORINFO lpNewColors, lpSrc, lpDst;
int iAfter;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (!Palette_InsertAdjustLabels(lpHeadPalette, iPalette, iBefore, 1))
		return(FALSE);
	if (lpPalette->iColors)
		iBefore = bound(iBefore, 0, lpPalette->iColors-1);
	lpNewColors = (LPCOLORINFO)Alloc((long)((lpPalette->iColors+1)*sizeof(COLORINFO)));
	if (!lpNewColors)
		return(FALSE);
	lpSrc = lpPalette->lpColorInfo;
	lpDst = lpNewColors;
	if (lpPalette->iColors && iBefore > 0)
		{
		copy((LPTR)lpSrc, (LPTR)lpDst, iBefore*sizeof(COLORINFO));
		lpSrc += iBefore;
		lpDst += iBefore;
		}

	// Set the new palette info
	*lpDst++ = *lpColorInfo;

	iAfter = lpPalette->iColors - iBefore;
	if (lpPalette->iColors && iAfter > 0)
		copy((LPTR)lpSrc, (LPTR)lpDst, iAfter*sizeof(COLORINFO));
	if (lpPalette->lpColorInfo)
        	FreeUp((LPTR)lpPalette->lpColorInfo);
	lpPalette->lpColorInfo = lpNewColors;
	++lpPalette->iColors;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_AppendEntry( LPPALETTE lpHeadPalette, int iPalette, int iAfter,
                              LPCOLORINFO lpColorInfo )
/************************************************************************/
{
LPPALETTE lpPalette;
LPCOLORINFO lpNewColors, lpSrc, lpDst;
int iBefore;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (!Palette_AppendAdjustLabels(lpHeadPalette, iPalette, iAfter, 1))
		return(FALSE);
	if (lpPalette->iColors)
		iAfter = bound(iAfter, 0, lpPalette->iColors-1);
	lpNewColors = (LPCOLORINFO)Alloc((long)((lpPalette->iColors+1)*sizeof(COLORINFO)));
	if (!lpNewColors)
		return(FALSE);
	lpSrc = lpPalette->lpColorInfo;
	lpDst = lpNewColors;
	iBefore = iAfter + 1;
	if (lpPalette->iColors && iBefore > 0)
		{
		copy((LPTR)lpSrc, (LPTR)lpDst, iBefore*sizeof(COLORINFO));
		lpSrc += iBefore;
		lpDst += iBefore;
		}

	*lpDst++ = *lpColorInfo;

	iAfter = lpPalette->iColors-iAfter-1;
	if (lpPalette->iColors && iAfter > 0)
		copy((LPTR)lpSrc, (LPTR)lpDst, iAfter*sizeof(COLORINFO));
	if (lpPalette->lpColorInfo)
		FreeUp((LPTR)lpPalette->lpColorInfo);
	lpPalette->lpColorInfo = lpNewColors;
	++lpPalette->iColors;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_DeleteEntry( LPPALETTE lpHeadPalette, int iPalette, int iEntry )
/************************************************************************/
{
return(Palette_DeleteEntries(lpHeadPalette, iPalette, iEntry, 1));
}

/************************************************************************/
BOOL Palette_DeleteEntries( LPPALETTE lpHeadPalette, int iPalette, int iStart,
                              int iCount )
/************************************************************************/
{
LPPALETTE lpPalette;
LPCOLORINFO lpNewColors, lpSrc, lpDst;
int iBefore, iAfter, iColors, iEnd;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (!lpPalette->iColors || iStart < 0 || iStart >= lpPalette->iColors)
		return(FALSE);
	if (!Palette_DeleteLabels(lpHeadPalette, iPalette, iStart, iCount))
		return(FALSE);
	iEnd = iStart + iCount - 1;
	iEnd = bound(iEnd, 0, lpPalette->iColors-1);
	iCount = iEnd - iStart + 1;
	iColors = lpPalette->iColors - iCount;
	if (iColors < 0)
		iColors = 0;
	if (!iColors)
		{
		if (lpPalette->lpColorInfo)
			{
			FreeUp((LPTR)lpPalette->lpColorInfo);
			lpPalette->lpColorInfo = NULL;
			}
		lpPalette->iColors = iColors;
		return(TRUE);
		}
	lpNewColors = (LPCOLORINFO)Alloc((long)iColors*(long)sizeof(COLORINFO));
	if (!lpNewColors)
		return(FALSE);
	lpSrc = lpPalette->lpColorInfo;
	lpDst = lpNewColors;
	iBefore = iStart;
	if (lpPalette->iColors && iBefore > 0)
		{
		copy((LPTR)lpSrc, (LPTR)lpDst, iBefore*sizeof(COLORINFO));
		lpSrc += iBefore;
		lpDst += iBefore;
		}
	lpSrc += iCount;
	iAfter = lpPalette->iColors - iStart - iCount;
	if (lpPalette->iColors && iAfter > 0)
		copy((LPTR)lpSrc, (LPTR)lpDst, iAfter*sizeof(COLORINFO));
	if (lpPalette->lpColorInfo)
		FreeUp((LPTR)lpPalette->lpColorInfo);
	lpPalette->lpColorInfo = lpNewColors;
	lpPalette->iColors = iColors;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_DeleteLabels( LPPALETTE lpHeadPalette, int iPalette, int iStart,
                              int iCount )
/************************************************************************/
{
	LPPALETTE lpPalette;
	int iColors, iEnd;
	LPTSTR lpDst, lpEnd, lpNewLabels, lpSrc;
	WORD LabelsLength, RemainLength, Index, NewIndex;
	BYTE Type, Length;

	lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
	if (lpPalette)
	{
		if (!lpPalette->iColors || iStart < 0 || iStart >= lpPalette->iColors)
			return(FALSE);

		iEnd = iStart + iCount - 1;
		iEnd = bound(iEnd, 0, lpPalette->iColors-1);
		iCount = iEnd - iStart + 1;

		iColors = lpPalette->iColors - iCount;
		if (iColors < 0)
			iColors = 0;

		if (!iColors)
		{
			if (lpPalette->lpLabels)
			{
				FreeUp((LPTR)lpPalette->lpLabels);
				lpPalette->lpLabels = NULL;
			}
			lpPalette->LabelsLength = 0;
			return(TRUE);
		}
		lpSrc = lpPalette->lpLabels;
		LabelsLength = lpPalette->LabelsLength;
		if (lpSrc)
			lpEnd = lpSrc + LabelsLength;
		else
			lpEnd = NULL;
		RemainLength = 0;
		while (lpSrc < lpEnd)
		{
			Type = *lpSrc;
			Length = *(lpSrc+1);
			Index = GetIndex(lpSrc);
			if (Index < iStart || Index > iEnd)
				RemainLength += Length;
			lpSrc += Length;
		}
		if (!RemainLength)
		{
			if (lpPalette->lpLabels)
			{
				FreeUp(lpPalette->lpLabels);
				lpPalette->lpLabels = NULL;
			}
			lpPalette->LabelsLength = 0;
			return(TRUE);
		}
		lpNewLabels = (LPTSTR)Alloc((long)RemainLength);
		if (!lpNewLabels)
			return(FALSE);
		lpSrc = lpPalette->lpLabels;
		lpDst = lpNewLabels;
		while (lpSrc < lpEnd)
		{
			Type = *lpSrc;
			Length = *(lpSrc+1);
			Index = GetIndex(lpSrc);
			if (Index < iStart || Index > iEnd)
			{
				copy((LPTR)lpSrc, (LPTR)lpDst, Length);
				if (Index > iEnd)
				{
					NewIndex = Index-iCount;
					SetIndex(lpDst, NewIndex);
				}
				lpDst += Length;
			}
			lpSrc += Length;
		}

		FreeUp(lpPalette->lpLabels);
		lpPalette->lpLabels = lpNewLabels;
		lpPalette->LabelsLength = RemainLength;
		return(TRUE);
	}
	return(FALSE);
}

/************************************************************************/
BOOL Palette_AppendAdjustLabels( LPPALETTE lpHeadPalette, int iPalette,
                                    int iAfter, int iCount )
/************************************************************************/
{
LPPALETTE lpPalette;
LPTSTR	lpSrc, lpEnd;
WORD LabelsLength, Index;
BYTE Type, Length;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (lpPalette->iColors)
		iAfter = bound(iAfter, 0, lpPalette->iColors-1);
	lpSrc = lpPalette->lpLabels;
	LabelsLength = lpPalette->LabelsLength;
	if (lpSrc)
		lpEnd = lpSrc + LabelsLength;
	else
		lpEnd = NULL;
	while (lpSrc < lpEnd)
		{
		Type = *lpSrc;
		Length = *(lpSrc+1);
		Index = GetIndex(lpSrc);
		if (Index > iAfter)
			Index += iCount;
		SetIndex(lpSrc, Index);
		lpSrc += Length;
		}
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_InsertAdjustLabels( LPPALETTE lpHeadPalette, int iPalette,
                                    int iBefore, int iCount )
/************************************************************************/
{
LPPALETTE lpPalette;
LPTSTR	lpSrc, lpEnd;
WORD LabelsLength, Index;
BYTE Type, Length;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (lpPalette->iColors)
		iBefore = bound(iBefore, 0, lpPalette->iColors-1);
	lpSrc = lpPalette->lpLabels;
	LabelsLength = lpPalette->LabelsLength;
	if (lpSrc)
		lpEnd = lpSrc + LabelsLength;
	else
		lpEnd = NULL;
	while (lpSrc < lpEnd)
		{
		Type = *lpSrc;
		Length = *(lpSrc+1);
		Index = GetIndex(lpSrc);
		if (Index >= iBefore)
			Index += iCount;
		SetIndex(lpSrc, Index);
		lpSrc += Length;
		}
	return(TRUE);
	}
return(FALSE);
}


/************************************************************************/
BOOL Palette_SetEntryLabel( LPPALETTE lpHeadPalette, int iPalette, int iEntry,
                              LPTSTR lpLabel )
/************************************************************************/
{
LPPALETTE lpPalette;
LPTSTR lpDst, lpEnd, lpNewLabels, lpSrc;
WORD LabelsLength, LengthBefore, LengthAfter, RemoveLength, Index;
BYTE Type, Length;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (iEntry < 0 || iEntry >= lpPalette->iColors)
		return(FALSE);
	lpSrc = lpPalette->lpLabels;
	LabelsLength = lpPalette->LabelsLength;
	if (lpSrc)
		lpEnd = lpSrc + LabelsLength;
	else
		lpEnd = NULL;
	LengthBefore = 0;
	RemoveLength = 0;
	while (lpSrc < lpEnd)
		{
		Type = *lpSrc;
		Length = *(lpSrc+1);
		Index = GetIndex(lpSrc);
		if (Index >= iEntry)
			{
			if (Index == iEntry)
				RemoveLength = Length;
			break;
			}
		LengthBefore += Length;
		lpSrc += Length;
		}
	LabelsLength -= RemoveLength;
	LengthAfter = LabelsLength - LengthBefore;
	if (lstrlen(lpLabel))
		LabelsLength += lstrlen(lpLabel)+1+4;
	lpNewLabels = (LPTSTR)Alloc((long)LabelsLength);
	if (!lpNewLabels)
		return(FALSE);
	lpSrc = lpPalette->lpLabels;
	lpDst = lpNewLabels;
	if (LengthBefore)
		copy((LPTR)lpSrc, (LPTR)lpDst, LengthBefore);
	lpDst += LengthBefore;
	lpSrc += (LengthBefore + RemoveLength);
	if (lstrlen(lpLabel))
		{
		Length = lstrlen(lpLabel)+1+4;
		*lpDst = 0x01;
		if (lstrlen(lpPalette->szFormat) > 0)
			*lpDst |= 0x80;
		*(lpDst+1) = Length;
		SetIndex(lpDst, iEntry);
		lstrcpy(lpDst+4, lpLabel);
		lpDst += Length;
		}
	if (LengthAfter)
		copy((LPTR)lpSrc, (LPTR)lpDst, LengthAfter);
	FreeUp(lpPalette->lpLabels);
	lpPalette->lpLabels = lpNewLabels;
	lpPalette->LabelsLength = LabelsLength;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_SetEntryColor( LPPALETTE lpHeadPalette, int iPalette, int iEntry,
                                 LPCOLORINFO lpColorInfo )
/************************************************************************/
{
LPPALETTE lpPalette;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (iEntry < 0 || iEntry >= lpPalette->iColors)
		return(FALSE);
	lpPalette->lpColorInfo[iEntry] = *lpColorInfo;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_GetEntryColor( LPPALETTE lpHeadPalette, int iPalette, int iEntry,
                              LPCOLORINFO lpColorInfo )
/************************************************************************/
{
LPPALETTE lpPalette;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (!lpPalette->iColors || iEntry < 0 || iEntry >= lpPalette->iColors)
		return(FALSE);
	*lpColorInfo = lpPalette->lpColorInfo[iEntry];
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL Palette_GetEntryLabel( LPPALETTE lpHeadPalette, int iPalette, int iEntry,
                              LPTSTR lpColorLabel )
/************************************************************************/
{
LPPALETTE lpPalette;
LPTSTR lpLabels, lpLast;
WORD Index;
BYTE Type, Length;

lpColorLabel[0] = _T('\0');
lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	if (!lpPalette->iColors || iEntry < 0 || iEntry >= lpPalette->iColors)
		return(FALSE);
	lpLabels = lpPalette->lpLabels;
	if (!lpLabels)
		return(TRUE);
	lpLast = lpLabels + lpPalette->LabelsLength;
	while (lpLabels < lpLast)
		{
		Index = GetIndex(lpLabels);
		Type = *lpLabels++;
		Length = *lpLabels++;
		lpLabels += 2;
		if (Index == iEntry)
			{
			Palette_FormatEntryLabel(lpPalette, Type, lpLabels,
					 lpColorLabel);
			break;
			}
		lpLabels += (Length-4);
		}
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
void Palette_FormatEntryLabel( LPPALETTE lpPalette, BYTE Type, LPTSTR lpLabel,
                                 LPTSTR lpColorLabel )
/************************************************************************/
{
	STRING szString;
	LPTSTR lpFormat, lpString, lpCLabel;
	WORD wChar;
	BYTE lo, hi;

	if (lstrlen(lpPalette->szFormat) <= 0 || Type & 0x80)
		lstrcpy(lpColorLabel, lpLabel);
	else if (Type == 0 || Type == 1)
	{
		if (Type == 0)
		{
			lo = *lpLabel;
			hi = *(lpLabel+1);
			wsprintf(szString, "%u", MAKEWORD(lo, hi));
		}
		else
			lstrcpy(szString, lpLabel);
		lpFormat = (LPTSTR)lpPalette->szFormat;
		lpCLabel = (LPTSTR)lpColorLabel;
		lpString = (LPTSTR)szString;

		while( *lpFormat != _T('\0' ))
		{
			if( IsDBCSLeadByte(( BYTE )*lpFormat ))
				wChar = *(( LPWORD )lpFormat );
			else
				wChar = *lpFormat;

			if( wChar != _T('%'))
			{
				if( IsDBCSLeadByte(( BYTE )*lpFormat ))
					*(( LPWORD )lpCLabel ) = wChar;
				else				
					*lpCLabel = wChar;

				lpCLabel = MyCharNext( lpCLabel );
			}
			else
			{
				while( *lpString != _T('\0') )
					*lpCLabel++ = *lpString++;
			}
			lpFormat = MyCharNext( lpFormat );
		}
		*lpCLabel = _T('\0');
	}
	else
		lpColorLabel[0] = _T('\0');
}

/************************************************************************/
BOOL Palette_FillEntries( LPPALETTE lpHeadPalette, int iPalette, int iAfter,
                           LPCOLORINFO lpColor1, LPCOLORINFO lpColor2,
                           int iEntries, BOOL DoHSL )
/************************************************************************/
{
	LFIXED rate1, rate2;
	long range1, range2, range3;
	int  start1, start2, start3;

	int iNewColors, iColors;
	LPCOLORINFO lpNewColors, lpSrc, lpDst;
	int iBefore, i, n, iStart, j;
	LPPALETTE lpPalette;
	BOOL fDoHS, fDoSL, fDoHL, fDoRG, fDoGB, fDoRB;
	BYTE Value;

	lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
	if (!lpPalette)
		return(FALSE);

	iStart = iAfter;

	if (DoHSL)
		{
		range1 = (long)lpColor2->hsl.hue - (long)lpColor1->hsl.hue;
		range2 = (long)lpColor2->hsl.sat - (long)lpColor1->hsl.sat;
		range3 = (long)lpColor2->hsl.lum - (long)lpColor1->hsl.lum;
		}
	else
		{
		range1 = (long)lpColor2->rgb.red   - (long)lpColor1->rgb.red;
		range2 = (long)lpColor2->rgb.green - (long)lpColor1->rgb.green;
		range3 = (long)lpColor2->rgb.blue  - (long)lpColor1->rgb.blue;
		}

	if (iEntries)
		iNewColors = iEntries;
	else
	{
		iNewColors = max(max(abs(range1), abs(range2)), abs(range3));
		iNewColors -= 2;
	}

	if (iNewColors <= 0)
		return(FALSE);

	fDoRG = iNewColors == 256 && !DoHSL && OPTION1;
	fDoGB = iNewColors == 256 && !DoHSL && OPTION2;
	fDoRB = iNewColors == 256 && !DoHSL && OPTION3;
	fDoHS = iNewColors == 256 && DoHSL && OPTION1;
	fDoSL = iNewColors == 256 && DoHSL && OPTION2;
	fDoHL = iNewColors == 256 && DoHSL && OPTION3;

	iColors     = lpPalette->iColors + iNewColors;
	lpNewColors = (LPCOLORINFO)Alloc((long)iColors*(long)sizeof(COLORINFO));

	if (!lpNewColors)
		return(FALSE);

	iBefore = iAfter + 1;
	lpSrc   = lpPalette->lpColorInfo;
	lpDst   = lpNewColors;

	if (lpPalette->iColors && iBefore > 0)
	{
		copy((LPTR)lpSrc, (LPTR)lpDst, iBefore*sizeof(COLORINFO));
		lpSrc += iBefore;
		lpDst += iBefore;
	}

	if (fDoHS || fDoSL || fDoHL)
	{
		if (fDoHS)
			Value = lpColor1->hsl.lum;
		else if (fDoSL)
			Value = lpColor1->hsl.hue;
		else
			Value = lpColor1->hsl.sat;

		rate1 = FGET(251, 15);
		rate2 = FGET(255, 15);
		for (i = 0; i < 16; ++i)
		{
			for (j = 0; j < 16; ++j)
			{
				if (fDoHL)
				{
					n = FMUL(i, rate1);
					lpDst->hsl.hue = mbound( n, 0, 251 );
					lpDst->hsl.sat = Value;
					n = FMUL(j, rate2);
					lpDst->hsl.lum = mbound( n, 0, 255 );
				}
				else
				if (fDoHS)
				{
					n = FMUL(i, rate1);
					lpDst->hsl.hue = mbound( n, 0, 251 );
					n = FMUL(j, rate2);
					lpDst->hsl.sat = mbound( n, 0, 255 );
					lpDst->hsl.lum = Value;
				}
				else  // fDoSL
				{
					lpDst->hsl.hue = Value;
					n = FMUL(i, rate2);
					lpDst->hsl.sat = mbound( n, 0, 255 );
					n = FMUL(j, rate2);
					lpDst->hsl.lum = mbound( n, 0, 255 );
				}

				SetColorInfo( lpDst, lpDst, CS_HSL );
				++lpDst;
			}
		}
	}
	else if (fDoRG || fDoGB || fDoRB)
	{
		if (fDoRG)
			Value = lpColor1->rgb.blue;
		else if (fDoGB)
			Value = lpColor1->rgb.red;
		else
			Value = lpColor1->rgb.green;
		rate1 = FGET(255, 15);
		for (i = 0; i < 16; ++i)
		{
			for (j = 0; j < 16; ++j)
			{
				if (fDoRG)
				{
					n = FMUL(i, rate1);
					lpDst->rgb.red = mbound( n, 0, 255 );
					n = FMUL(j, rate1);
					lpDst->rgb.green = mbound( n, 0, 255 );
					lpDst->rgb.blue = Value;
				}
				else
				if (fDoGB)
				{
					lpDst->rgb.red = Value;
					n = FMUL(i, rate1);
					lpDst->rgb.green = mbound( n, 0, 255 );
					n = FMUL(j, rate1);
					lpDst->rgb.blue = mbound( n, 0, 255 );
				}
				else  // fDoRB
				{
					n = FMUL(i, rate1);
					lpDst->rgb.red = mbound( n, 0, 255 );
					lpDst->rgb.green = Value;
					n = FMUL(j, rate1);
					lpDst->rgb.blue = mbound( n, 0, 255 );
				}

				SetColorInfo( lpDst, lpDst, CS_RGB );
				++lpDst;
			}
		}
	}
	else if (DoHSL)
	{
		start1 = lpColor1->hsl.hue;
		start2 = lpColor1->hsl.sat;
		start3 = lpColor1->hsl.lum;

		for (i = 0; i < iNewColors; ++i)
		{
			// range from 0 to 1
			rate1 = FGET(i+1, iNewColors+1);

			n = start1 + WHOLE(rate1*range1);
			lpDst->hsl.hue = mbound( n, 0, 255 );
			n = start2 + WHOLE(rate1*range2);
			lpDst->hsl.sat = mbound( n, 0, 255 );
			n = start3 + WHOLE(rate1*range3);
			lpDst->hsl.lum = mbound( n, 0, 255 );

			SetColorInfo( lpDst, lpDst, CS_HSL );
			++lpDst;
		}
	}
	else
	{
		start1 = lpColor1->rgb.red;
		start2 = lpColor1->rgb.green;
		start3 = lpColor1->rgb.blue;

		for (i = 0; i < iNewColors; ++i)
		{
			// range from 0 to 1
			rate1 = FGET(i+1, iNewColors+1);

			n = start1 + WHOLE(rate1*range1);
			lpDst->rgb.red   = mbound( n, 0, 255 );
			n = start2 + WHOLE(rate1*range2);
			lpDst->rgb.green = mbound( n, 0, 255 );
			n = start3 + WHOLE(rate1*range3);
			lpDst->rgb.blue  = mbound( n, 0, 255 );

			SetColorInfo( lpDst, lpDst, CS_RGB );
			++lpDst;
		}
	}

	iAfter = lpPalette->iColors-iAfter-1;

	if (lpPalette->iColors && iAfter > 0)
		copy((LPTR)lpSrc, (LPTR)lpDst, iAfter*sizeof(COLORINFO));

	if (lpPalette->lpColorInfo)
		FreeUp((LPTR)lpPalette->lpColorInfo);

	lpPalette->lpColorInfo = lpNewColors;
	lpPalette->iColors = iColors;

	Palette_AppendAdjustLabels(lpHeadPalette, iPalette, iStart, iNewColors);

	return(TRUE);
}

/************************************************************************/
BOOL Palette_AddEntries( LPPALETTE lpHeadPalette, int iPalette, int iAfter,
                           int iEntries, LPCOLORINFO lpColorInfo )
/************************************************************************/
{
int		iNewColors, iColors;
LPCOLORINFO lpNewColors, lpSrc, lpDst;
int		iBefore, i, iStart;
LPPALETTE 	lpPalette;

lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (!lpPalette)
	return(FALSE);
iStart = iAfter;
iNewColors = iEntries;
if (iNewColors <= 0)
	return(FALSE);
iColors = lpPalette->iColors + iNewColors;
lpNewColors = (LPCOLORINFO)Alloc((long)iColors*(long)sizeof(COLORINFO));
if (!lpNewColors)
	return(FALSE);
iBefore = iAfter + 1;
lpSrc = lpPalette->lpColorInfo;
lpDst = lpNewColors;
if (lpPalette->iColors && iBefore > 0)
	{
	copy((LPTR)lpSrc, (LPTR)lpDst, iBefore*sizeof(COLORINFO));
	lpSrc += iBefore;
	lpDst += iBefore;
	}
for (i = 0; i < iNewColors; ++i)
	{
	*lpDst = *lpColorInfo;
	++lpColorInfo;
	++lpDst;
	}
iAfter = lpPalette->iColors-iAfter-1;
if (lpPalette->iColors && iAfter > 0)
	copy((LPTR)lpSrc, (LPTR)lpDst, iAfter*sizeof(COLORINFO));
if (lpPalette->lpColorInfo)
	FreeUp((LPTR)lpPalette->lpColorInfo);
lpPalette->lpColorInfo = lpNewColors;
lpPalette->iColors = iColors;
Palette_AppendAdjustLabels(lpHeadPalette, iPalette, iStart, iNewColors);
return(TRUE);
}

/************************************************************************/
int Palette_FindEntryLabel( LPPALETTE lpHeadPalette, int iPalette,
                              LPINT lpEntry, LPTSTR lpColorLabel, int iWhich )
/************************************************************************/
{
LPPALETTE lpPalette;
LPTSTR lpLabels, lpLast;
WORD Index;
BYTE Type, Length;
STRING szLabel, szFound;
int	iNum;

iNum = 0;
szFound[0] = _T('\0');
lpPalette = Palette_Get(lpHeadPalette, NULL, iPalette);
if (lpPalette && lpPalette->iColors && lpPalette->lpLabels &&
	lstrlen(lpColorLabel))
	{
	lpLabels = lpPalette->lpLabels;
	lpLast = lpLabels + lpPalette->LabelsLength;
	while (lpLabels < lpLast)
		{
		Index = GetIndex(lpLabels);
		Type = *lpLabels++;
		Length = *lpLabels++;
		lpLabels += 2;
		Palette_FormatEntryLabel(lpPalette, Type, lpLabels, szLabel);
		if (Palette_MatchCriteria(szLabel, lpColorLabel))
			{
			if ((iWhich < 0 && iNum == 0) || iWhich == iNum)
				{
				lstrcpy(szFound, szLabel);
				*lpEntry = Index;
				if (iWhich == iNum)
					{
					++iNum;
					break;
					}
				}
			++iNum;
			}
		lpLabels += (Length-4);
		}
	}
lstrcpy(lpColorLabel, szFound);
return(iNum);
}

/************************************************************************/
BOOL Palette_MatchCriteria( LPTSTR lpLabel, LPTSTR lpCriteria )
/************************************************************************/
{
    WORD C1, C2;

    while( *lpCriteria )
    {
        if( IsDBCSLeadByte(( BYTE )*lpCriteria ))
		 	C1 = MBToLowerW( lpCriteria );
        else
		 	C1 = MBToLower( lpCriteria );

	    if( C1 == _T('*') )     // wildcard
		    return( TRUE );
	    else 
        if( !( *lpLabel ))
		    return( FALSE );
	    else
	    {
		    if( C1 != _T('?') )
		    {
                if( IsDBCSLeadByte(( BYTE )*lpLabel ))
		 	        C2 = MBToLowerW( lpLabel );
                else
		 	        C2 = MBToLower( lpLabel );

			    if( C1 != C2 )
				    return( FALSE );
		    }
	    }
        lpCriteria = MyCharNext( lpCriteria );
        lpLabel = MyCharNext( lpLabel );
    }
    return( *lpLabel == _T('\0') );
}


/************************************************************************/
LPPALETTE Palette_Combine( LPPALETTE lpHeadPalette )
/************************************************************************/
{
LPPALETTE lpPalette_New, lpPalette;
WORD Index;
int i, nEntry, nColors;
int nPalettes;
LPTSTR lpLabels, lpSrc, lpEnd;
BYTE Length;

lpPalette_New = Palette_New(_T(""));
if (!lpPalette_New)
	return(NULL);
// take palette format of first palette in list
lstrcpy(lpPalette_New->szFormat, lpHeadPalette->szFormat);

nPalettes = Palette_GetCount(lpHeadPalette);
for (i = 0; i < nPalettes; ++i)
	{
	lpPalette = Palette_Get(lpHeadPalette, NULL, i);
	lpPalette_New->iColors += lpPalette->iColors;
	lpPalette_New->LabelsLength += lpPalette->LabelsLength;
	}
lpPalette_New->lpColorInfo =
	(LPCOLORINFO)Alloc((long)lpPalette_New->iColors*(long)sizeof(COLORINFO));
if (!lpPalette_New->lpColorInfo)
	{
	Palette_FreeUp(lpPalette_New);
	return(NULL);
	}
lpPalette_New->lpLabels = (LPTSTR)Alloc((long)lpPalette_New->LabelsLength);
if (!lpPalette_New->lpLabels)
	{
	Palette_FreeUp(lpPalette_New);
	return(NULL);
	}
lpLabels = lpPalette_New->lpLabels;
nColors = 0;
for (i = 0; i < nPalettes; ++i)
	{
	lpPalette = Palette_Get(lpHeadPalette, NULL, i);
	// copy the colors to the new palette
	for (nEntry = 0; nEntry < lpPalette->iColors; ++nEntry)
		lpPalette_New->lpColorInfo[nEntry+nColors] =
						lpPalette->lpColorInfo[nEntry];
	// copy the labels to the new palette
	lpSrc = lpPalette->lpLabels;
	if (lpSrc)
		lpEnd = lpSrc + lpPalette->LabelsLength;
	else
		lpEnd = NULL;
	while (lpSrc < lpEnd)
		{
		Length = *(lpSrc+1);
		copy((LPTR)lpSrc, (LPTR)lpLabels, (int)Length);
		Index = GetIndex(lpLabels);
		SetIndex(lpLabels, Index+nColors);
		lpSrc += Length;
		lpLabels += Length;
		}
	nColors += lpPalette->iColors;
	}
return(lpPalette_New);
}

/************************************************************************/
LPPALETTE Palette_CreateFromColorMap( LPCOLORMAP lpColorMap )
/************************************************************************/
{
LPPALETTE lpPalette;
int i;

lpPalette = Palette_New(_T(""));
if (!lpPalette)
	return(NULL);
lpPalette->lpColorInfo = (LPCOLORINFO)Alloc(sizeof(COLORINFO) * lpColorMap->NumEntries);
if (!lpPalette->lpColorInfo)
	{
	Palette_FreeUp(lpPalette);
	return(NULL);
	}

lpPalette->iColors = lpColorMap->NumEntries;
for (i = 0; i < lpPalette->iColors; ++i)
	{
	lpPalette->lpColorInfo[i].rgb = lpColorMap->RGBData[i];
	lpPalette->lpColorInfo[i].ColorSpace = CS_RGB;
	}
return(lpPalette);
}
