//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "id.h"
#include "tiffio.h"

// Static prototypes
LOCAL LPIMAGE FastBitsLoadEx( LPIMAGE lpImage, LPFASTBITS_PARMS lpParms, BOOL bOpenDoc, LPFRAME lpFastbitsPreview);
static BOOL FastbitsToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg );
static LPFRAME AttachPreviewFrame( HWND hDlg, ITEMID id, LPFRAME lpFrame );
static void UpdateMemoryReadout( HWND hDlg, ITEMID id, LPRECT lpGridRect );
static void FreeUpPreview(LPFRAME lpPreviewFrame);
LOCAL LPIMAGE find_fastbits(LPSTR lpName);

// Static data
static LPFRAME lpPreviewFrame;
static ITEMID idDataType;
static RECT rGridRect;
static BOOL bGridLock;
static int iImageWidth, iImageHeight, iImageDepth;
static RECT rControl;
static int iFileWidth, iFileHeight, iFileDepth;
static long lFileRes, lSampledRes;
static int Resolution;
static ITEMID LowResFileType;
static ITEMID LowResDataType;
static LPSTR lpLowResFileName;

/***********************************************************************/
LOCAL BOOL FindOriginalInfo(LPSTR lpFileName, LPINT lpFileWidth,
							LPINT lpFileHeight, LPLONG lpFileRes,
							LPINT lpFileDepth, LPINT lpDataType,
							int depth, LPSTR lpStartFile)
/***********************************************************************/
{
	ITEMID idFileType;
	BOOL fRet;

	++depth;
	if (depth > 10)
	{
		Message(IDS_LORES_NI);
		return(FALSE);
	}

	idFileType = GetPPFileType(lpFileName);
	if ((idFileType == IDN_PP) || (idFileType == IDN_PPCOMPOUND))
	{
		CReadImage read(lpFileName);
		if (!read.ReadHeader())
			return(FALSE);
		// if we don't have an original version, try to find one
		if (read.HasOriginalVersion())
		{
			*lpFileWidth = read.GetFileWidth();
			*lpFileHeight = read.GetFileHeight();
			*lpFileRes = read.GetFileResolution();
			*lpFileDepth = read.GetFileDepth();
			*lpDataType = read.GetDataType();
			return(TRUE);
		}
		else
		{
			LIST PacketList;
			LPCMDPKT lpCmdPkt;
			LPSTR lpOrigFile;

			if (!read.ReadPacketList(TRUE))
				return(FALSE);
			read.GetPacketList(&PacketList);
			lpCmdPkt = (LPCMDPKT)ListGetHead(&PacketList);
			if (!lpCmdPkt)
				return(FALSE);
			lpOrigFile = GetLoadFileName(lpCmdPkt);
			if (!lpOrigFile)
			{
				DestroyPacketList(&PacketList);
				return(FALSE);
			}
			fRet = FindOriginalInfo(lpOrigFile, lpFileWidth, lpFileHeight,
								lpFileRes, lpFileDepth, lpDataType, depth,
								lpStartFile);
			DestroyPacketList(&PacketList);
			return(fRet);
		}
		
	}
	else
	if (idFileType == IDN_PP4 || idFileType == IDN_TIFF)
	{
		CReadImage read(lpFileName);
		LPTIFFHEADER lpHdr;

		if (idFileType == IDN_PP4)
			{
			lpHdr = read.ReadObjHeader();
			}
		else
			{
			lpHdr = read.ReadTiffHeader();
			}
		if (!lpHdr)
			return(FALSE);
		*lpFileWidth = lpHdr->npix;
		*lpFileHeight = lpHdr->nlin;
		*lpFileRes = lpHdr->lResolution;
		*lpFileDepth = lpHdr->OutDepth;
		read.FreeTiffHeader(lpHdr);
		return(TRUE);
	}
	else
	{
		Message(IDS_ENOLOWRESDATA, (LPSTR)lpStartFile, (LPSTR)lpFileName);
		return(FALSE);
	}
}

/***********************************************************************/
BOOL DoLowResLoad( ITEMID idFileType, LPSTR lpFileName, LPCMSINFO pCmsInfo )
/***********************************************************************/
{
BOOL fRet, fHasCurrent = FALSE;
LPLOWRESLOAD_PARMS lpParms;
OPENINFO OpenInfo;

LowResFileType = GetPPFileType(lpFileName);
if ( !LowResFileType  )
	LowResFileType = idFileType;
if (LowResFileType != IDN_TIFF &&
	LowResFileType != IDN_PPCOMPOUND &&
	LowResFileType != IDN_PP &&
	LowResFileType != IDN_PP4)
	{
	Message(IDS_LORES_NI);
	return(FALSE);
	}
//if ( !bCheckDuplicates )
//	lpNewName = lpFileName;
//else
//	{
//	if ( !OKToOpen( lpFileName, &lpNewName ) )
//		return(FALSE);
//	}

if (!FindOriginalInfo(lpFileName, &iFileWidth, &iFileHeight,
						&lFileRes, &iFileDepth, &LowResDataType, 0,
						lpFileName))
	return(FALSE);

if (LowResFileType == IDN_PP)
	{
	CReadImage read(lpFileName);
	if (!read.ReadHeader())
		return(FALSE);

	lSampledRes = read.GetSampledResolution();
	fHasCurrent = read.HasCurrentVersion();
	}

if (!lFileRes && !lSampledRes)
	lSampledRes = 72;
else
if (!fHasCurrent || !lSampledRes)
	lSampledRes = lFileRes / 2;

//if (lSampledRes < 72)
//	lSampledRes = 72;

lpLowResFileName = lpFileName;
lstrcpy( Names.Saved, lpFileName );
fRet = (BOOL)AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_LOWRES_OPEN, DlgLowResProc);

if (!fRet)
	return( FALSE );

lpParms = (LPLOWRESLOAD_PARMS)Alloc(sizeof(LOWRESLOAD_PARMS));
if (!lpParms)
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
if( pCmsInfo )
	lpParms->cmsInfo = *pCmsInfo;
else
	cmsInfoInit( &lpParms->cmsInfo );

lstrcpy(lpParms->szFileName, lpFileName);
GetVolumnID(lpParms->szFileName, lpParms->szVolumeID);
lpParms->idFileType = LowResFileType;
lpParms->Resolution = Resolution;

OpenInfo.idCommand = IDS_CMD_LOWRESLOAD;
OpenInfo.lpParms = (LPCMD_PARMS)lpParms;
OpenInfo.PhotoCDResOverride = 0;

CDocument *pDoc = PictPubApp.OpenDocumentFile(lpFileName, &OpenInfo);
if (!pDoc)
	FreeUp(lpParms);

return(pDoc != NULL);
}

/***********************************************************************/
BOOL LowResLoad( LPIMAGE lpImage, LPLOWRESLOAD_PARMS lpParms )
/***********************************************************************/
{
LPNEWIMAGEINFO lpInfo;

CReadImage Read(lpParms->szFileName, lpParms->idFileType, &lpParms->cmsInfo, lpParms->szVolumeID );
if (!Read.CanReaderSample())
	{
	Message(IDS_BADLOWRES);
	return(FALSE);
	}

lpInfo = (LPNEWIMAGEINFO)Alloc(sizeof(NEWIMAGEINFO));
if (!lpInfo)
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
clr((LPTR)lpInfo, sizeof(NEWIMAGEINFO));

//ProgressBegin(1, PROGRESS_ID(IDS_UNDOLOWRESOPEN));

Read.SetSampling(lpParms->Resolution);
Read.SetOptions(lpParms->lpOptions);
if (!(lpImage = Read.ReadImage(lpParms->lpCmdList, IMG_LOWRES)))
	{

//	ProgressEnd();
	FreeUp(lpInfo);
	return( FALSE );
	}

FRMTYPEINFO TypeInfo;

ImgGetTypeInfo(lpImage, &TypeInfo);
if (IsDstPTSelected(&TypeInfo.ptInfo))
	lpParms->cmsInfo.dst = TypeInfo;
Read.GetCMSSrcTypeInfo(&lpParms->cmsInfo.src);

//ProgressEnd();
if (lpImage)
	{
	AddRecallImageName(lpParms->szFileName);    
	lpImage->fChanged = YES;
	lpInfo->lpImage = lpImage;
	ListAddTail(&lpParms->Common.NewImageList, lpInfo);
	lpParms->Common.UpdateType = UT_NEWIMAGE;
	return(TRUE);
	}
else
	{
	FreeUp(lpInfo);
	return(FALSE);
	}
}

// LOCAL prototypes
LOCAL BOOL LowRes_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void LowRes_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static LFIXED LowRes_Width, LowRes_Height;

/***********************************************************************/
BOOL WINPROC EXPORT DlgLowResProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, LowRes_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, LowRes_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_FASTBITS);
	}
}

/************************************************************************/
LOCAL BOOL LowRes_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPSTR lpUnit, lpString;
STRING szFormat, szString, szValue;
int iType;

CenterPopup( hDlg );

SetDlgItemText(hDlg, IDC_INFOFILENAME, stripdir(lpLowResFileName) );
iType = IDI_TIFF + (LowResFileType - IDN_TIFF);
 if (AstralStr(iType, &lpString))
	SetDlgItemText(hDlg, IDC_INFOFILETYPE, lpString);
if (AstralStr(LowResDataType, &lpString))
	SetDlgItemText(hDlg, IDC_INFODATATYPE, lpString);
if (AstralStr(IDS_INFOSIZE, &lpString))
	{
	lstrcpy(szFormat, lpString);
	if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES,
	     &lpUnit ) )
		{
		Unit2String(FGET(iFileWidth, lFileRes),
			szValue);
		wsprintf(szString, szFormat, iFileWidth,
			(LPSTR)szValue, lpUnit);
		SetDlgItemText(hDlg, IDC_INFOWIDTH, szString);
		Unit2String(FGET(iFileHeight, lFileRes),
			szValue);
		wsprintf(szString, szFormat, iFileHeight,
			(LPSTR)szValue, lpUnit);
		SetDlgItemText(hDlg, IDC_INFOHEIGHT, szString);
		}
	}
SetDlgItemInt(hDlg, IDC_INFORESOLUTION, (int)lFileRes, NO);
LowRes_Width  = FGET( iFileWidth, lFileRes );
LowRes_Height = FGET( iFileHeight, lFileRes );
Resolution = (int)lSampledRes;
InitDlgItemSpin( hDlg, IDC_OPENRES, Resolution, NO, 1, (int)lFileRes);
ImageMemory( hDlg, IDC_INFOIMAGEMEMORY, LowRes_Width, LowRes_Height,
				(int)lFileRes, iFileDepth);
ImageMemory( hDlg, IDC_MEMORY, LowRes_Width, LowRes_Height, Resolution, iFileDepth );
return(TRUE);
}

/************************************************************************/
LOCAL void LowRes_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;

switch (id)
	{
   case IDC_OPENRES:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Resolution = GetDlgItemSpin( hDlg, id, &Bool, NO );
	if ( Resolution <= 0 )
		Resolution = 1;
	ImageMemory( hDlg, IDC_MEMORY, LowRes_Width, LowRes_Height, Resolution,
			iFileDepth );
	break;

	case IDOK:
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}
   
/************************************************************************/
BOOL FastBitsLoad( LPIMAGE lpImage, LPFASTBITS_PARMS lpParms )
/************************************************************************/
{
	return(FastBitsLoadEx(lpImage, lpParms, NO, NULL) != NULL);
}

/************************************************************************/
LOCAL  LPIMAGE  FastBitsLoadEx( LPIMAGE lpImage, LPFASTBITS_PARMS lpParms, BOOL bOpenDoc, LPFRAME lpFastbitsPreview)
/************************************************************************/
{
	LPNEWIMAGEINFO lpInfo;
	RECT rCrop;
	FNAME szUntitled;
	CDocument *pDoc;

	CReadImage Read(lpParms->szFileName, IDN_TIFF, &lpParms->cmsInfo, lpParms->szVolumeID);
	if (!Read.CanCrop())
	{
		Message(IDS_FASTBITS_NI);
		   return(NULL);
	}

	lpInfo = (LPNEWIMAGEINFO)Alloc(sizeof(NEWIMAGEINFO));
	if (!lpInfo)
	{
		   Message(IDS_EMEMALLOC);
		   return(NULL);
	}
	clr((LPTR)lpInfo, sizeof(NEWIMAGEINFO));

	ProgressBegin(1, PROGRESS_ID(IDS_UNDOFASTBITSOPEN));

	rCrop.left   = lpParms->Rect.left;
	rCrop.top    = lpParms->Rect.top;
	rCrop.right  = lpParms->Rect.right;
	rCrop.bottom = lpParms->Rect.bottom;

	Read.SetCropping(&rCrop);

	// read in image
	if (!(lpImage = Read.ReadImage(lpParms->lpCmdList, IMG_DOCPIECE)))
	{
		ProgressEnd();
		FreeUp(lpInfo);
		return( NULL);
	}

	FRMTYPEINFO TypeInfo;

	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (IsDstPTSelected(&TypeInfo.ptInfo))
		lpParms->cmsInfo.dst = TypeInfo;
	Read.GetCMSSrcTypeInfo(&lpParms->cmsInfo.src);

	ProgressEnd();
	if (lpImage)
   	{
		// 'fake' an untitled image
	    lpImage->lpFastbitsPreview = lpFastbitsPreview;
		AddRecallImageName(lpParms->szFileName);    
		lpImage->rFastbitsGrid = rCrop;
		lpImage->fChanged = YES;
		lpImage->fNoSize = YES;
		lstrcpy(lpImage->ImageName, lpParms->szFileName);
		if (!bOpenDoc)
		{
			// setup for command list to open the doc
			lpInfo->lpImage = lpImage;
			lpParms->Common.UpdateType = UT_NEWIMAGE;
			ListAddTail(&lpParms->Common.NewImageList, lpInfo);
		}
		else
		{	
			FreeUp(lpInfo);
			pDoc = PictPubApp.OpenDocumentFile(lpImage->CurFile,lpImage); 
			if (!pDoc)
				delete lpImage;
		}
		return(lpImage);
	}
	else
	{
		FreeUp(lpInfo);
		return(NULL);
	}
}

/***********************************************************************/
BOOL DoFastbitsSave( ITEMID idDataType, ITEMID idFileType, LPSTR lpFileName )
/***********************************************************************/
{
FNAME szFileName;
LPIMAGE lpImage;

if (!(lpImage = GetActiveImage()))
	return(FALSE);
if (ImgMultipleObjects(lpImage))
	{
	Message(IDS_OBJECTSNEEDCOMBINE);
	return(FALSE);
	}
// Copy the filename bacause the close below will free lpImage->CurFile
lstrcpy( szFileName, lpFileName );
if (lpImage->lpFastbitsPreview)
	FreeUpPreview(lpImage->lpFastbitsPreview);
CReadImage read(szFileName);
if (!read.WriteTiffData(ImgGetBaseEditFrame(lpImage),
				lpImage->rFastbitsGrid.left, lpImage->rFastbitsGrid.top))
	return(NO);
return( TRUE );
}


/***********************************************************************/
BOOL DoFastbitsLoad(ITEMID idFileType, 
	LPSTR lpFileName, 
	LPCMSINFO pCmsInfo /* NULL*/,
	LPFRAME lpFastbitsPreview  /* NULL*/, 
	LPRECT lpFastbitsGridRect  /* NULL*/, 
	BOOL bOpenNow /*TRUE*/)
/***********************************************************************/
{  
    ITEMID           Type;
    LPIMAGE          lpImage = NULL;
    FASTBITS_PARMS   parms, tmpparms;
    int              res;
	BOOL 			bRet = FALSE;

    if ( !lpFileName )
        return(FALSE);

    Type = GetPPFileType(lpFileName);
    if (!Type )
        Type = idFileType;
    if (Type != IDN_TIFF)
    {
        Message(IDS_FASTBITS_NI);
        return(FALSE);
    }

	// use give preview or find any existing fastbits
	if (lpFastbitsGridRect)
	{
		rGridRect = *lpFastbitsGridRect;
		bGridLock = YES;
	}
	else
	{
		AstralSetRectEmpty(&rGridRect);
		bGridLock = NO;
	}
	
	if (lpFastbitsPreview)
		::lpPreviewFrame = lpFastbitsPreview;
	else
	{
		lpImage = find_fastbits(lpFileName);
		if (lpImage)
		{
    		::lpPreviewFrame = lpImage->lpFastbitsPreview;
			if (!lpFastbitsGridRect)
			{
				rGridRect = lpImage->rFastbitsGrid;
    			bGridLock = YES;
			}
		}
		else
    		::lpPreviewFrame = NULL;
	}
	
    lstrcpy( ::Names.Saved, lpFileName );
    if ( !AstralDlg( NO, ::PictPubApp.GetResourceHandle(), ::PictPubApp.Get_hWndAstral(), IDD_FASTBITS, 
				        DlgFastbitsProc) )
		goto Done;

	lstrcpy( parms.szFileName, lpFileName);
	GetVolumnID(parms.szFileName, parms.szVolumeID);
	parms.Rect.left   = rGridRect.left;
	parms.Rect.top    = rGridRect.top;
	parms.Rect.right  = rGridRect.right;
	parms.Rect.bottom = rGridRect.bottom;
	
	parms.lpCmdList = NULL;
	
	if( pCmsInfo )
		parms.cmsInfo = *pCmsInfo;
	else
		cmsInfoInit( &parms.cmsInfo );
	if (bOpenNow)
	{
		// open image and document
		tmpparms = parms;
		if (lpImage = FastBitsLoadEx(NULL, &parms, YES, lpPreviewFrame))
		{
			bRet = TRUE;
			PostCommand(lpImage->lpCmdList, IDS_CMD_FASTBITS, &tmpparms);
		}
	}
	else
	{
    	bRet = ProcessLoadCommand(IDS_CMD_FASTBITS, &parms);
	}
	
Done:
	// lpPreviewFrame may have been installed in the new image
	// THIS MAY NOT WORK IN A TRUE MULTITHREAD WORLD!!??
	if ( lpPreviewFrame && !PreviewBeingUsed(lpPreviewFrame))
	{
	 	FrameClose( lpPreviewFrame );
	 	lpPreviewFrame = NULL;
	}
	
	return(bRet);
}


// LOCAL prototypes
LOCAL BOOL Fastbits_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Fastbits_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Fastbits_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange);

/***********************************************************************/
BOOL WINPROC EXPORT DlgFastbitsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Fastbits_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Fastbits_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Fastbits_OnPaletteChanged);

	HANDLE_DLGMSG(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_FASTBITS);
	}
}

/************************************************************************/
LOCAL BOOL Fastbits_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{    
RECT rRect;
HWND hControl;

CenterPopup( hDlg );
CheckDlgButton( hDlg, IDC_FAST_LOCK, bGridLock );

// Get the preview control's handle and size
if ( !(hControl = GetDlgItem( hDlg, IDC_FAST_PREVIEW )) )
	return(TRUE);
GetClientRect( hControl, &rRect );
WindowsToAstralRect(&rRect);

// Open the passed in image at the right size
if (!lpPreviewFrame)
	{
	CReadImage Read(Names.Saved);
	Read.SetSampling(&rRect);
	lpPreviewFrame = Read.ReadFrame();
	if (lpPreviewFrame)
		{
		iImageWidth = Read.GetFileWidth();
		iImageHeight = Read.GetFileHeight();
		idDataType = Read.GetDataType();
		}
	}

// iImageWidth and iImageHeight are used for Image2Control mapping
if ( !(iImageDepth  = FrameDepth(lpPreviewFrame)) )
	iImageDepth = 1;

// Attach the image to the preview control
AttachPreviewFrame( hDlg, IDC_FAST_PREVIEW, lpPreviewFrame );

if (iImageWidth < 3) iImageWidth = 3;
if (iImageHeight < 3) iImageHeight = 3;

// Initialize the grid rectangle, if necessary
if ( AstralIsRectEmpty( &rGridRect ) )
	{ // Break it into thirds
	rGridRect.left = iImageWidth/3;
	rGridRect.top  = iImageHeight/3;
	rGridRect.right  = 2 * rGridRect.left;
	rGridRect.bottom = 2 * rGridRect.top;
	}
	
UpdateMemoryReadout( hDlg, IDC_MEMORY, &rGridRect );
return(TRUE);
}

/***********************************************************************/
void Fastbits_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange)
/***********************************************************************/
{
if (IsOurWindow(hwndPaletteChange))
	AstralControlPaint(hDlg, IDC_FAST_PREVIEW);
}

/************************************************************************/
LOCAL void Fastbits_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_FAST_LOCK:
	bGridLock = !bGridLock;
	InvalidateRect( GetDlgItem(hDlg, IDC_FAST_PREVIEW), NULL, NO );
	CheckDlgButton( hDlg, id, bGridLock );
	break;

	case IDC_FAST_PREVIEW:
	UpdateMemoryReadout( hDlg, IDC_MEMORY, &rGridRect );
	if ( codeNotify != 1 )
		break;
	// Fall through if a double click

	case IDOK:
   if (!bGridLock)
	   bGridLock = !bGridLock;
	// Unattach the preview frame
	lpPreviewFrame = AttachPreviewFrame( hDlg, IDC_FAST_PREVIEW, NULL );
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	// Unattach any preview frame
	lpPreviewFrame = AttachPreviewFrame( hDlg, IDC_FAST_PREVIEW, NULL );
	AstralDlgEnd( hDlg, FALSE );
	// Close the preview frame
	if ( lpPreviewFrame && !PreviewBeingUsed(lpPreviewFrame))
		{
		FrameClose( lpPreviewFrame );
		lpPreviewFrame = NULL;
		}
	break;

   default:
	break;
   }
}


/************************************************************************/
// finds any opened fastbits image for the given file.
// if there is more than 1 then will give the one with a preview frame.
/************************************************************************/
LOCAL LPIMAGE find_fastbits(LPSTR lpFileName)
/************************************************************************/
{
	LPIMAGE lpImage = NULL;
	BOOL found = FALSE;
	// jtm
	POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
	while (TemplatePos)
	{
		CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
		POSITION DocPos = pTemplate->GetFirstDocPosition();
		while (DocPos)
		{
		    CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
		    if (!pDoc)
				continue;
	        if (!pDoc->m_lpImage || pDoc->m_lpImage->DocumentType != IMG_DOCPIECE)
				continue;
            if (!StringsEqual(pDoc->m_lpImage->CurFile, lpFileName))
				continue;
			lpImage = pDoc->m_lpImage;
		    if (lpImage->lpFastbitsPreview)
		    {
		    	found = TRUE;
		        break;
		    }
		}
		if (found)
			break;
	}

	return(lpImage);
}


/************************************************************************/
BOOL PreviewBeingUsed(LPFRAME lpPreviewFrame)
/************************************************************************/
{
BOOL found = FALSE;
// jtm
POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
while (TemplatePos)
{
	CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
	POSITION DocPos = pTemplate->GetFirstDocPosition();
	while (DocPos)
	{
	    CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
	    if (pDoc)
	    {
	        if (pDoc->m_lpImage)
	        {
	            if (pDoc->m_lpImage->lpFastbitsPreview == lpPreviewFrame)
	            {
	                found = TRUE;
	                break;
	            }
	        }
	    }
	}
	if (found)
		break;
}

return(found);
}

/************************************************************************/
static void FreeUpPreview(LPFRAME lpPreviewFrame)
/************************************************************************/
{
POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
while (TemplatePos)
{
	CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
	POSITION DocPos = pTemplate->GetFirstDocPosition();
	while (DocPos)
	{
	    CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
	    if (pDoc)
	    {
	        if (pDoc->m_lpImage)
	        {
	            if (pDoc->m_lpImage->lpFastbitsPreview == lpPreviewFrame)
	            {
					pDoc->m_lpImage->lpFastbitsPreview = NULL;
	            }
	        }
	    }
	}
}

FrameClose(lpPreviewFrame);
}

/************************************************************************/
static LPFRAME AttachPreviewFrame( HWND hDlg, ITEMID id, LPFRAME lpFrame )
/************************************************************************/
{
HWND hControl;
LPFRAME lpOldFrame;

// Get the handle to the preview image control
if ( !(hControl = GetDlgItem( hDlg, id )) )
	return( FALSE );

// Link the passed in frame to the sample image control
lpOldFrame = (LPFRAME)GetWindowLong( hControl, GWL_IMAGE );
SetWindowLong( hControl, GWL_IMAGE, (long)lpFrame );
SetWindowLong( hControl, GWL_IMAGE_TOOLPROC, (long)FastbitsToolProc );
InvalidateRect( hControl, NULL, NO );
return( lpOldFrame );
}


/************************************************************************/
static BOOL FastbitsToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg )
/************************************************************************/
{
POINT pt;
static HDC  shDC = NULL;
static RECT SelectRect;
static BOOL bTrack;

switch (msg)
	{

	case WM_SIZE: // Values used for Image2Control mapping
	rControl.right  = rControl.left + x - 1;
	rControl.bottom = rControl.top  + y - 1;
	SetImageControlMapping( iImageWidth, iImageHeight, &rControl );
	break;

	case WM_MOVE: // Values used for Image2Control mapping
	rControl.left = x;
	rControl.top = y;
	SetImageControlMapping( iImageWidth, iImageHeight, &rControl );
	break;

	case WM_LBUTTONDOWN:
	if ( bTrack )
		break;
	bTrack = YES;
	UpdateWindow( hWindow );
	shDC = GetDC( hWindow );
	SetClipDC( shDC, &rControl );
	if ( !bGridLock )
		{
		DrawFastbitsGrid( hWindow, shDC, &rGridRect, SL_GRID ); // turn it off
		InvertGrid( hWindow, shDC, &rGridRect, 0, 0 );
		pt.x = x; pt.y = y;
		StartSelection( hWindow, shDC, &SelectRect, SL_GRID | SL_SPECIAL,
			pt, 0L );
		rGridRect = SelectRect;
//		Control2Image( &rGridRect.left, &rGridRect.top );
//		Control2Image( &rGridRect.right, &rGridRect.bottom );
		}
	else
		{
		InvertGrid( hWindow, shDC, &rGridRect, x, y );
		}
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	break;

	case WM_LBUTTONUP:
	if (!bTrack)
		break;
	bTrack = NO;
	if ( !bGridLock )
		{
		if (!shDC)
			break;
		EndSelection( hWindow, shDC, &SelectRect, SL_GRID, YES );
		rGridRect = SelectRect;
//		Control2Image( &rGridRect.left, &rGridRect.top );
//		Control2Image( &rGridRect.right, &rGridRect.bottom );
		DrawFastbitsGrid( hWindow, shDC, &rGridRect, SL_GRID ); // turn it on
		InvertGrid( hWindow, shDC, &rGridRect, 0, 0 );
		}
	ReleaseDC( hWindow, shDC );
	shDC = NULL;
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	break;

	case WM_MOUSEMOVE:
	if (!bTrack)
		break;
	if (!shDC)
		break;
	if ( !bGridLock )
		{
		pt.x = x; pt.y = y;
		UpdateSelection( hWindow, shDC, &SelectRect, SL_GRID,
			pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||RBUTTON,FROMCENTER);
		rGridRect = SelectRect;
//		Control2Image( &rGridRect.left, &rGridRect.top );
//		Control2Image( &rGridRect.right, &rGridRect.bottom );
		}
	else
		{
		InvertGrid( hWindow, shDC, &rGridRect, x, y );
		}
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_PAINT:
	hDC = GetDC( hWindow );
	SetClipDC( hDC, &rControl );
	DrawFastbitsGrid( hWindow, hDC, &rGridRect, SL_GRID );
	InvertGrid( hWindow, hDC, &rGridRect, 0, 0 );
	ReleaseDC( hWindow, hDC );
	break;
	}

return(TRUE);
}


/***********************************************************************/
static void UpdateMemoryReadout( HWND hDlg, ITEMID id, LPRECT lpGridRect )
/***********************************************************************/
{
RECT rRect;

rRect = *lpGridRect;
BoundRect( &rRect, 0, 0, iImageWidth-1, iImageHeight-1 );
ImageMemory( hDlg, id, FGET( RectWidth(&rRect), 1 ),
	FGET( RectHeight(&rRect), 1 ), 1/*Resolution*/, iImageDepth );
}

