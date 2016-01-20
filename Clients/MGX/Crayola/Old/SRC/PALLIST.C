//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"

/************************************************************************/
LPPALETTE NewPalette(LPSTR lpPaletteName)
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
LPPALETTE LinkPalette(
/************************************************************************/
LPPALETTE 	lpHeadPalette,
LPPALETTE 	lpPalette)
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
void FreeUpPalette(LPPALETTE lpPalette)
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
int NumPalettes(LPPALETTE lpHeadPalette)
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
LPPALETTE GetPalette(
/************************************************************************/
LPPALETTE	lpHeadPalette,
LPSTR 		lpPaletteName,
int			index)
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
BOOL GetPaletteName(
/************************************************************************/
LPPALETTE	lpHeadPalette,
int			iPalette,
LPSTR 		lpPaletteName)
{
LPPALETTE lpPalette;

lstrcpy(lpPaletteName, "");
lpPalette = GetPalette(lpHeadPalette, NULL, iPalette);
if (lpPalette)
	{
	lstrcpy(lpPaletteName, lpPalette->szName);
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
int GetPaletteNum(
/************************************************************************/
LPPALETTE	lpHeadPalette,
LPSTR 		lpPaletteName,
LPPALETTE	lpPalette)
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
BOOL SetPaletteEntryLabel(
/************************************************************************/
LPPALETTE	lpHeadPalette,
int			iPalette,
int			iEntry,
LPSTR		lpLabel)
{
LPPALETTE lpPalette;
LPTR lpDst, lpEnd, lpNewLabels, lpSrc;
LPWORD lpWord;
WORD LabelsLength, LengthBefore, LengthAfter, RemoveLength, Index;
BYTE Type, Length;

lpPalette = GetPalette(lpHeadPalette, NULL, iPalette);
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
		lpWord = (LPWORD)lpSrc;
		Index = *(lpWord+1);
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
	lpNewLabels = Alloc((long)LabelsLength);
	if (!lpNewLabels)
		return(FALSE);
	lpSrc = lpPalette->lpLabels;
	lpDst = lpNewLabels;
	if (LengthBefore)
		copy(lpSrc, lpDst, LengthBefore);
	lpDst += LengthBefore;
	lpSrc += (LengthBefore + RemoveLength);
	if (lstrlen(lpLabel))
		{
		Length = lstrlen(lpLabel)+1+4;
		*lpDst = 0x01;
		if (lstrlen(lpPalette->szFormat) > 0)
			*lpDst |= 0x80;
		*(lpDst+1) = Length;
		lpWord = (LPWORD)lpDst;
		*(lpWord+1) = iEntry;
		lstrcpy((LPSTR)lpDst+4, lpLabel);
		lpDst += Length;
		}
	if (LengthAfter)
		copy(lpSrc, lpDst, LengthAfter);
	FreeUp(lpPalette->lpLabels);
	lpPalette->lpLabels = lpNewLabels;
	lpPalette->LabelsLength = LabelsLength;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL SetPaletteEntryColor(
/************************************************************************/
LPPALETTE	lpHeadPalette,
int			iPalette,
int			iEntry,
LPCOLORINFO lpColorInfo)
{
LPPALETTE lpPalette;

lpPalette = GetPalette(lpHeadPalette, NULL, iPalette);
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
BOOL GetPaletteEntryColor(
/************************************************************************/
LPPALETTE	lpHeadPalette,
int			iPalette,
int			iEntry,
LPCOLORINFO lpColorInfo)
{
LPPALETTE lpPalette;

lpPalette = GetPalette(lpHeadPalette, NULL, iPalette);
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
BOOL GetPaletteEntryLabel(
/************************************************************************/
LPPALETTE	lpHeadPalette,
int			iPalette,
int			iEntry,
LPTR		lpColorLabel)
{
LPPALETTE lpPalette;
LPTR lpLabels, lpLast;
WORD Index;
BYTE Type, Length;

lpColorLabel[0] = '\0';
lpPalette = GetPalette(lpHeadPalette, NULL, iPalette);
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
		Type = *lpLabels++;
		Length = *lpLabels++;
		Index = *((LPWORD)lpLabels);
		lpLabels += 2;
		if (Index == iEntry)
			{
			FormatPaletteEntryLabel(lpPalette, Type, (LPSTR)lpLabels,
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
void FormatPaletteEntryLabel(
/************************************************************************/
LPPALETTE	lpPalette,
BYTE		Type,
LPSTR		lpLabel,
LPTR		lpColorLabel)
{
LPWORD lpNum;
STRING szString;
LPTR lpFormat, lpString, lpCLabel;
char c1;

if (lstrlen(lpPalette->szFormat) <= 0 || Type & 0x80)
	lstrcpy((LPSTR)lpColorLabel, lpLabel);
else if (Type == 0 || Type == 1)
	{
	if (Type == 0)
		{
		lpNum = (LPWORD)lpLabel;
		wsprintf(szString, "%u", *lpNum);
		}
	else
		lstrcpy(szString, lpLabel);
	lpFormat = (LPTR)lpPalette->szFormat;
	lpCLabel = (LPTR)lpColorLabel;
	lpString = (LPTR)szString;
	while ((c1 = *lpFormat++) != '\0')
		{
		if (c1 != '%')
			{
			*lpCLabel++ = c1;
			}
		else
			{
			while (*lpString != '\0')
				*lpCLabel++ = *lpString++;
			}
		}
	*lpCLabel = '\0';
	}
else
	lpColorLabel[0] = '\0';
}

/************************************************************************/
int FindPaletteEntryLabel(
/************************************************************************/
LPPALETTE	lpHeadPalette,
int			iPalette,
LPINT		lpEntry,
LPTR		lpColorLabel,
int 		iWhich)
{
LPPALETTE lpPalette;
LPTR lpLabels, lpLast;
WORD Index;
BYTE Type, Length;
STRING szLabel, szFound;
int	iNum;

iNum = 0;
szFound[0] = '\0';
lpPalette = GetPalette(lpHeadPalette, NULL, iPalette);
if (lpPalette && lpPalette->iColors && lpPalette->lpLabels &&
	lstrlen((LPSTR)lpColorLabel))
	{
	lpLabels = lpPalette->lpLabels;
	lpLast = lpLabels + lpPalette->LabelsLength;
	while (lpLabels < lpLast)
		{
		Type = *lpLabels++;
		Length = *lpLabels++;
		Index = *((LPWORD)lpLabels);
		lpLabels += 2;
		FormatPaletteEntryLabel(lpPalette, Type, (LPSTR)lpLabels, (LPTR)szLabel);
		if (MatchCriteria(szLabel, lpColorLabel))
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
lstrcpy((LPSTR)lpColorLabel, szFound);
return(iNum);
}

/************************************************************************/
BOOL MatchCriteria(
/************************************************************************/
LPSTR	lpLabel,
LPTR	lpCriteria)
{
char c1, c2;

while ((c1 = *lpCriteria++) != '\0')
	{
	if (IsCharUpper(c1))
		AnsiLowerBuff(&c1,1);
	if (c1 == '*')    // wildcard
		return(TRUE);
	else if ((c2 = *lpLabel++) == '\0')
		return(FALSE);
	else
		{
		if (c1 != '?')
			{
			if (IsCharUpper(c2))
				AnsiLowerBuff(&c2,1);
			if (c1 != c2)
				return(FALSE);
			}
		}
	}
return(*lpLabel == '\0');
}

/************************************************************************/
LPPALETTE CombinePalettes(LPPALETTE lpHeadPalette)
/************************************************************************/
{
LPPALETTE lpNewPalette, lpPalette;
int i, nEntry, nColors;
int nPalettes;
LPTR lpLabels, lpSrc, lpEnd;
LPWORD lpWord;
BYTE Length;

lpNewPalette = NewPalette("");
if (!lpNewPalette)
	return(NULL);
// take palette format of first palette in list
lstrcpy(lpNewPalette->szFormat, lpHeadPalette->szFormat);

nPalettes = NumPalettes(lpHeadPalette);
for (i = 0; i < nPalettes; ++i)
	{
	lpPalette = GetPalette(lpHeadPalette, NULL, i);
	lpNewPalette->iColors += lpPalette->iColors;
	lpNewPalette->LabelsLength += lpPalette->LabelsLength;
	}
lpNewPalette->lpColorInfo =
	(LPCOLORINFO)Alloc((long)lpNewPalette->iColors*(long)sizeof(COLORINFO));
if (!lpNewPalette->lpColorInfo)
	{
	FreeUpPalette(lpNewPalette);
	return(NULL);
	}
lpNewPalette->lpLabels = Alloc((long)lpNewPalette->LabelsLength);
if (!lpNewPalette->lpLabels)
	{
	FreeUpPalette(lpNewPalette);
	return(NULL);
	}
lpLabels = lpNewPalette->lpLabels;
nColors = 0;
for (i = 0; i < nPalettes; ++i)
	{
	lpPalette = GetPalette(lpHeadPalette, NULL, i);
	// copy the colors to the new palette
	for (nEntry = 0; nEntry < lpPalette->iColors; ++nEntry)
		lpNewPalette->lpColorInfo[nEntry+nColors] =
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
		copy(lpSrc, lpLabels, (int)Length);
		lpWord = (LPWORD)lpLabels;
		*(lpWord+1) += nColors;
		lpSrc += Length;
		lpLabels += Length;
		}
	nColors += lpPalette->iColors;
	}
return(lpNewPalette);
}
