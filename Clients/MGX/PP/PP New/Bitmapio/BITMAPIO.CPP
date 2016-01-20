//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "stdafx.h"
#include "mgxbm.h"
#include "cmsutil.h"
#include "list.h"
#include "mask.h"
#include "object.h"

LOCAL void write_thumb(LPTSTR lpName, LPFRAME lpFrame, LPOBJECTLIST lpObjList, FRMDATATYPE DataType, BITMAP_FILE_TYPE FileType);
LOCAL BOOL SetFileExt(LPTSTR lpFileName, BITMAP_FILE_TYPE FileType);
LOCAL BITMAP_FILE_TYPE GetFileTypeFromExt(LPTSTR lpFileName);

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

// Constructor
CReadBitmap::CReadBitmap(	LPTSTR lpFileName,
							BITMAP_FILE_TYPE FileType,
							LPCMSINFO pcmsInfo)
{
	BITMAP_FILE_TYPE Type;
	ASSERT(lpFileName);

	Init(pcmsInfo);

	m_lpFileName = lpFileName;
	
    Type = GetFileTypeFromExt( m_lpFileName );
    if( !Type )
    	Type = FileType;

    m_FileType = Type;

   	ASSERT(m_FileType);
}

// Constructor
CReadBitmap::CReadBitmap(CFile *pFile,
						 BITMAP_FILE_TYPE FileType,
						 BOOL bEscapable, 
						 LPCMSINFO pCmsInfo)
{
	ASSERT(pFile != NULL);
	ASSERT(FileType != BFT_UNKNOWN);

	Init(pCmsInfo);
   	m_pFile = pFile;
    m_FileType = FileType;
	m_bEscapable = bEscapable;

	// readers that do not support CFile's
	ASSERT(m_FileType != BFT_JPEG);
	ASSERT(m_FileType != BFT_PHOTOCD);
}

// Destructor
CReadBitmap::~CReadBitmap()
{
	DestroyPacketList(&m_PacketList);
}

void CReadBitmap::Init(LPCMSINFO pcmsInfo)
{
	if( !pcmsInfo )
		cmsInfoInit( &m_cmsInfo );
	else
		m_cmsInfo = *pcmsInfo;

    m_lpFileName = NULL;
    m_FileType = BFT_UNKNOWN;
    m_pFile = NULL;
    m_SampleMode = CReadBitmap::modeNoSampling;
    m_fCrop = FALSE;
    AstralSetRectEmpty(&m_rArea);
    m_lpFrame = NULL; 
    m_ObjList.lpHead = m_ObjList.lpTail = NULL;
    m_iWidth = 0;     
    m_iHeight = 0;         
    m_iRes = 0;   
	m_BitsPerPixel = 8;
    m_lpOptions = NULL;
	m_fReadCombined = TRUE;
	m_bEscapable = TRUE;
	ListInit(&m_PacketList);
	m_fReadCmdList = TRUE;
	m_fReadOriginal = TRUE;
	m_fWantOriginal = FALSE;
	m_PhotoCDResOverride = 0;
	m_fHasCurrent = FALSE;
	m_fHasOriginal = TRUE;
	m_fReadNativeType = FALSE;

    m_fLineArtAsGray = FALSE; 
	m_fNoUndo = FALSE;
	m_fOneBitMask = TRUE;
	m_CMSEnabled = FALSE;
	m_bNoHiResRebuild = TRUE;
	m_fTGACMYK = FALSE;
	m_DitherType = DT_DEFAULT;

	m_pStorage = NULL;
	m_fOpenedStorage = FALSE;
}

CFile *CReadBitmap::OpenFile(UINT nOpenFlags)
{
	const TCHAR szContents[] = _T("Contents");

	m_fOpenedStorage = FALSE;
	if (m_pFile)
		return(m_pFile);
	else
    if (m_pStorage != NULL)
	{
		COleStreamFile* pFile = NULL;
		TRY
		{
			CFileException fe;
			pFile = new COleStreamFile();
			if (!pFile->OpenStream(
				m_pStorage, 
				szContents, 
				nOpenFlags|CFile::shareExclusive,
				&fe))
				AfxThrowFileException(fe.m_cause, fe.m_lOsError);
		}
		CATCH_ALL(e)
		{
			SetError(BEC_errOpen);
			if (pFile)
			{
				pFile->Abort(); // will not throw an exception
				delete pFile;
				pFile = NULL;
			}
		}
		END_CATCH_ALL

  		return pFile;
	}
	else
	{
		ASSERT(m_lpFileName!=NULL);
		SCODE sc;

		m_pStorage = NULL;

		// test the m_lpFileName to see if it is a Compound Storage
		sc = StgIsStorageFile(m_lpFileName);

   		if (sc == S_OK)
		{
			// Open the Root Storage
			sc = StgOpenStorage(
				m_lpFileName, 
				NULL,
				STGM_READ|STGM_SHARE_EXCLUSIVE,
				NULL, 
				0, 
				&m_pStorage);

			if (FAILED(sc) || (m_pStorage == NULL))
			{
				SetError(BEC_errOpen);
				return(NULL);
			}
			COleStreamFile* pFile = NULL;
			TRY
			{
				CFileException fe;
				pFile = new COleStreamFile();
				if (!pFile->OpenStream(
					m_pStorage, 
					szContents, 
					nOpenFlags|CFile::shareExclusive,
					&fe))
					AfxThrowFileException(fe.m_cause, fe.m_lOsError);
			}
			CATCH_ALL(e)
			{
				SetError(BEC_errOpen);
				if (pFile)
				{
					pFile->Abort(); // will not throw an exception
					delete pFile;
					pFile = NULL;
				}
				ReleaseInterface((LPUNKNOWN*)&m_pStorage);
			}
			END_CATCH_ALL
			m_fOpenedStorage = TRUE;
  			return pFile;
		}
		else
		{
			CFile *pFile = new CFile();
			if (!pFile->Open(m_lpFileName, nOpenFlags))
			{
				delete pFile;
				pFile = NULL;
				SetError(BEC_errOpen);
				//Message (IDS_EOPEN, m_lpFileName);
			}
			return pFile;
		}
	}
}

void CReadBitmap::CloseFile(CFile *pFile)
{
    if (m_pFile == NULL)
    {
    	TRY
		{
        	pFile->Close();
			delete pFile;
		}
		CATCH_ALL(e)
		{
		}
    	END_CATCH_ALL
    }    
	if (m_pStorage != NULL && m_fOpenedStorage)
		ReleaseInterface((LPUNKNOWN*)&m_pStorage);
}

// function to read in the file header
BOOL CReadBitmap::ReadHeader()
{
	// only support PP5 to begin with
	ASSERT(m_FileType == BFT_PP || m_FileType == BFT_PPCOMPOUND);

	switch (m_FileType)
	{
		case BFT_PP:
        case BFT_PPCOMPOUND:
		return(PP5ReadHdr());
		break;

		default:
		return(FALSE);
	}
}

BOOL CReadBitmap::ReadPacketList(BOOL fReadLoadPacketOnly)
{
	BOOL fRet;

	// only support PP5 to begin with
	ASSERT(m_FileType == BFT_PP || m_FileType == BFT_PPCOMPOUND);

	switch (m_FileType)
	{
		case BFT_PP:
        case BFT_PPCOMPOUND:
		fRet = PP5ReadPacketList(fReadLoadPacketOnly);
		return(fRet);
		break;

		default:
		return(FALSE);
	}
}

LPOBJECT CReadBitmap::CreateObjectFromFrame(LPFRAME lpFrame)
{
	#ifdef UNUSED
	LPALPHA lpAlpha = MaskCreate(	NULL,
									FrameXSize(lpFrame),
									FrameYSize(lpFrame),
									TRUE,
									m_fNoUndo,
									FDT_GRAYSCALE);
	if (!lpAlpha)
		return(NULL);
	LPOBJECT lpObject = ObjCreateFromFrame(ST_PERMANENT, lpFrame, lpAlpha,
                                        NULL, m_fNoUndo);
	if (!lpObject)
		MaskClose(lpAlpha);
	#endif
	LPOBJECT lpObject = ObjCreateFromFrame(ST_PERMANENT, lpFrame, NULL,
                                        NULL, m_fNoUndo);

	return(lpObject);
}

// function to read in a frame
BOOL CReadBitmap::ReadObjectList(LPOBJECTLIST lpObjList)
{
	m_fReadNativeType = TRUE;
	return(ReadObjectList(lpObjList, m_cmsInfo.dst));
}

// function to read in a frame
BOOL CReadBitmap::ReadObjectList(LPOBJECTLIST lpObjList, FRMTYPEINFO TypeInfo)
{
	// read the data (either a frame or an object list)
	if (!ReadData(TypeInfo))
		return(FALSE);

	lpObjList->lpHead = lpObjList->lpTail = NULL;

	// did reader return a frame or an object list?
	if (m_lpFrame)
	{
		LPOBJECT lpObject = CreateObjectFromFrame(m_lpFrame);
        if (!lpObject)
		{
			SetError(BEC_errMemory);
            FrameClose(m_lpFrame);
		}
		else
		{
			ObjAddTail(lpObjList, lpObject);
		}
	}
	else
	{
		// there is an assumption that if a reader can return multiple
		// objects, then it can also sample objects
		if (!CanReaderSample() && (m_SampleMode != modeNoSampling))
			m_ObjList.lpHead = m_ObjList.lpTail = SampleObject(m_ObjList.lpHead);
		*lpObjList = m_ObjList;
	}
	return(TRUE);
}


// function to read in an object
LPOBJECT CReadBitmap::ReadObject()
{
	m_fReadNativeType = TRUE;
	return(ReadObject(m_cmsInfo.dst));
}

// function to read in an object
LPOBJECT CReadBitmap::ReadObject(FRMTYPEINFO TypeInfo)
{
	LPOBJECT lpObject;

	// read the data (either a frame or an object list)
	if (!ReadData(TypeInfo))
		return(NULL);

	// did reader return a frame or an object list?
	if (m_lpFrame)
	{
		// make an object
		lpObject = CreateObjectFromFrame(m_lpFrame);
        if (!lpObject)
		{
			SetError(BEC_errMemory);
            FrameClose(m_lpFrame);
		}
	}
	else
	{
		lpObject = GetSingleObject();
		if (lpObject && !CanReaderSample() && (m_SampleMode != modeNoSampling))
			lpObject = SampleObject(lpObject);
	}
	return(lpObject );
}

// function to read in a frame
LPFRAME CReadBitmap::ReadFrame()
{
	m_fReadNativeType = TRUE;
	return(ReadFrame(m_cmsInfo.dst));
}

// function to read in a frame
LPFRAME CReadBitmap::ReadFrame(FRMTYPEINFO TypeInfo)
{
	LPFRAME lpFrame = NULL;

	if (!ReadData(TypeInfo))
		return(NULL);

	if (m_lpFrame)
		lpFrame = m_lpFrame;
	else
	{
		LPOBJECT lpObject = GetSingleObject();
		if (lpObject)
		{
			lpFrame = ObjGetEditFrame(lpObject);
			lpObject->Pixmap.EditFrame = NULL;
			ObjFreeUp(lpObject);
		}
		if (lpFrame && !CanReaderSample() && (m_SampleMode != modeNoSampling))
  			lpFrame = SampleFrame(lpFrame);
	}

    return(lpFrame);
}

// function to read in a frame or object list
BOOL CReadBitmap::ReadData(FRMTYPEINFO TypeInfo)
{
	BOOL fRet = FALSE;

	m_cmsInfo.dst = TypeInfo;
    switch (m_FileType)
    {
        case BFT_TIFF:
			fRet = TiffRead();
			break;
				
		case BFT_PP4:
			fRet = PP4Read();
			break;

		case BFT_PP:
		case BFT_PPCOMPOUND:
			fRet = PP5Read();
			break;

        case BFT_TARGA:
			fRet = TGARead();
			break;

        case BFT_BMP:
        case BFT_DIB:
			fRet = BMPRead();
            break;

        case BFT_SCITEX: 
            fRet = CTRead();
            break;

        case BFT_GIF: 
            fRet = GIFRead();
            break;

        case BFT_PCX:
            fRet = PCXRead();
            break;

        case  BFT_JPEG:
            fRet = JPGRead();
            break;

        case BFT_AVI:
			// fRet = AVIRead();
			//ITEMID DataType;
             //lpFrame = avirdr(m_lpFileName, (int)m_outType.DataType, &DataType, -1);  /* Load AVI image */
			//m_inDataType = DataType;
            break;
        case BFT_PHOTOCD:
			fRet = PCDRead();
            break;				    

        default:
			SetError(BEC_errBadFormat);
			//Message(IDS_EWRONGDATAOPEN);
            break;
    }
	// frames will be sampled on the way in in ReadData() cause
	// then we only need to handle it in one place.  If an
	// object list is read in then the caller of ReadData() must
	// do the sampling because the context of the call needs to
	// be known to sample efficiently (i.e. - don't sample the
	// entire object list if only one object or frame is desired)
	if (m_lpFrame && !CanReaderSample() && (m_SampleMode != modeNoSampling))
  		m_lpFrame = SampleFrame(m_lpFrame);

    return(fRet);
}

// determine whether reader can sample
BOOL CReadBitmap::CanReaderSample()
{
    return( m_FileType == BFT_TIFF || m_FileType == BFT_PP4 ||
			m_FileType == BFT_PP   || m_FileType == BFT_PPCOMPOUND );
}

// determine whether reader can crop
BOOL CReadBitmap::CanCrop()
{
	return( m_FileType == BFT_TIFF );
}

void CReadBitmap::SetSampling(LPRECT lpRect)
{
    m_fCrop = FALSE;
	m_SampleMode = modeSampleRect;
    m_rArea = *lpRect;   
}

void CReadBitmap::SetSampling(int res)
{
    m_fCrop = FALSE;
	m_SampleMode = CReadBitmap::modeSampleRes;
	m_SampleRes = res;
}

void CReadBitmap::SetCropping(LPRECT lpRect)
{
    m_SampleMode = CReadBitmap::modeNoSampling;
    m_fCrop = TRUE;
    m_rArea = *lpRect;   
}

// function to get the packet list read in
void CReadBitmap::GetPacketList(LPLIST lpPacketList)
{
    *lpPacketList = m_PacketList;
	ListInit(&m_PacketList);
}

// function to get the packet list read in
int CReadBitmap::GetFileDepth()
{
	if (m_BitsPerPixel == 1)
		return(0);
	else
	if (m_BitsPerPixel < 8)
		return(1);
	else
		return(m_BitsPerPixel / 8);
}

LPOBJECT CReadBitmap::GetSingleObject()
{
	LPOBJECT lpObject;

	if (m_fReadCombined)
		lpObject = CombineToBase(&m_ObjList);
	else
	{
		LPOBJECT lpNext;
		// we're only interested in the first object
		lpObject = (LPOBJECT)ObjGetNextObject(&m_ObjList, NULL, TRUE);
		while (lpNext = ObjGetNextObject(&m_ObjList, lpObject, YES))
			ObjDeleteObject(&m_ObjList , lpNext);
	}
	return(lpObject);
}

LPOBJECT CReadBitmap::CombineToBase(LPOBJECTLIST lpObjList)
{
	int y, depth, iWidth;
	RECT rFloaters;
	LPFRAME lpFrame;
	LPTR lpDataBuf;
	LPOBJECT lpObject, lpBase;

	// get the first base object
	lpBase = (LPOBJECT)ObjGetNextObject(lpObjList, NULL, YES);
	ASSERT(lpBase);

	// get the base objects frame and depth
	lpFrame = ObjGetEditFrame(lpBase);
	depth = FrameDepth(lpFrame);
	if (!depth)
		depth = 1;

	// see if we have enough objects for combine
	lpObject = (LPOBJECT)ObjGetNextObject(lpObjList, lpBase, YES);
	if (lpObject)
	{
		AstralSetRectEmpty(&rFloaters);
		while (lpObject)
		{
			AstralUnionRect(&rFloaters, &rFloaters, &lpObject->rObject);	
			lpObject = (LPOBJECT)ObjGetNextObject(lpObjList, lpObject, YES);
		}
		
		// ensure that we dont go over bounds
		AstralIntersectRect(&rFloaters,&rFloaters, &lpBase->rObject);
		
		iWidth = RectWidth(&rFloaters);
		lpDataBuf = (LPTR)Alloc((long)iWidth * depth);
		if (!lpDataBuf)
		{
			ObjDeleteObjects(lpObjList, ST_ALL);
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			return(NULL);
		}
		// do it to it
		for (y = rFloaters.top; y <= rFloaters.bottom; ++y)
    	{
    		// FIX ImgGetLine(NULL, lpBase, rFloaters.left, y, iWidth, lpDataBuf);
    		FrameWrite(lpFrame, rFloaters.left, y, iWidth,
                		lpDataBuf, iWidth);
    	}
		FreeUp(lpDataBuf);
	}
	ObjUnlinkObject(lpObjList, lpBase);
	// wack the rest of the object list
	ObjDeleteObjects(lpObjList, ST_ALL);
	return(lpBase);
}


//***************************************************************************
LPOBJECT CReadBitmap::SampleObject(LPOBJECT lpObject)
//***************************************************************************
{
	LPFRAME lpDataFrame = ObjGetEditFrame(lpObject);
	LPALPHA lpAlpha = ObjGetAlpha(lpObject);

	lpDataFrame = SampleFrame(lpDataFrame);
	if (!lpDataFrame)
	{
		lpObject->Pixmap.EditFrame = NULL;
		ObjFreeUp(lpObject);
		return(NULL);
	}
	lpObject->Pixmap.EditFrame = lpDataFrame;

	if (lpAlpha)
	{
		LPFRAME lpAlphaFrame = AlphaGetEditFrame(lpAlpha);
		lpAlphaFrame = SampleFrame(lpAlphaFrame);
		if (!lpAlphaFrame)
		{
			lpAlpha->Pixmap.EditFrame = NULL;
			ObjFreeUp(lpObject);
			return(NULL);
		}
		lpAlpha->Pixmap.EditFrame = lpAlphaFrame;
	}
	lpObject->rObject.right = lpObject->rObject.left + FrameXSize(lpDataFrame) - 1;
	lpObject->rObject.bottom = lpObject->rObject.top + FrameYSize(lpDataFrame) - 1;
	return(lpObject);
}

//***************************************************************************
LPFRAME CReadBitmap::SampleFrame(LPFRAME lpFrame)
//***************************************************************************
{
	LPFRAME lpOutFrame;
	LFIXED 	xrate, yrate, line;
	LPTR	lpSrc, lpDst;
	int 	yread;
	int 	y, lin, pix, xsize, ysize;
	
	// find size
	xsize = FrameXSize(lpFrame);
	ysize = FrameYSize(lpFrame);
	if (m_SampleMode == modeSampleRect)
	{
		lin = RectHeight(&m_rArea);
		pix = RectWidth(&m_rArea);
	}
	else
	{
		xrate = FGET(m_SampleRes, FrameResolution(lpFrame));
		pix = FMUL(xsize, xrate);
		lin = FMUL(ysize, xrate);
	}

	if (lin > ysize)
		lin = ysize;
	if (pix > xsize)
		pix = xsize;
	ScaleToFit(&pix, &lin, xsize, ysize);
	lin = Max(1, lin);
	pix = Max(1, pix);	
	xrate = FGET(xsize, pix);
	yrate = FGET(ysize, lin);

	// get new frame 
	lpOutFrame = FrameOpen(FrameTypeInfo(lpFrame), 
						pix, 
						lin,
						FrameResolution(lpFrame));
	if (!lpOutFrame)
	{
		FrameClose(lpFrame);
		return(NULL);
	}
	// sample frame
	line = FDIV2(yrate);
	yread = 0;
	for (y=0;y<lin;y++)
	{
		yread = WHOLE( line );
		line += yrate; 
		lpDst = FramePointer(lpOutFrame, 0,y,YES);
		lpSrc = FramePointer(lpFrame, 0,yread,NO);
		if (!lpSrc || !lpDst)
		{
			FrameClose(lpOutFrame);
			return(NULL);
		}
		FrameSample(lpOutFrame, lpSrc, 0, lpDst, 0, pix, xrate);
	} 	// for y
	FrameClose(lpFrame);
	return(lpOutFrame);
}


BOOL CReadBitmap::SetupTypes(
	LPFRMTYPEINFO	pInType,		// the data type in the file
	LPFRMTYPEINFO	pOutType,	// the output data type
	BOOL			fGraymap,	// if a palette colored palette is gray
 	LPVOID lpXform)			// transform read from the file
{
	// Make adjustments to output depth
	*pOutType = m_cmsInfo.dst;
	if ( pOutType->DataType == FDT_NONE ) // Read Native Data?
	{
		*pOutType = *pInType;
		if( (pOutType->DataType == FDT_PALETTECOLOR && fGraymap) ||
			(pOutType->DataType == FDT_LINEART && m_fLineArtAsGray) )
			FrameSetTypeInfo( pOutType, FDT_GRAYSCALE );
	}

	// see if the image is color.  If not, it is not an error
	// we just can't color manage it
	if (!m_CMSEnabled ||
		!IsDstPTSelected(&pOutType->ptInfo) ||
		(pInType->DataType != FDT_RGBCOLOR &&
		 pInType->DataType != FDT_CMYKCOLOR))
	{
		// make sure color management stuff gets wacked so we
		// don't try to do a conversion when we shouldn't
		pOutType->ptInfo.frRCS = 0;
		pOutType->ptInfo.toRCS = 0;
	}
	else						 
	// any xform in the file?
	if (lpXform == NULL)
	{
		// do we have a src PT specified 
		if (IsSrcPTSelected(&m_cmsInfo.src.ptInfo))
		{
			if (pInType->DataType != m_cmsInfo.src.DataType)
			{
				if (pInType->DataType == FDT_RGBCOLOR)
					SetError(BEC_errFileRGBPTCMYK);
				else
					SetError(BEC_errFileCMYKPTRGB);
				return(FALSE);
			}
			// intype becomes source user selected if all is well
			*pInType = m_cmsInfo.src;
			return(TRUE);
		}
	}
	else
	{
		// if we reading in PT data then wack the ptInfo in inType
		// so that FrameTypeInfoEqual() calls will fail and data
		// will get color managed
		pInType->ptInfo.toRCS = pInType->ptInfo.frRCS = -1;
	}
	// we always set source to the intype we are going to use so
	// that the caller can query the source used
	m_cmsInfo.src = *pInType;
	return(TRUE);
}

// Constructor
CWriteBitmap::CWriteBitmap( LPTSTR lpFileName, BITMAP_FILE_TYPE FileType )
{
	ASSERT( lpFileName );

	Init();

    if ( FileType )
    {
        m_FileType = FileType;

        // force file extension based on file type
        if( !SetFileExt( lpFileName, FileType ))    // if ID no good, set
            m_FileType = GetFileTypeFromExt( lpFileName );   // ext, filetype to TIF
    }
    else
        m_FileType = GetFileTypeFromExt( lpFileName );

    m_lpFileName = lpFileName;

	ASSERT( m_FileType );
}

// Constructor
CWriteBitmap::CWriteBitmap(CFile *pFile, BITMAP_FILE_TYPE FileType, BOOL bEscapable)
{
	ASSERT(pFile != NULL);
	ASSERT(FileType != BFT_UNKNOWN);

	Init();
    m_pFile = pFile;
    m_bEscapable = bEscapable;
    m_FileType = FileType;
	
	// writers that do not support CFile's
	ASSERT(m_FileType != BFT_JPEG);
	ASSERT(m_FileType != BFT_PHOTOCD);
}

void CWriteBitmap::Init()
{
    m_lpFrame = NULL;
    m_lpObjList = NULL;
    m_lpFileName = NULL;
    m_pFile = NULL;
    m_DataType = FDT_NONE;
    m_fSrcArea = NO;
    AstralSetRectEmpty(&m_rSrcArea);
    m_lpOptions = NULL;
	m_bEscapable = TRUE;
	ListInit(&m_PacketList);
	m_bUpdateThumb = FALSE;
	m_lpColorMap = NULL;
	m_pStorage = NULL;

	m_BitsPerPixel = 8;
	m_DitherType = DT_DEFAULT;

	m_fOpenedStorage = FALSE;
}

CFile *CWriteBitmap::OpenFile(UINT nOpenFlags)
{
	const TCHAR szContents[] = _T("Contents");
	CFile *pFile = NULL;
	COleStreamFile *pOleFile = NULL;
    
	TRY
	{
		if (m_pFile != NULL)
			pFile = m_pFile;
		else
    	if (m_pStorage != NULL)
		{
			CFileException fe;
			static const TCHAR szContents[] = _T("Contents");
			COleStreamFile* pOleFile = new COleStreamFile();
			if (nOpenFlags & CFile::modeCreate)
			{
        		if (!pOleFile->CreateStream(
            		m_pStorage, 
            		szContents,
        			nOpenFlags|CFile::shareExclusive, 
            		&fe))
        		{
        			AfxThrowFileException(fe.m_cause, fe.m_lOsError);
        		}
			}
			else
			{
        		if (!pOleFile->OpenStream(
            		m_pStorage, 
            		szContents,
        			nOpenFlags|CFile::shareExclusive, 
            		&fe))
        		{
        			AfxThrowFileException(fe.m_cause, fe.m_lOsError);
        		}
			}
			pFile = (CFile *)pOleFile;
		}
		else
		{
			if (nOpenFlags == CFile::modeReadWrite)
			{
				// test the m_lpFileName to see if it is a Compound Storage
				SCODE sc = StgIsStorageFile(m_lpFileName);
   				if (sc == S_OK)
				{
					// Open the Root Storage
					sc = StgOpenStorage(
						m_lpFileName, 
						NULL,
						STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
						NULL, 
						0, 
						&m_pStorage);

					if (FAILED(sc) || (m_pStorage == NULL))
					{
						SetError(BEC_errOpen);
						return(NULL);
					}
					COleStreamFile* pFile = NULL;
					TRY
					{
						CFileException fe;
						pFile = new COleStreamFile();
						if (!pFile->OpenStream(
							m_pStorage,
							szContents, 
							nOpenFlags|CFile::shareExclusive,
							&fe))
							AfxThrowFileException(fe.m_cause, fe.m_lOsError);
					}
					CATCH_ALL(e)
					{
						SetError(BEC_errOpen);
						if (pFile)
						{
							pFile->Abort(); // will not throw an exception
							delete pFile;
							pFile = NULL;
						}
						ReleaseInterface((LPUNKNOWN*)&m_pStorage);
					}
					END_CATCH_ALL
					m_fOpenedStorage = TRUE;
  					return pFile;
				}
			}
			pFile = new CFile();
			if (!pFile->Open(m_lpFileName, nOpenFlags))
			{
				delete pFile;
				pFile = NULL;
				SetError(BEC_errWriteFile);
			}
		}
	}
	CATCH_ALL(e)
	{
		if (pFile)
		{
			delete pFile;
			pFile = NULL;
		}
		if (pOleFile)
			delete pOleFile;
 		SetError(BEC_errWrite);
 	}
	END_CATCH_ALL

	return pFile;
}

void CWriteBitmap::CloseFile(CFile *pFile, BOOL fSuccess)
{
	if (m_pFile == NULL)
	{
		TRY
		{
			pFile->Close();
			delete pFile;
			if (!fSuccess && !m_pStorage)
				CFile::Remove(m_lpFileName);
		}
		CATCH_ALL(e)
		{
		}
		END_CATCH_ALL
	}
	if (m_pStorage != NULL && m_fOpenedStorage)
		ReleaseInterface((LPUNKNOWN*)&m_pStorage);
}

// function to write out an object list
BOOL CWriteBitmap::WriteObjectList(LPOBJECTLIST lpObjList, FRMDATATYPE DataType)
{
	return(WriteData(NULL, lpObjList, DataType));
}

// function to write out an object
BOOL CWriteBitmap::WriteObject(LPOBJECT lpObject, FRMDATATYPE DataType)
{
OBJECTLIST ObjList;

ObjList.lpHead = ObjList.lpTail = lpObject;
return(WriteData(NULL, &ObjList, DataType));
}

// function to write out a frame or an object
BOOL CWriteBitmap::WriteFrame(LPFRAME lpFrame, FRMDATATYPE DataType)
{
return(WriteData(lpFrame, NULL, DataType));
}

#pragma optimize( "", off)

// function to write out a frame or an object
BOOL CWriteBitmap::WriteData(LPFRAME lpFrame, LPOBJECTLIST lpObjList, FRMDATATYPE DataType)
{
    long lMaxSize;
	BOOL fRet;
	int iResult;
    FNAME szName, szTempName;
    BOOL bTemp = FALSE;
    LPRECT lpSrcRect;
    FRMTYPEINFO TypeInfo;
	LPOBJECT lpObject = NULL;
	BOOL fFreeOptions = FALSE;

    if (lpObjList)
        lpFrame = ObjGetEditFrame((LPOBJECT)lpObjList->lpHead);

	ASSERT(lpFrame);

	if (!m_lpOptions)
	{
		if (!SetDefaultOptions())
		{
			SetError(BEC_errMemory);
			return(FALSE);
		}
		fFreeOptions = m_lpOptions != NULL;
	}
		
    m_lpFrame = lpFrame;
    m_lpObjList = lpObjList;

    FrameGetTypeInfo(m_lpFrame, &TypeInfo);
	if (DataType == FDT_NONE)
		m_DataType = TypeInfo.DataType;
	else
	    m_DataType = DataType;

    if ( TypeInfo.DataType <= FDT_GRAYSCALE && m_DataType == FDT_PALETTECOLOR )
        m_DataType = FDT_GRAYSCALE;

    if (m_fSrcArea)
        lpSrcRect = &m_rSrcArea;
    else
        lpSrcRect = NULL;
    if ( lpSrcRect )
    {
        BoundRect(lpSrcRect, 0, 0, FrameXSize(m_lpFrame)-1, FrameYSize(m_lpFrame)-1);
        lMaxSize = (long)RectWidth(lpSrcRect) * RectHeight(lpSrcRect);
    }
    else
        lMaxSize = (long)FrameXSize(m_lpFrame) * FrameYSize(m_lpFrame);

   	if (!m_lpOptions)
   	{
		// what do we do here, manufacture some options?
   	}

	if (GetStorage())
		m_bEscapable = FALSE;
	else
	if (m_lpFileName)
	{
    	lstrcpy(szName, m_lpFileName);
    	if (m_FileType != BFT_JPEG)
    	{
        	switch (DataType)
        	{
            	case FDT_CMYKCOLOR:
                	lMaxSize = lMaxSize * 4;
                	break;
            	case FDT_RGBCOLOR:
                	lMaxSize = lMaxSize * 3;
                	break;
            	case FDT_LINEART:
                	lMaxSize = lMaxSize / 8;
                	break;
            	default:
                	break;
        	}
        	if ( m_FileType == BFT_EPS || m_FileType == BFT_DCS )
            	lMaxSize = lMaxSize * 2;
        	lMaxSize = lMaxSize + 5000;
        	if ( lMaxSize > DiskSpace(szName) )
        	{
				SetError(BEC_errDiskSpace);
            	//Message( IDS_EDISKSPACE, Lowercase(szName),
                //	(lMaxSize+512)/1024, (DiskSpace(szName)+512)/1024 );
    			if (fFreeOptions && m_lpOptions)
 				{
        			FreeUp(m_lpOptions);
					m_lpOptions = NULL;
 				}
            	return( FALSE );
        	}
    	}

    	// if file already exists, copy it into a temporary
    	// file in case write fails
    	if (FileExists(szName))  // is this an overwrite?
    	{ 
        	// make sure we have write access to destination file
        	if (!FilePermission(szName,  2 /*WRITE*/))  // write access?
        	{
				SetError(BEC_errWriteProtect);
            	//Message(IDS_EWRITEPROTECT, m_lpFileName);
    			if (fFreeOptions && m_lpOptions)
 				{
        			FreeUp(m_lpOptions);
					m_lpOptions = NULL;
 				}
            	return(FALSE);
        	}
        	// create temporary file name
        	lstrcpy(szTempName, szName);
        	stripfile(szTempName);
        	lstrcat(szTempName, _T("pp"));
        	if (GetTempFilename(szTempName, extension(szName)))
            	// temp name creation was successful
            	bTemp = RenameFile(szName, szTempName); // rename original
    	}
		if (m_bEscapable)
			m_bEscapable = !FileExists(m_lpFileName);
	}

    iResult = -1;
    switch (m_FileType)
   {
        case BFT_PP4:
        fRet = PP4Write();
        break;
        case BFT_PP:
        fRet = PP5Write();
		break;
        case BFT_PPCOMPOUND:
        fRet = PPFWrite();
        break;
        case BFT_TIFF:
        fRet = TiffWrite();
        break;
        case BFT_BMP:
        case BFT_DIB:
        fRet = BMPWrite();
        break;
        case BFT_SCITEX:
        fRet = CTWrite();
        break;
        case BFT_GIF:
        fRet = GIFWrite();
        break;
        case BFT_TARGA:
        fRet = TGAWrite();
        break;
        case BFT_PCX:
        fRet = PCXWrite();
        break;
        case BFT_EPS:
	    fRet = EPSWrite(FALSE);
        break;
        case BFT_DCS:
		fRet = EPSWrite(TRUE);
	    break;
        case BFT_JPEG:
        fRet = JPGWrite();
        break;
        case BFT_AVI:
		//if (m_lpObjList)
		//	lpObject = m_lpObjList->lpHead;
        //if (bTemp)
        //    iResult = aviwrt( szName, lpObject, m_lpFrame, lpSrcRect, idDataType, szTempName );
        //else
        //    iResult = aviwrt( szName, lpObject, m_lpFrame, lpSrcRect, idDataType, NULL);
		//fRet = iResult >= 0;
        break;
        default:
        break;
   }

	// save thumbnail
	if (fRet && m_bUpdateThumb)
	{
		write_thumb(szName, lpFrame, m_lpObjList, DataType, m_FileType);
	}

    // see if we created a temporary save file
    if (bTemp)
    {
        if (fRet)  // successful write, bag temp file
            FileDelete(szTempName); // don't care if error
        else        // unsuccessful write, rename temp to original
        {
            if (!RenameFile(szTempName, szName)) // do rename
                FileDelete(szTempName); // error, bag temp file
        }
    }

    if (fFreeOptions && m_lpOptions)
 	{
        FreeUp(m_lpOptions);
		m_lpOptions = NULL;
 	}

    return(fRet);
}

#pragma optimize( "", on)

BOOL CWriteBitmap::SetDefaultOptions()
{
	switch (m_FileType)
	{
        case BFT_TIFF:
		{
			LPTIFFOPTIONS lpOptions = (LPTIFFOPTIONS)Alloc(sizeof(TIFFOPTIONS));
			if (!lpOptions)
				return(FALSE);
			lpOptions->TIFFCompress = FALSE;
			lpOptions->TIFFDiff = FALSE;
			lpOptions->bWritePT = FALSE;	 
			lpOptions->bSaveMask = FALSE;
			m_lpOptions = (LPVOID)lpOptions;
			break;
		}	
        case BFT_TARGA:
		{
			LPTGAOPTIONS lpOptions = (LPTGAOPTIONS)Alloc(sizeof(TGAOPTIONS));
			if (!lpOptions)
				return(FALSE);
			lpOptions->bSaveMask = FALSE;
			m_lpOptions = (LPVOID)lpOptions;
			break;
		}	
        case BFT_JPEG:
		{
			LPJPEGOPTIONS lpOptions = (LPJPEGOPTIONS)Alloc(sizeof(JPEGOPTIONS));
			if (!lpOptions)
				return(FALSE);
			lpOptions->nCompressionFactor = 50;
			lpOptions->nSubsampling = 1;
			m_lpOptions = (LPVOID)lpOptions;
			break;
		}	
		case BFT_PP:
		case BFT_PPCOMPOUND:
		case BFT_PP4:
		{
			LPPPFFOPTIONS lpOptions = (LPPPFFOPTIONS)Alloc(sizeof(PPFFOPTIONS));
			if (!lpOptions)
				return(FALSE);
			lpOptions->PPFFCompress = FALSE;
			lpOptions->bSaveMask = FALSE;
			lpOptions->bSaveProxy = FALSE;
			lpOptions->bSaveCmdList = FALSE;
			lpOptions->bWritePT = FALSE;		 
			lpOptions->bSaveLink = FALSE;
			lpOptions->bSaveAsCompoundStg = FALSE;
			m_lpOptions = (LPVOID)lpOptions;
			break;
		}

        case BFT_PHOTOCD:
		{
			LPPCDOPTIONS lpOptions = (LPPCDOPTIONS)Alloc(sizeof(PCDOPTIONS));
			if (!lpOptions)
				return(FALSE);
			lpOptions->PhotoCDFormat = 0;
			lpOptions->PhotoCDRes = 0;	  
			lpOptions->BrightButton = 0;
			lpOptions->RedButton = 0;
			lpOptions->GreenButton = 0;
			lpOptions->BlueButton = 0;
			lpOptions->SatButton = 0;
			lpOptions->OutOfGamut = FALSE;
		   	lpOptions->SubtractSBA = FALSE;
			lpOptions->LutName[0] = '\0';
			lpOptions->fHaveArea = FALSE;
			lpOptions->rArea;
			m_lpOptions = (LPVOID)lpOptions;
            break;
		}
		default:
			break;
	}
	return(TRUE);
}

// function to write out a packet list to a file
BOOL CWriteBitmap::UpdatePacketList(LPLIST lpPacketList)
{
	ASSERT(m_FileType == BFT_PP || m_FileType == BFT_PPCOMPOUND);
	SetPacketList(lpPacketList);
	return(PP5Update());
}
// function to set source area for write
void CWriteBitmap::SetSourceArea(LPRECT lpRect)
{
    m_fSrcArea = YES;
    m_rSrcArea = *lpRect;
}

// function to set write specific options
void CWriteBitmap::SetOptions(LPVOID lpWriteOptions)
{
    m_lpOptions = lpWriteOptions;
}

// function to set the packet list to write
void CWriteBitmap::SetPacketList(LPLIST lpPacketList)
{
    m_PacketList = *lpPacketList;
}

LOCAL void write_thumb(LPTSTR lpName, LPFRAME lpFrame, LPOBJECTLIST lpObjList, FRMDATATYPE DataType, BITMAP_FILE_TYPE FileType)
{
	RECT rArea;
 	LPFRAME lpCombinedFrame;
	
	if (!lpFrame && !lpObjList)
		return;
	if (!lpFrame)
		 lpFrame = ObjGetEditFrame((LPOBJECT)lpObjList->lpHead);
	if (!lpFrame)
		return;
	
	lpCombinedFrame = lpFrame;
	
	if (lpObjList)
	{
		
		rArea.left = rArea.top = 0;
		rArea.right = 63;
		rArea.bottom = 63;
		// FIX lpCombinedFrame = ImgCopyFrame(NULL,lpObjList->lpHead,&rArea,NO,NO);
	}
	// FIX if (lpCombinedFrame)
	//	UpdateThumbnail(lpName,
	//		lpCombinedFrame,
	//		FrameXSize(lpFrame), 
	//		FrameYSize(lpFrame),
	//		idDataType,
	//		FileType);
	//if (lpCombinedFrame && lpCombinedFrame != lpFrame)
	//	FrameClose(lpCombinedFrame);
}

//////////////////////////////////////////////////////////////////////////
//  truncates the existing file extension and replaces it with the correct 
//  extension based on the specified file type.
//
//  If file type passed is not in the list, file name extension is set to .TIF 
//  and the caller is warned in the return value to set new file type.
//////////////////////////////////////////////////////////////////////////
LOCAL BOOL SetFileExt(LPTSTR lpFileName, BITMAP_FILE_TYPE FileType)
{
    BOOL bValidID = TRUE;

    // remove the extension, if it exists
    char *pDot = extension( lpFileName );
    if( pDot )
        *pDot = '\0';        

    // Force the extension based on the file type
    if ( FileType == BFT_SCITEX )   lstrcat( lpFileName, _T(".sct")); else
    if ( FileType == BFT_EPS )      lstrcat( lpFileName, _T(".eps")); else   
    if ( FileType == BFT_TIFF )     lstrcat( lpFileName, _T(".tif")); else
    if ( FileType == BFT_BMP )      lstrcat( lpFileName, _T(".bmp")); else
    if ( FileType == BFT_DIB )      lstrcat( lpFileName, _T(".dib")); else
    if ( FileType == BFT_GIF )      lstrcat( lpFileName, _T(".gif")); else
    if ( FileType == BFT_PCX )      lstrcat( lpFileName, _T(".pcx")); else
    if ( FileType == BFT_TARGA )    lstrcat( lpFileName, _T(".tga")); else
    if ( FileType == BFT_JPEG )     lstrcat( lpFileName, _T(".jpg")); else
    if ( FileType == BFT_JPEG )     lstrcat( lpFileName, _T(".jif")); else
    if ( FileType == BFT_AVI )      lstrcat( lpFileName, _T(".avi")); else
    if ( FileType == BFT_PHOTOCD )  lstrcat( lpFileName, _T(".pcd")); else
    if ( FileType == BFT_DCS ) 	  	lstrcat( lpFileName, _T(".dcs")); else
    if ( FileType == BFT_PP ) 	  	lstrcat( lpFileName, _T(".pp5")); else
    if ( FileType == BFT_PPCOMPOUND ) lstrcat( lpFileName, _T(".ppf")); else
    if ( FileType == BFT_PP4 )      lstrcat( lpFileName, _T(".pp4")); else
    {
        // if file type not in the list, set extension to TIF and inform 
        // caller that file type is not in the list.
        lstrcat( lpFileName, _T(".tif"));
        bValidID = FALSE;
    }
    return( bValidID );
}

LOCAL BITMAP_FILE_TYPE GetFileTypeFromExt(LPTSTR lpFileName)
{
	BITMAP_FILE_TYPE Type = BFT_UNKNOWN;
	LPCTSTR lp;
	TCHAR tmp[4] = { 0,0,0,0 };

	// find last dot '.' in name
	lp = extension(lpFileName);
	if( !( *lp++ ))
		return( Type );

	// put extension into local buffer, can make lower case
	if ( *lp )
		tmp[0] = tolower( *lp++ );
	if ( *lp )
		tmp[1] = tolower( *lp++ );
	if ( *lp )
		tmp[2] = tolower( *lp );

	// Force the file type if the extension is a known one
	if ( StringsEqual( _T("sct"), tmp )) Type = BFT_SCITEX;		else
	if ( StringsEqual( _T("eps"), tmp )) Type = BFT_EPS;		else    
	if ( StringsEqual( _T("tif"), tmp )) Type = BFT_TIFF;		else
	if ( StringsEqual( _T("bmp"), tmp )) Type = BFT_BMP;		else
	if ( StringsEqual( _T("dib"), tmp )) Type = BFT_DIB;		else
	if ( StringsEqual( _T("gif"), tmp )) Type = BFT_GIF;		else
	if ( StringsEqual( _T("pcx"), tmp )) Type = BFT_PCX;		else
	if ( StringsEqual( _T("tga"), tmp )) Type = BFT_TARGA;		else
	if ( StringsEqual( _T("jpg"), tmp )) Type = BFT_JPEG;		else
	if ( StringsEqual( _T("jif"), tmp )) Type = BFT_JPEG;		else
	if ( StringsEqual( _T("avi"), tmp )) Type = BFT_AVI;		else
	if ( StringsEqual( _T("pcd"), tmp )) Type = BFT_PHOTOCD; 	else
	if ( StringsEqual( _T("dcs"), tmp )) Type = BFT_DCS; 		else
	if ( StringsEqual( _T("pp5"), tmp )) Type = BFT_PP; 		else
	if ( StringsEqual( _T("ppf"), tmp )) Type = BFT_PPCOMPOUND;	else
	if ( StringsEqual( _T("pp4"), tmp )) Type = BFT_PP4;

	return(Type);
}




