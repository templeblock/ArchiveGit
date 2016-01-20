//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "ppafx.h"

BOOL CImage::WritePP4ClipOut(LPTR lpFileName, LPTR lpMaskName,
					LPRECT lpRect, int DataType )
{
	FNAME szFileName, szMaskName;
	LPFRAME lpDataFrame, lpAlphaFrame;
	LPALPHA lpAlpha;
	LPOBJECT lpObject;
	LPTR lpAlphaBuf;
	RECT rClip;
	BOOL bSaveNames;
	int iWidth, iHeight, y;
	
	if ( !lpFileName )
	{
		bSaveNames = YES;
        FormFileName (szFileName, IDN_CLIPBOARD,"WINCLIP.TIF");
		lpFileName = (LPTR)szFileName;
        FormFileName (szMaskName, IDN_MASK,"WINMASK.TIF");

		lpMaskName = (LPTR)szMaskName;
	}
	else
		bSaveNames = NO;

	if (GetMaskRect(&rClip))
	{ // There is a mask...
		ProgressBegin(lpMaskName ? 2 : 1, 0);
		if (lpMaskName)
		{
			lpAlpha = GetMask( );
			CWriteImage w((LPSTR)lpMaskName,IDN_TIFF);
			w.SetSourceArea((LPRECT)&rClip);
			if (!w.WriteFrame(lpAlpha->Pixmap.EditFrame,IDC_SAVECT))
			{
				ProgressEnd();
				return( NO );
			}
		}
		if ( !MultipleObjects( ) )
		{ // Mask with only one object (the base)

			CWriteImage w((LPSTR)lpFileName,IDN_TIFF);
			w.SetSourceArea((LPRECT)&rClip);
			if (!w.WriteFrame(GetBaseEditFrame(),DataType))
			{
				if (lpMaskName)
					FileDelete((LPSTR) lpMaskName );
				ProgressEnd();
				return( NO );
			}
		}
		else
		{ // Mask with multiple objects
			if (!(lpDataFrame = ImgCopyFrame(this, NULL, &rClip, YES, NO)))
			{
				ProgressEnd();
				Message(IDS_EMEMALLOC);
				if (lpMaskName)
					FileDelete((LPSTR) lpMaskName );
				return( NO );
			}
			CWriteImage w((LPSTR)lpFileName,IDN_TIFF);
			w.SetSourceArea((LPRECT)&rClip);
			if (!w.WriteFrame(lpDataFrame,DataType))
			{
				ProgressEnd();
				FrameClose( lpDataFrame );
				if (lpMaskName)
					FileDelete((LPSTR) lpMaskName );
				return( NO );
			}
			FrameClose( lpDataFrame );
		}
	}
	else
	{ // No mask...
		GetSelObjectRect(&rClip, NO);
		if ( CountSelObjects( NULL ) == 1 &&
			 GetSelObject(NULL) == GetBase( ) )
		{ // Only the base is selected
			ProgressBegin(1, 0);
			CWriteImage w((LPSTR)lpFileName,IDN_TIFF);
			w.SetSourceArea((LPRECT)&rClip);
			if (!w.WriteFrame(GetBaseEditFrame(),DataType))
			{
				ProgressEnd();
				return( NO );
			}
			if (lpMaskName)
				lpMaskName[0] = '\0'; // No mask file will be created
			SetRectEmpty( &rClip );
		}
		else
		{ // Multiple objects are selected
			ProgressBegin(lpMaskName ? 2 : 1, 0);
			if (!(lpDataFrame = ImgCopyFrame(this, NULL, &rClip, YES, YES)))
			{
				ProgressEnd();
				Message(IDS_EMEMALLOC);
				return( NO );
			}
			CWriteImage w((LPSTR)lpFileName,IDN_TIFF);
			w.SetSourceArea((LPRECT)&rClip);
			if (!w.WriteFrame(lpDataFrame,DataType))
			{
				ProgressEnd();
				FrameClose( lpDataFrame );
				return( NO );
			}
			FrameClose( lpDataFrame );
			if (lpMaskName)
			{
				iWidth = RectWidth(&rClip);
				iHeight = RectHeight(&rClip);
				if ( !(lpAlphaFrame = FrameOpen(FDT_GRAYSCALE, iWidth, iHeight, 75 )) )
				{
					ProgressEnd();
					Message(IDS_EMEMALLOC);
					FileDelete( (LPSTR)lpFileName );
					return( NO );
				}
				if (!(lpAlphaBuf = Alloc((long)iWidth)))
				{
					ProgressEnd();
					Message(IDS_EMEMALLOC);
					FrameClose(lpAlphaFrame);
					FileDelete( (LPSTR)lpFileName );
					return( NO );
				}
				lpObject = GetSelObject(NULL);
				for (y = 0; y < iHeight; ++y)
				{
					ImgMaskLoad(this, lpObject, NULL, rClip.left, y+rClip.top,
						iWidth, lpAlphaBuf, YES, CR_OR, 0, 0);
					FrameWrite(lpAlphaFrame, 0, y, iWidth, lpAlphaBuf, iWidth);
				}
				FreeUp( lpAlphaBuf );
				CWriteImage w((LPSTR)lpMaskName,IDN_TIFF);
				w.SetSourceArea((LPRECT)&rClip);
				if (!w.WriteFrame(lpAlphaFrame,IDC_SAVECT))
				{
					ProgressEnd();
					FrameClose(lpAlphaFrame);
					FileDelete( (LPSTR)lpFileName );
					return( NO );
				}
				FrameClose(lpAlphaFrame);
			}
		}
	}

	// Remember the locations of the files we have cut or copied
	// and use them when we render; only set these strings here
	if ( bSaveNames )
	{
		lstrcpy( Names.ClipImageFile, (LPSTR)lpFileName );
		lstrcpy( Names.ClipMaskFile, (LPSTR)lpMaskName );
	}

	if ( lpRect )
		*lpRect = rClip; // Pass back the size of the cut object
	ProgressEnd();

	return( YES );
}

BOOL CImage::WriteClipOut(LPSTR lpFileName, int DataType ,ITEMID Type, BOOL bUpdateThumb)
{
    BOOL bResult = FALSE;
    LPOBJECT lpObject;
	LPFRAME lpObjFrame, lpThmFrame;
	BOOL fAddedCommand = FALSE;

    ProgressBegin(1, 0);
    if (lpObject = ImgCreateClipOutObject(this))
    {
		CWriteImage write(lpFileName, Type);

		PPFFOPTIONS Options;
		Options.bPPFFPromptUser = FALSE;
		Options.PPFFCompress = FALSE;   
		Options.bSaveMask = TRUE;
		Options.bSaveProxy = TRUE;
		Options.bSaveCmdList = TRUE;
		Options.bSaveLink = TRUE;
		Options.bWritePT = TRUE;
		write.SetOptions((LPVOID)&Options);

		// only write command list if the load command has a
		// variable resolution, i.e. - low res load or
		// a photo cd load
		LPCMDPKT lpCmdPkt = (LPCMDPKT)ListGetHead(&lpCmdList->UndoPacketList);
		if (lpCmdPkt && IsVariableResLoadCmd(lpCmdPkt))
		{
			CMD_PARMS parms;

			LPCMSINFO lpCmsInfo = GetLoadCMSInfo(lpCmdPkt);

			lpCmdPkt = CreatePacket(IDS_CMD_CREATECLIPOBJECT, &parms, YES);
			if (!lpCmdPkt)
			{
				Message(IDS_EMEMALLOC);
				ObjFreeUp(lpObject);
				return(bResult);
			}
			ListAddTail(&lpCmdList->UndoPacketList, lpCmdPkt);

			fAddedCommand = TRUE;
			write.SetPacketList(&lpCmdList->UndoPacketList);
		}

		bResult = write.WriteObject(lpObject, DataType);
		
		// draw thumbnail
		if (bResult && bUpdateThumb)
		{
			COLORINFO Color;
			RECT rSize;
			
			lpObjFrame = ObjGetEditFrame(lpObject);
			// get frame to write
			Color.gray = 0xC0;
			SetColorInfo(&Color, &Color, CS_GRAY);
			rSize.left = rSize.top = 0;	rSize.right = 63; rSize.bottom = 63;
			lpThmFrame = ImgCopyMaskedFrame(NULL,lpObject,&rSize,NO,NO,YES,&Color);
			if (!lpThmFrame)
				lpThmFrame = lpObjFrame;
			UpdateThumbnail(lpFileName, 
						lpThmFrame, 
						FrameXSize(lpObjFrame), 
						FrameYSize(lpObjFrame), 
						DataType, 
						Type);
			if (lpThmFrame != lpObjFrame)
				FrameClose(lpThmFrame);
		}

		if (fAddedCommand)
		{
			lpCmdPkt = (LPCMDPKT)ListGetTail(&lpCmdList->UndoPacketList);
			ListUnlink(&lpCmdList->UndoPacketList, lpCmdPkt);
		}
		ObjFreeUp(lpObject);
    }    
    ProgressEnd();
    return( bResult );
}

LPOBJECT CImage::CreateClipOutObject(BOOL CopyObj)
{
LPFRAME lpDataFrame, lpAlphaFrame;
LPMASK lpMask;
LPALPHA lpAlpha;
LPOBJECT lpObject;
LPTR lpAlphaBuf;
RECT    rClip;
int iWidth, iHeight, y;
FRMTYPEINFO TypeInfo;
FRMDATATYPE FrameType = FDT_GRAYSCALE;

GetTypeInfo(&TypeInfo);
if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
   FrameType = FDT_LINEART;

lpAlphaFrame = NULL;
if (GetMaskRect(&rClip) && !CopyObj)
    {
    if (!(lpDataFrame = ImgCopyFrame(this, NULL, &rClip, YES, NO)))
        {
        Message(IDS_EMEMALLOC);
        return(NULL);
        }
    lpMask = GetMask();
    if (!(lpAlphaFrame = FrameCopy(lpMask->Pixmap.EditFrame, &rClip)))
        {
        FrameClose(lpDataFrame);
        Message(IDS_EMEMALLOC);
        return(NULL);
        }
    }
else 
    {
    GetSelObjectRect(&rClip, NO);
    if (!(lpDataFrame = ImgCopyFrame(this, NULL, &rClip, YES, YES)))
        {
        Message(IDS_EMEMALLOC);
        return(NULL);
        }
    if ( CountSelObjects( NULL ) != 1 ||
         GetSelObject(NULL) != GetBase( ) )
        {
        iWidth = RectWidth(&rClip);
        iHeight = RectHeight(&rClip);
        if (!(lpAlphaFrame = FrameOpen(FrameType, iWidth, iHeight,
                            FrameResolution(lpDataFrame))))
            {
            FrameClose(lpDataFrame);
            Message(IDS_EMEMALLOC);
            return(NULL);
            }
        if (!(lpAlphaBuf = Alloc((long)iWidth)))
            {
            FrameClose(lpDataFrame);
            FrameClose(lpAlphaFrame);
            Message(IDS_EMEMALLOC);
            return(NULL);
            }
        lpObject = GetSelObject(NULL);
        for (y = 0; y < iHeight; ++y)
            {
            ImgMaskLoad(this, lpObject, NULL, rClip.left, y+rClip.top,
                    iWidth, lpAlphaBuf, YES, CR_OR, 0, 0);
            FrameWrite(lpAlphaFrame, 0, y, iWidth, lpAlphaBuf, iWidth);
            }
        FreeUp(lpAlphaBuf);
        }
    }
lpAlpha = NULL;
if (lpAlphaFrame)
    {
    if (!(lpAlpha = MaskCreate(lpAlphaFrame, 0, 0, NO, YES)))
        {
        FrameClose(lpDataFrame);
        FrameClose(lpAlphaFrame);
        Message(IDS_EMEMALLOC);
        return(NULL);
        }
    }
OffsetRect(&rClip, -rClip.left, -rClip.top);
if (!(lpObject = ObjCreateFromFrame(ST_PERMANENT, lpDataFrame, lpAlpha,
                                    &rClip, YES)))
    {
    FrameClose(lpDataFrame);
    if (lpAlpha)
        MaskClose(lpAlpha);
    Message(IDS_EMEMALLOC);
    return(NULL);
    }
return(lpObject);
}

LPOBJECT CImage::CreateCutoutObject(LPRECT lpRect)
 {
int XSize, YSize;
LPOBJECT lpObject;
LPFRAME lpDataFrame, lpMaskFrame;
LPMASK lpMask, lpAlpha;
FRMTYPEINFO TypeInfo;
FRMDATATYPE FrameType = FDT_GRAYSCALE;

GetTypeInfo(&TypeInfo);
if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
FrameType = FDT_LINEART;

GetInfo(&XSize, &YSize);

if ( !(lpDataFrame = ImgCopyFrame(this, NULL, lpRect, YES, NO)) )
	{
	Message(IDS_EMEMALLOC);
	return( NULL );
	}

// Create the mask frame
if ( !(lpMask = GetMask()) )
	lpMaskFrame = NULL;
else
	{
	if ( lpMaskFrame = PixmapFrame( &lpMask->Pixmap, PMT_EDIT ) )
    	lpMaskFrame = FrameCopy( lpMaskFrame, lpRect );
	}

// Create a new mask from the frame
if ( !(lpAlpha = MaskCreate( lpMaskFrame, RectWidth(lpRect),
    	RectHeight(lpRect), ON, Control.NoUndo, FrameType)) )
	{
	Message(IDS_EMEMALLOC);
	FrameClose(lpDataFrame);
	FrameClose(lpMaskFrame);
	return( NULL );
	}

if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY, lpDataFrame, lpAlpha,
	lpRect, Control.NoUndo )) )
	{
	Message(IDS_EMEMALLOC);
	MaskClose( lpAlpha );
	FrameClose(lpDataFrame);
	return( NULL );
	}
AddNewObject(lpObject);
return( lpObject );
}

LPOBJECT CImage::CreateMaskObject(LPRECT lpRect)
{
int XSize, YSize;
LPOBJECT lpBase, lpObject;
LPFRAME lpMaskFrame;
LPMASK lpMask, lpAlpha;
FRMTYPEINFO TypeInfo;
FRMDATATYPE FrameType = FDT_GRAYSCALE;

GetTypeInfo(&TypeInfo);
if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
   FrameType = FDT_LINEART;

lpBase = GetBase();
PixmapGetInfo(&lpBase->Pixmap, PMT_EDIT, &XSize, &YSize, NULL, NULL);

// Create the mask frame
if ( !(lpMask = GetMask( )) )
    lpMaskFrame = NULL;
else
    {
    if ( lpMaskFrame = PixmapFrame( &lpMask->Pixmap, PMT_EDIT ) )
        lpMaskFrame = FrameCopy( lpMaskFrame, lpRect );
    }

// Create a new mask from the frame
if ( !(lpAlpha = MaskCreate( lpMaskFrame, RectWidth(lpRect),
        RectHeight(lpRect), ON, Control.NoUndo, FrameType)) )
    {
    Message(IDS_EMEMALLOC);
    FrameClose(lpMaskFrame);
    return( NULL );
    }
lpAlpha->bInvert = lpMask->bInvert;
if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY, NULL, lpAlpha,
    lpRect, Control.NoUndo )) )
    {
    Message(IDS_EMEMALLOC);
    MaskClose( lpAlpha );
    return( NULL );
    }

AddNewObject(lpObject);
return( lpObject );
}

LPOBJECT ImgCreateClipInObject(LPIMAGE lpImage, LPSTR lpImageFile,
 								LPSTR lpMaskFile, BOOL bDoCMS,
								LPFRMTYPEINFO lpSrcToReturn,
								LPFRMTYPEINFO lpSrcToUse)
{
	int xData, yData;
	LPOBJECT lpObject;
	LPALPHA lpAlpha;
	LPFRAME lpFrame;
	RECT rObject;
	CMSINFO		CmsInfo;
	FRMTYPEINFO TypeInfo;
	FRMTYPEINFO MaskType;
	FRMDATATYPE FrameType = FDT_GRAYSCALE;

	// 2 reads if we have a mask file (an old PP4 clipboard)
	if (lpMaskFile && *lpMaskFile)
		ProgressBegin(2, 0);
	else
		ProgressBegin(1, 0);

	if (lpImage)
		lpImage->GetTypeInfo(&TypeInfo);
	else
		FrameSetTypeInfo(&TypeInfo, FDT_NONE);

	cmsInfoInit( &CmsInfo );
	if( bDoCMS && IsDstPTSelected( &TypeInfo.ptInfo ) )
	{
		if (lpSrcToUse && IsSrcPTSelected(&lpSrcToUse->ptInfo))
			CmsInfo.src = *lpSrcToUse;
		else
		{
			CmsGetSrcPT( lpImageFile, &CmsInfo );
			CmsInfo.dst = TypeInfo;
		}
	}

	if (lpImage &&
		lpImage->GetRootCompoundStorage() &&
		IsStorageClipbit(lpImageFile) )
	{
		LPSTORAGE lpStorage;
		COleStreamFile *pFile = OpenClipStream(lpImage->GetRootCompoundStorage(), lpImageFile, &lpStorage);
		if (!pFile)
		{
    		ProgressEnd();
    		Message(IDS_ECLIPOPEN, lpImageFile);
    		return( NULL );
		}
		// read in the object matching the type of our image
		CReadImage read(pFile, IDN_PPCOMPOUND, FALSE, &CmsInfo);
		if (lpImage && lpImage->DocumentType == IMG_LOWRES)
			read.SetSampling(FrameResolution(lpImage->GetBaseEditFrame()));
		if (lpImage->lpCmdList->fPlayback)
			read.SetPhotoCDResOverride(lpImage->lpCmdList->PhotoCDResOverride);

		lpObject = read.ReadObject(TypeInfo);
		CloseClipStream(lpStorage, pFile);
		if (!lpObject)
     	{
    		ProgressEnd();
    		Message(IDS_ECLIPOPEN, lpImageFile);
    		return( NULL );
     	}
		if (lpSrcToReturn)
			read.GetCMSSrcTypeInfo(lpSrcToReturn);
	}
	else
	{
		// read in the object matching the type of our image
		CReadImage read(lpImageFile, NULL, &CmsInfo, NULL);
		if (lpImage)
	 	{
			if (lpImage && lpImage->DocumentType == IMG_LOWRES)
				read.SetSampling(FrameResolution(lpImage->GetBaseEditFrame()));
			if (lpImage->lpCmdList->fPlayback)
				read.SetPhotoCDResOverride(lpImage->lpCmdList->PhotoCDResOverride);
	 	}

		if ( !(lpObject = read.ReadObject(TypeInfo)))   
     	{
    		ProgressEnd();
    		Message(IDS_ECLIPOPEN, lpImageFile);
    		return( NULL );
     	}
		if (lpSrcToReturn)
			read.GetCMSSrcTypeInfo(lpSrcToReturn);
	}

	// get size info of object
	lpFrame = ObjGetEditFrame(lpObject);
	xData = FrameXSize(lpFrame);
	yData = FrameYSize(lpFrame);
	// if the object has no alpha we need to read on or create one
	if (!lpObject->lpAlpha)
	{
		LPFRAME lpMaskFrame = NULL;

		if (lpImage)
		{
   			lpImage->GetTypeInfo(&MaskType);
			if (MaskType.DataType == FDT_LINEART && Mask.OneBitMask)
	    		FrameType = FDT_LINEART;
		}

		// if a mask file was passed in (an old PP4 clipboard)
		if (lpMaskFile && *lpMaskFile)
		{
			CReadImage read(lpMaskFile);

			FrameSetTypeInfo(&MaskType, FrameType);
			lpMaskFrame = read.ReadFrame(MaskType);
			if (lpMaskFrame)
			{
				if (xData != FrameXSize(lpMaskFrame) ||
					yData != FrameYSize(lpMaskFrame))
				{
					FrameClose(lpMaskFrame);
					lpMaskFrame = NULL;
				}
			}
		}
		if ( !(lpAlpha = MaskCreate( lpMaskFrame, xData, yData, ON, YES/*fNoUndo*/ ,FrameType)) )
		{
			ProgressEnd();
			Message(IDS_EMEMALLOC);
			ObjFreeUp(lpObject);
			return( NULL );
		}
		lpObject->lpAlpha = lpAlpha;
	}
	rObject.left = rObject.top = 0;
	rObject.right = rObject.left + xData - 1;
	rObject.bottom = rObject.top + yData - 1;
	lpObject->rObject = rObject;

	ProgressEnd();
	return( lpObject );
}

BOOL IsStorageClipbit(LPTSTR lpFileName)
{
	if (*lpFileName == '.')
		return(FALSE);
	return(!IsAbsolutePath(lpFileName));
}

BOOL IsStorageMaskbit(LPTSTR lpFileName)
{
	if (*lpFileName == '.')
		return(FALSE);
	return(!IsAbsolutePath(lpFileName));
}

void GetUniqueStreamName(
	LPSTORAGE lpStorage,
	LPCTSTR lpTemplate,
	LPCTSTR lpExtension,
	LPTSTR lpStreamName)
{
	COleStreamFile* pFile = NULL;
	long lTicks = GetTickCount();

	while (TRUE)
	{
		CFileException fe;
		COleStreamFile theFile;
		TCHAR szNum[10];
	    long lNum = lTicks % 9999L;
	    wsprintf( szNum, "%04ld", lNum );

		lstrcpy(lpStreamName, lpTemplate);
		lstrcat(lpStreamName, szNum);
		lstrcat(lpStreamName, lpExtension);

		if (!theFile.OpenStream(
			lpStorage, 
			lpStreamName, 
			CFile::modeRead,
			&fe))
		{
			if (fe.m_cause == CFileException::fileNotFound ||
				fe.m_cause == CFileException::generic ||
				fe.m_cause == CFileException::badPath)
				break;
		}
		theFile.Close();
		++lTicks;
	}
}

BOOL CopyToStream(LPTSTR lpSrcFile, LPTSTR lpDstStream, LPSTORAGE lpStorage)
{
	CFileException fe;
	CFile	srcFile;
	COleStreamFile dstStream;
	LPTR lpBuf = NULL;
	long lBufSize, lBytesRead;
	BOOL fRet = FALSE;

	if (!srcFile.Open(lpSrcFile, CFile::modeRead, &fe))
	{
		Message(IDS_EOPEN, lpSrcFile);
		return(FALSE);
	}

	if (!dstStream.CreateStream(
		lpStorage,
		lpDstStream,
		CFile::modeReadWrite|CFile::shareExclusive|CFile::modeCreate,
		&fe))
	{
		Message(IDS_EOPEN, lpDstStream);
		srcFile.Close();
		return(FALSE);
	}

	lBufSize = 1024L * 1024L;
	while (lBufSize)
	{
		lpBuf = Alloc(lBufSize);
		if (lpBuf)
			break;
		lBufSize /= 2;
	}

	if (!lpBuf)
	{
		Message(IDS_EMEMALLOC);
		srcFile.Close();
		dstStream.Close();
		return(FALSE);
	}
	TRY
	{
		// copy the file
		while (TRUE)
		{
			lBytesRead = srcFile.Read(lpBuf, lBufSize);
			if (!lBytesRead)
				break;
			dstStream.Write(lpBuf, lBytesRead);
		}
		fRet = TRUE;
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	if (lpBuf)
		FreeUp(lpBuf);
	srcFile.Close();
	dstStream.Close();
	if (!fRet)
		lpStorage->DestroyElement(lpDstStream);
	return(fRet);
}

BOOL CopyFromStream(LPTSTR lpSrcStream, LPTSTR lpDstFile, LPSTORAGE pRootStorage)
{
	CFileException fe;
	CFile	dstFile;
	COleStreamFile dstStream;
	LPTR lpBuf = NULL;
	long lBufSize, lBytesRead;
	LPSTORAGE pStorage;
	BOOL fRet = FALSE;

	COleStreamFile *pSrcFile = OpenClipStream(pRootStorage, lpSrcStream, &pStorage);
	if (!pSrcFile)
	{
		Message(IDS_EOPEN, lpSrcStream);
 		return(FALSE);
	}

	if (!dstFile.Open(lpDstFile, CFile::modeWrite|CFile::modeCreate, &fe))
	{
		CloseClipStream(pStorage, pSrcFile);
		Message(IDS_EOPEN, lpDstFile);
		return(FALSE);
	}

	lBufSize = 1024L * 1024L;
	while (lBufSize)
	{
		lpBuf = Alloc(lBufSize);
		if (lpBuf)
			break;
		lBufSize /= 2;
	}

	if (!lpBuf)
	{
		Message(IDS_EMEMALLOC);
		CloseClipStream(pStorage, pSrcFile);
		dstFile.Close();
		return(FALSE);
	}
	TRY
	{
		// copy the file
		while (TRUE)
		{
			lBytesRead = pSrcFile->Read(lpBuf, lBufSize);
			if (!lBytesRead)
				break;
			dstFile.Write(lpBuf, lBytesRead);
		}
		fRet = TRUE;
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	if (lpBuf)
		FreeUp(lpBuf);
	CloseClipStream(pStorage, pSrcFile);
	dstFile.Close();
	if (!fRet)
		CFile::Remove(lpDstFile);
	return(fRet);
}

BOOL UpdateClipBits(LPSTORAGE pRootStorage, LPLIST pPacketList, BOOL bSavingCmdList, BOOL fUpdateNamedClipbits)
{
	SCODE sc;
	LPCMDPKT pCmdPkt;
	LPSTORAGE pStorage = NULL;
	LPENUMSTATSTG pEnumStatStg = NULL;
	LPMALLOC pMalloc = NULL;
	BOOL fRet = TRUE;
	const TCHAR szClipBits[] = _T("CLIPBITS");

	if (!bSavingCmdList)
	{
		sc = pRootStorage->DestroyElement(szClipBits);
		return(TRUE);
	}

	// first thing to do is to make sure we have a clipbits substorage
	sc = pRootStorage->OpenStorage(szClipBits, NULL,
		STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
		0, 0, &pStorage);
	// if the storage does not exist then create it
	if (sc == STG_E_FILENOTFOUND)
	{
		sc = pRootStorage->CreateStorage(szClipBits,
			STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
			0, 0, &pStorage);
	}
	if (sc != NOERROR)
	{
		Message(IDS_EOPEN, szClipBits);
		return(FALSE);
	}

	// get a directory listing of this storage
	pStorage->EnumElements(0, NULL, 0, &pEnumStatStg);
	CoGetMalloc(MEMCTX_TASK, &pMalloc);
	if (pEnumStatStg && pMalloc)
	{
		STATSTG StatStg;
		while (pEnumStatStg->Next(1, &StatStg, NULL) == S_OK)
		{
			if (StatStg.type == STGTY_STREAM)
			{
				BOOL bFound = FALSE;
				pCmdPkt = (LPCMDPKT)ListGetHead(pPacketList);
				while (pCmdPkt && !bFound)
				{
					if (pCmdPkt->idCommand == IDS_CMD_PASTEFROM)
					{
						LPPASTEFROM_PARMS pParms = (LPPASTEFROM_PARMS)pCmdPkt->lpParms;
						if (!lstrlen(pParms->szClipboard) &&
							IsStorageClipbit(pParms->szClipFileName))
						{
							bFound = StringsEqual(
											filename(pParms->szClipFileName),
											StatStg.pwcsName);
						}
					}
					pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
				}
				if (!bFound)
					pStorage->DestroyElement(StatStg.pwcsName);
			}
			pMalloc->Free(StatStg.pwcsName);
		}
	} 

	if (pMalloc)
	  	ReleaseInterface((LPUNKNOWN*)&pMalloc);
	if (pEnumStatStg)
	  	ReleaseInterface((LPUNKNOWN*)&pEnumStatStg);

	pCmdPkt = (LPCMDPKT)ListGetHead(pPacketList);
	while (pCmdPkt)
	{
		if (pCmdPkt->idCommand == IDS_CMD_PASTEFROM)
		{
			TCHAR szClipTemplate[] = _T("CLIP");
			TCHAR szClipExt[] = _T(".PPF");

			LPPASTEFROM_PARMS pParms = (LPPASTEFROM_PARMS)pCmdPkt->lpParms;
			// don't include extended name clipboards
			// Relative path is already in storage
			if ((!lstrlen(pParms->szClipboard) || fUpdateNamedClipbits) &&
				!IsStorageClipbit(pParms->szClipFileName))
			{
				FNAME szStreamName;
				GetUniqueStreamName(pStorage, szClipTemplate, szClipExt, szStreamName);
				if (CopyToStream(pParms->szClipFileName, szStreamName, pStorage))
				{
					if (pParms->Common.fParmsStored && pParms->fTemporary)
					{
						PictPubApp.AddClipboardFileReference(pParms->szClipFileName);
						pParms->Common.fParmsStored = FALSE;
					}
					pParms->fTemporary = FALSE;
					pParms->szClipboard[0] = '\0';
					// setup name in storage - relative path
					lstrcpy(pParms->szClipFileName, szClipBits);
					lstrcat(pParms->szClipFileName, _T("\\"));
					lstrcat(pParms->szClipFileName, szStreamName);
				}
				else
					fRet = FALSE;
			}
		}
		pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
	}
    pStorage->Commit(STGC_ONLYIFCURRENT);
   	ReleaseInterface((LPUNKNOWN*)&pStorage);

	return(fRet);
}

BOOL UpdateMaskBits(LPSTORAGE pRootStorage, LPLIST pPacketList, BOOL bSavingCmdList, BOOL fUpdateNamedMaskbits)
{
	SCODE sc;
	LPCMDPKT pCmdPkt;
	LPSTORAGE pStorage = NULL;
	LPENUMSTATSTG pEnumStatStg = NULL;
	LPMALLOC pMalloc = NULL;
	BOOL fRet = TRUE;
	const TCHAR szMaskBits[] = _T("MASKS");

	if (!bSavingCmdList)
	{
		sc = pRootStorage->DestroyElement(szMaskBits);
		return(TRUE);
	}

	// first thing to do is to make sure we have a maskbits substorage
	sc = pRootStorage->OpenStorage(szMaskBits, NULL,
		STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
		0, 0, &pStorage);
	// if the storage does not exist then create it
	if (sc == STG_E_FILENOTFOUND)
	{
		sc = pRootStorage->CreateStorage(szMaskBits,
			STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
			0, 0, &pStorage);
	}
	if (sc != NOERROR)
	{
		Message(IDS_EOPEN, szMaskBits);
		return(FALSE);
	}

	// get a directory listing of this storage
	pStorage->EnumElements(0, NULL, 0, &pEnumStatStg);
	CoGetMalloc(MEMCTX_TASK, &pMalloc);
	if (pEnumStatStg && pMalloc)
	{
		STATSTG StatStg;
		while (pEnumStatStg->Next(1, &StatStg, NULL) == S_OK)
		{
			if (StatStg.type == STGTY_STREAM)
			{
				BOOL bFound = FALSE;
				pCmdPkt = (LPCMDPKT)ListGetHead(pPacketList);
				while (pCmdPkt && !bFound)
				{
					if (pCmdPkt->idCommand == IDS_CMD_LOADMASK)
					{
						LPLOADMASK_PARMS pParms = (LPLOADMASK_PARMS)pCmdPkt->lpParms;
						if (!lstrlen(pParms->szMask) &&
							IsStorageMaskbit(pParms->szMaskFileName))
						{
							bFound = StringsEqual(
											filename(pParms->szMaskFileName),
											StatStg.pwcsName);
						}
					}
					pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
				}
				if (!bFound)
					pStorage->DestroyElement(StatStg.pwcsName);
			}
			pMalloc->Free(StatStg.pwcsName);
		}
	} 

	if (pMalloc)
	  	ReleaseInterface((LPUNKNOWN*)&pMalloc);
	if (pEnumStatStg)
	  	ReleaseInterface((LPUNKNOWN*)&pEnumStatStg);

	pCmdPkt = (LPCMDPKT)ListGetHead(pPacketList);
	while (pCmdPkt)
	{
		if (pCmdPkt->idCommand == IDS_CMD_LOADMASK)
		{
			TCHAR szMaskTemplate[] = _T("MASK");
			TCHAR szMaskExt[] = _T(".TIF");

			LPLOADMASK_PARMS pParms = (LPLOADMASK_PARMS)pCmdPkt->lpParms;
			// don't include extended name masks
			// Relative path is already in storage
			if ((!lstrlen(pParms->szMask) || fUpdateNamedMaskbits) &&
				!IsStorageMaskbit(pParms->szMaskFileName))
			{
				FNAME szStreamName;
				GetUniqueStreamName(pStorage, szMaskTemplate, szMaskExt, szStreamName);
				if (CopyToStream(pParms->szMaskFileName, szStreamName, pStorage))
				{
					pParms->szMask[0] = '\0';
					// setup name in storage - relative path
					lstrcpy(pParms->szMaskFileName, szMaskBits);
					lstrcat(pParms->szMaskFileName, _T("\\"));
					lstrcat(pParms->szMaskFileName, szStreamName);
				}
				else
					fRet = FALSE;
			}
		}
		pCmdPkt = (LPCMDPKT)ListGetNext(pCmdPkt);
	}
    pStorage->Commit(STGC_ONLYIFCURRENT);
   	ReleaseInterface((LPUNKNOWN*)&pStorage);

	return(fRet);
}

COleStreamFile *OpenClipStream(LPSTORAGE lpRootStorage, LPTSTR lpStreamName, LPSTORAGE *lppClipStorage)
{
	COleStreamFile *pFile = NULL;
	CFileException fe;
	SCODE sc;
	LPTSTR lp;
	FNAME szStorageName, szStreamName;

	lstrcpy(szStreamName, filename(lpStreamName));
	lstrcpy(szStorageName, lpStreamName);
	lp = filename(szStorageName);
	lp = MyCharPrev(szStorageName, lp);
	*lp = '\0';

	// first thing to do is to make sure we have a clipbits substorage
	sc = lpRootStorage->OpenStorage(szStorageName, NULL,
		STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
		0, 0, lppClipStorage);
	if (sc != NOERROR)
		return(NULL);

	TRY
	{
		pFile = new COleStreamFile();
		if (!pFile->OpenStream(
			*lppClipStorage,
			szStreamName,
			CFile::modeReadWrite|CFile::shareExclusive,
			&fe))
		{
			AfxThrowFileException(fe.m_cause, fe.m_lOsError);
		}
		return(pFile);
	}
	CATCH_ALL(e)
	{
		if (pFile)
		{
			pFile->Abort(); // will not throw an exception
			delete pFile;
		}
		ReleaseInterface((LPUNKNOWN*)lppClipStorage);
		return(NULL);
	}
	END_CATCH_ALL
}

void CloseClipStream(LPSTORAGE lpStorage, COleStreamFile *pFile)
{
	pFile->Close();
	delete pFile;
	ReleaseInterface((LPUNKNOWN*)&lpStorage);
}


