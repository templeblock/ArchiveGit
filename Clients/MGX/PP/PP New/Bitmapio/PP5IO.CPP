//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "bitmapio.h"
#include "pp5io.h"
#include "cpo.h"
#include "mask.h"
#include "object.h"
#include "list.h"

#define DATA_BUF_SIZE 2048L

#define GETSHORT(shtptr)	((INT16)*shtptr++)
#define GETWORD(shtptr) 	((WORD)*shtptr++)
#define GETLONG(shtptr)		(GetLong(&shtptr))
#define PUTSHORT(val, shtptr) (*shtptr++ = (INT16)(val))
#define PUTWORD(val, shtptr) (*shtptr++ = (WORD)(val))
#define PUTLONG(val, shtptr) {*shtptr++ = LOWORD(val);*shtptr++ = HIWORD(val);}

inline DWORD AFXAPI ReleaseInterface(LPUNKNOWN* lplpUnknown)
{
	ASSERT(lplpUnknown != NULL);
	if (*lplpUnknown != NULL)
	{
		DWORD dwRef = (*lplpUnknown)->Release();
		*lplpUnknown = NULL;
		return dwRef;
	}
	return 0;
}

CReadTiff::CReadTiff(CReadBitmap *pReader, CFile *pFile, BOOL bEscapable, LPCMSINFO pCmsInfo) : 
		CReadBitmap(pFile, BFT_TIFF, bEscapable, pCmsInfo)
		
{
	m_pReader = pReader;
	m_fLineArtAsGray = pReader->m_fLineArtAsGray; 
	m_fNoUndo = pReader->m_fNoUndo;
	m_fOneBitMask = pReader->m_fOneBitMask;
	m_CMSEnabled = pReader->m_CMSEnabled;
	m_bNoHiResRebuild = pReader->m_bNoHiResRebuild;
	m_fTGACMYK = pReader->m_fTGACMYK;
	m_DitherType = pReader->m_DitherType;
}

CWriteTiff::CWriteTiff(CWriteBitmap *pWriter, CFile *pFile, BOOL bEscapable) : 
	  	CWriteBitmap(pFile, BFT_TIFF, bEscapable)
		
{
	m_pWriter = pWriter;
	m_CMSEnabled = pWriter->m_CMSEnabled;
	m_DitherType = pWriter->m_DitherType;
}

/************************************************************************/
LOCAL LONG GetLong(LPPWORD lpshtptr)
/************************************************************************/
{
LPWORD shtptr = *lpshtptr;
WORD wLo = *shtptr++;
WORD wHi = *shtptr++;
*lpshtptr = shtptr;
return(MAKELONG(wLo, wHi));
}

/************************************************************************/
LOCAL BOOL ReadBlockInfo(CFile *pFile, LPDWORD lpBytes,
								LPPPFF_OPCODE lpOpCode)
/************************************************************************/
{
UINT numBytes;
WORD OpCode;

TRY
	{
	// read the size of the block
	numBytes = 4;
	if ( pFile->Read(lpBytes, numBytes) != numBytes )
		return(FALSE);
	// read the opcode of the block
	numBytes = 2;
	if ( pFile->Read(&OpCode, numBytes) != numBytes )
		return(FALSE);
	*lpOpCode = (PPFF_OPCODE)OpCode;
	return(TRUE);
	}
CATCH_ALL(e)
	{
	return(FALSE);
	}
END_CATCH_ALL
}

/************************************************************************/
CTextObject *ReadTextBlock(CFile *pFile, DWORD dwBytes)
/************************************************************************/
{
COLORSPACE Space;
COLORINFO Color;
BOOL Dirty;
int AntiAlias, Justification;
FONTID FontID;
CString szString;
LPSTR lpStr;
UINT Len, TextLen, ColorLen, NameLen;
char szColor[20];
POINT			DestPos;
int iBaseRes;

LPTR lpBuf = NULL;
TRY
	{
	lpBuf = Alloc(dwBytes);
	if (!lpBuf)
		AfxThrowMemoryException();
	if (pFile->Read(lpBuf, dwBytes) != dwBytes)
		AfxThrowFileException(CFileException::generic);

	LPWORD shtptr = (LPWORD)lpBuf;
	Dirty = GETWORD(shtptr);
	AntiAlias = GETWORD(shtptr);
	Justification = GETWORD(shtptr);
	FontID.FontDesc.fdPitchAndFamily = (BYTE)GETWORD(shtptr);
	FontID.FontDesc.fdCharSet = (BYTE)GETWORD(shtptr);
	FontID.FontDesc.FontType = (BYTE)GETWORD(shtptr);
	FontID.Size = GETWORD(shtptr);
	FontID.Italic = GETWORD(shtptr);
	FontID.Underline = GETWORD(shtptr);
	FontID.Strikeout = GETWORD(shtptr);
	FontID.Weight = GETWORD(shtptr);
	FontID.Angle = GETWORD(shtptr);
	Space = (COLORSPACE)GETWORD(shtptr);
   	DestPos.x = GETLONG(shtptr);
   	DestPos.y = GETLONG(shtptr);
	iBaseRes = GETWORD(shtptr);

	// skip passed 5 reserved words
	shtptr += 5;

	TextLen = GETLONG(shtptr);
	Len = TextLen;
	if (Len & 1)
		++Len;

	lpStr = szString.GetBuffer(TextLen);
	copy((LPTR)shtptr, (LPTR)lpStr, TextLen);
	shtptr += (Len/2);
	szString.ReleaseBuffer(TextLen);

	ColorLen = GETLONG(shtptr);
	Len = ColorLen;
	if (Len & 1)
		++Len;
	copy((LPTR)shtptr, (LPTR)szColor, ColorLen);
	shtptr += (Len/2);
	szColor[ColorLen] = '\0';
	SetColorInfoString(szColor, &Color, Space);

	NameLen = GETLONG(shtptr);
	Len = NameLen;
	if (Len & 1)
		++Len;
	copy((LPTR)shtptr, (LPTR)FontID.FontDesc.fdFaceName, NameLen);
	FontID.FontDesc.fdFaceName[NameLen] = '\0';

	CTextObject *lpText = new CTextObject(szString, Dirty, AntiAlias,
						&Color, Justification, &FontID, &DestPos, iBaseRes);
	if (!lpText)
		AfxThrowMemoryException();
	FreeUp(lpBuf);
	return(lpText);

	}
CATCH_ALL(e)
	{
	if (lpBuf)
		FreeUp(lpBuf);
	return(NULL);
	}
END_CATCH_ALL
}

/************************************************************************/
LPFRAME CReadBitmap::ReadFrameBlock(CFile *pFile, DWORD dwBytes, UINT SampleMode,
						LPRECT lpSampleRect, int SampleRes,
						FRMTYPEINFO outType, BOOL bEscapable, LPCMSINFO lpCmsInfo)
/************************************************************************/
{
LPFRAME lpFrame = NULL;

TRY
	{
	DWORD dwPos = pFile->GetPosition();
	CReadTiff TiffRead(this, pFile, bEscapable, lpCmsInfo);
	if (SampleMode == CReadBitmap::modeSampleRect)
		TiffRead.SetSampling(lpSampleRect);
	else
	if (SampleMode == CReadBitmap::modeSampleRes)
		TiffRead.SetSampling(SampleRes);
		
	lpFrame = TiffRead.ReadFrame(outType);
	pFile->Seek(dwPos+dwBytes, CFile::begin);
	return(lpFrame);
	}								  
CATCH_ALL(e)
	{
	}
END_CATCH_ALL

if (lpFrame)
	FrameClose(lpFrame);
return(NULL);
}

/************************************************************************/
LPOBJECT CReadBitmap::ReadObjectBlock(CFile *pFile, LPTR lpData, DWORD dwBytes,
								BOOL fBase, FRMTYPEINFO SrcTypeInfo)
/************************************************************************/
{
LPFRAME lpDataFrame = NULL;
LPFRAME lpAlphaFrame = NULL;
LPALPHA lpAlpha = NULL;
LPOBJECT lpObject = NULL;
BOOL fProgressSetup = FALSE;

TRY
	{
	UINT numBytes;
	OBJECT Obj;
	BOOL bInvert, fSaveAlpha, fHaveTextData;
	RECT rMask;
	PPFF_OPCODE OpCode;
	ITEMID idFileType;

	numBytes = dwBytes;
	if ( pFile->Read(lpData, numBytes) != numBytes )
		AfxThrowFileException(CFileException::generic);

	LPWORD shtptr = (LPWORD)lpData;
	Obj.ObjectType 		= (OBJECT_TYPE)GETWORD(shtptr);
	Obj.dwObjectID 			= GETLONG(shtptr);
	copy((LPTR)shtptr, (LPTR)Obj.szObjectName, sizeof(Obj.szObjectName));
	shtptr				+= (sizeof(Obj.szObjectName)/2);
	Obj.rObject.left 	= GETLONG(shtptr);
	Obj.rObject.top 	= GETLONG(shtptr);
	Obj.rObject.right 	= GETLONG(shtptr);
	Obj.rObject.bottom 	= GETLONG(shtptr);
	Obj.Opacity	  		= GETWORD(shtptr);
	Obj.MergeMode		= (MERGE_MODE)GETWORD(shtptr);
	Obj.wGroupID		= GETWORD(shtptr);

	idFileType 			= GETWORD(shtptr);
	bInvert				= GETWORD(shtptr);
	rMask.left			= GETLONG(shtptr);
	rMask.top			= GETLONG(shtptr);
	rMask.right			= GETLONG(shtptr);
	rMask.bottom 		= GETLONG(shtptr);

	Obj.fSelected		= GETWORD(shtptr);
	fSaveAlpha			= GETWORD(shtptr);
	fHaveTextData		= GETWORD(shtptr);
	Obj.bLocked 		= GETWORD(shtptr);

	ProgressBegin(fSaveAlpha ? 2 : 1);
	fProgressSetup = TRUE;

	// if sampling by rectangle, figure out sampling rates for
	// all objects from the base object.  fSetupSampling is TRUE
	// when the base object is being read
	// if sampling by resolution, sample the data frame of the base
	// object using the resolution and then setup the sampling rates
	// to read in the base's alpha and all other objects
	if (fBase)
		{
		if (m_SampleMode == modeSampleRect)
			{
			int opix = RectWidth(&m_rArea);
			int olin = RectHeight(&m_rArea);
			int baseW = RectWidth(&Obj.rObject);
			int baseH = RectHeight(&Obj.rObject);
			m_xrate = ScaleToFit(&opix, &olin, baseW, baseH);
			if (opix > baseW || olin > baseH)
				{ // No upsizing allowed
				opix = baseW;
				olin = baseH;
				}
			m_xrate = FGET( opix, baseW );
			m_yrate = FGET( olin, baseH );
			}
		else
			{
			// No upsizing allowed
			if (m_SampleRes > m_iCurrentRes)
				m_SampleRes = m_iCurrentRes;
			}
		}

	// insure that base always starts at 0,0
	if (fBase)
		Obj.rObject.left = Obj.rObject.top = 0;
		
	if (m_SampleMode == modeSampleRect && (m_xrate != FUNITY))
		{
		Obj.rObject.left	= FMUL(Obj.rObject.left, m_xrate);
		Obj.rObject.top 	= FMUL(Obj.rObject.top, m_yrate);
		Obj.rObject.right	= FMUL(Obj.rObject.right, m_xrate);
		Obj.rObject.bottom 	= FMUL(Obj.rObject.bottom, m_yrate);

		// scale the mask rect
		rMask.left	 	= FMUL(rMask.left	 , m_xrate);
		rMask.top 	 	= FMUL(rMask.top 	 , m_yrate);
		rMask.right 	= FMUL(rMask.right , m_xrate);
		rMask.bottom	= FMUL(rMask.bottom, m_yrate);
		}

	// now read the frame data for the image
	// keep reading until we hit end-of-file
	if (!ReadBlockInfo(pFile, &dwBytes, &OpCode))
		AfxThrowFileException(CFileException::generic);
	if (OpCode != PPFF_DATA_OPCODE)
		{
		SetError(BEC_errInvalidOpCode);
		//Message(IDS_INVALIDOPCODE);
		AfxThrowFileException(CFileException::generic);
		}

	PP5CheckCmsInfo(&m_cmsInfo,
					&m_PacketList,
					SrcTypeInfo,
					m_fWantOriginal,
					m_fReadNativeType);

	if (!(lpDataFrame = ReadFrameBlock(pFile, dwBytes, m_SampleMode,
									&Obj.rObject, m_SampleRes, m_cmsInfo.dst,
									m_bEscapable, &m_cmsInfo)))
		AfxThrowFileException(CFileException::generic);

	if (m_SampleMode == modeSampleRes)
		{
		if (fBase)
			{
			// scale the mask rect
			m_SampleRes = FrameResolution(lpDataFrame);
			ResConvertRect(m_iCurrentRes, m_SampleRes, &rMask);
			}
		else
			{
			// scale the object location
			ResConvertUL(m_iCurrentRes, m_SampleRes, &Obj.rObject.left, &Obj.rObject.top);
			}
		}
	Obj.rObject.right = Obj.rObject.left + FrameXSize(lpDataFrame) - 1;
	Obj.rObject.bottom = Obj.rObject.top + FrameYSize(lpDataFrame) - 1;

	if (fSaveAlpha)
		{
		FRMTYPEINFO AlphaType;
	     FRMDATATYPE FrameType = FDT_GRAYSCALE;

      	if (m_cmsInfo.dst.DataType == FDT_LINEART && m_fOneBitMask)
        	FrameType = FDT_LINEART;

		// now read the frame data for the image
		// keep reading until we hit end-of-file
		if (!ReadBlockInfo(pFile, &dwBytes, &OpCode))
			AfxThrowFileException(CFileException::generic);
		if (OpCode != PPFF_ALPHA_OPCODE)
			{
			SetError(BEC_errInvalidOpCode);
			//Message(IDS_INVALIDOPCODE);
			AfxThrowFileException(CFileException::generic);
			}

		FrameSetTypeInfo(&AlphaType, FrameType);
		if (!(lpAlphaFrame = ReadFrameBlock(pFile, dwBytes, m_SampleMode,
										&Obj.rObject, m_SampleRes,
										AlphaType, m_bEscapable, NULL)))
		if (!lpAlphaFrame)
			AfxThrowFileException(CFileException::generic);
		lpAlpha = MaskCreate(lpAlphaFrame, 0, 0, OFF, m_fNoUndo);
		if (!lpAlpha)
			AfxThrowMemoryException();
		lpAlpha->bInvert = bInvert;
		lpAlpha->rMask = rMask;
		}

	// make sure the object rectangle is the correct size
	lpObject = ObjCreateFromFrame(ST_PERMANENT, lpDataFrame, lpAlpha,
											&Obj.rObject, m_fNoUndo);
	if (!lpObject)
		AfxThrowMemoryException();

	lpObject->ObjectType = Obj.ObjectType;
	lpObject->dwObjectID = Obj.dwObjectID;
	lpObject->Opacity = Obj.Opacity;
	lpObject->MergeMode = Obj.MergeMode;
	lpObject->wGroupID = Obj.wGroupID;
	lpObject->fSelected = Obj.fSelected;
	lpObject->bLocked = Obj.bLocked;
	lstrcpy(lpObject->szObjectName, Obj.szObjectName);

	if (fHaveTextData)
		{
		if (!ReadBlockInfo(pFile, &dwBytes, &OpCode))
			AfxThrowFileException(CFileException::generic);
		if (OpCode != PPFF_TEXT_OPCODE)
			{
			SetError(BEC_errInvalidOpCode);
			AfxThrowFileException(CFileException::generic);
			}
		lpObject->lpText = ReadTextBlock(pFile, dwBytes);
		if (!lpObject->lpText)
			AfxThrowFileException(CFileException::generic);
		}
	ProgressEnd();
	return(lpObject);
	}
CATCH_ALL(e)
	{
	if (fProgressSetup)
		ProgressEnd();
	if (lpDataFrame)
		FrameClose(lpDataFrame);
	if (lpAlpha)
		MaskClose(lpAlpha);
	else
	if (lpAlphaFrame)
		FrameClose(lpAlphaFrame);
	return(NULL);
	}
END_CATCH_ALL
}

/************************************************************************/
LOCAL BITMAP_ERROR_CODE pp5_read_file_header(CFile *pFile, LPTR lpData)
/************************************************************************/
{
	UINT numBytes;
	WORD wByteOrder, wVersion;
	BITMAP_ERROR_CODE error = BEC_errNone;

	TRY
	{
		// read the file signature
		numBytes = 4;
		if ( pFile->Read(lpData, numBytes) != numBytes )
			return(BEC_errFileRead);
		if (lpData[0] != 'P' || lpData[1] != 'P' ||
			lpData[2] != 'U' || lpData[3] != 'B')
		{
			//Message(IDS_BADSIGNATURE);
			return(BEC_errBadSignature);
		}
		// read the byte order
		numBytes = 2;
		if ( pFile->Read(&wByteOrder, numBytes) != numBytes )
			return(BEC_errFileRead);

		// read the file version number
		numBytes = 2;
		if ( pFile->Read(&wVersion, numBytes) != numBytes )
			return(BEC_errFileRead);
		error = BEC_errNone;
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	return(error);
}

/************************************************************************/
BOOL CReadBitmap::pp5_read_packet_list(CFile *pFile, LPLIST lpPacketList, BOOL fReadLoadPacketOnly)
/************************************************************************/
{
	DWORD dwBytes;
	PPFF_OPCODE OpCode;
	BOOL fRet = FALSE;

	TRY
	{
		while (TRUE)
		{
			// keep reading until we hit end-of-file
			if (!ReadBlockInfo(pFile, &dwBytes, &OpCode))
				break;
			switch (OpCode)
			{
				case PPFF_CMDLIST_OPCODE:
					return(ReadCmdList(pFile, lpPacketList, dwBytes, fReadLoadPacketOnly));

				default:
					pFile->Seek(dwBytes, CFile::current);
					break;
		  	}
		}
		fRet = TRUE; // no errors reading
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	return(fRet);
}

/************************************************************************/
BOOL CReadBitmap::pp5_read_header(CFile *pFile, LPTR lpData,
						LPBOOL lpReadObjectBlock,
						LPINT lpNumObjects, LPFRMTYPEINFO lpSrcTypeInfo)
/************************************************************************/
{
	LPWORD shtptr;
	UINT numBytes;
	DWORD dwBytes, dwPos;
	PPFF_OPCODE OpCode, FirstOpCode;
	BOOL fUseCurrent, fLowRes;
	BOOL fRet = FALSE;

	*lpReadObjectBlock = TRUE;
	TRY
	{
		// read the first block
		// this block should be an original header or
		// a current version header - the current version
		// can be either a low-res proxy or the full res
		FrameSetTypeInfo(lpSrcTypeInfo, FDT_NONE);
		dwPos = pFile->GetPosition();
		if (!ReadBlockInfo(pFile, &dwBytes, &FirstOpCode))
			return(FALSE);

		m_fHasOriginal = m_fHasCurrent = FALSE;

		if (FirstOpCode != PPFF_ORIGHDR_OPCODE &&
			FirstOpCode != PPFF_CURRHDR_OPCODE)
		{
			*lpReadObjectBlock = FALSE;
			pFile->Seek(dwPos, CFile::begin);
			return(FirstOpCode == PPFF_CMDLIST_OPCODE);
		}

		m_fHasOriginal = FirstOpCode == PPFF_ORIGHDR_OPCODE;
		m_fHasCurrent = FirstOpCode == PPFF_CURRHDR_OPCODE;

		// read the header for the first image block
		numBytes = dwBytes;
		if ( pFile->Read(lpData, numBytes) != numBytes )
			return(FALSE);

		shtptr = (LPWORD)lpData;
		*lpNumObjects = GETSHORT(shtptr);
		//m_idDataType = GETWORD(shtptr) + IDC_FIRSTDATATYPE;	/* 2 - data type */
		ITEMID idDataType = GETWORD(shtptr);	/* 2 - data type */
		m_iWidth = GETLONG(shtptr);			/* 4 - width */
		m_iHeight = GETLONG(shtptr);			/* 4 - height */
		m_iCurrentRes = m_iRes = GETSHORT(shtptr);				/* 2 - resolution */
		fLowRes = GETSHORT(shtptr);				/* 2 - low res flag */
		lpSrcTypeInfo->DataType = (FRMDATATYPE)GETSHORT(shtptr); /* 2 - src data type */
		lpSrcTypeInfo->ColorMap = NULL;
		lpSrcTypeInfo->ptInfo.toRCS = GETLONG(shtptr); /* 4 - to RCS */
		lpSrcTypeInfo->ptInfo.frRCS = GETLONG(shtptr); /* 4 - fr RCS */

		// if the only version of the image in the
		// file is an update-to-date version and
		// not the original, then we have to see if it will
		// satisfy the callers needs.  If the caller wants
		// the original, then they are out of luck or
		// if the caller wants hi-res and all we have is low-res
		// he is out of luck.  If we have low-res and the caller
		// wants low-res, we check to see if we have high enough
		// res for them
		if (FirstOpCode == PPFF_CURRHDR_OPCODE)
		{
			// image data is of no use to us in this case
			if (m_fWantOriginal ||
				((m_SampleMode == modeNoSampling) && fLowRes && !m_bNoHiResRebuild) )
				*lpReadObjectBlock = FALSE; 
			else
			if (!m_bNoHiResRebuild)
			{
				if (m_SampleMode == modeSampleRect)
				{
					// if caller wants to sample to a rectangle,
					// see if the current version will suffice
					if (RectWidth(&m_rArea) > m_iWidth ||
						RectHeight(&m_rArea) > m_iHeight)
						*lpReadObjectBlock = FALSE;
				}
				else
				if (m_SampleMode == modeSampleRes)
				{
					if (m_iRes < m_SampleRes)
						*lpReadObjectBlock = FALSE;
				}
				else
				if (m_PhotoCDResOverride)
				{
					if (DoPCDResOverride(&m_PacketList, m_PhotoCDResOverride))
						*lpReadObjectBlock = FALSE;
				}
			}
			if (*lpReadObjectBlock)
				m_fReadOriginal = FALSE;
		}

		// see if there is a "current" version of the image
		if (!m_fWantOriginal && FirstOpCode == PPFF_ORIGHDR_OPCODE)
			{
			fUseCurrent = FALSE;
			dwPos = pFile->GetPosition();
			while (TRUE)
				{
				// keep reading until we hit end-of-file
				if (!ReadBlockInfo(pFile, &dwBytes, &OpCode))
					break;
				if (OpCode == PPFF_CURRHDR_OPCODE)
					{
					FRMTYPEINFO SrcTypeInfo;
					// read the header
					numBytes = dwBytes;
					if ( pFile->Read(lpData, numBytes) != numBytes )
						return(FALSE);
 	
					m_fHasCurrent = TRUE;
					shtptr = (LPWORD)lpData;
					int nObj = GETSHORT(shtptr);
					ITEMID inDataType = GETWORD(shtptr)/* + IDC_FIRSTDATATYPE*/;	/* 2 - data type */
					int iWidth = GETLONG(shtptr);			/* 4 - width */
					int iHeight = GETLONG(shtptr);			/* 4 - height */
					m_iCurrentRes = GETSHORT(shtptr);		/* 2 - resolution */
					int fLowRes = GETSHORT(shtptr);			/* 2 - fLowRes */
					SrcTypeInfo.DataType = (FRMDATATYPE)GETSHORT(shtptr); /* 2 - src data type */
					SrcTypeInfo.ColorMap = NULL;
					SrcTypeInfo.ptInfo.toRCS = GETLONG(shtptr); /* 4 - to RCS */
					SrcTypeInfo.ptInfo.frRCS = GETLONG(shtptr); /* 4 - fr RCS */
					if (m_SampleMode == modeNoSampling ||
						m_bNoHiResRebuild)
						{
						// if caller wants no sampling and the current
						// we need at least as much resolution as the original
						if (m_iCurrentRes >= m_iRes)
							{
							fUseCurrent = TRUE;
							m_iWidth = iWidth;
							m_iHeight = iHeight;
							m_iRes = m_iCurrentRes;
							}
						}
					else
					if (m_SampleMode == modeSampleRect)
						{
						// if caller wants to sample to a rectangle,
						// see if the current version will suffice
						if (RectWidth(&m_rArea) <= iWidth &&
							RectHeight(&m_rArea) <= iHeight)
							fUseCurrent = TRUE;
						}
					else
					if (m_SampleMode == modeSampleRes)
						{
						if (m_iCurrentRes >= m_SampleRes)
							fUseCurrent = TRUE;
						}
					if (fUseCurrent)
					{
						*lpNumObjects = nObj;
						*lpSrcTypeInfo = SrcTypeInfo;
					}
					break;
					}
				else
					// skip the block
					pFile->Seek(dwBytes, CFile::current);
				}
			m_fReadOriginal = !fUseCurrent;
			if (!fUseCurrent)
				pFile->Seek(dwPos, CFile::begin);
			}
		fRet = TRUE;
		}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	return(fRet);
}

/************************************************************************/
BOOL CReadBitmap::PP5Read()
/************************************************************************/
{
CFile *pTheFile;
LPOBJECT lpObject;
DWORD dwBytes, dwPos;
int nObjects = 0, res;
PPFF_OPCODE OpCode;
LPTR lpData;
OBJECTLIST ObjList;
FRMTYPEINFO SrcTypeInfo;
BOOL	fReadObjectBlock;
BOOL 	fRet = FALSE;
BITMAP_ERROR_CODE err = BEC_errNone;
int nProgress = 0;

TRY
	{
	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}
	++nProgress;
	m_xrate = m_yrate = FUNITY;
	ObjList.lpHead = ObjList.lpTail = NULL;
	lpData = NULL;

	if (!(lpData = Alloc(DATA_BUF_SIZE)))
		{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto BadPP5;
		}

	if ((err = pp5_read_file_header(pTheFile, lpData)) != BEC_errNone)
	{
		goto BadRead;
	}


	// always read the command list first
	if (m_fReadCmdList)
	{
		dwPos = pTheFile->GetPosition();
		if (!pp5_read_packet_list(pTheFile, &m_PacketList, FALSE))
		{
			goto BadRead;
		}
		pTheFile->Seek(dwPos, CFile::begin);
	}

	if (!pp5_read_header(pTheFile, lpData, &fReadObjectBlock,
									&nObjects, &SrcTypeInfo))
	{
		goto BadRead;
	}

	if (fReadObjectBlock)
	{
		if (!nObjects)
			nObjects = 1;
		ProgressBegin(nObjects);
		++nProgress;
	}
	while (TRUE)
		{
		// keep reading until we hit end-of-file
		if (!ReadBlockInfo(pTheFile, &dwBytes, &OpCode))
			break;
		// if we don't have an object opcode, we must be
		// done reading the image
		if (OpCode != PPFF_OBJECT_OPCODE)
			fReadObjectBlock = FALSE;
		switch (OpCode)
			{
			case PPFF_OBJECT_OPCODE:
				if (!fReadObjectBlock)
					pTheFile->Seek(dwBytes, CFile::current);
				else
					{
					lpObject = ReadObjectBlock(pTheFile, lpData, dwBytes,
									(ObjList.lpHead == NULL), SrcTypeInfo);
					if (!lpObject)
						goto BadRead;
					if (ObjList.lpHead == NULL)
						res = FrameResolution(ObjGetEditFrame(lpObject));
					else
						{
						FrameSetResolution(ObjGetEditFrame(lpObject), res);
						if (lpObject->lpAlpha)
							FrameSetResolution(AlphaGetEditFrame(lpObject->lpAlpha), res);
						}
					ObjAddTail(&ObjList, lpObject);
					}
				break;

			default:
				pTheFile->Seek(dwBytes, CFile::current);
				break;
			}
		}

	if (!ObjList.lpHead && !ListGetCount(&m_PacketList))
		{
		SetError(BEC_errNoData);
		//Message(IDS_NODATA);
		goto BadRead;
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
if (err == BEC_errNone)
	SetError(BEC_errFileRead);
else
	SetError(err);
//if (m_lpFileName)
//	Message (IDS_EREAD, m_lpFileName);
BadPP5:
while (lpObject = (LPOBJECT)ObjGetNextObject(&ObjList, NULL, YES))
	{
	ObjUnlinkObject(&ObjList, lpObject);
	ObjFreeUp(lpObject);
	}

Exit:

CloseFile(pTheFile); 
while (--nProgress >= 0)
	ProgressEnd();

if (lpData)
	FreeUp(lpData);
return( fRet );
}

/************************************************************************/
BOOL CReadBitmap::PP5ReadHdr()
/************************************************************************/
{
	CFile *pTheFile;
	BOOL fReadObjectBlock;
	int nObjects;
	LPTR lpData;
	BITMAP_ERROR_CODE err = BEC_errFileRead;
	BOOL 	fRet = FALSE;
	FRMTYPEINFO SrcTypeInfo;

	if ((pTheFile = OpenFile()) == NULL)
		return(FALSE);

 	if (!(lpData = Alloc(DATA_BUF_SIZE)))
	{
		err = BEC_errMemory;
		goto BadRead;
	}

	if ((err = pp5_read_file_header(pTheFile, lpData)) != BEC_errNone)
		goto BadRead;

	if (!pp5_read_header(pTheFile, lpData, &fReadObjectBlock,
						   			&nObjects, &SrcTypeInfo))
		goto BadRead;

	m_cmsInfo.src = SrcTypeInfo;
	fRet = TRUE;
	goto Exit;

	BadRead:
	SetError(err);

	Exit:

	if (lpData)
		FreeUp(lpData);
	CloseFile(pTheFile);
	return( fRet );
}


/************************************************************************/
LPTIFFHEADER CReadBitmap::PP5ReadTiffHeader()
/************************************************************************/
{
	LPTR lpData;
	BITMAP_ERROR_CODE err = BEC_errFileRead;
	LPTIFFHEADER lpTH = NULL;
	BOOL 	fRet = FALSE;
	CFile*	pTheFile;
	PPFF_OPCODE OpCode;
	DWORD	dwBytes;
	DWORD	dwCurrentPos = 0;
	DWORD	dwOriginalPos = 0;
 	
	if ((pTheFile = OpenFile()) == NULL)
		return(FALSE);

	if (!(lpData = Alloc(DATA_BUF_SIZE)))
	{
		SetError(BEC_errMemory);
		goto BadPP5;
	}

	if ((err = pp5_read_file_header(pTheFile, lpData)) != BEC_errNone)
		goto BadRead;

	TRY
	{
		while (TRUE)
		{
			// keep reading until we hit end-of-file
			if (!ReadBlockInfo(pTheFile, &dwBytes, &OpCode))
				break;
			pTheFile->Seek(dwBytes, CFile::current);

			if (OpCode == PPFF_ORIGHDR_OPCODE)
				dwOriginalPos = pTheFile->GetPosition();
			else
			if (OpCode == PPFF_CURRHDR_OPCODE)
			{
				dwCurrentPos = pTheFile->GetPosition();
				break;
			}
		}

		if (!dwCurrentPos)
			dwCurrentPos = dwOriginalPos;

		if (!dwCurrentPos)
			goto Exit;
	
	 	pTheFile->Seek(dwCurrentPos, CFile::begin);

		// now search for first object block past the header
		if (!ReadBlockInfo(pTheFile, &dwBytes, &OpCode))
			goto BadRead;
		pTheFile->Seek(dwBytes, CFile::current);
		if (OpCode != PPFF_OBJECT_OPCODE)
			goto BadRead;

		// now read the frame data for the image
		// keep reading until we hit end-of-file
		if (!ReadBlockInfo(pTheFile, &dwBytes, &OpCode))
			goto BadRead;
		if (OpCode != PPFF_DATA_OPCODE)
			goto BadRead;
	 	lpTH = ReadTiffHeader( pTheFile );
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadRead;
	}
	END_CATCH_ALL

	BadRead:
	SetError(err);
	BadPP5:

	Exit:
	CloseFile(pTheFile);

	if (lpData)
		FreeUp(lpData);
	return( lpTH );
}

/************************************************************************/
BOOL CReadBitmap::PP5ReadPacketList(BOOL fReadLoadPacketOnly)
/************************************************************************/
{
	CFile *pTheFile;
	LPTR lpData;
	BITMAP_ERROR_CODE err = BEC_errFileRead;
	BOOL 	fRet = FALSE;

	if ((pTheFile = OpenFile()) == NULL)
		return(FALSE);

	if (!(lpData = Alloc(DATA_BUF_SIZE)))
	{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto BadPP5;
	}

	if ((err = pp5_read_file_header(pTheFile, lpData)) != BEC_errNone)
		goto BadRead;

	if (!pp5_read_packet_list(pTheFile, &m_PacketList, fReadLoadPacketOnly))
		goto BadRead;
		
	fRet = TRUE;
	goto Exit;

	BadRead:
	SetError(err);
	BadPP5:

	Exit:
    if (lpData)
		FreeUp(lpData);
	CloseFile(pTheFile);
	return( fRet );
}

/************************************************************************/
BOOL CWriteBitmap::WriteBlockInfo(CFile *pFile, DWORD dwBytes, int OpCode)
/************************************************************************/
{
UINT numBytes;
WORD wOpCode;

TRY
	{
	// write the size of the block
	numBytes = 4;
	pFile->Write(&dwBytes, numBytes);
	// read the opcode of the block
	numBytes = 2;
	wOpCode = OpCode;
	pFile->Write(&wOpCode, numBytes);
	return(TRUE);
	}
CATCH_ALL(e)
	{
	return(FALSE);
	}
END_CATCH_ALL
}


/************************************************************************/
BOOL CWriteBitmap::WriteTextBlock(CFile *pFile, LPTR lpBuf,
								CTextObject *pText)
/************************************************************************/
{
COLORSPACE Space;
char szColor[20];
UINT TextLen, FileTextLen;
UINT NameLen, FileNameLen;
UINT ColorLen, FileColorLen;
WORD wBytes;
DWORD dwLen;

TRY
	{
	FileTextLen = TextLen = pText->lpText.GetLength();
	if (FileTextLen & 1)
		++FileTextLen;
	Space = GetColorInfoString( szColor, &pText->Color );
	FileColorLen = ColorLen = lstrlen(szColor);
	if (FileColorLen & 1)
		++FileColorLen;
	FileNameLen = NameLen = lstrlen(pText->FontID.FontDesc.fdFaceName);
	if (FileNameLen & 1)
		++FileNameLen;

	DWORD dwTotal = FileTextLen + FileColorLen + FileNameLen + 12 + PPFF_TEXT_SIZE;
	if (!WriteBlockInfo(pFile, dwTotal, PPFF_TEXT_OPCODE))
		return(FALSE);
	LPWORD shtptr = (LPWORD)lpBuf;
	PUTWORD(pText->Dirty, 			shtptr);
	PUTWORD(pText->AntiAlias, 		shtptr);
	PUTWORD(pText->Justification,	shtptr);
	PUTWORD(pText->FontID.FontDesc.fdPitchAndFamily, shtptr);
	PUTWORD(pText->FontID.FontDesc.fdCharSet, shtptr);
	PUTWORD(pText->FontID.FontDesc.FontType, shtptr);
	PUTWORD(pText->FontID.Size, shtptr);
	PUTWORD(pText->FontID.Italic, shtptr);
	PUTWORD(pText->FontID.Underline, shtptr);
	PUTWORD(pText->FontID.Strikeout, shtptr);
	PUTWORD(pText->FontID.Weight, shtptr);
	PUTWORD(pText->FontID.Angle, shtptr);
	PUTWORD(Space, shtptr);
	PUTLONG(pText->DestPos.x, shtptr);
	PUTLONG(pText->DestPos.y, shtptr);
	PUTWORD(pText->iBaseRes, shtptr);

	// reserved bytes

	PUTWORD(0, shtptr);
	PUTWORD(0, shtptr);
	PUTWORD(0, shtptr);
	PUTWORD(0, shtptr);
	PUTWORD(0, shtptr);

	wBytes = (LPTR)shtptr - (LPTR)lpBuf;
	ASSERT(wBytes == PPFF_TEXT_SIZE);
	pFile->Write(lpBuf, wBytes);

	dwLen = TextLen;
	pFile->Write(&dwLen, 4);
	pFile->Write(pText->lpText.GetBuffer(0), FileTextLen);
	pText->lpText.ReleaseBuffer();

	dwLen = ColorLen;
	pFile->Write(&dwLen, 4);
	pFile->Write(szColor, FileColorLen);

	dwLen = NameLen;
	pFile->Write(&dwLen, 4);
	pFile->Write(pText->FontID.FontDesc.fdFaceName, FileNameLen);
	return(TRUE);
	}
CATCH_ALL(e)
	{
	return(FALSE);
	}
END_CATCH_ALL
}

/************************************************************************/
BOOL CWriteBitmap::WriteFrameBlock(CWriteBitmap *pWriter, CFile *pFile, LPFRAME lpFrame,
			  		FRMDATATYPE DataType, BOOL fCompressed,
			  		int OpCode, BOOL bEscapable, BOOL bWritePT,
					int res, LPCOLORMAP lpColorMap)
/************************************************************************/
{
DWORD dwBlockPos, dwFrameBegin, dwFrameEnd, dwFrameSize;
TIFFOPTIONS options;

TRY
	{
	FrameSetResolution(lpFrame, res);
	CWriteTiff TiffWrite(pWriter, pFile, bEscapable);
	options.TIFFCompress = fCompressed;
	options.TIFFDiff = NO;
	options.bWritePT = FALSE /*bWritePT*/;
	options.bSaveMask = NO;
	TiffWrite.SetOptions(&options);
	TiffWrite.SetColorMap(lpColorMap);

	dwBlockPos = pFile->GetPosition();
	if (!WriteBlockInfo(pFile, 0, OpCode))
		return(FALSE);
	dwFrameBegin = pFile->GetPosition();
	if (!TiffWrite.WriteFrame(lpFrame, DataType))
		return(FALSE);
	dwFrameEnd = pFile->GetPosition();
	dwFrameSize = dwFrameEnd - dwFrameBegin;
	pFile->Seek(dwBlockPos, CFile::begin);
	if (!WriteBlockInfo(pFile, dwFrameSize, OpCode))
		return(FALSE);
	pFile->Seek(dwFrameEnd, CFile::begin);
	return(TRUE);
	}
CATCH_ALL(e)
	{
	}
END_CATCH_ALL
return(FALSE);
}

/************************************************************************/
BOOL CWriteBitmap::WriteObjectBlock(CFile *pFile, LPTR lpBuf, LPOBJECT lpObject,
								BOOL fSaveAlpha, BOOL fCompressed, BOOL bWritePT,
								int res, LPCOLORMAP lpColorMap)
/************************************************************************/
{
WORD wBytes;
LPWORD shtptr;

TRY
	{
	if (!WriteBlockInfo(pFile, PPFF_OBJECT_SIZE, PPFF_OBJECT_OPCODE))
		return(FALSE);

	if (!lpObject->lpAlpha)
		fSaveAlpha = NO;

	ProgressBegin(fSaveAlpha ? 2 : 1);

	// write out the OBJECT structure
	shtptr = (LPWORD)lpBuf;
	PUTWORD(lpObject->ObjectType,			shtptr);	// 2 - object type
	PUTLONG(lpObject->dwObjectID,			shtptr);	// 4 - object id
	copy((LPTR)lpObject->szObjectName, (LPTR)shtptr, sizeof(lpObject->szObjectName));
	shtptr += (sizeof(lpObject->szObjectName)/2);
	PUTLONG( lpObject->rObject.left,		shtptr);	// 4 - object rect
	PUTLONG( lpObject->rObject.top,		shtptr); 		// 4
	PUTLONG( lpObject->rObject.right,		shtptr); 	// 4
	PUTLONG( lpObject->rObject.bottom,		shtptr); 	// 4
	PUTWORD ( lpObject->Opacity,			shtptr); 	// 2
	PUTWORD ( lpObject->MergeMode,			shtptr); 	// 2
	PUTWORD ( lpObject->wGroupID,			shtptr); 	// 2
	PUTWORD (/*IDN_TIFF*/0,						shtptr); 	// 2
	if (lpObject->lpAlpha)
		{
		PUTWORD (lpObject->lpAlpha->bInvert,		shtptr);// 2
		PUTLONG(lpObject->lpAlpha->rMask.left,		shtptr);// 4
		PUTLONG(lpObject->lpAlpha->rMask.top,		shtptr);// 4
		PUTLONG(lpObject->lpAlpha->rMask.right,	shtptr); 	// 4
		PUTLONG(lpObject->lpAlpha->rMask.bottom,	shtptr);// 4
		}
	else
		{
		PUTWORD (0, shtptr); 	// 2
		PUTLONG(0, shtptr); 	// 4
		PUTLONG(0, shtptr); 	// 4
		PUTLONG(0, shtptr); 	// 4
		PUTLONG(0, shtptr); 	// 4
		}
	PUTWORD(lpObject->fSelected, shtptr);		// 2
	PUTWORD(fSaveAlpha, shtptr);				// 2 - saving alpha
	PUTWORD(lpObject->lpText != NULL, shtptr);	// 2 - save text info
	PUTWORD(lpObject->bLocked, shtptr);			// 2 - locked flag
	PUTWORD(0, shtptr);							// 2
	PUTWORD(0, shtptr);							// 2 - reserved
	PUTWORD(0, shtptr);							// 2
	PUTWORD(0, shtptr);							// 2 - reserved
	PUTWORD(0, shtptr);							// 2
	PUTWORD(0, shtptr);							// 2 - reserved
	PUTWORD(0, shtptr);							// 2
	PUTWORD(0, shtptr);							// 2
	PUTWORD(0, shtptr);							// 2

	// Write out Object data
	wBytes = (LPTR)shtptr - (LPTR)lpBuf;
	ASSERT(wBytes == PPFF_OBJECT_SIZE);
	pFile->Write(lpBuf, wBytes);

	if (!WriteFrameBlock(this, pFile, ObjGetEditFrame(lpObject), m_DataType,
				fCompressed, PPFF_DATA_OPCODE, m_bEscapable, bWritePT, res,
				lpColorMap))
		{
		ProgressEnd();
		return(FALSE);
		}

	if (fSaveAlpha)
		{
		if (!WriteFrameBlock(this, pFile, lpObject->lpAlpha->Pixmap.EditFrame,
					FDT_GRAYSCALE,	fCompressed, PPFF_ALPHA_OPCODE, m_bEscapable,
					bWritePT, res, NULL))
			{
			ProgressEnd();
			return(FALSE);
			}
		}
	// write out the CTextObject class
	if (lpObject->lpText)
		{
		if (!WriteTextBlock(pFile, lpBuf, lpObject->lpText))
			{
			ProgressEnd();
			return(FALSE);
			}
		}
	ProgressEnd();
	return(TRUE);
	}
CATCH_ALL(e)
	{
	ProgressEnd();
	return(FALSE);
	}
END_CATCH_ALL
}

/************************************************************************/
LOCAL BOOL WriteHeader(CFile *pFile, LPTR lpData)
/************************************************************************/
{
	LPSTR lpChar;
	WORD wBytes;
	LPWORD shtptr;
	BOOL fRet = FALSE;

	TRY
	{
		lpChar = (LPSTR)lpData;
		*lpChar++ = 'P';			   		/* 4 - file signature */
		*lpChar++ = 'P';
		*lpChar++ = 'U';
		*lpChar++ = 'B';
		shtptr = (LPWORD)lpChar;
	
		PUTWORD(PPFF_BYTEORDER, shtptr);			/* 2 - byte order is LSB,MSB */
		PUTWORD(PPFF_VERSION, shtptr);    			/* 2 - Version Number */

		wBytes = (LPTR)shtptr - (LPTR)lpData;
		pFile->Write(lpData, wBytes);
		fRet = TRUE;
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	return(fRet);
}

/************************************************************************/
BOOL CWriteBitmap::pp5_write_image(CFile *pFile, LPTR lpData,
								BOOL fOriginal, BOOL fLowRes)
/************************************************************************/
{
	LPOBJECT lpObject;
	LPFRAME lpFrame;
	DWORD dwWidth, dwHeight;
	WORD wDataType, wBytes;
	int nObjects, res;
	LPWORD shtptr;
	FRMTYPEINFO TypeInfo, inType;
	BOOL fSaveAlpha, bSaveMask;
	LPCOLORMAP lpColorMap = NULL;
	BOOL fRet = FALSE;

	TRY
	{
		lpObject = m_lpObjList->lpHead;
		ASSERT(lpObject);
		lpFrame = ObjGetEditFrame(lpObject);
		ASSERT(lpFrame);

		LPPPFFOPTIONS lpOptions = (LPPPFFOPTIONS)m_lpOptions;

		FrameGetTypeInfo(lpFrame, &inType);

		dwWidth = FrameXSize(lpFrame);
		dwHeight = FrameYSize(lpFrame);
		if (lpOptions->bWritePT)
			TypeInfo = inType;
		else
			FrameSetTypeInfo(&TypeInfo, FDT_NONE);
		res = FrameResolution(lpFrame);
		//wDataType = m_idDataType - IDC_FIRSTDATATYPE;
		wDataType = m_DataType;

		if (fOriginal)
			bSaveMask = lpOptions->bSaveMask;
		else
			bSaveMask = TRUE;
		nObjects = 0;
		while (lpObject)
		{
			if (!lpObject->fDeleted)
				++nObjects;

			lpObject = (LPOBJECT)lpObject->lpNext;
		}
		ProgressBegin(nObjects);

		// are we converting to palette color
		if (m_DataType == FDT_PALETTECOLOR && inType.DataType != FDT_PALETTECOLOR)
		{
			// see if we need to create a palette
			lpColorMap = FrameCreateColorMap();
			if (!lpColorMap)
			{
				SetError(BEC_errMemory);
				//Message(IDS_EMEMALLOC);
				goto BadWrite;
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
				goto BadWrite;
			}
		}

		// header block info - 6 bytes
		if (fOriginal)
		{
			if (!WriteBlockInfo(pFile, PPFF_HDR_SIZE, PPFF_ORIGHDR_OPCODE))
				goto BadWrite;
		}
		else
		{
			if (!WriteBlockInfo(pFile, PPFF_HDR_SIZE, PPFF_CURRHDR_OPCODE))
				goto BadWrite;
		}

		// header
		shtptr = (LPWORD)lpData;
		PUTSHORT(nObjects, shtptr);			/* 2 - Number of Objects */
		PUTWORD(wDataType, shtptr);			/* 2 - data type */
		PUTLONG(dwWidth, shtptr);			/* 4 - width */
		PUTLONG(dwHeight, shtptr);			/* 4 - height */
		PUTSHORT(res, shtptr);				/* 2 - resolution */

		// reserved - 20 bytes
		PUTWORD(fLowRes, shtptr);	 		/* 2 - LowRes flag */
		PUTWORD(TypeInfo.DataType, shtptr);	/* 2 - reserved */
		PUTLONG(TypeInfo.ptInfo.toRCS, shtptr);	/* 4 - reserved */
		PUTLONG(TypeInfo.ptInfo.frRCS, shtptr);	/* 4 - reserved */

		PUTWORD(0, shtptr);					/* 2 - reserved */
		PUTWORD(0, shtptr);					/* 2 - reserved */
		PUTWORD(0, shtptr);					/* 2 - reserved */
		PUTWORD(0, shtptr);					/* 2 - reserved */

		// Write out header info
		wBytes = (LPTR)shtptr - (LPTR)lpData;
		ASSERT(wBytes == (PPFF_HDR_SIZE));
		pFile->Write(lpData, wBytes);

		lpObject = m_lpObjList->lpHead;
		while (lpObject)
		{
			// don't do deleted objects
			if (!lpObject->fDeleted)
				{
				fSaveAlpha = (lpObject != m_lpObjList->lpHead) || bSaveMask;
				// write out the object
				if (!WriteObjectBlock(pFile, lpData, lpObject, fSaveAlpha,
							lpOptions->PPFFCompress, lpOptions->bWritePT, res,
							lpColorMap))
					goto BadWrite;
				}
			// get the next object
			lpObject = (LPOBJECT)lpObject->lpNext;
		}
		fRet = TRUE;
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL

BadWrite:
	if (lpColorMap)
		FrameDestroyColorMap(lpColorMap);
	ProgressEnd();
	return(fRet);
}

/************************************************************************/
BOOL CWriteBitmap::PPFWrite()
/************************************************************************/
{
	// do we already have a storage?
	if (m_pStorage)
		return(PP5Write());
	else
	{
		BOOL fRet;
        SCODE sc = ::StgCreateDocfile(m_lpFileName, 
        	STGM_READWRITE|STGM_SHARE_EXCLUSIVE|STGM_CREATE,
        	0, 
        	&m_pStorage);
        
        if (sc != NOERROR)
        	return FALSE;

		fRet = PP5Write();

		if (fRet)
			m_pStorage->Commit(STGC_ONLYIFCURRENT);
		ReleaseInterface((LPUNKNOWN*)&m_pStorage);
		if (!fRet)
			CFile::Remove(m_lpFileName);
		return(fRet);
	}
}

/************************************************************************/
BOOL CWriteBitmap::PP5Write()
/************************************************************************/
{
	CFile *pTheFile;
	LPTR lpData;
	BOOL fRet = FALSE, bSaveCmdList;
	BOOL fOriginal, fLowRes, fFastBits, fPhotoCD;

	ASSERT(m_lpObjList);
	ASSERT(m_lpOptions);

	LPPPFFOPTIONS lpOptions = (LPPPFFOPTIONS)m_lpOptions;

	if ((pTheFile = OpenFile()) == NULL)
		return(FALSE);

 	if (!(lpData = Alloc(DATA_BUF_SIZE)))
	{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto Exit;
	}

	if (!WriteHeader(pTheFile, lpData))
		goto BadWrite;
	
	// if there are no edit commands in the command list
	// (i.e. - not more than one command in command list)
	// and the image was not loaded low-res, then
	// it is considered an original rendition
	GetCommandInfo(&m_PacketList, lpOptions, &fLowRes, &fFastBits, &fPhotoCD, &bSaveCmdList);

	// the data is considered original if:
	// 1. the user doesn't want the command list saved or
	// 2. the are no commands in the command list or
	// 3. or it was a fastbits load

	if (!bSaveCmdList || fFastBits)
		fOriginal = TRUE;
	else
		fOriginal = FALSE;

	if (fOriginal || lpOptions->bSaveProxy)
	{
		if (!pp5_write_image(pTheFile, lpData, fOriginal, fLowRes))
			goto BadWrite;
	}

	// write out packet list
	// don't write out command list for an original rendition
	if ((bSaveCmdList || !lpOptions->bSaveProxy) &&	!fOriginal)
	{
		if (!WriteCmdList(pTheFile, &m_PacketList))
			goto BadWrite;
	}

	fRet = TRUE;
	goto Exit;

	BadWrite:
		SetError(BEC_errWriteFile);
		//if (m_lpFileName)
		//	Message( IDS_EWRITE, m_lpFileName );
	Exit:

	FreeUp(lpData);
	CloseFile(pTheFile, fRet);
	return( fRet );
}

/************************************************************************/
BOOL CWriteBitmap::PP5Update()
/************************************************************************/
{
	CFile *pTheFile;
	DWORD dwBytes, dwPos;
	PPFF_OPCODE OpCode;
	LPTR lpData;
	BITMAP_ERROR_CODE err = BEC_errFileRead;
	BOOL 	fRet = FALSE, bSaveCmdList;

	if ((pTheFile = OpenFile(CFile::modeReadWrite)) == NULL)
		return(FALSE);

	if (m_lpObjList)
	{
		ASSERT(m_lpOptions);
	}

	LPPPFFOPTIONS lpOptions = (LPPPFFOPTIONS)m_lpOptions;

	TRY
	{
		// can never escape out of an update
		m_bEscapable = FALSE;
		lpData = NULL;
		if (!(lpData = Alloc(DATA_BUF_SIZE)))
		{
			SetError(BEC_errMemory);
			goto BadPP5;
		}

		if ((err = pp5_read_file_header(pTheFile, lpData)) != BEC_errNone)
			goto BadRead;

		while (TRUE)
		{
			// keep reading until we hit end-of-file
			if (!ReadBlockInfo(pTheFile, &dwBytes, &OpCode))
			{
				dwPos = pTheFile->GetPosition();
				break;
 			}
			switch (OpCode)
			{
				case PPFF_CMDLIST_OPCODE:
					// truncate the file at beginning of this block
					dwPos = pTheFile->GetPosition();
					pTheFile->Seek(-6, CFile::current);
					pTheFile->SetLength(dwPos-6);
					break;

				case PPFF_CURRHDR_OPCODE:
					if (m_lpObjList)
					{
						// truncate the file at beginning of this block
						dwPos = pTheFile->GetPosition();
						pTheFile->Seek(-6, CFile::current);
						pTheFile->SetLength(dwPos-6);
						break;				   
					}
					// fall thru if we are not writing image data
				default:
					// seek by all other blocks
					pTheFile->Seek(dwBytes, CFile::current);
					break;
			}
		}

		if (m_lpObjList && lpOptions->bSaveProxy)
		{
			BOOL fLowRes, fFastBits, fPhotoCD, bSaveCmdList;
			GetCommandInfo(&m_PacketList, m_lpOptions, &fLowRes, &fFastBits, &fPhotoCD, &bSaveCmdList);
			if (!pp5_write_image(pTheFile, lpData, FALSE, fLowRes))
				goto BadWrite;
		}

		// write out packet list
		if (m_lpObjList)
			bSaveCmdList = (lpOptions->bSaveCmdList || !lpOptions->bSaveProxy);
		else
			bSaveCmdList = TRUE;
		if (bSaveCmdList &&	(ListGetCount(&m_PacketList) > 0) )
		{
			if (!WriteCmdList(pTheFile, &m_PacketList))
				goto BadWrite;
		}

		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadRead;
	}
	END_CATCH_ALL

	BadWrite:
	SetError(BEC_errWriteFile);
	goto BadPP5;

	BadRead:
	SetError(err);
	BadPP5:
	Exit:
	if (lpData)
		FreeUp(lpData);
	CloseFile(pTheFile, TRUE/* always act like a sucess */);
	return( fRet );
}


