//®FD1¯®PL1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "clipbrd.h"
#include "dataitem.h"
#include "svritem.h"
#include "objutil.h"

// Static prototypes
static BOOL PasteBitmap(HBITMAP hBmp, HPALETTE hPal,LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL PasteMetafile( HGLOBAL hMemory, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL PasteMgxImage(HANDLE hData, LPSTR lpImageFile);
static BOOL PasteDIB(HANDLE hData, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL PastePP4MgxImage( HANDLE hData, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL WriteDIB( LPBITMAPINFOHEADER lpInfo, LPSTR lpImageFile );
static BOOL WriteDIBClip( LPBITMAPINFOHEADER lpInfo, LPSTR lpImageFile, LPSTR lpMaskFile );
static BOOL WriteDIBMask( LPBITMAPINFOHEADER lpInfo, LPSTR lpMaskFile );

/************************************************************************/
HANDLE ConstructMGXImage( LPSTR lpImageFile)
/************************************************************************/
{
LPMGX_IMAGEHDR    lpMgxHdr;
LPMGX_IMAGENAME   lpMgxName;
long              lDataSize, lCount;
LPSTR             lpClipMem;
HANDLE            hMemory;

lDataSize = sizeof(MGX_IMAGENAME) + lstrlen(lpImageFile) + 1;
lCount = sizeof(MGX_IMAGEHDR) + lDataSize;
// allocate the memory to hold the MGX_IMAGE
if ( !(lpClipMem = ( LPSTR )Alloc( lCount )) )
    {
    Message( IDS_EMEMALLOC );
    return( NULL );
    }

#ifdef WIN32
hMemory = GlobalHandle( lpClipMem );
#else
{
DWORD dwValue;
dwValue = GlobalHandle( HIWORD(lpClipMem) );
hMemory = (HANDLE)LOWORD( dwValue );
}
#endif
lpMgxHdr = (LPMGX_IMAGEHDR)lpClipMem;
lpMgxHdr->wHeaderSize = sizeof(MGX_IMAGEHDR);
lpMgxHdr->wVersion = MGX_IMAGE_VERSION;
lpMgxHdr->wImageType = MGX_IMAGETYPE;
lpMgxHdr->lDataSize = lDataSize;
lpClipMem += lpMgxHdr->wHeaderSize;
lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
lpMgxName->wNameHeaderSize = sizeof(MGX_IMAGENAME);
lpMgxName->wDataType = MGX_IMAGETIFF;
lpMgxName->wNameLength = lstrlen(lpImageFile) + 1;
lpClipMem += sizeof(MGX_IMAGENAME);
lstrcpy(lpClipMem, lpImageFile);

return( hMemory );
}

/************************************************************************/
HANDLE ConstructPP4MGXImage( LPTR lpImageFile, LPTR lpMaskFile )
/************************************************************************/
{
	LPMGX_IMAGEHDR lpMgxHdr;
	LPMGX_IMAGENAME lpMgxName;
	long lDataSize, lCount;
	LPTR lpClipMem;
	HANDLE hMemory;

	lDataSize = sizeof(MGX_IMAGENAME) + lstrlen((LPSTR)lpImageFile) + 1 + 
		sizeof(MGX_IMAGENAME) + lstrlen((LPSTR)lpMaskFile) + 1;
	lCount = sizeof(MGX_IMAGEHDR) + lDataSize;
	// allocate the memory to hold the MGX_IMAGE
	if ( !(lpClipMem = Alloc( lCount )) )
	{
		Message( IDS_EMEMALLOC );
		return( NULL );
	}
#ifdef WIN32
	hMemory = GlobalHandle( lpClipMem );
#else
{
	DWORD dwValue;
	dwValue = GlobalHandle( HIWORD(lpClipMem) );
	hMemory = (HANDLE) LOWORD( dwValue );
}
#endif
	lpMgxHdr = (LPMGX_IMAGEHDR)lpClipMem;
	lpMgxHdr->wHeaderSize = sizeof(MGX_IMAGEHDR);
	lpMgxHdr->wVersion = MGX_IMAGE_VERSION;
	lpMgxHdr->wImageType = MGX_IMAGEMASKTYPE;
	lpMgxHdr->lDataSize = lDataSize;
	lpClipMem += lpMgxHdr->wHeaderSize;
	lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
	lpMgxName->wNameHeaderSize = sizeof(MGX_IMAGENAME);
	lpMgxName->wDataType = MGX_IMAGETIFF;
	lpMgxName->wNameLength = lstrlen((LPSTR)lpImageFile) + 1;
	lpClipMem += sizeof(MGX_IMAGENAME);
	lstrcpy((LPSTR)lpClipMem, (LPSTR)lpImageFile);
	lpClipMem += lpMgxName->wNameLength;
	lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
	lpMgxName->wNameHeaderSize = sizeof(MGX_IMAGENAME);
	lpMgxName->wDataType = MGX_MASKPP;
	lpMgxName->wNameLength = lstrlen((LPSTR)lpMaskFile) + 1;
	lpClipMem += sizeof(MGX_IMAGENAME);
	lstrcpy((LPSTR)lpClipMem, (LPSTR)lpMaskFile);

	return( hMemory );
}

/************************************************************************/
HANDLE FrameToBitmap( LPFRAME lpFrame, LPRECT lpRect )
/************************************************************************/
{
FRMTYPEINFO TypeInfo;
int y, dx, dy;
HDC hDC;
HBITMAP hBitmap;
RECT rRect, DestRect;
LPTR lp;
BLTSESSION BltSession;

if ( !lpFrame )
    return( NULL );

if ( lpRect )
    rRect = *lpRect;
else
    SetRect(&rRect, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
dx = RectWidth( &rRect );
dy = RectHeight( &rRect );

if ( !(hDC = GetDC(PictPubApp.Get_hWndAstral())) )
    return( NULL );

if ( !(hBitmap = CreateCompatibleBitmap(hDC, dx, dy)) )
    {
    Message( IDS_EMEMALLOC );
    ReleaseDC( PictPubApp.Get_hWndAstral(), hDC );
    return( NULL );
    }

DestRect.left = DestRect.top = 0;
DestRect.right = DestRect.left + dx - 1;
DestRect.bottom = DestRect.top + dy - 1;

FrameGetTypeInfo(lpFrame, &TypeInfo);

StartSuperBlt( &BltSession, hDC, hBitmap, lpBltScreen, &DestRect,
        TypeInfo, 0, 0, 0, YES, NULL, NULL );
for (y = rRect.top; y <= rRect.bottom; ++y)
    {
    if ( !(lp = FramePointer(lpFrame, rRect.left, y, NO)) )
        break;
    SuperBlt(&BltSession, lp);
    }
SuperBlt( &BltSession, NULL );
ReleaseDC( PictPubApp.Get_hWndAstral(), hDC );
return( hBitmap );
}


/************************************************************************/
HANDLE ConstructPalette( HPALETTE hPal )
/************************************************************************/
{
if ( !hPal )
    return( NULL );
return( CopyPalette( hPal ) );
}

/************************************************************************/
LPOBJECT GetClipboardObject(
	LPIMAGE lpDstImage, // can be NULL
	LPSTR	lpFileName, // return clipboard file to be used for regeneration
	LPDWORD lpImageID, // unique id to indentify image source
	LPFRMTYPEINFO lpSrcTypeInfo,
    int cfFormat )     // specific format ; Used Only for External Clipboard Pastes if present
/************************************************************************/
{
	LPOBJECT lpObject;

	if (lpImageID)
		*lpImageID = 0;
	if (lpFileName)
		*lpFileName = '\0';
	if (lpSrcTypeInfo)
		FrameSetTypeInfo(lpSrcTypeInfo, FDT_NONE);

    // First put clipboard contents into a file(s)
	// see if we are the clipboard owner, if so we just need to
	// copy the data we need
	CDataSource *pDataSource = (CDataSource*)COleDataSource::GetClipboardOwner();
	if (pDataSource && cfFormat == -1)
	{
		LPIMAGE lpSrcImage;
		DWORD	dwImageID = 0;

		lpSrcImage = (LPIMAGE)pDataSource->GetImage();
		ASSERT(lpSrcImage);

		if (lpDstImage)
			dwImageID = lpDstImage->lpCmdList->dwImageID;

		if (lpImageID)
			*lpImageID = lpSrcImage->lpCmdList->dwImageID;

		// create a snapshot of the clipboard contents
		// if we are gettting this clipboard for the same
		// image that is it's source
		if (dwImageID != lpSrcImage->lpCmdList->dwImageID)
		{
			return(GetObjectFromInternalData(lpDstImage,
											 lpSrcImage,
											 TRUE,
											 lpFileName,
											 lpSrcTypeInfo));
		}
		else
		{
			FRMTYPEINFO DstTypeInfo;
			LPOBJECT	lpSrcObject;

			lpSrcObject = ImgGetBase(lpSrcImage);
			ASSERT(lpSrcObject);

			if (lpDstImage)
				ImgGetTypeInfo(lpDstImage, &DstTypeInfo);
			else
				FrameSetTypeInfo(&DstTypeInfo, FDT_NONE);

			lpObject = ObjDuplicateObject(lpSrcObject,
										  DstTypeInfo,
										  Control.CMSEnabled,
										  lpSrcTypeInfo);
			if (!lpObject)
				Message(IDS_EMEMALLOC);
			return(lpObject);
		}
	}
	else
	{
        BOOL fNeedSnapShot;
        
        // Other formats need to create the file, therefore we stuff the path
        // strings first
        fNeedSnapShot =	(lpFileName != NULL) &&
        				Control.DoCommandList &&
        				Control.WriteFileOnPaste &&
        				(PictPubApp.GetClipboardFileName(lpFileName) == NULL);
        FormFileName (Names.PasteImageFile, IDN_CLIPBOARD,"WINCLIP.BMP");
        Names.PasteMaskFile[0] = '\0';
        ProgressBegin(fNeedSnapShot ? 2 : 1, PROGRESS_ID(IDS_UNDOPASTECLIP));
        if (!PasteFromClipboard(AfxGetMainWnd()->GetSafeHwnd(),
        						Names.PasteImageFile,
        						Names.PasteMaskFile,
                                cfFormat))
        {
            ProgressEnd();
        	return(NULL);
        }
        
        lpObject = GetObjectFromExternalData(lpDstImage,
        									 Names.PasteImageFile,
        								 	 Names.PasteMaskFile,
        								 	 TRUE,
        								 	 lpFileName,
        							  		 lpSrcTypeInfo);
        ProgressEnd();
        return(lpObject);
	}
}

	
/************************************************************************/
LPOBJECT GetObjectFromInternalData( LPIMAGE lpDstImage,
									LPIMAGE lpSrcImage,
									BOOL fClipboard,
							 		LPSTR lpFileName,
									LPFRMTYPEINFO lpSrcTypeInfo)
/************************************************************************/
{
	FRMTYPEINFO SrcTypeInfo, DstTypeInfo;
	LIST PacketList, NewPacketList;
	LPCMDPKT lpCmdPkt;
	LPOBJECT lpSrcObject, lpObject;
	FNAME	szExtName;
	int iRes, PhotoCDResOverride;
	BOOL 	fHaveCmdList = FALSE;

	if (lpFileName)
		*lpFileName = '\0';
	if (lpSrcTypeInfo)
		FrameSetTypeInfo(lpSrcTypeInfo, FDT_NONE);

	lpSrcObject = ImgGetBase(lpSrcImage);
	ASSERT(lpSrcObject);
	PacketList = lpSrcImage->lpCmdList->UndoPacketList;

	iRes = PhotoCDResOverride = 0;
	// if destination image passed in, get info for rebuilds and conversion
	if (lpDstImage)
	{
		ImgGetTypeInfo(lpDstImage, &DstTypeInfo);
		if (lpDstImage->DocumentType == IMG_LOWRES)
			iRes = FrameResolution(ImgGetBaseEditFrame(lpDstImage));
		// don't use override when doing initial paste
		//PhotoCDResOverride = lpImage->lpCmdList->PhotoCDResOverride;
        if( ListGetCount( &lpDstImage->lpCmdList->UndoPacketList ))
			fHaveCmdList = TRUE;
	}
	else
		FrameSetTypeInfo(&DstTypeInfo, FDT_NONE);

	// see if we are in a mode where we would need a snapshot
	// see if we already have a snapshot of the current clipboard
	if (lpFileName &&
		fHaveCmdList &&
		Control.WriteFileOnPaste &&
		(!fClipboard ||	!PictPubApp.GetClipboardFileName(lpFileName)))
	{
		PPFFOPTIONS Options;
		DWORD dwClipID;

		if (fClipboard)
			dwClipID = ((CMDIFrame*)PictPubApp.m_pMainWnd)->GetClipboardID();
		else
			dwClipID = 0xFFFFFFFFL;

		// create a base name for the clipboard filename
   		wsprintf(szExtName, "clip%04lu", dwClipID);

		// create a filename to write to
		// always a PP5
		CreateFileName( szExtName, IDN_CLIPBOARD, lpFileName);
	
		// write the clipboard object out to a file
		// and write its packet list if it's low-res
		CWriteImage write(lpFileName);

		// if the load command handles variable resolutions,
		// then save the packet list (lowres and photocd)
		lpCmdPkt = (LPCMDPKT)ListGetHead(&PacketList);
		if (lpCmdPkt && IsVariableResLoadCmd(lpCmdPkt))
			write.SetPacketList(&PacketList);

		// setup PP5 options
		Options.bPPFFPromptUser = FALSE;
		Options.PPFFCompress = FALSE;   
		Options.bSaveMask = TRUE;
		Options.bSaveProxy = TRUE;
		Options.bSaveCmdList = TRUE;
		Options.bWritePT = TRUE;
		Options.bSaveLink = TRUE;
		write.SetOptions((LPVOID)&Options);
		if (write.WriteObject(lpSrcObject))
			PictPubApp.SetClipboardFileName(lpFileName, dwClipID);
	}

	lpCmdPkt = (LPCMDPKT)ListGetHead(&PacketList);
	if (lpCmdPkt && IsVariableResLoadCmd(lpCmdPkt) &&
		CheckForRebuild(lpCmdPkt, iRes,
						FrameResolution(ObjGetEditFrame(lpSrcObject)),
						PhotoCDResOverride))
   	{
		// make a copy of the packet list from the clipboard
		if (!CopyPacketList(&PacketList, &NewPacketList))
		{
			Message(IDS_EMEMALLOC);
			return(NULL);
		}

		// we need hi-res, crank on the command list
		if (!ConvertLowResLoad(&NewPacketList))
		{
			DestroyPacketList(&NewPacketList);
			Message(IDS_EMEMALLOC);
			return(NULL);
		}

		// now build the image from the packet list
		LPIMAGE lpClipImage = BuildImage(NULL, &NewPacketList, PhotoCDResOverride);
		DestroyPacketList(&NewPacketList);
		if (!lpClipImage)
		{
			Message(IDS_EMEMALLOC);
			return(NULL);
		}
		ImgGetTypeInfo(lpClipImage, &SrcTypeInfo);
		if (DstTypeInfo.DataType == FDT_NONE)
			DstTypeInfo = SrcTypeInfo;
		lpObject = ImgGetBase(lpClipImage);
		lpClipImage->ObjList.lpHead = lpClipImage->ObjList.lpTail = NULL;
		DestroyImage(lpClipImage);
		if (!FrameTypeInfoEqual(SrcTypeInfo, DstTypeInfo))
		{
			LPOBJECT lpNewObject = ObjDuplicateObject(lpObject, DstTypeInfo, Control.CMSEnabled, lpSrcTypeInfo);
			ObjFreeUp(lpObject);
			lpObject = lpNewObject;
		}
	}
	else
	{
		lpObject = ObjDuplicateObject(lpSrcObject, DstTypeInfo, Control.CMSEnabled, lpSrcTypeInfo);
	}

	if (!lpObject)
		Message(IDS_EMEMALLOC);
		
	return(lpObject);
}

/************************************************************************/
LPOBJECT GetObjectFromExternalData( LPIMAGE lpDstImage,
									LPSTR lpImageFile, LPSTR lpMaskFile,
									BOOL fClipboard,
							 		LPSTR lpFileName,
							 		LPFRMTYPEINFO lpSrcTypeInfo)
/************************************************************************/
{
	LPOBJECT 	lpObject, lpNewObject;
	DWORD 		dwClipID;
	FNAME 		szExtName;
	FRMTYPEINFO DstTypeInfo, SrcTypeInfo;
	BOOL 		fHaveCmdList = FALSE;

	if (lpFileName)
		*lpFileName = '\0';
	if (lpSrcTypeInfo)
		FrameSetTypeInfo(lpSrcTypeInfo, FDT_NONE);
	if (lpDstImage)
	{
		ImgGetTypeInfo(lpDstImage, &DstTypeInfo);
        if( ListGetCount( &lpDstImage->lpCmdList->UndoPacketList ))
			fHaveCmdList = TRUE;
	}
	else
		FrameSetTypeInfo(&DstTypeInfo, FDT_NONE);

	// create a snapshot of the clipboard contents
	// if we don't already have one
	if (lpFileName != NULL &&
		fHaveCmdList &&
		Control.WriteFileOnPaste &&
		(!fClipboard || !PictPubApp.GetClipboardFileName(lpFileName)))
	{
		lpObject = ImgCreateClipInObject(NULL, lpImageFile, lpMaskFile);
		if (!lpObject)
			return(NULL);

		if (fClipboard)
			dwClipID = ((CMDIFrame*)PictPubApp.m_pMainWnd)->GetClipboardID();
		else
			dwClipID = 0xFFFFFFFFL;

		// create a base name for the clipboard filename
		wsprintf(szExtName, "clip%04lu", dwClipID);

		// create a filename to write to
		CreateFileName( szExtName, IDN_CLIPBOARD, lpFileName);
	
		// write the clipboard object out to a file
		// and write its packet list if it's low-res
		CWriteImage write(lpFileName);
		if (!write.WriteObject(lpObject))
		{
			ObjFreeUp(lpObject);
			return(NULL);
		}
		PictPubApp.SetClipboardFileName(lpFileName, dwClipID);
	}
	else
	{
		lpObject = ImgCreateClipInObject(lpDstImage, lpImageFile, lpMaskFile);
		if (!lpObject)
			return(NULL);
	}

	FrameGetTypeInfo(ObjGetEditFrame(lpObject), &SrcTypeInfo);
	if (DstTypeInfo.DataType == FDT_NONE)
		DstTypeInfo = SrcTypeInfo;

	if (!FrameTypeInfoEqual(DstTypeInfo, SrcTypeInfo))
	{
		lpNewObject = ObjDuplicateObject(lpObject, DstTypeInfo, Control.CMSEnabled, lpSrcTypeInfo);
		ObjFreeUp(lpObject);
		lpObject = lpNewObject;
		if (!lpObject)
		{
			Message(IDS_EMEMALLOC);
			return(NULL);
		}
	}
	return(lpObject);
}

// Paste from (read in) the windows clipboard
/************************************************************************/
BOOL PasteFromClipboard( HWND hWindow, LPSTR lpImageFile, LPSTR lpMaskFile,
    int cbFormat /* -1 */ )
/************************************************************************/
{
	BOOL bResult = TRUE;

    if (cbFormat == -1 )
    {
    	// The MGX format will stuff the path strings since the files already exist
    	if ( IsClipboardFormatAvailable( Control.cfImage ) )
    		cbFormat = 	Control.cfImage;
    	else if ( IsClipboardFormatAvailable( Control.cfImagePP4 ) )
    		cbFormat = 	Control.cfImagePP4;
    	else if ( IsClipboardFormatAvailable( CF_DIB ) )
    		cbFormat = CF_DIB;
    	else if ( IsClipboardFormatAvailable( CF_BITMAP ) )
    		cbFormat = CF_BITMAP;
    	else
    		bResult = FALSE;
   	}

	if (bResult)
	{
	    HANDLE hMemory = NULL;

		bResult = FALSE;	
	    if (OpenClipboard(hWindow) )
		{
		   	hMemory = GetClipboardData(cbFormat);
			if (hMemory)
			{
				if (cbFormat == Control.cfImage)
				    bResult = PasteMgxImage( hMemory, lpImageFile);    
				else if (cbFormat == Control.cfImagePP4)
					bResult = PastePP4MgxImage( hMemory, lpImageFile, lpMaskFile);
				else if (cbFormat == CF_METAFILEPICT)
                    bResult = PasteMetafile( hMemory, lpImageFile, lpMaskFile);
				else if (cbFormat == CF_DIB)
					bResult = PasteDIB( hMemory, lpImageFile, lpMaskFile);
				else if (cbFormat == CF_BITMAP)
				{
 					HPALETTE hPal;
					if (!(hPal = (HPALETTE)GetClipboardData(CF_PALETTE)) )
					    hPal = lpBltScreen->hPal;
					bResult =  PasteBitmap( (HBITMAP)hMemory, hPal, lpImageFile, lpMaskFile);
				}
				else
					ASSERT(FALSE);  // unsupported fileformat
				if (!bResult)
					GlobalFree(hMemory);
		    }
		    CloseClipboard();
		}
	}
	else
		Message(IDS_CLIPBOARDEMPTY);

	return (bResult);
}

// Paste from Drag/Drop Op
/************************************************************************/
BOOL PasteDropData(UINT cfFormat, HANDLE hData, LPSTR lpImageFile, LPSTR lpMaskFile )
/************************************************************************/
{
	BOOL bResult = FALSE;

	if (cfFormat == Control.cfImage)
	    bResult = PasteMgxImage( hData, lpImageFile);    
	else if (cfFormat == CF_DIB)
		bResult = PasteDIB( hData, lpImageFile, lpMaskFile);
	else if (cfFormat == CF_BITMAP)
	{
		HPALETTE hPal = lpBltScreen->hPal;
		bResult =  PasteBitmap( (HBITMAP)hData, hPal, lpImageFile, lpMaskFile);
	}
	return (bResult);
}

/************************************************************************/
static BOOL PasteMgxImage( HANDLE hData, LPSTR lpImageFile)
/************************************************************************/
{
	ASSERT(hData);
    LPMGX_IMAGEHDR  lpMgxHdr;
    LPMGX_IMAGENAME lpMgxName;
    LPSTR lpClipMem;
    
    lpClipMem = ( LPSTR )GlobalLock(hData);
    lpMgxHdr = (LPMGX_IMAGEHDR)lpClipMem;
    if (lpMgxHdr->wHeaderSize != sizeof(MGX_IMAGEHDR) ||
        lpMgxHdr->wVersion != MGX_IMAGE_VERSION ||
        lpMgxHdr->wImageType != MGX_IMAGETYPE)
    {
        Message(IDS_BADMGXIMAGE);
        GlobalUnlock(hData);
        return(FALSE);
    }
    lpClipMem += lpMgxHdr->wHeaderSize;
    lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
    lpClipMem += lpMgxName->wNameHeaderSize;
    lstrcpy(lpImageFile, lpClipMem);
    lpClipMem += lpMgxName->wNameLength;
    
    GlobalUnlock(hData);
    return(TRUE);
}

/************************************************************************/
static BOOL PastePP4MgxImage( HANDLE hData, LPSTR lpImageFile, LPSTR lpMaskFile )
/************************************************************************/
{
	ASSERT(hData);
	LPMGX_IMAGEHDR  lpMgxHdr;
	LPMGX_IMAGENAME lpMgxName;
	LPSTR       lpClipMem;
	
	lpClipMem = ( LPSTR )GlobalLock(hData);
	lpMgxHdr = (LPMGX_IMAGEHDR)lpClipMem;
	if (lpMgxHdr->wHeaderSize != sizeof(MGX_IMAGEHDR) ||
	    lpMgxHdr->wVersion != MGX_IMAGE_VERSION ||
	    lpMgxHdr->wImageType != MGX_IMAGEMASKTYPE)
	    {
	    Message(IDS_BADMGXIMAGE);
	    GlobalUnlock(hData);
	    return(FALSE);
	    }
	lpClipMem += lpMgxHdr->wHeaderSize;
	lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
	lpClipMem += lpMgxName->wNameHeaderSize;
	
	lstrcpy(lpImageFile, lpClipMem);
	lpClipMem += lpMgxName->wNameLength;
	lpMgxName = (LPMGX_IMAGENAME)lpClipMem;
	lpClipMem += lpMgxName->wNameHeaderSize;
	
	lstrcpy(lpMaskFile, lpClipMem);
	GlobalUnlock(hData);
	return(TRUE);
}

/************************************************************************/
static BOOL PasteDIB( HANDLE hData, LPSTR lpImageFile, LPSTR lpMaskFile )
/************************************************************************/
{
	ASSERT(hData);
	LPBITMAPINFOHEADER lpDIB;
	BOOL bRet;

	if ( !(lpDIB = (LPBITMAPINFOHEADER)GlobalLock(hData)) )
	    return( NO );
	bRet = WriteDIBClip( lpDIB, lpImageFile, lpMaskFile );
	GlobalUnlock( hData );
	return( bRet );
}


/************************************************************************/
static BOOL PasteBitmap(HBITMAP hBmp, HPALETTE hPal, LPSTR lpImageFile, LPSTR lpMaskFile )
/************************************************************************/
{
	ASSERT(hBmp);
	ASSERT(hPal);

	BOOL bRet;
	LPBITMAPINFOHEADER lpDIB;
	if ( !(lpDIB = BitmapToDIB( NULL/*hDC*/, hBmp, hPal )) )
		return( NO );
	bRet = WriteDIBClip( lpDIB, lpImageFile, lpMaskFile );
	FreeUp( (LPTR)lpDIB );
	return( bRet );
}

/************************************************************************/
static BOOL PasteMetafile( HGLOBAL hMemory, LPSTR lpImageFile, LPSTR lpMaskFile )
/************************************************************************/
{
    BOOL bResult = FALSE;
	LPMETAFILEPICT lpMetafilePict = (LPMETAFILEPICT)GlobalLock (hMemory);
	if (lpMetafilePict && lpMetafilePict->hMF)
	{
		CDC cdc;
        CDC *pScreenDC = CDC::FromHandle(GetDC(NULL));
		CBitmap bmp;
        unsigned long w ,h ;

		cdc.CreateCompatibleDC(pScreenDC);
        cdc.SetMapMode (lpMetafilePict->mm);
        w = h = 100;
        if (lpMetafilePict->mm >= MM_ISOTROPIC)
        {
            SIZE ptView;
            if (lpMetafilePict->xExt == 0) // no sugessted size/aspect ration 
            {
                ptView.cx = w;
                ptView.cy = h;
            }
            else if (lpMetafilePict->xExt > 0) // is a size suggested?
            {
                ptView.cx = lpMetafilePict->xExt;
                ptView.cy = lpMetafilePict->yExt;
                cdc.HIMETRICtoDP(&ptView);
                w = ptView.cx;
                h = ptView.cy;
            }
            else if (lpMetafilePict->xExt < 0) // is a aspect ratio sugested?
            {
                SIZE ptMM;
                ptMM.cx = w;
                ptMM.cy = h;
                cdc.DPtoHIMETRIC(&ptMM);
                // see what would happen to each axis if scaled
                ptView.cx = (short) ((long)ptMM.cy * lpMetafilePict->xExt) / lpMetafilePict->yExt;
                ptView.cy = (short) ((long)ptMM.cx * lpMetafilePict->yExt) / lpMetafilePict->xExt;
                // one side will be too long if scaled
                if (ptView.cx > ptMM.cx)
                    ptView.cx = ptMM.cx;
                else
                    ptView.cy = ptMM.cy;
                cdc.HIMETRICtoDP(&ptView);
                w = ptView.cx;
                h = ptView.cy;
            }
            cdc.SetViewportExt(ptView.cx,ptView.cy);
        }
        bmp.CreateCompatibleBitmap	(pScreenDC, w,h);
        cdc.SelectObject(&bmp);
        cdc.FloodFill(0,0,RGB(255,255,255));
    	if (cdc.PlayMetaFile(lpMetafilePict->hMF))
		{
            HBITMAP hBitmap = (HBITMAP) bmp.Detach();								
            if (hBitmap)
        	{
        		HPALETTE hPal;
        		if (!(hPal = (HPALETTE)GetClipboardData(CF_PALETTE)) )
        		    hPal = lpBltScreen->hPal;
        		bResult =  PasteBitmap( (HBITMAP)hBitmap,
                    hPal, lpImageFile, lpMaskFile);
        	}
		}
        GlobalUnlock (hMemory);
	}
    return (bResult);
}

/************************************************************************/
static BOOL WriteDIBClip( LPBITMAPINFOHEADER lpInfo, LPSTR lpImageFile,
                              LPSTR lpMaskFile )
/************************************************************************/
{
FNAME szFileName;

if ( lpInfo->biSize != 40 || // make sure it's the correct length
     lpInfo->biCompression != BI_RGB ) // no compression
    return(FALSE);

if ( lpImageFile )
    {
    if ( !WriteDIB(lpInfo, lpImageFile) )
        return(FALSE);
    }

if ( lpMaskFile && *lpMaskFile)
    {
    if ( !WriteDIBMask(lpInfo, lpMaskFile) )
        {
        lstrcpy(szFileName, lpImageFile);
        FileDelete(szFileName);
        return(FALSE);
        }
    }

return(TRUE);
}


/************************************************************************/
static BOOL WriteDIB( LPBITMAPINFOHEADER lpInfo, LPSTR lpImageFile )
/************************************************************************/
{
DWORD             lCount;
WORD              wCount;
HPTR              lpClipPtr;
BITMAPFILEHEADER  bmhdr;
FILE_HANDLE       ofh;
DWORD             bpl;

bmhdr.bfType = 0x4D42;
bmhdr.bfReserved1 = 0;
bmhdr.bfReserved2 = 0;
bmhdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
if (lpInfo->biBitCount == 24)
    {
    bpl = lpInfo->biWidth * 3;
    }
else
if (lpInfo->biBitCount == 8)
    {
    bpl = lpInfo->biWidth;
    bmhdr.bfOffBits += ( 256 * sizeof(RGBQUAD) );
    }
else
if (lpInfo->biBitCount == 4)
    {
    bpl = (lpInfo->biWidth+1)/2;
    bmhdr.bfOffBits += ( 16 * sizeof(RGBQUAD) );
    }
else
if (lpInfo->biBitCount == 1)
    {
    bpl = (lpInfo->biWidth+7)/8;
    bmhdr.bfOffBits += ( 2 * sizeof(RGBQUAD) );
    }
bpl = (bpl + 3) & ~3;

if (!lpInfo->biSizeImage)
    lpInfo->biSizeImage = bpl * lpInfo->biHeight;

lCount = bmhdr.bfOffBits + lpInfo->biSizeImage;
bmhdr.bfSize = lCount/2;

// write the winclip.bmp file
if ( (ofh = FileOpen( lpImageFile, FO_CREATE|FO_WRITE )) == FILE_HANDLE_INVALID )
    {
    Message (IDS_EOPEN, lpImageFile);
    return(FALSE);
    }
if ( FileWrite( ofh, (LPTR)&bmhdr, 14 ) != 14 )
    {
    Message (IDS_EWRITE, lpImageFile);
    FileClose(ofh);
    return(FALSE);
    }
lCount -= 14;
lpClipPtr = (HPTR)lpInfo;
while( lCount > 0L )
    {
    wCount = ( ( lCount > 65500L ) ? 65500 : lCount );
	DWORD dwCnt = FileWrite( ofh, lpClipPtr, wCount );
	if ( dwCnt != wCount )
        {
        Message( IDS_EWRITE, lpImageFile );
        FileClose(ofh);
        return(FALSE);
        }
    lpClipPtr += wCount;
    lCount -= wCount;
    }

FileClose(ofh);
return(TRUE);
}


/************************************************************************/
static BOOL WriteDIBMask( LPBITMAPINFOHEADER lpInfo, LPSTR lpMaskFile )
/************************************************************************/
{
BOOL     bRet;
LPMASK   lpMask;

// create the mask
if ( !(lpMask = MaskCreate( NULL, (int)lpInfo->biWidth,
    (int)lpInfo->biHeight, ON, YES )) )
        return(FALSE);

// write out the mask
bRet = WriteMask( lpMaskFile, lpMask, FALSE );
MaskClose( lpMask );

return( bRet );
}


#define WIDTHBYTES(i) \
    ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */

//**********************************************************************
LPBITMAPINFOHEADER BitmapToDIB( HDC hDC, HBITMAP hBitmap, HPALETTE hPal )
//**********************************************************************
{
LPTR lp;
LPBITMAPINFOHEADER lpDIB;
DWORD dwLen;
BITMAP bm;
BITMAPINFOHEADER bi;
BOOL bDCPassedIn;
HPALETTE hOldPal;

if ( !GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&bm ) )
    {
    Message( IDS_EMEMALLOC );
    return( NULL );
    }

bi.biSize           = sizeof(BITMAPINFOHEADER);
bi.biWidth          = bm.bmWidth;
bi.biHeight         = bm.bmHeight;
bi.biPlanes         = 1;
bi.biCompression    = BI_RGB;
bi.biXPelsPerMeter  = (10000L*(long)72)/254L;
bi.biYPelsPerMeter  = (10000L*(long)72)/254L;
bi.biClrImportant   = 0;
bi.biBitCount       = bm.bmPlanes * bm.bmBitsPixel;
if ( bi.biBitCount >= 16 )
    bi.biBitCount = 24;
if ( bi.biBitCount <= 8 )
    bi.biClrUsed = (int)(1 << (int)bi.biBitCount);
else
    bi.biClrUsed = 0;

// fill in the size based on the width, height and bit depth.
bi.biSizeImage = (DWORD)WIDTHBYTES(bm.bmWidth * bi.biBitCount) * bm.bmHeight;

// Alloc the buffer to be big enough to hold all the bits
dwLen = (DWORD)bi.biSize + (bi.biClrUsed * sizeof(RGBQUAD)) + bi.biSizeImage;
if ( !(lpDIB = (LPBITMAPINFOHEADER)Alloc( dwLen )) )
    {
    Message( IDS_EMEMALLOC );
    return( NULL );
    }

if ( !hDC )
    {
    bDCPassedIn = NO;
    hDC = GetDC( NULL );
    }
else
    bDCPassedIn = YES;

if ( hPal )
    {
    hOldPal = SelectPalette( hDC, hPal, FALSE );
    RealizePalette( hDC );
    }
else
    hOldPal = NULL;

// call GetDIBits and get the bits
lp = (LPTR)lpDIB + bi.biSize + (bi.biClrUsed * sizeof(RGBQUAD));
*lpDIB = bi;
if ( !GetDIBits( hDC, hBitmap, 0, (WORD)bm.bmHeight, lp,
    (LPBITMAPINFO)lpDIB, DIB_RGB_COLORS ) )
    {
    Message( IDS_EMEMALLOC );
    FreeUp( (LPTR)lpDIB );
    return( NULL );
    }

if ( hOldPal )
    {
    SelectPalette( hDC, hOldPal, FALSE );
    RealizePalette( hDC );
    }

if ( !bDCPassedIn )
    ReleaseDC( NULL, hDC );

lpDIB->biClrUsed = bi.biClrUsed;
return( lpDIB );
}


//**********************************************************************
LPFRAME BitmapToFrame( HDC hDC, HBITMAP hBitmap, HPALETTE hPal )
//**********************************************************************
{
LPBITMAPINFOHEADER lpDIB;
LPFRAME lpFrame;

if ( !(lpDIB = BitmapToDIB( hDC, hBitmap, hPal )) )
    return( NULL );
lpFrame = DIBToFrame( (LPTR)lpDIB, YES );
FreeUp( (LPTR)lpDIB );
return( lpFrame );
}






