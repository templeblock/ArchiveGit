//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "bitmapio.h"
#include "objio.h"
#include "pp5io.h"
#include "cpo.h"
#include "object.h"
#include "pixmap.h"
#include "mask.h"

#define DATA_BUF_SIZE 2048L

#define GETSHORT(result, shtptr) \
	(result=(INT16)*shtptr++)
#define GETWORD(result, shtptr) \
	(result=(WORD)*shtptr++)
#define GETLONG(result, shtptr) \
	{wLo = *shtptr++; wHi = *shtptr++;result = MAKELONG(wLo, wHi);}
#define GETINT(result, shtptr, b32bit) \
	{if (b32Bit) \
	{wLo = *shtptr++; wHi = *shtptr++;result = MAKELONG(wLo, wHi);} \
	else \
	result=(INT16)*shtptr++;}
	
#define PUTSHORT(val, shtptr) \
	(*shtptr++ = (INT16)val)
#define PUTWORD(val, shtptr) \
	(*shtptr++ = (WORD)val)
#define PUTLONG(val, shtptr) \
	{*shtptr++ = LOWORD(val);*shtptr++ = HIWORD(val);}

/************************************************************************/
BOOL CReadBitmap::PP4Read()
/************************************************************************/
{
LPOBJECT lpObject;
LPWORD shtptr;
int nObjects, i, idFileType, nPasses, res;
WORD wBytes, wByteOrder, wVersion, wLo, wHi;
long lObjSize, lObjStart;
LPOBJOFFSETS lpOffsets;
OBJECT Obj;
LFIXED xrate, yrate;
LPTR lpData;
OBJECTLIST ObjList;
LPFRAME lpFrame;
FRMTYPEINFO AlphaType;
BOOL 	fRet = FALSE;
long	lTop;
CFile*	pTheFile;

if ((pTheFile = OpenFile()) == NULL)
	return(FALSE);

TRY
	{
	xrate = yrate = FUNITY;
	ObjList.lpHead = ObjList.lpTail = NULL;
	lpData = NULL;
	lpOffsets = NULL;
	nPasses = 0;

	lTop = pTheFile->GetPosition();

	if (!(lpData = Alloc(DATA_BUF_SIZE)))
		{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto BadPP4;
		}

	// Read in header info
	wBytes = OBJ_HDR_SIZE;
	if ( pTheFile->Read(lpData, wBytes) != wBytes )
		goto BadRead;

	shtptr = (LPWORD)lpData;
	wByteOrder = *shtptr++;				/* byte order is LSB,MSB */
	GETWORD (wVersion,shtptr);			/* Version Number */
	GETSHORT(nObjects,shtptr);				/* Number of Objects */
	GETLONG(lObjSize, shtptr); 
	GETLONG(lObjStart, shtptr); 			/* start of object data */

	if (!(lpOffsets = (LPOBJOFFSETS)Alloc(sizeof(OBJOFFSETS)*nObjects)))
		{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto BadPP4;
		}
	pTheFile->Seek (lObjStart+lTop, CFile::begin);
	wBytes = (WORD)lObjSize;
	for (i = 0; i < nObjects; ++i)
		{
		if ( pTheFile->Read(lpData, wBytes) != wBytes )
			goto BadRead;
		shtptr = (LPWORD)lpData;
	
		Obj.ObjectType = (OBJECT_TYPE)*shtptr++;
		GETSHORT( Obj.rObject.left,		shtptr);
		GETSHORT( Obj.rObject.top,		shtptr);
		GETSHORT( Obj.rObject.right,	shtptr);
		GETSHORT( Obj.rObject.bottom,	shtptr);
		GETWORD ( Obj.Opacity,			shtptr);
		Obj.MergeMode = (MERGE_MODE)*shtptr++;
		GETWORD ( Obj.wGroupID,			shtptr);

		if (!(lpObject = ObjCreateFromFrame(ST_PERMANENT, NULL, NULL, &Obj.rObject,
											m_fNoUndo)))
			{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto BadPP4;
			}
		GETWORD (idFileType,				shtptr);
		GETWORD (lpOffsets[i].bInvert,		shtptr);
		GETSHORT(lpOffsets[i].rMask.left,	shtptr);
		GETSHORT(lpOffsets[i].rMask.top,	shtptr);
		GETSHORT(lpOffsets[i].rMask.right,	shtptr);
		GETSHORT(lpOffsets[i].rMask.bottom,	shtptr);

		GETLONG(lpOffsets[i].lDataOffset,	shtptr);
		GETLONG(lpOffsets[i].lAlphaOffset,	shtptr);

		// versions after 1.1 saved selected state
		if (wVersion == 0x0101)
			Obj.fSelected = NO;
		else
			GETWORD (Obj.fSelected, shtptr);

		lpObject->ObjectType = Obj.ObjectType;
		lpObject->Opacity = Obj.Opacity;
		lpObject->MergeMode = Obj.MergeMode;
		lpObject->wGroupID = Obj.wGroupID;
		lpObject->fSelected = Obj.fSelected;

		ObjAddTail(&ObjList, lpObject);

		++nPasses;
		if (lpOffsets[i].lAlphaOffset)
			++nPasses;
		}

	FrameSetTypeInfo(&AlphaType, FDT_GRAYSCALE);

	ProgressBegin(nPasses);
	lpObject = NULL;
	i = 0;
	while (lpObject = ObjGetNextObject(&ObjList, lpObject, YES))
		{
		CReadTiff TiffRead(this, pTheFile, m_bEscapable, &m_cmsInfo);

		TiffRead.Progress(0, 0, FALSE);

		if (i==0)
			{
			if (m_SampleMode == modeSampleRect)
				{
				int opix = RectWidth(&m_rArea)-1;
				int olin = RectHeight(&m_rArea)-1;
				int baseW = RectWidth(&lpObject->rObject)-1;
				int baseH = RectHeight(&lpObject->rObject)-1;
				m_xrate = ScaleToFit(&opix, &olin, baseW, baseH);
				if (opix > baseW || olin > baseH)
					{ // No upsizing allowed
					opix = baseW;
					olin = baseH;
					}
				m_xrate = FGET( opix, baseW );
				m_yrate = FGET( olin, baseH );
				}
			}

		if (m_SampleMode == modeSampleRect && (m_xrate != FUNITY))
			{
			// scale the object rect
			lpObject->rObject.left	= FMUL(lpObject->rObject.left, m_xrate);
			lpObject->rObject.top 	= FMUL(lpObject->rObject.top, m_yrate);
			lpObject->rObject.right 	= FMUL(lpObject->rObject.right, m_xrate);
			lpObject->rObject.bottom 	= FMUL(lpObject->rObject.bottom, m_yrate);

			// scale the mask rect
			lpOffsets[i].rMask.left	 = FMUL(lpOffsets[i].rMask.left	 , xrate);
			lpOffsets[i].rMask.top 	 = FMUL(lpOffsets[i].rMask.top 	 , yrate);
			lpOffsets[i].rMask.right = FMUL(lpOffsets[i].rMask.right , xrate);
			lpOffsets[i].rMask.bottom= FMUL(lpOffsets[i].rMask.bottom, yrate);
			TiffRead.SetSampling(&lpObject->rObject);
			}
		else
		if (m_SampleMode == modeSampleRes)
			TiffRead.SetSampling(m_SampleRes);

		pTheFile->Seek (lpOffsets[i].lDataOffset+lTop, CFile::begin);
		lpFrame = TiffRead.ReadFrame(m_cmsInfo.dst);
		if (!lpFrame)
			goto BadPP4;

		if (i == 0)
			{
			res = FrameResolution(lpFrame);
			lpObject->rObject.left = lpObject->rObject.top = 0;
			m_iWidth = TiffRead.GetFileWidth();
			m_iHeight = TiffRead.GetFileHeight();
			m_iRes = TiffRead.GetFileResolution();
			TiffRead.GetCMSSrcTypeInfo(&m_cmsInfo.src);
			}
		else
			FrameSetResolution(lpFrame, res);

		if (m_SampleMode == modeSampleRes)
			{
			if (i == 0)
				{
				m_SampleRes = FrameResolution(lpFrame);
		 		ResConvertRect(m_iRes, m_SampleRes, &lpOffsets[i].rMask);
				}
			else
				{
				// scale the object location
				ResConvertUL(m_iRes, m_SampleRes, &lpObject->rObject.left, &lpObject->rObject.top);
				}
			}
		lpObject->rObject.right = lpObject->rObject.left + FrameXSize(lpFrame) - 1;
		lpObject->rObject.bottom = lpObject->rObject.top + FrameYSize(lpFrame) - 1;

		PixmapSetup(&lpObject->Pixmap, lpFrame, m_fNoUndo);
		if (lpOffsets[i].lAlphaOffset)
			{
			CReadTiff TiffReadAlpha(this, pTheFile, m_bEscapable, &m_cmsInfo);

			// we set the sampling again for some unknown reason
			if (m_SampleMode == modeSampleRect && (m_xrate != FUNITY))
				TiffReadAlpha.SetSampling(&lpObject->rObject);
			else
			if (m_SampleMode == modeSampleRes)
				TiffReadAlpha.SetSampling(m_SampleRes);

			pTheFile->Seek (lpOffsets[i].lAlphaOffset+lTop, CFile::begin);

			lpFrame = TiffReadAlpha.ReadFrame(AlphaType);
			if (!lpFrame)
				goto BadPP4;
			FrameSetResolution(lpFrame, res);
			lpObject->lpAlpha = MaskCreate(lpFrame, 0, 0, OFF, m_fNoUndo);
			if (!lpObject->lpAlpha)
				{
				SetError(BEC_errMemory);
				//Message(IDS_EMEMALLOC);
				goto BadPP4;
				}
			lpObject->lpAlpha->bInvert = lpOffsets[i].bInvert;
			lpObject->lpAlpha->rMask = lpOffsets[i].rMask;
			}
		++i;
		}

	m_ObjList = ObjList;

	fRet = TRUE;
	goto Exit;
	}
CATCH_ALL(e)
	{
	goto BadRead;
	}
END_CATCH_ALL


BadRead:
SetError(BEC_errFileRead);
//if (m_lpFileName)
//	Message (IDS_EREAD, m_lpFileName);
BadPP4:
while (lpObject = (LPOBJECT)ObjGetNextObject(&ObjList, NULL, YES))
	{
	ObjUnlinkObject(&ObjList, lpObject);
	ObjFreeUp(lpObject);
	}

Exit:
CloseFile(pTheFile);
FreeUp(lpData);
FreeUp( (LPTR)lpOffsets );
if (nPasses)
	ProgressEnd();
return( fRet );
}

/************************************************************************/
BOOL CWriteBitmap::PP4Write()
/************************************************************************/
{
LPOBJECT lpObject;
LPWORD shtptr;
int nObjects, i, nPasses, res;
long lObjStart;
WORD wBytes;
LPTR lpData;
LPOBJOFFSETS lpOffsets;
LPCOLORMAP lpColorMap = NULL;
TIFFOPTIONS options;
FRMTYPEINFO inType;
BOOL fRet = FALSE;
LPFRAME lpFrame;
CFile 	theFile;
CFile*	pTheFile;

ASSERT(m_lpObjList);
ASSERT(m_lpOptions);

LPPPFFOPTIONS lpOptions = (LPPPFFOPTIONS)m_lpOptions;

lpObject = m_lpObjList->lpHead;
lpFrame = ObjGetEditFrame(lpObject);

nObjects = nPasses = 0;
while (lpObject)
	{
	if (!lpObject->fDeleted)
		{
		if (lpObject->lpAlpha && (nObjects || lpOptions->bSaveMask))
			++nPasses;
		++nObjects;
		++nPasses;
		}
	lpObject = (LPOBJECT)lpObject->lpNext;
	}
if (!(lpOffsets = (LPOBJOFFSETS)Alloc(sizeof(OBJOFFSETS)*nObjects)))
	{
	SetError(BEC_errMemory);
	//Message(IDS_EMEMALLOC);
	return(FALSE);
	}
if (!(lpData = Alloc(DATA_BUF_SIZE)))
	{
	FreeUp(lpOffsets);
	SetError(BEC_errMemory);
	//Message(IDS_EMEMALLOC);
	return(FALSE);
	}
if (m_pFile)
	pTheFile = m_pFile;
else
	{
	if (!theFile.Open(m_lpFileName, CFile::modeCreate|CFile::modeWrite))
		{
		SetError(BEC_errWriteFile);
		//Message (IDS_EWRITE, m_lpFileName);
		FreeUp(lpOffsets);
		FreeUp(lpData);
		return(FALSE);
		}
	pTheFile = &theFile;
	}

CWriteTiff TiffWrite(this, pTheFile, m_bEscapable);
options.TIFFCompress = lpOptions->PPFFCompress;
options.TIFFDiff = NO;
options.bWritePT = lpOptions->bWritePT;
options.bSaveMask = NO;
TiffWrite.SetOptions(&options);

TRY
	{
	ProgressBegin(nPasses);

	// are we converting to palette color
	FrameGetTypeInfo(lpFrame, &inType);
	if (m_DataType == FDT_PALETTECOLOR && 
		inType.DataType != FDT_PALETTECOLOR)
		{
		// see if we need to create a palette
		lpColorMap = FrameCreateColorMap();
		if (!lpColorMap)
			{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto BadPP4;
			}
		lpColorMap->NumEntries = 256;
		if (!CreateOptimizedPalette(lpObject, lpFrame,
									lpColorMap->RGBData,
									&lpColorMap->NumEntries,
									TRUE,
									NULL))
			{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto BadPP4;
			}
		}

	// Fill in header info
	shtptr = (LPWORD)lpData;
	*shtptr++ = 0x4949;								/* byte order is LSB,MSB */
	PUTWORD (OBJ_VERSION, shtptr); 					/* Version Number */
	PUTSHORT(nObjects, shtptr);		/* Number of Objects */

	PUTLONG(OBJ_DATA_SIZE, shtptr);					/* size of object data */
	PUTLONG(OBJ_HDR_SIZE, shtptr);					/* start of object data */

	// Write out header info
	wBytes = (LPTR)shtptr - (LPTR)lpData;
	pTheFile->Write(lpData, wBytes);
	lObjStart = pTheFile->GetPosition();
	wBytes = OBJ_DATA_SIZE * nObjects;
	pTheFile->Write(lpData, wBytes);

	lpObject = m_lpObjList->lpHead;
	i = 0;
	while (lpObject)
		{
		// don't do deleted objects
		if (lpObject->fDeleted)
			{
			lpObject = (LPOBJECT)lpObject->lpNext;
			continue;
			}
		if (i == 0)
			res = FrameResolution(ObjGetEditFrame(lpObject));
		else
			{
			FrameSetResolution(ObjGetEditFrame(lpObject), res);
			}

		lpOffsets[i].lDataOffset = pTheFile->GetPosition();
		lpOffsets[i].lAlphaOffset = 0L;

		TiffWrite.SetColorMap(lpColorMap);
		if (!TiffWrite.WriteFrame(ObjGetEditFrame(lpObject), m_DataType))
			goto BadPP4;

		if (!lpObject->lpAlpha || (!i && !lpOptions->bSaveMask))
			{
			++i;
			lpObject = (LPOBJECT)lpObject->lpNext;
			continue;
			}
		FrameSetResolution(lpObject->lpAlpha->Pixmap.EditFrame, res);

		lpOffsets[i].lAlphaOffset = pTheFile->GetPosition();
		TiffWrite.SetColorMap(NULL);
		if (!TiffWrite.WriteFrame(lpObject->lpAlpha->Pixmap.EditFrame, FDT_GRAYSCALE))
			goto BadPP4;
		++i;

		// get the next object
		lpObject = (LPOBJECT)lpObject->lpNext;
		}

	// fill in object data information and write it
	pTheFile->Seek(lObjStart, CFile::begin);
	lpObject = m_lpObjList->lpHead;
	i = 0;
	while (lpObject)
		{
		// don't do deleted objects
		if (lpObject->fDeleted)
			{
			lpObject = (LPOBJECT)lpObject->lpNext;
			continue;
			}
		shtptr = (LPWORD)lpData;
		*shtptr++ = (WORD)lpObject->ObjectType;
		PUTSHORT( lpObject->rObject.left,		shtptr);
		PUTSHORT( lpObject->rObject.top,		shtptr);
		PUTSHORT( lpObject->rObject.right,		shtptr);
		PUTSHORT( lpObject->rObject.bottom,		shtptr);
		PUTWORD ( lpObject->Opacity,			shtptr);
		*shtptr++ = (WORD)lpObject->MergeMode;
		PUTWORD ( lpObject->wGroupID,			shtptr);
		PUTWORD (BFT_TIFF,					shtptr);
		if (lpObject->lpAlpha)
			{
			PUTWORD (lpObject->lpAlpha->bInvert,		shtptr);
			PUTSHORT(lpObject->lpAlpha->rMask.left,		shtptr);
			PUTSHORT(lpObject->lpAlpha->rMask.top,		shtptr);
			PUTSHORT(lpObject->lpAlpha->rMask.right,	shtptr);
			PUTSHORT(lpObject->lpAlpha->rMask.bottom,	shtptr);
			}
		else
			{
			PUTWORD (0, shtptr);
			PUTSHORT(0, shtptr);
			PUTSHORT(0, shtptr);
			PUTSHORT(0, shtptr);
			PUTSHORT(0, shtptr);
			}
		PUTLONG(lpOffsets[i].lDataOffset, shtptr);
		PUTLONG(lpOffsets[i].lAlphaOffset, shtptr);
		PUTWORD(lpObject->fSelected, shtptr);
		++i;
		// Write out Object data
		wBytes = OBJ_DATA_SIZE;;
		pTheFile->Write(lpData, wBytes);

		// get the next object
		lpObject = (LPOBJECT)lpObject->lpNext;
		}

	fRet = TRUE;
	goto Exit;
	}
CATCH_ALL(e)
	{
	goto BadWrite;
	}
END_CATCH_ALL


BadWrite:
	SetError(BEC_errWriteFile);
//	if (m_lpFileName)
//		Message( IDS_EWRITE, m_lpFileName );
BadPP4:
if (!m_pFile) // did we create the file?
	{
	TRY
		{
		pTheFile->Close();
		if (!fRet )
			CFile::Remove(m_lpFileName);
		}
	END_TRY
	}

Exit:

if (lpColorMap)
	FrameDestroyColorMap(lpColorMap);
FreeUp( (LPTR)lpOffsets );
FreeUp(lpData);
if (nPasses)
	ProgressEnd();
return( fRet );
}


/************************************************************************/
LPTIFFHEADER CReadBitmap::ReadObjHeader()
/************************************************************************/
{
	LPWORD shtptr;
	int nObjects;
	WORD wBytes, wByteOrder, wVersion, wLo, wHi;
	long lObjSize, lObjStart;
	LONG lDataOffset;
	LPTR lpData;
	LPTIFFHEADER lpTH = NULL;
	CFile theFile;

	if (!theFile.Open(m_lpFileName, CFile::modeRead))
		{
		SetError(BEC_errOpen);
 		//Message (IDS_EOPEN, lpFileName);
		return(NULL);
		}

	TRY
		{
		if (!(lpData = Alloc(DATA_BUF_SIZE)))
			{
			theFile.Close();
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			return(NULL);
			}

		// Read in header info
		wBytes = OBJ_HDR_SIZE;
		if ( theFile.Read(lpData, wBytes) != wBytes )
			goto BadRead;
		shtptr = (LPWORD)lpData;
		wByteOrder = *shtptr++;				/* byte order is LSB,MSB */
		GETWORD (wVersion,shtptr);			/* Version Number */
		GETSHORT(nObjects,shtptr);				/* Number of Objects */
		GETLONG(lObjSize, shtptr); 
		GETLONG(lObjStart, shtptr); 			/* start of object data */

		theFile.Seek (lObjStart, CFile::begin);
		wBytes = (WORD)lObjSize;
		if ( theFile.Read(lpData, wBytes) != wBytes )
			goto BadRead;
		shtptr = (LPWORD)lpData;
		
  		shtptr += 8;	// Obj data
 		shtptr++;		// Data type
  		shtptr += 5;	// lpOffsets before lDataOffset

		GETLONG(lDataOffset,	shtptr);

		theFile.Seek (lDataOffset, CFile::begin);
		
		lpTH = ReadTiffHeader( &theFile);
	}
	END_TRY
	
BadRead:
	TRY
		{
		theFile.Close();
		}
	END_TRY
	FreeUp(lpData);
	return( lpTH );
}

