//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "objio.h"

#define TIF_MM (0x4D4D)
#define TIF_II (0x4949)

/************************************************************************/
BOOL objrdr( LPSTR lpFileName, int outDepth, LPINT lpDataType,
			LPOBJECTLIST lpObjList)
/************************************************************************/
{
	return(ReadObjData(lpFileName, lpObjList, outDepth, lpDataType,
				NULL/*lpScaleRect*/,NULL /*lpFullFileX*/, NULL /*lpFullFileY*/, NULL /*lpFullResolution*/));
}

/************************************************************************/
BOOL ReadObjData( LPSTR lpFileName, LPOBJECTLIST lpObjList, int outDepth,
		LPINT lpDataType, LPRECT lpScaleRect, LPINT lpFullFileX, LPINT lpFullFileY, LPINT lpFullResolution)

/************************************************************************/
{
LPFRAME lpFrame;
LPOBJECT lpObject;
int ifh;
LPLONG lngptr;
LPWORD shtptr;
int nObjects, i, idFileType, nPasses;
WORD wBytes, wByteOrder, wVersion;
long lObjSize, lObjStart;
LPOBJOFFSETS lpOffsets;
OBJECT Obj;
LFIXED xrate, yrate;
int opix, olin, baseW, baseH;
LPRECT lpLoadRect;

xrate = yrate = UNITY;
lpOffsets = NULL;

if ( !lpObjList )
	return( FALSE );

lpObjList->lpHead = lpObjList->lpTail = NULL;
if ( (ifh = _lopen(lpFileName, OF_READ)) < 0)
	{
	Message( IDS_EOPEN, lpFileName );
	return( FALSE );
	}

// Read in header info
wBytes = OBJ_HDR_SIZE;
if ( _lread(ifh, LineBuffer[0], wBytes) != wBytes )
	goto BadRead;
shtptr = (LPWORD)LineBuffer[0];
wByteOrder = GetNextWord(&shtptr);	/* byte order is LSB,MSB */
if (wByteOrder != TIF_II && wByteOrder != TIF_MM)
	goto BadRead;
wVersion  = GetNextWord(&shtptr);	/* Version Number */
nObjects  = GetNextWord(&shtptr);	/* Number of Objects */
lngptr    = (LPLONG)shtptr;
lObjSize  = GetNextLong(&lngptr);	/* size of object data */
lObjStart = GetNextLong(&lngptr);	/* start of object data */

if (!(lpOffsets = (LPOBJOFFSETS)Alloc(sizeof(OBJOFFSETS)*nObjects)))
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
_llseek (ifh, lObjStart, 0);
wBytes = (WORD)lObjSize;
nPasses = 0;
for (i = 0; i < nObjects; ++i)
	{
	if ( _lread(ifh, LineBuffer[0], wBytes) != wBytes )
		goto BadRead;
	shtptr = (LPWORD)LineBuffer[0];

	Obj.ObjectType 		= (OBJECT_TYPE)GetNextWord(&shtptr);
	Obj.rObject.left 	= GetNextWord(&shtptr);
	Obj.rObject.top 	= GetNextWord(&shtptr);
	Obj.rObject.right 	= GetNextWord(&shtptr);
	Obj.rObject.bottom 	= GetNextWord(&shtptr);
	Obj.Opacity 		= GetNextWord(&shtptr);
	Obj.MergeMode 		= (MERGE_MODE)GetNextWord(&shtptr);
	Obj.wGroupID 		= GetNextWord(&shtptr);

	if (i==0 && lpScaleRect)
	{
		opix = RectWidth(lpScaleRect);
		olin = RectHeight(lpScaleRect);
		baseW = RectWidth(&Obj.rObject);
		baseH = RectHeight(&Obj.rObject);
		xrate = ScaleToFit(&opix, &olin, baseW, baseH);
		if (opix > baseW || olin > baseH)
		{ // No upsizing allowed
			opix = baseW;
			olin = baseH;
		}
		xrate = FGET( opix, baseW );
		yrate = FGET( olin, baseH );
	}
	// scale the object rect
	Obj.rObject.left	= FMUL(Obj.rObject.left, xrate);
	Obj.rObject.top 	= FMUL(Obj.rObject.top, yrate);
	Obj.rObject.right 	= FMUL(Obj.rObject.right, xrate);
	Obj.rObject.bottom 	= FMUL(Obj.rObject.bottom, yrate);

	if (!(lpObject = ObjCreateFromFrame(ST_PERMANENT, NULL, NULL, &Obj.rObject,
										Control.NoUndo)))
		{
		Message(IDS_EMEMALLOC);
		goto BadRead;
		}
	idFileType                = GetNextWord(&shtptr);
	lpOffsets[i].bInvert      = GetNextWord(&shtptr);
	lpOffsets[i].rMask.left   = GetNextWord(&shtptr);
	lpOffsets[i].rMask.top    = GetNextWord(&shtptr);
	lpOffsets[i].rMask.right  = GetNextWord(&shtptr);
	lpOffsets[i].rMask.bottom = GetNextWord(&shtptr);
	// scale the mask rect
	lpOffsets[i].rMask.left	  = FMUL(lpOffsets[i].rMask.left,   xrate);
	lpOffsets[i].rMask.top 	  = FMUL(lpOffsets[i].rMask.top,    yrate);
	lpOffsets[i].rMask.right  = FMUL(lpOffsets[i].rMask.right,  xrate);
	lpOffsets[i].rMask.bottom = FMUL(lpOffsets[i].rMask.bottom, yrate);
	lngptr = (LPLONG)shtptr;
	lpOffsets[i].lDataOffset  = GetNextLong(&lngptr);
	lpOffsets[i].lAlphaOffset = GetNextLong(&lngptr);
	shtptr = (LPWORD)lngptr;

	// versions after 1.1 saved selected state
	if (wVersion == 0x0101)
		Obj.fSelected = NO;
	else
		Obj.fSelected = GetNextWord(&shtptr);

    if (wVersion < 0x0103 || lObjSize == 41)
    {
        Obj.ObjectDataID = OBJECT_DATA_NONE;
        Obj.dwObjectData = 0;
    }
	else
    {
        Obj.ObjectDataID = GetNextWord(&shtptr);
        Obj.dwObjectData = *(LPDWORD)shtptr;
        shtptr += 2;
    }

	lpObject->ObjectType 	= Obj.ObjectType;
	lpObject->Opacity 		= Obj.Opacity;
	lpObject->MergeMode 	= Obj.MergeMode;
	lpObject->wGroupID 		= Obj.wGroupID;
	lpObject->fSelected 	= Obj.fSelected;
    lpObject->ObjectDataID 	= Obj.ObjectDataID;
    lpObject->dwObjectData 	= Obj.dwObjectData;

	ObjAddTail(lpObjList, (LPPRIMOBJECT)lpObject);

	++nPasses;
	if (lpOffsets[i].lAlphaOffset)
		++nPasses;
	}

ProgressBegin(nPasses, 0);
lpObject = NULL;
i = 0;
while (lpObject = (LPOBJECT)ObjGetNextObject(lpObjList, (LPPRIMOBJECT)lpObject, YES))
	{
	if (lpScaleRect && i!=0)
		{
		lpLoadRect = &lpObject->rObject;
		}
	else
		lpLoadRect = lpScaleRect;
	_llseek (ifh, lpOffsets[i].lDataOffset, 0);
	lpFrame = ReadTiffData(ifh, lpFileName, outDepth, lpDataType,
		NO/*bReadOnly*/, lpLoadRect, NO, lpFullFileX, lpFullFileY,
		lpFullResolution);
	if (i == 0)
		{
		lpDataType = NULL;
		lpFullFileX = NULL;
		lpFullFileY = NULL;
		lpFullResolution = NULL;
		}
	if (!lpFrame)
		goto BadRead;
	PixmapSetup(&lpObject->Pixmap, lpFrame, Control.NoUndo);
	if (!lpOffsets[i].lAlphaOffset)
		{
		++i;
		continue;
		}
	_llseek (ifh, lpOffsets[i].lAlphaOffset, 0);
	lpFrame = ReadTiffData(ifh, lpFileName, outDepth, lpDataType,
		NO/*bReadOnly*/, lpLoadRect, NO, NULL, NULL, NULL);
	if (!lpFrame)
		goto BadRead;
	lpObject->lpAlpha = MaskCreate(lpFrame, 0, 0, OFF, Control.NoUndo);
	if (!lpObject->lpAlpha)
		{
		Message(IDS_EMEMALLOC);
		goto BadRead;
		}
	lpObject->lpAlpha->bInvert = lpOffsets[i].bInvert;
	lpObject->lpAlpha->rMask = lpOffsets[i].rMask;
	++i;
	}

if ( lpOffsets )
	FreeUp( (LPTR)lpOffsets );
_lclose(ifh);
ProgressEnd();
return( TRUE );

BadRead:
while (lpObject = (LPOBJECT)ObjGetNextObject(lpObjList, NULL, YES))
	{
	ObjUnlinkObject(lpObjList, (LPPRIMOBJECT)lpObject);
	ObjFreeUp(lpObject);
	}
_lclose(ifh);
if ( lpOffsets )
	FreeUp( (LPTR)lpOffsets );
if (nPasses)
	ProgressEnd();
Message( IDS_EREAD, lpFileName );
return( FALSE );
}

/************************************************************************/
int objwrt(LPIMAGE lpImage, LPSTR lpFileName, int idFileType, LPRECT lpRect,
			int flag, BOOL fCompressed)
/************************************************************************/
{
LPOBJECT lpObject;
int ofp;
LPLONG lngptr;
LPWORD shtptr;
FNAME temp;
BOOL bEscapable;
int nObjects, i, nPasses;
long lObjStart;
WORD wBytes;
LPOBJOFFSETS lpOffsets;

if (!lpImage)
	return( -1 );

lpObject = NULL;
nObjects = nPasses = 0;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (lpObject->lpAlpha && (nObjects || Save.bSaveMask))
		++nPasses;
	++nObjects;
	++nPasses;
	}
if (!(lpOffsets = (LPOBJOFFSETS)Alloc(sizeof(OBJOFFSETS)*nObjects)))
	{
	Message(IDS_EMEMALLOC);
	return(-1);
	}
bEscapable = !FileExists(lpFileName);

#ifdef _MAC
// The Mac file system can't handle the following pathname:
//		Bad --->        "DIR:FILE.EXT"
//		Needs to be -> ":DIR:FILE.EXT"
// Add the additional ':' here to specify starting at our directory.
if (lpFileName[0] != ':')
{
	STRING szName;
	lstrcpy(szName, lpFileName);
	lstrcpy(lpFileName+1, szName);
	lpFileName[0] = ':';
}
#endif

if ((ofp = _lcreat(lpFileName,0)) < 0)
	{
	FreeUp((LPTR)lpOffsets);
	Message( IDS_EOPEN, lpFileName );
	return(-1);
	}

ProgressBegin(nPasses, 0);

// Fill in header info
shtptr = (LPWORD)LineBuffer[0];
SetNextWord(&shtptr, 0x4949);					/* byte order is LSB,MSB */
SetNextWord(&shtptr, OBJ_VERSION);				/* Version Number */
SetNextWord(&shtptr, ImgCountObjects(lpImage));	/* Number of Objects */
lngptr = (LPLONG)shtptr;
SetNextLong(&lngptr, OBJ_DATA_SIZE);			/* size of object data */
SetNextLong(&lngptr, OBJ_HDR_SIZE);				/* start of object data */

// Write out header info
wBytes = (LPTR)lngptr - (LPTR)LineBuffer[0];
if ( _lwrite(ofp, LineBuffer[0], wBytes) != wBytes )
	goto BadWrite;

lObjStart = _llseek (ofp, 0L, 1);
wBytes = OBJ_DATA_SIZE * ImgCountObjects(lpImage);
if ( _lwrite(ofp, LineBuffer[0], wBytes) != wBytes )
	goto BadWrite;
lpObject = NULL;
i = 0;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	lpOffsets[i].lDataOffset = _llseek (ofp, 0L, 1);
	lpOffsets[i].lAlphaOffset = 0L;
	if (TiffWriteFrame(ofp, lpFileName, NULL, lpObject->Pixmap.EditFrame,
				NULL, flag, fCompressed, bEscapable) < 0)
		goto BadWrite;
	if (!lpObject->lpAlpha || (!i && !Save.bSaveMask))
		{
		++i;
		continue;
		}
	lpOffsets[i].lAlphaOffset = _llseek (ofp, 0L, 1);
	if (TiffWriteFrame(ofp, lpFileName, NULL, lpObject->lpAlpha->Pixmap.EditFrame,
				NULL, IDC_SAVECT, fCompressed, bEscapable) < 0)
		goto BadWrite;
	++i;
	}

// fill in object data information and write it
_llseek (ofp, lObjStart, 0);
lpObject = NULL;
i = 0;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	shtptr = (LPWORD)LineBuffer[0];
	SetNextWord(&shtptr, lpObject->ObjectType);
	SetNextWord(&shtptr, lpObject->rObject.left);
	SetNextWord(&shtptr, lpObject->rObject.top);
	SetNextWord(&shtptr, lpObject->rObject.right);
	SetNextWord(&shtptr, lpObject->rObject.bottom);
	SetNextWord(&shtptr, lpObject->Opacity);
	SetNextWord(&shtptr, lpObject->MergeMode);
	SetNextWord(&shtptr, lpObject->wGroupID);
	SetNextWord(&shtptr, idFileType);
	if (lpObject->lpAlpha)
		{
		SetNextWord(&shtptr, lpObject->lpAlpha->bInvert);
		SetNextWord(&shtptr, lpObject->lpAlpha->rMask.left);
		SetNextWord(&shtptr, lpObject->lpAlpha->rMask.top);
		SetNextWord(&shtptr, lpObject->lpAlpha->rMask.right);
		SetNextWord(&shtptr, lpObject->lpAlpha->rMask.bottom);
		}
	else
		{
		SetNextWord(&shtptr, 0);
		SetNextWord(&shtptr, 0);
		SetNextWord(&shtptr, 0);
		SetNextWord(&shtptr, 0);
		SetNextWord(&shtptr, 0);
		}
	lngptr = (LPLONG)shtptr;
	SetNextLong(&lngptr, lpOffsets[i].lDataOffset);
	SetNextLong(&lngptr, lpOffsets[i].lAlphaOffset);
	shtptr = (LPWORD)lngptr;
	SetNextWord(&shtptr, lpObject->fSelected);
    SetNextWord(&shtptr, lpObject->ObjectDataID);
	lngptr = (LPLONG)shtptr;
    SetNextLong(&lngptr, lpObject->dwObjectData);
	shtptr = (LPWORD)lngptr;

	++i;
	// Write out Object data
	wBytes = OBJ_DATA_SIZE;;
	if ( _lwrite(ofp, LineBuffer[0], wBytes) != wBytes )
		goto BadWrite;
	}

FreeUp( (LPTR)lpOffsets );
_lclose(ofp);
ProgressEnd();
return( 0 );

BadWrite:
_lclose(ofp);
FreeUp( (LPTR)lpOffsets );
lstrcpy(temp,lpFileName);
FileDelete(temp);
if (nPasses)
	ProgressEnd();
return( -1 );
}


/************************************************************************/
BOOL ReadObjHeader(LPSTR lpFileName, LPINT lpDataType, LPTIFFHEADER lpHeader )
/************************************************************************/
{
	int ifh;
	LPLONG lngptr;
	LPWORD shtptr;
	int nObjects;
	WORD wBytes, wByteOrder, wVersion;
	long lObjSize, lObjStart;
	LONG lDataOffset;
	BOOL bRet = FALSE;

	if ( (ifh = _lopen(lpFileName, OF_READ)) < 0)
		{
		Message( IDS_EOPEN, lpFileName );
		return( FALSE );
		}

	// Read in header info
	wBytes = OBJ_HDR_SIZE;
	if ( _lread(ifh, LineBuffer[0], wBytes) != wBytes )
		goto BadRead;
	shtptr     = (LPWORD)LineBuffer[0];
	wByteOrder = GetNextWord(&shtptr);	/* byte order is LSB,MSB */
	wVersion   = GetNextWord(&shtptr);	/* Version Number */
	nObjects   = GetNextWord(&shtptr);	/* Number of Objects */
	lngptr     = (LPLONG)shtptr;
	lObjSize   = GetNextLong(&lngptr);	/* size of object data */
	lObjStart  = GetNextLong(&lngptr);	/* start of object data */

	_llseek (ifh, lObjStart, 0);
	wBytes = (WORD)lObjSize;
	if ( _lread(ifh, LineBuffer[0], wBytes) != wBytes )
		goto BadRead;
	shtptr = (LPWORD)LineBuffer[0];

   	shtptr += 8;	// Obj data
   	shtptr++;		// Data type
   	shtptr += 5;	// lpOffsets before lDataOffset
	lngptr = (LPLONG)shtptr;
    lDataOffset = *lngptr;

	_llseek (ifh, lDataOffset, 0);
	bRet = ReadTiffHeader( ifh, lpFileName, -1, lpDataType, NO/*bReadOnly*/,
		lpHeader );

BadRead:
	_lclose(ifh);
	return( bRet );
}


WORD GetNextWord(LPWORD *lplpWord)
{
// Get the next WORD and increment the pointer.
#ifdef _MAC
	swapw(*lplpWord);
#endif	
	WORD w = **lplpWord;
	(*lplpWord)++;
	return w;
}

long GetNextLong(LPLONG *lplpLong)
{
// Get the next LONG and increment the pointer.
#ifdef _MAC
	swapl((LPDWORD)*lplpLong);
#endif	
	long l = **lplpLong;
	(*lplpLong)++;
	return l;
}

void SetNextWord(LPWORD *lplpWord, WORD w)
{
// Set the WORD and increment the pointer.
	**lplpWord = w;
#ifdef _MAC
	swapw(*lplpWord);
#endif
	(*lplpWord)++;	
}

void SetNextLong(LPLONG *lplpLong, long l)
{
// Set the LONG and increment the pointer.
	**lplpLong = l;
#ifdef _MAC
	swapl((LPDWORD)*lplpLong);
#endif
	(*lplpLong)++;	
}


