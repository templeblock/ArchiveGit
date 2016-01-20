//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

typedef struct _calmaps
	{
	MAP		ScanMap[CALSCAN_MAPS];
	MAP		PrintMap[CAL_MAPS];
	BYTE	Lut[4][LUTSIZE];	// Gray, Red, Cyan
	FLOAT	CalPnts[256];
	FLOAT	TmpPnts[256];
	} CALMAPS, FAR *LPCALMAPS;

LOCAL void mapinfo_to_map(LPMAPINFO lpInfo, LPMAP lpMap);

LOCAL STATUS_CODE ApplyMaps(LPIMAGE lpImage, LPMAPS lpMaps, ITEMID idDirty, LPRECT lpUpdateRect, LPUPDATE_TYPE lpUpdateType);
LOCAL STATUS_CODE ApplyCalibration(LPIMAGE lpImage, ITEMID dirty,	LPSTR lpScanMap, LPSTR lpPrintMap, LPRECT lpUpdateRect, LPUPDATE_TYPE lpUpdateType);
LOCAL void ApplyMapsProc(int y, int left, int right, LPTR lpDst, LPTR lpSrc,
							int depth, LPMAPS lpMaps);
LOCAL void MapLineGrayProc( int y, int x1, int x2, LPTR lpSrc, LPTR lpDst, int depth, LPMAPS lpMaps );
LOCAL void ApplyMaps8(LPTR lpSrc, int dx, LPMAPS lpMaps);
LOCAL void ApplyMaps24(LPTR lpSrc, int dx, LPMAPS lpMaps);
LOCAL void ApplyMaps32(LPTR lpSrc, int dx, LPMAPS lpMaps);
LOCAL void LineCalMapProc(int yy, int left, int right, LPTR lpSrc, LPTR lpDst, int depth, LPCALMAPS lpMaps );
LOCAL void PaletteMapPixmap(LPPIXMAP lpPixmap, CFrameTypeConvert *pTypeConvert);

/************************************************************************/
BOOL ContBritImage(LPIMAGE lpImage, LPCONTBRIT_PARMS lpParms)
/************************************************************************/
{
	LPMAPS lpMaps;
	int i;
	RECT rUpdate;
	UPDATE_TYPE UpdateType;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART)
		return(FALSE);
   else if (type == FDT_PALETTECOLOR && ImgGetMask(lpImage))
		return(FALSE);

	if (!(lpMaps = (LPMAPS)Alloc((long)sizeof(MAPS))))
	{
		lpParms->Common.StatusCode = SC_MEMERROR;
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	ResetMapEx( &lpMaps->MasterMap, MAPPOINTS, NO, YES);
	ResetMap( &lpMaps->RedMap,    MAPPOINTS, NO ); // sometimes Cyan
	ResetMap( &lpMaps->GreenMap,  MAPPOINTS, NO ); // sometimes Magenta
	ResetMap( &lpMaps->BlueMap,   MAPPOINTS, NO ); // sometimes Yellow
	ResetMap( &lpMaps->BlackMap,   MAPPOINTS, NO );
	ResetMap( &lpMaps->HueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->SatMap,    MAPPOINTS, NO );
	lpMaps->MasterMap.Contrast = lpParms->Contrast;
	for (i = 0; i < NUM_BANDS; ++i)
		lpMaps->MasterMap.Brightness[i] = lpParms->Brightness[i];
	MakeMap(&lpMaps->MasterMap);

	lpParms->Common.StatusCode = ApplyMaps(lpImage, lpMaps, lpParms->Common.idDirty, &rUpdate, &UpdateType);
	if (UpdateType)
	{
		lpParms->Common.UpdateType = UpdateType;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	
	FreeUp(lpMaps);
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/
BOOL BalanceImage(LPIMAGE lpImage, LPBALANCE_PARMS lpParms)
/************************************************************************/
{
	LPMAPS lpMaps;
	int i;
	RECT rUpdate;
	UPDATE_TYPE UpdateType;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART || type == FDT_GRAYSCALE)
		return(FALSE);
   else if (type == FDT_PALETTECOLOR && ImgGetMask(lpImage))
		return(FALSE);

	if (!(lpMaps = (LPMAPS)Alloc((long)sizeof(MAPS))))
	{
		lpParms->Common.StatusCode = SC_MEMERROR;
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	ResetMapEx( &lpMaps->MasterMap, MAPPOINTS, NO, YES);
	ResetMap( &lpMaps->RedMap,    MAPPOINTS, NO ); // sometimes Cyan
	ResetMap( &lpMaps->GreenMap,  MAPPOINTS, NO ); // sometimes Magenta
	ResetMap( &lpMaps->BlueMap,   MAPPOINTS, NO ); // sometimes Yellow
	ResetMap( &lpMaps->BlackMap,   MAPPOINTS, NO );
	ResetMap( &lpMaps->HueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->SatMap,    MAPPOINTS, NO );
	lpMaps->MasterMap.Contrast = lpParms->MasterContrast;
	for (i = 0; i < NUM_BANDS; ++i)
		lpMaps->MasterMap.Brightness[i] = lpParms->MasterBrightness[i];
	lpMaps->MasterMap.Reverse = lpParms->fMasterReverse;
	MakeMap(&lpMaps->MasterMap);
	lpMaps->RedMap.Contrast = lpParms->RedContrast;
	for (i = 0; i < NUM_BANDS; ++i)
		lpMaps->RedMap.Brightness[i] = lpParms->RedBrightness[i];
	lpMaps->RedMap.Reverse = lpParms->fRedReverse;
	MakeMap(&lpMaps->RedMap);
	lpMaps->GreenMap.Contrast = lpParms->GreenContrast;
	for (i = 0; i < NUM_BANDS; ++i)
		lpMaps->GreenMap.Brightness[i] = lpParms->GreenBrightness[i];
	lpMaps->GreenMap.Reverse = lpParms->fGreenReverse;
	MakeMap(&lpMaps->GreenMap);
	lpMaps->BlueMap.Contrast = lpParms->BlueContrast;
	for (i = 0; i < NUM_BANDS; ++i)
		lpMaps->BlueMap.Brightness[i] = lpParms->BlueBrightness[i];
	lpMaps->BlueMap.Reverse = lpParms->fBlueReverse;
	MakeMap(&lpMaps->BlueMap);

	lpMaps->BlackMap.Contrast = lpParms->BlackContrast;
	for (i = 0; i < NUM_BANDS; ++i)
		lpMaps->BlackMap.Brightness[i] = lpParms->BlackBrightness[i];
	lpMaps->BlackMap.Reverse = lpParms->fBlackReverse;
	MakeMap(&lpMaps->BlackMap);

	lpParms->Common.StatusCode = ApplyMaps(lpImage, lpMaps, lpParms->Common.idDirty, &rUpdate, &UpdateType);
	if (UpdateType)
	{
		lpParms->Common.UpdateType = UpdateType;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	FreeUp(lpMaps);
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/
BOOL MapImage(LPIMAGE lpImage, LPMODIFYMAP_PARMS lpParms)
/************************************************************************/
{
	LPMAPS lpMaps;
	RECT rUpdate;
	UPDATE_TYPE UpdateType;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART)
		return(FALSE);
   else if (type == FDT_PALETTECOLOR && ImgGetMask(lpImage))
		return(FALSE);

	if (!(lpMaps = (LPMAPS)Alloc((long)sizeof(MAPS))))
	{
		lpParms->Common.StatusCode = SC_MEMERROR;
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	ResetMap( &lpMaps->MasterMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->RedMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->GreenMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlackMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->HueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->SatMap,    MAXPOINTS, NO );

	mapinfo_to_map(&lpParms->MasterInfo, &lpMaps->MasterMap);
	mapinfo_to_map(&lpParms->RedInfo, &lpMaps->RedMap);
	mapinfo_to_map(&lpParms->GreenInfo, &lpMaps->GreenMap);
	mapinfo_to_map(&lpParms->BlueInfo, &lpMaps->BlueMap);
	mapinfo_to_map(&lpParms->BlackInfo, &lpMaps->BlackMap);

	MakeMap(&lpMaps->MasterMap);
	MakeMap(&lpMaps->RedMap);
	MakeMap(&lpMaps->GreenMap);
	MakeMap(&lpMaps->BlueMap);
	MakeMap(&lpMaps->BlackMap);

	lpParms->Common.StatusCode = ApplyMaps(lpImage, lpMaps, lpParms->Common.idDirty, &rUpdate, &UpdateType);
	if (UpdateType)
	{
		lpParms->Common.UpdateType = UpdateType;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	FreeUp(lpMaps);
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}


//************************************************************************
//			mapinfo_to_map
// DESCRIPTION:
//			Copies the relevent data from the given MAPINFO structure 
//			into the given MAP structure.  lpMap should be already 
//			initialized.
//************************************************************************
LOCAL void mapinfo_to_map(LPMAPINFO lpInfo, LPMAP lpMap)
//************************************************************************
{
	lpMap->iCurves = lpInfo->iCurves; 
	lpMap->Reverse = lpInfo->Reverse; 
	lpMap->gamma = lpInfo->gamma; 
	lpMap->Points = lpInfo->Points; 
	copy((LPTR)&lpInfo->Pnt, (LPTR)&lpMap->Pnt, sizeof(lpMap->Pnt));
}

/************************************************************************/
BOOL PosterizeImage(LPIMAGE lpImage, LPPOSTERIZE_PARMS lpParms)
/************************************************************************/
{
	LPMAPS lpMaps;
	RECT rUpdate;
	UPDATE_TYPE UpdateType;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART)
		return(FALSE);
   else if (type == FDT_PALETTECOLOR && ImgGetMask(lpImage))
		return(FALSE);
	if (!(lpMaps = (LPMAPS)Alloc((long)sizeof(MAPS))))
	{
		lpParms->Common.StatusCode = SC_MEMERROR;
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	ResetMap( &lpMaps->MasterMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->RedMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->GreenMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlackMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->HueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->SatMap,    MAXPOINTS, NO );

	lpMaps->MasterMap.Levels = lpParms->MasterInfo.Levels;
	lpMaps->MasterMap.Threshold = lpParms->MasterInfo.Threshold;
	lpMaps->RedMap.Levels = lpParms->RedInfo.Levels;
	lpMaps->RedMap.Threshold = lpParms->RedInfo.Threshold;
	lpMaps->GreenMap.Levels = lpParms->GreenInfo.Levels;
	lpMaps->GreenMap.Threshold = lpParms->GreenInfo.Threshold;
	lpMaps->BlueMap.Levels = lpParms->BlueInfo.Levels;
	lpMaps->BlueMap.Threshold = lpParms->BlueInfo.Threshold;
	lpMaps->BlackMap.Levels = lpParms->BlackInfo.Levels;
	lpMaps->BlackMap.Threshold = lpParms->BlackInfo.Threshold;

	MakeMap(&lpMaps->MasterMap);
	MakeMap(&lpMaps->RedMap);
	MakeMap(&lpMaps->GreenMap);
	MakeMap(&lpMaps->BlueMap);
	MakeMap(&lpMaps->BlackMap);

	lpParms->Common.StatusCode = ApplyMaps(lpImage, lpMaps, lpParms->Common.idDirty, &rUpdate, &UpdateType);
	if (UpdateType)
	{
		lpParms->Common.UpdateType = UpdateType;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	FreeUp(lpMaps);
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/
BOOL HueImage(LPIMAGE lpImage, LPHUE_PARMS lpParms)
/************************************************************************/
{
	LPMAPS lpMaps;
	RECT rUpdate;
	UPDATE_TYPE UpdateType;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART || type == FDT_GRAYSCALE)
		return(FALSE);
   else if (type == FDT_PALETTECOLOR && ImgGetMask(lpImage))
		return(FALSE);
	if (!(lpMaps = (LPMAPS)Alloc((long)sizeof(MAPS))))
	{
		lpParms->Common.StatusCode = SC_MEMERROR;
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	ResetMap( &lpMaps->MasterMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->RedMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->GreenMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlackMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->HueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->SatMap,    MAXPOINTS, NO );

	lpMaps->HueMap.Points = lpParms->HueInfo.Points;
	copy((LPTR)&lpParms->HueInfo.Pnt, (LPTR)&lpMaps->HueMap.Pnt, sizeof(lpMaps->HueMap.Pnt));
	lpMaps->SatMap.Brightness[0] = lpParms->Saturation;
	lpMaps->MasterMap.Brightness[0] = lpParms->Brightness;

	MakeMap(&lpMaps->HueMap);
	MakeMap(&lpMaps->SatMap);
	MakeMap(&lpMaps->MasterMap);

	lpParms->Common.StatusCode = ApplyMaps(lpImage, lpMaps, lpParms->Common.idDirty, &rUpdate, &UpdateType);
	if (UpdateType)
	{
		lpParms->Common.UpdateType = UpdateType;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	FreeUp(lpMaps);
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/
BOOL ApplyCalImage(LPIMAGE lpImage, LPAPPLYCAL_PARMS lpParms)
/************************************************************************/
{
	RECT rUpdate;
	UPDATE_TYPE UpdateType;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART)
		return(FALSE);
   else if (type == FDT_PALETTECOLOR && ImgGetMask(lpImage))
		return(FALSE);
	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	lpParms->Common.StatusCode = ApplyCalibration(lpImage, lpParms->Common.idDirty,
						lpParms->szScanMap, lpParms->szPrintMap, &rUpdate, &UpdateType);
	if (UpdateType)
	{
		lpParms->Common.UpdateType = UpdateType;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/
BOOL ToneImage(LPIMAGE lpImage, LPTONE_PARMS lpParms)
/************************************************************************/
{
	LPMAPS lpMaps;
	RECT rUpdate;
	UPDATE_TYPE UpdateType;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART)
		return(FALSE);
   else if (type == FDT_PALETTECOLOR && ImgGetMask(lpImage))
		return(FALSE);
	if (!(lpMaps = (LPMAPS)Alloc((long)sizeof(MAPS))))
	{
		lpParms->Common.StatusCode = SC_MEMERROR;
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	ResetMap( &lpMaps->MasterMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->RedMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->GreenMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->BlackMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->HueMap,    MAXPOINTS, NO );
	ResetMap( &lpMaps->SatMap,    MAXPOINTS, NO );

	mapinfo_to_map(&lpParms->MasterInfo, &lpMaps->MasterMap);
	mapinfo_to_map(&lpParms->RedInfo, &lpMaps->RedMap);
	mapinfo_to_map(&lpParms->GreenInfo, &lpMaps->GreenMap);
	mapinfo_to_map(&lpParms->BlueInfo, &lpMaps->BlueMap);
	mapinfo_to_map(&lpParms->BlackInfo, &lpMaps->BlackMap);

	MakeMap(&lpMaps->MasterMap);
	MakeMap(&lpMaps->RedMap);
	MakeMap(&lpMaps->GreenMap);
	MakeMap(&lpMaps->BlueMap);
	MakeMap(&lpMaps->BlackMap);

	lpParms->Common.StatusCode = ApplyMaps(lpImage, lpMaps, lpParms->Common.idDirty, &rUpdate, &UpdateType);
	if (UpdateType)
	{
		lpParms->Common.UpdateType = UpdateType;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	FreeUp(lpMaps);
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/
LOCAL STATUS_CODE ApplyMaps(LPIMAGE lpImage, LPMAPS lpMaps, ITEMID idDirty,
						LPRECT lpUpdateRect, LPUPDATE_TYPE lpUpdateType)
/************************************************************************/
{
	ENGINE Engine;
	FRMTYPEINFO TypeInfo;
	LPCOLORMAP lpColorMap;
	STATUS_CODE StatusCode;

	AstralSetRectEmpty(lpUpdateRect);
	*lpUpdateType = UT_NONE;
	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
	{
		lpColorMap = FrameCopyColorMap(TypeInfo.ColorMap);
		if (!lpColorMap)
		{
			Message(IDS_EMEMALLOC);
			return(SC_MEMERROR);
		}
		ApplyMaps24((LPTR)lpColorMap->RGBData, lpColorMap->NumEntries, lpMaps);
		StatusCode = ApplyColorMap(lpImage, lpColorMap, FALSE, DT_NONE, idDirty);
		FrameDestroyColorMap(lpColorMap);
		if (StatusCode == SC_SUCCESS)
			*lpUpdateType = UT_DATATYPE;
	}
	else
	{
		SetEngineDef(&Engine);
		Engine.lpDataProc = (LPDATAPROC)ApplyMapsProc;
		Engine.lpParam = lpMaps;
		StatusCode = LineEngineSelObj(lpImage, &Engine, idDirty);
		if (!AstralIsRectEmpty(&Engine.rUpdate))
		{
			*lpUpdateRect = Engine.rUpdate;
			*lpUpdateType = UT_AREA;
		}
	}
	return(StatusCode);
}

/************************************************************************/
STATUS_CODE ApplyColorMap(LPIMAGE lpImage, LPCOLORMAP lpColorMap, BOOL fReMap,
							DITHER_TYPE DitherType,	ITEMID idDirty)
/************************************************************************/
{
	UNDO_TYPE UndoType;
	int iMaxWidth;
	FRMTYPEINFO OldTypeInfo, NewTypeInfo;
	LPOBJECT lpObject;
	RECT rEdit;
	CFrameTypeConvert TypeConvert;

	UndoType = UT_COLORMAP;
	if (fReMap)
		UndoType |= UT_DATA;
	if (!ImgEditInit(lpImage, ET_ALLOBJECTS, UndoType, NULL))
		return(SC_UNDOERROR);

	ProgressBegin(ImgCountObjects(lpImage), idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	if (fReMap)
	{	
		ImgGetTypeInfo(lpImage, &OldTypeInfo);
		FrameSetTypeInfo(&NewTypeInfo, FDT_PALETTECOLOR, lpColorMap);

		iMaxWidth = 0;
		lpObject = NULL;
		while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		{
			if (RectWidth(&lpObject->rObject) > iMaxWidth)
				iMaxWidth = RectWidth(&lpObject->rObject);
		}
		if (!TypeConvert.Init(OldTypeInfo, NewTypeInfo, iMaxWidth, DitherType))
		{
			ProgressEnd();
			Message(IDS_EMEMALLOC);
			return(SC_MEMERROR);
		}
	}
	lpObject = NULL;
	while (lpObject = ImgGetNextObjectEx(lpImage, lpObject, YES, NO, YES))
	{
		if (fReMap)
			PaletteMapPixmap(&lpObject->Pixmap, &TypeConvert);
		FrameSetColorMap(ObjGetEditFrame(lpObject), lpColorMap);
		rEdit = lpObject->rObject;
		ImgEditedObject(lpImage, lpObject, idDirty, &rEdit);
	}
	ImgColorMapChanged(lpImage);
	ProgressEnd();

	return(SC_SUCCESS);
}

/***********************************************************************/
LOCAL void PaletteMapPixmap(LPPIXMAP lpPixmap, CFrameTypeConvert *pTypeConvert)
/***********************************************************************/
{
int y, w, h;
LPTR lp;

ProgressBegin(1, 0);
w = FrameXSize(lpPixmap->EditFrame);
h = FrameYSize(lpPixmap->EditFrame);
for (y = 0; y < h; ++y)
	{
	AstralClockCursor(y, h, NO);
	lp = PixmapPtr(lpPixmap, PMT_EDIT, 0, y, YES);
	if (!lp)
		break;
	pTypeConvert->ConvertData(lp, lp, y, w);
	}
ProgressEnd();
}

/************************************************************************/
LOCAL void ApplyMapsProc(int y, int left, int right, LPTR lpDst, LPTR lpSrc,
							int depth, LPMAPS lpMaps)
/************************************************************************/
{
int iCount = right - left + 1;

switch(depth)
	{
	case 0 :
	case 1 :
		copy (lpDst, lpSrc, iCount);
		ApplyMaps8(lpSrc, iCount, lpMaps);
	break;

	case 3 :
		copy (lpDst, lpSrc, iCount*3);
		ApplyMaps24(lpSrc, iCount, lpMaps);
	break;

	case 4 :
		copy (lpDst, lpSrc, iCount*4);
		ApplyMaps32(lpSrc, iCount, lpMaps);
	break;
	}
}

/************************************************************************/
LOCAL void ApplyMaps8(LPTR lpSrc, int dx, LPMAPS lpMaps)
/************************************************************************/
{
MapLine8(lpMaps->MasterMap.Lut, lpSrc, dx);
}

/************************************************************************/
LOCAL void ApplyMaps24(LPTR lpSrc, int dx, LPMAPS lpMaps)
/************************************************************************/
{
// do actual mapping
if (!lpMaps->HueMap.bModified && !lpMaps->SatMap.bModified)
	MapLine24(	lpMaps->MasterMap.Lut,
				lpMaps->RedMap.Lut,
				lpMaps->GreenMap.Lut,
				lpMaps->BlueMap.Lut,
				lpSrc, dx);
else
	MapLine24HSL(	lpMaps->HueMap.Lut,
					lpMaps->SatMap.Lut,
					lpMaps->MasterMap.Lut,
					lpSrc, dx);
}

/************************************************************************/
LOCAL void ApplyMaps32(LPTR lpSrc, int dx, LPMAPS lpMaps)
/************************************************************************/
{
if (!lpMaps->HueMap.bModified && !lpMaps->SatMap.bModified)
	MapLine32(	lpMaps->MasterMap.Lut,
				lpMaps->RedMap.Lut,
				lpMaps->GreenMap.Lut,
				lpMaps->BlueMap.Lut,
				lpMaps->BlackMap.Lut,
				lpSrc, dx);
else
	MapLine32HSL(	lpMaps->HueMap.Lut,
					lpMaps->SatMap.Lut,
					lpMaps->MasterMap.Lut,
					lpSrc, dx);
}


//***********************************************************************
LOCAL void MapLineGrayProc( int y, int x1, int x2, LPTR lpSrc, LPTR lpDst, int depth, LPMAPS lpMaps )
//***********************************************************************
{
int dx;
LPTR lpLut;

dx = x2 - x1 + 1;

lpLut = lpMaps->MasterMap.Lut;
switch(depth)
	{
	case 0 :
	case 1 :
		while ( dx-- > 0 )
			{
			*lpDst++ = 255 - lpLut[ 255 - (*lpSrc++) ];
			}
	break;

	case 3 :
		while ( dx-- > 0 )
			{
			*lpDst++ = 255 - lpLut[ 255 - (*lpSrc++) ];
			*lpDst++ = 255 - lpLut[ 255 - (*lpSrc++) ];
			*lpDst++ = 255 - lpLut[ 255 - (*lpSrc++) ];
			}
	break;

	case 4 :
		while ( dx-- > 0 )
			{
			*lpDst++ = lpLut[ (*lpSrc++) ];
			*lpDst++ = lpLut[ (*lpSrc++) ];
			*lpDst++ = lpLut[ (*lpSrc++) ];
			*lpDst++ = lpLut[ (*lpSrc++) ];
			}
	break;
	}
}

//************************************************************************
LOCAL STATUS_CODE ApplyCalibration(
	LPIMAGE		lpImage,
	ITEMID      dirty,
	LPSTR       lpScanMap,
	LPSTR       lpPrintMap,
	LPRECT		lpUpdateRect,
	LPUPDATE_TYPE lpUpdateType)
//************************************************************************
{
	FNAME  szFileName;
	ENGINE Engine;
	LPCALMAPS lpMaps;
	BOOL fScanMap, fPrintMap;
	FRMTYPEINFO TypeInfo;
	LPCOLORMAP lpColorMap;
	STATUS_CODE StatusCode;
	int i;
	
	ASSERT(lpImage);

	AstralSetRectEmpty(lpUpdateRect);
	*lpUpdateType = UT_NONE;

	ImgGetTypeInfo(lpImage, &TypeInfo);
	lpMaps = (LPCALMAPS)Alloc(sizeof(CALMAPS));
	if (!lpMaps)
	{
		Message(IDS_EMEMALLOC);
		return(SC_MEMERROR);
	}
	for (i = 0; i < CALSCAN_MAPS; ++i)
		ResetMap( &lpMaps->ScanMap[i], CALPOINTS, NO );

	if (fScanMap = (lstrlen(lpScanMap) != 0))
	{
		if ( LookupExtFileN( lpScanMap, szFileName, IDN_SCANMAP, NO ) )
		{
			// Remember that the maps load in XRGB order
			if (!LoadMap( 	&lpMaps->ScanMap[CALSCAN_GRAY],
							&lpMaps->ScanMap[CALSCAN_RED],
							&lpMaps->ScanMap[CALSCAN_GREEN],
							&lpMaps->ScanMap[CALSCAN_BLUE],
							szFileName ))
			{
				FreeUp(lpMaps);
				return(SC_READERROR);
			}
			if (TypeInfo.DataType == FDT_CMYKCOLOR)
			{
				// make maps work on cmyk
				for (i = 0; i < CALSCAN_MAPS; ++i)
					ReverseMap(&lpMaps->ScanMap[i], FALSE);
			}
		}
		else
		{
			fScanMap = NO;
		}
	}

	for (i = 0; i < CAL_MAPS; ++i)
		ResetMap( &lpMaps->PrintMap[i], CALPOINTS, NO );
	if (fPrintMap = (lstrlen(lpPrintMap) != 0))
	{
		if ( LookupExtFileN( lpPrintMap, szFileName, IDN_CALMAP, NO ) )
		{
			// Remember that the maps load in MCMYK order
			if (!LoadCalMap(	&lpMaps->PrintMap[CAL_MASTER],
								&lpMaps->PrintMap[CAL_CYAN],
								&lpMaps->PrintMap[CAL_MAGENTA],
								&lpMaps->PrintMap[CAL_YELLOW],
								&lpMaps->PrintMap[CAL_BLACK],
								szFileName ))
			{
				FreeUp(lpMaps);
				return(SC_READERROR);
			}
			if (TypeInfo.DataType != FDT_CMYKCOLOR)
			{
				// make maps work on rgb
				for (i = 0; i < CAL_MAPS; ++i)
					ReverseMap(&lpMaps->PrintMap[i], FALSE);
			}
		}
		else
		{
			fPrintMap = NO;
			lpPrintMap = NULL;
		}
	}

	if (!fScanMap && !fPrintMap)
	{
		FreeUp(lpMaps);
		return(SC_SUCCESS); // user selected no maps??? 
	}

	switch (TypeInfo.DataType)
	{
		case FDT_LINEART:
		case FDT_GRAYSCALE:
			MakeFloatMap(&lpMaps->ScanMap[CALSCAN_GRAY], lpMaps->CalPnts);
			MakeFloatMap(&lpMaps->PrintMap[CAL_BLACK], lpMaps->TmpPnts);
			MapCombine(lpMaps->CalPnts, lpMaps->TmpPnts);
			MapCreateLut(lpMaps->CalPnts, lpMaps->Lut[0]);
		break;

		case FDT_CMYKCOLOR:
			for (i = 0; i < 4; ++i)
			{
				if (i == 3) // black
					MakeFloatMap(&lpMaps->ScanMap[CALSCAN_GRAY], lpMaps->CalPnts);
				else
					MakeFloatMap(&lpMaps->ScanMap[CALSCAN_RED+i], lpMaps->CalPnts);
				MakeFloatMap(&lpMaps->PrintMap[CAL_MASTER], lpMaps->TmpPnts);
				MapCombine(lpMaps->CalPnts, lpMaps->TmpPnts);
				MakeFloatMap(&lpMaps->PrintMap[CAL_CYAN+i], lpMaps->TmpPnts);
				MapCombine(lpMaps->CalPnts, lpMaps->TmpPnts);
				MapCreateLut(lpMaps->CalPnts, lpMaps->Lut[i]);
			}
		break;

		default:
		case FDT_RGBCOLOR:
		case FDT_PALETTECOLOR:
			for (i = 0; i < 3; ++i)
			{
				MakeFloatMap(&lpMaps->ScanMap[CALSCAN_RED+i], lpMaps->CalPnts);
				MakeFloatMap(&lpMaps->PrintMap[CAL_MASTER], lpMaps->TmpPnts);
				MapCombine(lpMaps->CalPnts, lpMaps->TmpPnts);
				MakeFloatMap(&lpMaps->PrintMap[CAL_CYAN+i], lpMaps->TmpPnts);
				MapCombine(lpMaps->CalPnts, lpMaps->TmpPnts);
				MapCreateLut(lpMaps->CalPnts, lpMaps->Lut[i]);
			}
		break;
	}


	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
	{
		lpColorMap = FrameCopyColorMap(TypeInfo.ColorMap);
		if (!lpColorMap)
		{
			Message(IDS_EMEMALLOC);
			return(SC_MEMERROR);
		}
		LineCalMapProc(0, 0, lpColorMap->NumEntries-1,
							(LPTR)lpColorMap->RGBData,
							(LPTR)lpColorMap->RGBData,
							3,
							lpMaps);
		StatusCode = ApplyColorMap(lpImage, lpColorMap, FALSE, DT_NONE, dirty);
		FrameDestroyColorMap(lpColorMap);
		if (StatusCode == SC_SUCCESS)
			*lpUpdateType = UT_DATATYPE;
	}
	else
	{
		SetEngineDef(&Engine);
		Engine.lpDataProc = (LPDATAPROC)LineCalMapProc;

		Engine.lpParam = lpMaps;
		StatusCode = LineEngineSelObj(lpImage, &Engine, dirty);
		if (!AstralIsRectEmpty(&Engine.rUpdate))
		{
			*lpUpdateRect = Engine.rUpdate;
			*lpUpdateType = UT_AREA;
		}
	}
	FreeUp(lpMaps);
	return(StatusCode);
}

//************************************************************************
static void LineCalMapProc( int yy, int left, int right, LPTR lpSrc, LPTR lpDst, int depth, LPCALMAPS lpMaps )
//************************************************************************
{
int iCount;
LPTR lpLut0, lpLut1, lpLut2, lpLut3;

iCount = right - left + 1;

lpLut0 = lpMaps->Lut[0];
lpLut1 = lpMaps->Lut[1];
lpLut2 = lpMaps->Lut[2];
lpLut3 = lpMaps->Lut[3];
switch(depth)
	{
	case 0 :
	case 1 :
		while (iCount-- > 0)
			{
			*lpDst++ = lpLut0[ *lpSrc++ ];
			}
	break;

	case 3 :
		while (iCount-- > 0)
			{
			*lpDst++ = lpLut0[ *lpSrc++ ];
			*lpDst++ = lpLut1[ *lpSrc++ ];
			*lpDst++ = lpLut2[ *lpSrc++ ];
			}
	break;

	case 4 :
		while (iCount-- > 0)
			{
			*lpDst++ = lpLut0[ *lpSrc++ ];
			*lpDst++ = lpLut1[ *lpSrc++ ];
			*lpDst++ = lpLut2[ *lpSrc++ ];
			*lpDst++ = lpLut3[ *lpSrc++ ];
			}
	break;
	}
}

