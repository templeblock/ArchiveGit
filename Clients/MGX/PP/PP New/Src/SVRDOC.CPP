/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                svrdoc.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"    
#include "ppafx.h"
#include "svrdoc.h"
#include "svritem.h"
#include "svrview.h"
#include "dataitem.h"
#include "ipframe.h"
#include "imgio.h"
#include "pptbars.h"
#include "ctxhelp.h"
#include "io.h"
#include "id.h"
#include "ppimport.h"
#include "pptbars.h"
#include "maskutil.h"
#include "dlgsumry.h"
#include "id.h"

extern BOOL SetupImportOptions( int );


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE szContents[] = "Contents";
static char BASED_CODE szOLE10Contents[] = "\1Ole10Native";

IMPLEMENT_DYNCREATE(CServerDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CServerDoc, COleServerDoc)		   
	ON_COMMAND(				IDM_CLOSE,			OnCloseImage)
	ON_UPDATE_COMMAND_UI(	IDM_CLOSE, 			OnUpdateCloseImage)
	ON_COMMAND(				ID_FILE_UPDATE,		OnFileUpdate)
	ON_UPDATE_COMMAND_UI(	ID_FILE_UPDATE,		OnUpdateFileUpdate)
	ON_COMMAND(				ID_FILE_SAVE_COPY_AS,OnFileSaveCopyAs)
	ON_UPDATE_COMMAND_UI(	ID_FILE_SAVE_COPY_AS,OnUpdateFileSaveCopyAs)
	ON_COMMAND(				IDM_SAVE, 			OnSave)
	ON_UPDATE_COMMAND_UI(	IDM_SAVE, 			OnUpdateSave)
	ON_COMMAND(				IDM_SAVEAS, 		OnSaveAs)
	ON_UPDATE_COMMAND_UI(	IDM_SAVEAS, 		OnUpdateSaveAs)
	ON_COMMAND(				IDM_PRINT, 			OnPrint)
	ON_UPDATE_COMMAND_UI(	IDM_PRINT, 			OnUpdatePrint)
END_MESSAGE_MAP()

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */

#if 0
    #define IFDEBUG(s) {s;}
    #define IFCOPY(s) s
#else
    #define IFDEBUG(s)
    #define IFCOPY(s)
#endif

/* ********************************** Macros ******************************** */
/* ********************************** Types ********************************* */


class CInsertDialog : public CDialog
{
public:
	CInsertDialog (UINT nIDTemplate,CWnd *pParentWnd);
	virtual  BOOL OnInitDialog( );
	virtual void OnOK( );
	UINT m_ActionID;

    DECLARE_MESSAGE_MAP()                 
	afx_msg void OnImport() {CheckRadioButton(IDC_INSERTEXIST,IDC_INSERTIMPORT,IDC_INSERTIMPORT);};
	afx_msg void OnNew() {CheckRadioButton(IDC_INSERTEXIST,IDC_INSERTIMPORT,IDC_INSERTNEW);};
	afx_msg void OnOpen() {CheckRadioButton(IDC_INSERTEXIST,IDC_INSERTIMPORT,IDC_INSERTEXIST);};

};

BEGIN_MESSAGE_MAP(CInsertDialog, CDialog)
	ON_COMMAND(IDC_INSERTIMPORT, OnImport)
	ON_COMMAND(IDC_INSERTNEW, OnNew)
	ON_COMMAND(IDC_INSERTEXIST, OnOpen)
END_MESSAGE_MAP()

/* ******************************* Local Data ******************************* */
/* ******************************* Public Data ****************************** */

extern HINSTANCE  hInstAstral;
extern HINSTANCE  hInstStringLib;
extern STRING szAppName;
extern BOOL fAppActive;

/* *************************** Private Functions **************************** */

BOOL anotherDocWithSameNameExits(CImage* pImage, FNAME szFileName)
{
	BOOL bResult = FALSE;
	CServerDoc *pDoc;
	POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
	CString SaveAsName (szFileName);
	CString DocName;
	SaveAsName.MakeLower();

	while (TemplatePos && !bResult)
	{
	    CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);
	    POSITION DocPos = pTemplate->GetFirstDocPosition();
	    while (DocPos)
	    {
	        pDoc = (CServerDoc*)pTemplate->GetNextDoc (DocPos);
	        if (pDoc)
	        {
				DocName = pDoc->GetPathName();
				DocName.MakeLower();
				if ((DocName == SaveAsName) && (pDoc->m_lpImage != pImage))
				{
					bResult = TRUE;
					break;
				}
	        }
	    }
	}
	return (bResult);
}

CImage* open_image_as_new(BOOL bImporting)
{
	CImage*     pImage = NULL;
    OPENSTATE   info;
    FNAME       szName;
    LPTSTR       lpString;
	ITEMID		FileType;
	BOOL		fRet;
    
    /* Bring up the modal 'File Open' box */
    info.bSaving = FALSE;
    info.szFileSpec[0] = info.szAlbum[0] = info.szPath[0] = '\0';
    info.idFileType = 0;
    info.wFileAttr = (WORD)-1;
    info.idDataType = IDC_SAVELA;
    info.bImported = bImporting;

    if (!AstralStr(bImporting ? IDS_FILEIMPORT_TITLE:IDS_FILEOPEN_TITLE, &lpString))
        lstrcpy(info.szTitle, "Browser");
    else
        lstrcpy(info.szTitle, lpString);
    if (!AstralStr(bImporting ? IDS_FILEIMPORT_BUTTON:IDS_FILEOPEN_BUTTON, &lpString))
        lstrcpy(info.szOK, "OPEN");
    else
        lstrcpy(info.szOK, lpString);

	CWnd *pParent = CWnd::GetActiveWindow();
	if (!pParent)
		pParent = AfxGetMainWnd();
    if ( !(DoOpenDlg( pParent->GetSafeHwnd(), &info, TRUE)))
        return(NULL);

	if (info.numFiles <= 0)
		return(NULL);

    lstrcpy(szName, info.lpFileList[0]);
    fRet = PromptForVolume(szName, &info.lpFileListVolumes[0], YES);
	FreeUp(info.lpFileList);
	if (fRet)
	{
		FileType = GetPPFileType(szName);
    	if( FileType >= IDN_FIRSTIMPORTFILETYPE && 
            FileType <= IDN_LASTIMPORTFILETYPE )
    	{
        	if( !SetupImportOptions( FileType ))
            	return(NULL);
    	}
		CReadImage read(szName, FileType);
		pImage = read.ReadImage(NULL);
	}

	return (pImage);
}

CImage* create_new_image(
NEW_PARMS parms)
{
    CImage* pImage = NULL;
    int pix, lin;
    LPFRAME lpFrame;
    ITEMID DataType;
    LPNEWIMAGEINFO lpInfo;

    lpInfo = (LPNEWIMAGEINFO)Alloc(sizeof(NEWIMAGEINFO));
    if (!lpInfo)
    {
        Message(IDS_EMEMALLOC);
        return(FALSE);
    }
    clr((LPTR)lpInfo, sizeof(NEWIMAGEINFO));

    pix = FMUL( parms.Resolution, parms.Width );
    lin = FMUL( parms.Resolution, parms.Height );

    if ( !(lpFrame = FrameOpen( parms.Type, pix, lin, parms.Resolution )) )
    {
        FrameError( IDS_EIMAGEOPEN );
        FreeUp(lpInfo);
        return( FALSE );
    }

    if (FrameType(lpFrame) == FDT_CMYKCOLOR)
        FrameSetBackground(lpFrame, 0);
    else
        FrameSetBackground(lpFrame, 255);

    switch (FrameType(lpFrame))
    {
        case FDT_LINEART:
            DataType = IDC_SAVELA;
            break;
        case FDT_RGBCOLOR:
            DataType = IDC_SAVE24BITCOLOR;
            break;
        case FDT_CMYKCOLOR:
            DataType = IDC_SAVE32BITCOLOR;
            break;
        case FDT_GRAYSCALE:
        default:
            DataType = IDC_SAVECT;
            break;
    }
    pImage = CreateImage(NULL, 
                            lpFrame,
                            NULL,   
                            parms.lpCmdList,
                            Control.DefaultFileType,
                            DataType,
                            IMG_DOCUMENT,
                            NULL);
    if (!pImage)
        FreeUp(lpInfo);
    return (pImage);
}

BOOL create_tmp_filename (CString & ImageFile,ITEMID FileType)
{   
    BOOL bResult;
    CString TempFile;
    
    GetTempFileName(0, "svr", 0, TempFile.GetBuffer(MAX_FNAME_LEN));                     
    TempFile.ReleaseBuffer();
    stripext( TempFile.GetBuffer( TempFile.GetLength()));    
    TempFile.ReleaseBuffer();

    bResult = CreateFileName( TempFile.GetBuffer(TempFile.GetLength()), 
        FileType, ImageFile.GetBuffer(MAX_FNAME_LEN) );
    ImageFile.ReleaseBuffer ();

    return (bResult);
}        

BOOL CServerDoc::do_save(
LPCTSTR lpFileName, // for now can be curfile in lpimage
BOOL bReplace /*TRUE*/, // change name on save as ?
BOOL bCommand /*TRUE*/)  // save background  mode ?
{
    FNAME       szFileName;
    LPTSTR       lpString;
    ITEMID      idFileType;
    OPENSTATE   info;
    BOOL        bStatus = YES;
	BOOL 		bSaveRemember;
    ASSERT(m_lpImage);

    if (Control.Crippled)
    {
        Message(Control.Crippled);
        return(FALSE);
    }

    if (ImgInMaskEditMode(m_lpImage))
    {
        ImgSetupMaskEditMode(m_lpImage);
        UpdateStatusBar(NO, NO, NO, NO);
    }

    if (lpFileName) 
    {    
        lstrcpy(szFileName,lpFileName);
        idFileType = m_lpImage->FileType;
		if (m_lpImage->DocumentType == IMG_DOCPIECE)
		{
			bCommand = FALSE;
			bReplace = FALSE;
		}
    }         
    else //saveas
    {

DoDialog:
        /* Bring up the modal 'SaveAs' box */
        // save current image format information
	    info.bImported = FALSE;
        info.bSaving = TRUE;
        
        if (m_lpImage->fUntitled)
            info.szFileSpec[0] = info.szPath[0] = '\0';
        else
        {
            lstrcpy(info.szPath, m_lpImage->CurFile);
            lstrcpy(info.szFileSpec, filename(m_lpImage->CurFile));
        }
                
        if (!AstralStr(IDS_FILESAVE_TITLE, &lpString))
            lstrcpy(info.szTitle, "Browser");
        else
            lstrcpy(info.szTitle, lpString);
        if (!AstralStr(IDS_FILESAVE_BUTTON, &lpString))
            lstrcpy(info.szOK, "SAVE");
        else
            lstrcpy(info.szOK, lpString);
        info.szAlbum[0] = '\0';
        info.idDataType = m_lpImage->DataType;
        if (ImgGetBaseEditFrame(m_lpImage))
            info.frameDepth = ImgGetBaseEditFrame(m_lpImage)->RealDepth;
        else 
            info.frameDepth = 3;
        info.idFileType = m_lpImage->FileType;
        info.wFileAttr = (WORD)-1;
        // Ask User for a name
        if ( !(DoOpenDlg(PictPubApp.m_pMainWnd->GetSafeHwnd(), &info)))
            return(FALSE);
        if (info.numFiles < 1)
            return(FALSE);

        // use first file
        lstrcpy(szFileName, *(info.lpFileList));
        FreeUp((LPTR)info.lpFileList);

		if (anotherDocWithSameNameExits(m_lpImage,szFileName))
		{
			if (AstralAffirm ( IDS_ESAMEFILENAME ))
				goto DoDialog;
		}

        // save the image in the user specified file type 
        idFileType = info.idFileType;
    }            
	
	// at this point the following should be set up:
	// szFileName - filename to save to
	// idFileType - file type to save (IDN_...)
    // we need to see whether we can kick off a background save
	// this can be done for all cases except when we are
	// doing an embedded save.  Embedded() save as is ok to background
	if (bCommand && (!IsEmbedded() || !lpFileName))
	{
	    return(AstralImageSaveBG(m_lpImage,
								 idFileType,
								 m_lpImage->DataType,
								 (LPTSTR)szFileName));
	}

	m_FileType = idFileType; 
	bSaveRemember = m_bRemember;
	m_bRemember = bReplace;

    if (bStatus = OnSaveDocument(szFileName))
    {
		if (bReplace)
		{
	        m_lpImage->fUntitled = FALSE;                     
	        lstrcpy(m_lpImage->CurFile,szFileName);
			m_lpImage->FileType = idFileType;
			SetPathName((const char *)szFileName,TRUE);
			SetTitle(filename(szFileName));
		}
    }                
	m_bRemember = bSaveRemember;
    
    return (bStatus);
}

void CServerDoc::do_status(LPCSTR lpString)
{
	if ( !fAppActive )
		return;
	StatusForum.SetStatus(lpString);
}

void CServerDoc::set_bubble_hintstate (BOOL bActivate)
{
	if (bActivate)
	{
		// turn on hints, if enabled
		if( Hints.fBubbleHintsOn )
		{
		    HelpContext.BubbleHintsOn( YES );                    // reset state
		    HelpContext.SetHintWindows( NULL, NULL, HT_STATIC ); // turn on timer
		}
	}
	else
	{
		// turn off hints, if enabled 
		if( Hints.fBubbleHintsOn )
		{
		    HelpContext.SetHintState( "" );                     // flush current hint
		    HelpContext.SetHintWindows( NULL, NULL, HT_OFF );   // turn off timer
		    HelpContext.BubbleHintsOn( NO );                    // reset state
            HelpContext.display_bubble( FALSE );                // Remove the current hint popup window.
		}
		if( Hints.fStatusHintsOn )                              // flush hint from status bar
		    HintLine( 0 );
	}
}


/* ************************ Public Functions ***************************** */

CImage* Load(CArchive & ar, ITEMID FileType, BOOL bOle10 /* FALSE */)
{
	CImage* pImage = NULL;
    
	TRY
	{
		ar.Flush();
		CFile *pFile = ar.GetFile();
		ASSERT(pFile);
		
		if (!bOle10)
		{
			CReadImage read(pFile, FileType);
			pImage = read.ReadImage();
		}
		else // OLE10 embedding 
		{
			HANDLE hMem;
			LPTSTR lpData;
			LPFRAME lpFrame;
			DWORD dwLen;

			// Skip the DWORD length at the beginning of the OLE10Native Stream
			pFile->Read(&dwLen,sizeof(DWORD));

			if ( !(hMem = GlobalAlloc( GMEM_SHARE | GMEM_MOVEABLE, dwLen+1) ))
			{
			    Message( IDS_EMEMALLOC );
				ASSERT(FALSE);
				return NULL;
			}
			lpData = (LPTSTR)GlobalLock(hMem);
			ASSERT(lpData);
			if (lpData)
			{
				if (dwLen == pFile->ReadHuge((void FAR*)lpData, dwLen))
				{
					lpFrame = DIBToFrame( (LPTR)lpData, /*YES force to RGB??*/ NO);
					ASSERT(lpFrame);
					if (lpFrame)
					{
						pImage = CreateImage(NULL,lpFrame,NULL,NULL,IDN_PP,
							IDC_SAVECT,IMG_DOCUMENT,NULL);
						ASSERT(pImage);
					}
					GlobalUnlock( hMem );
					GlobalFree (hMem);
				}
			}
		}
	}
	END_TRY

	return (pImage);
}

BOOL StoreDIB( LPBITMAPINFOHEADER lpInfo, CFile * pFile)	
{
	DWORD lCount;
	HPTR  lpClipPtr;
	DWORD x;
	DWORD bpl;

	x = sizeof(BITMAPINFOHEADER);

	if (lpInfo->biBitCount == 24)
	{
    	bpl = lpInfo->biWidth * 3;
    }
	else
	if (lpInfo->biBitCount == 8)
    {
    	bpl = lpInfo->biWidth;
	    x += ( 256 * sizeof(RGBQUAD) );
    }
	else
	if (lpInfo->biBitCount == 4)
    {
	    bpl = (lpInfo->biWidth+1)/2;
	    x += ( 16 * sizeof(RGBQUAD) );
    }
	else
	if (lpInfo->biBitCount == 1)
    {
	    bpl = (lpInfo->biWidth+7)/8;
	    x += ( 2 * sizeof(RGBQUAD) );
    }

	bpl = (bpl + 3) & ~3;

	if (!lpInfo->biSizeImage)
    	lpInfo->biSizeImage = bpl * lpInfo->biHeight;

	lCount = x + lpInfo->biSizeImage;

	TRY
	{

		// Write Total Size
		pFile->Write((const void FAR*)&lCount,sizeof(DWORD));

		lpClipPtr = (HPTR)lpInfo;
		if (lCount > 0L)
			pFile->WriteHuge((const void FAR *)lpClipPtr,lCount);
		return(TRUE);
    }
	END_TRY

    return(FALSE);
}



void Store(CArchive & ar, ITEMID FileType, CImage* pImage,
	BOOL m_fReadFromOLE10 /* FALSE */ ,BOOL m_fConvert /* FALSE */)
{
    ASSERT(pImage);    

	TRY
	{
		if (m_fReadFromOLE10 && !m_fConvert) // Emulating an OLE1.0 PP Object
		{
			LPFRAME lpFrame = ImgGetBaseEditFrame(pImage);
			ASSERT(lpFrame);
			LPTR lpDIB = FrameToDIB(lpFrame,(LPRECT)NULL);
			if (lpDIB)
			{
				ar.Flush();
				CFile *pFile = ar.GetFile();
				ASSERT(pFile);
				BOOL bRet = StoreDIB((LPBITMAPINFOHEADER)lpDIB,pFile);

			    HGLOBAL hMem;
#ifdef WIN32
        	    hMem = GlobalHandle( lpDIB );
#else
            	{
	            DWORD dwValue = GlobalHandle( HIWORD(lpDIB) );
	            hMem = (HGLOBAL)LOWORD( dwValue );
	            }
#endif
    	        GlobalUnlock( hMem );
				GlobalFree (hMem);
			}
		}
		else
		{
			PPFFOPTIONS Options;

			ASSERT(FileType == IDN_PP);
			ar.Flush();
			CFile *pFile = ar.GetFile();
			ASSERT(pFile);
			CWriteImage write(pFile, FileType);
			Options.bPPFFPromptUser = FALSE;
			Options.PPFFCompress = FALSE;
			Options.bSaveMask = TRUE;
			Options.bSaveProxy = TRUE;
			Options.bSaveCmdList = FALSE;
			Options.bSaveLink = FALSE;
			Options.bWritePT = TRUE;  // Precision Transform CMS
			write.SetOptions((LPVOID)&Options);
			write.WriteImage(pImage);
		}
	}
	END_TRY
}


/* ***************************** Private Methods *************************** */

CInsertDialog::CInsertDialog(UINT nIDTemplate,CWnd *pParentWnd) : 
	CDialog(nIDTemplate,pParentWnd)
{
	m_ActionID = IDC_INSERTEXIST;
}

BOOL CInsertDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	CenterPopup(this->GetSafeHwnd());
	CheckRadioButton(IDC_INSERTEXIST,IDC_INSERTIMPORT,IDC_INSERTEXIST);
	return (TRUE);
}

void CInsertDialog::OnOK( )
{
	m_ActionID = GetCheckedRadioButton(IDC_INSERTEXIST,IDC_INSERTIMPORT);
	EndDialog(IDOK);
}

/* ***************************** Public Methods ***************************** */
                        
CServerDoc::CServerDoc () : COleServerDoc ()
{                 
    m_lpImage = NULL;
	m_bIPFrameActive = FALSE;
	m_bIPDeactivating = FALSE;

	m_sizeDoc = CSize(0, 0);

	// OLE 1.0 Indicator flags
	m_fReadFromOLE10 = FALSE; // until the LoadFromStorage is done we don't know
	m_fConvert = FALSE;		  // TRUE if convert OLE1.0 otherwise Emulate

    m_lpRootImageStorage = NULL;
	m_SummaryInfo.Init();
}

CServerDoc::~CServerDoc()
{
    if (m_lpImage!=NULL)
        DestroyImage(m_lpImage);    
    m_lpImage = NULL;
    if (m_lpRootImageStorage)
    {
    	ReleaseInterface((LPUNKNOWN*)&m_lpRootImageStorage);
        m_lpRootImageStorage = NULL;
    }
}        

void CServerDoc::SetImage(CImage *pImage)
{
    m_lpImage = pImage;
	m_FileType = m_lpImage->FileType;
	m_DataType = m_lpImage->DataType;
	// hook image back to server doc
	m_lpImage->pServerDoc = this;
	SetCmdList(m_lpImage->lpCmdList);
}

void CServerDoc::SetCmdList(LPCMDLIST lpCmdList)
{
	// stow command list pointer away in CServerDoc, this is
	// necessary because we at times have a command list
	// pointer but no image and we need some data stored
	// in the command list structure
	// see SetupCursor() in astral.cpp
	m_lpCmdList = lpCmdList;

	// link thread data back to document
	// we do this so thay we can tell if a thread is
	// associated with the active document or not
	// again, we use this for progress
	lpCmdList->ThreadData.pDocument = this;
}

void CServerDoc::AttachImage(CImage *pImage)
{
	RECT rUpdate;
	int dx, dy;

	if (m_lpImage)
		delete m_lpImage;
    pImage->pServerDoc = this;
	// unplug document from command list
	// to prevent other images from attaching
	pImage->lpCmdList->pDocument = NULL; 
	m_lpImage = pImage;

	if (m_lpRootImageStorage == NULL)
		InitCompoundFile();

	// document always has the correct file type
	m_lpImage->FileType = m_FileType;
	// image always has the correct data type
	m_DataType = m_lpImage->DataType;
	// document always has the correct filename (except for fastbits)
	if (m_lpImage->DocumentType != IMG_DOCPIECE)
		lstrcpy(m_lpImage->CurFile, GetPathName());
	if (!m_lpImage->fUntitled)
	{
		m_lpImage->fUntitled = !FileExists(m_lpImage->CurFile);
		if (m_lpImage->fUntitled)
			m_lpImage->fChanged = TRUE;
	}

	// get position of first view in list
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		// get first view
		CServerView *pView = (CServerView*)GetNextView(pos);
		if (pView)
			pView->SendMessage(WM_INITIALUPDATE);
	}
	m_lpImage->GetInfo(&dx, &dy);
	rUpdate.left = rUpdate.top = 0;
	rUpdate.right = dx-1;			    
	rUpdate.bottom = dy-1;
	CUpdateArea UpdateArea(AstralToWindowsRect(&rUpdate));
	UpdateAllViews(NULL,0L, (CObject*)&UpdateArea);
}

CServerView* CServerDoc::GetActiveView()
{
	CServerView* pView = NULL;

	// get position of first view in list
	POSITION pos = GetFirstViewPosition();
	if (pos)
	{
		// get first view
		pView = (CServerView*)GetNextView(pos);
		if (pView)
		{
			// get active view from views parent frame
			CFrameWnd *pFrame = (CFrameWnd*)pView->GetParent();
			pView = (CServerView *)pFrame->GetActiveView();
		}
	}
	return(pView);
}

void CServerDoc::UpdateInPlaceDisplay(CWnd *pParentWnd)
{
	ASSERT(m_lpImage);
	CServerView *pView = (CServerView*)GetActiveView();
	ASSERT(pView);
	if (!pView)
		return;

	int w, h, dx, dy;
	int iWidth, iHeight, iImageWidth, iImageHeight;
	LFIXED DispRate;
	RECT DispRect;
	BOOL bRulersOn = pView->m_lpDisplay->HasRulers;

	ImgInitDisplay(m_lpImage);
	ImgGetInfo(m_lpImage, &dx, &dy, NULL, NULL);
	
	pView->m_lpDisplay->FileRate = TINYRATE;
	pView->m_lpDisplay->ViewPercentage = 0;
	SetRect( &pView->m_lpDisplay->FileRect, 0,0,dx-1, dy-1);
	AstralSetRectEmpty( &pView->m_lpDisplay->UpdateRect );
	AstralSetRectEmpty( &pView->m_lpDisplay->PaintRect );
	                     
	pView->m_lpDisplay->iScrollMin[0]    = pView->m_lpDisplay->iScrollMin[1] = 0;
	pView->m_lpDisplay->iScrollMax[0]    = pView->m_lpDisplay->iScrollMax[1] = 100;
	pView->m_lpDisplay->iScrollOutMin[0] = pView->m_lpDisplay->iScrollOutMin[1] = 0;
	pView->m_lpDisplay->iScrollOutMax[0] = pView->m_lpDisplay->iScrollOutMax[1] = 100;
	pView->m_lpDisplay->ScrollRate[0]    = pView->m_lpDisplay->ScrollRate[1] = FUNITY;

	// -----------------------------------------
	// Kind of like ComputeDispRect 
	// -----------------------------------------
	if (bRulersOn)
	{
		pView->OnRulerShow(); //turn off before scaling
		pView->UpdateWindow();
	}
	// get max size of the parent
	pParentWnd->GetClientRect( &DispRect );
	w = RectWidth(&DispRect);
	h = RectHeight(&DispRect);
    iWidth = w;
    iHeight = h;
    iImageWidth = m_lpImage->npix;
    iImageHeight = m_lpImage->nlin;
    DispRate = ScaleToFit(&iWidth, &iHeight, iImageWidth, iImageHeight,
		TRUE);
	pView->m_lpDisplay->DispRect.top = DispRect.top;
	pView->m_lpDisplay->DispRect.left = DispRect.left;
	pView->m_lpDisplay->DispRect.bottom = 
		pView->m_lpDisplay->DispRect.top + iHeight - 1;
	pView->m_lpDisplay->DispRect.right = 
		pView->m_lpDisplay->DispRect.left + iWidth - 1;
    if (!pView->m_lpDisplay->ViewPercentage)
        pView->m_lpDisplay->ViewPercentage = FMUL(100,DispRate);
	if (bRulersOn)
		pView->OnRulerShow(); //turn back on
}

void CServerDoc::InPlaceInitialUpdate(CWnd *pParentWnd)
// This method is called by CInPlaceFrame class when the inplace session is 
// started up.  Its purpose is similar to CServerView's OnInitialUpdate, however
// a bit more tailored to the inplace frame's needs.
{
	CServerView *pView = (CServerView*)GetActiveView();
	ASSERT(pView);
	if (!pView)
		return;
	
	if (!pView->m_lpDisplay)
	{
		if (!(pView->m_lpDisplay = (LPDISPLAY)Alloc( (long)sizeof(DISPLAY) )) )
		{
		    ASSERT(pView->m_lpDisplay);
		    return;
		}
	}
	clr ((LPTR)pView->m_lpDisplay, sizeof(DISPLAY));

	UpdateInPlaceDisplay(pParentWnd);
}

void CServerDoc::UpdateImageSize()
{
	CServerView *pView = GetActiveView();
    HWND hActiveMDIWnd = GetActiveDoc(); // Active MDIChild
	LPFRAME lpFrame = ImgGetBaseEditFrame(m_lpImage);

	if (pView && lpFrame)
	{
	    HWND hMDIWnd = pView->GetParentFrame()->GetSafeHwnd();
	
	    if (hMDIWnd == hActiveMDIWnd)
	        ZoomMainViewChanged(pView->GetSafeHwnd());
	    if (IsIconic(hMDIWnd))
	    {
	        // Compute new DispRect and FileRect
	        ComputeDispRect(pView->GetSafeHwnd());
	        ComputeFileRect(pView->GetSafeHwnd(), -1, -1, 
	            pView->m_lpDisplay->FileRate );
	        pView->InvalidateRect(NULL, TRUE);
	    }
		else
		if (IsInPlaceActive())
		{
			m_lpImage->npix = FrameXSize(lpFrame);
			m_lpImage->nlin = FrameYSize(lpFrame);
			CRect rPosRect;
			GetItemPosition((LPRECT)&rPosRect);
			rPosRect.right = rPosRect.left + m_lpImage->npix;
			rPosRect.bottom = rPosRect.top + m_lpImage->nlin;
			RequestPositionChange((LPCRECT)&rPosRect);
		}
	    else
		{
			int cx, cy, left, top;

			m_lpImage->npix = FrameXSize(lpFrame);
			m_lpImage->nlin = FrameYSize(lpFrame);
			pView->m_lpDisplay->FileRate = TINYRATE;
			pView->m_lpDisplay->ViewPercentage = 0;
			SetRect( &pView->m_lpDisplay->FileRect, 0,0,FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
			AstralSetRectEmpty( &pView->m_lpDisplay->UpdateRect );
			AstralSetRectEmpty( &pView->m_lpDisplay->PaintRect );
			SaveLastView(pView->GetSafeHwnd());
			GetWinXY(hMDIWnd, &left, &top);
			// always display from upper-left
			cx = cy = 0;
			FullViewWindow(pView->GetSafeHwnd(), left, top,
			            cx, cy, YES);
			if ( Tool.hRibbon && (pView->GetSafeHwnd() == GetActiveDoc()))
			    POST_WM_DOCCHANGED( Tool.hRibbon );
		}
		PictPubApp.DocumentChanged(m_lpImage);
	}
}

void CServerDoc::CloseDocumentFile() 
// All of this is done because we do not use MFC's Reserved ID_FILE_CLOSE
// and because of the command processign we need to do due to the NT multi-
// threading.  This is a way to call CDoc's OnFileClose which is otherwise
// protected.  OnFileClose will Prompt before closing a modified doc.
{
    CDocument::OnFileClose(); 
}
                          
void CServerDoc::DeleteContents ()
{   
    if (m_lpImage)
    {
        if (m_lpImage->lpFastbitsPreview)               
        {
            LPFRAME lpMyFrame = m_lpImage->lpFastbitsPreview;
            m_lpImage->lpFastbitsPreview = NULL;
    
            if (!PreviewBeingUsed(lpMyFrame))
                FrameClose(lpMyFrame);
        }
        DestroyImage(m_lpImage);    
        m_lpImage = NULL;            
    }     

	COleServerDoc::DeleteContents ();
}

void CServerDoc::FileUpdate()
{
	POSITION pos = GetStartPosition();
    COleServerItem* pItem;
    while ((pItem = GetNextServerItem(pos)) != NULL)
		pItem->NotifyChanged(DVASPECT_CONTENT);

    // Always force an update to the client            
    BOOL bWasModified = IsModified();
    SetModifiedFlag(TRUE);
    COleServerDoc::OnFileUpdate();
    SetModifiedFlag(bWasModified);
}
                 
void CServerDoc::FileSaveCopyAs()
{
    //COleServerDoc::OnFileSaveCopyAs();
    ASSERT_VALID(this);
	ASSERT(GetImage());
    ASSERT(m_lpClientSite != NULL);

    LPSTORAGE lpOrigStg = m_lpRootStg;
    m_lpRootStg = NULL; // ignore embedded storage for now

    TRY
    {
        // call do_save to perform Save Copy As...
        do_save(NULL,FALSE,FALSE);
    }
    END_TRY

    m_lpRootStg = lpOrigStg;
}

void CServerDoc::OnCloseDocument()
{
    extern BOOL OwnActiveModalDlg(HWND hOwnerView);
    CServerView *pView = GetActiveView();
    if (pView)
    {    
        if (OwnActiveModalDlg(pView->GetSafeHwnd()))
        {
            // This is case we are really looking for:
            // "Are we OpenEditing an embedded object with a MODAL Dialog
            // up when the object is being deleted back in the container app?"
            if (IsEmbedded() && !IsInPlaceActive())
                Message (IDS_E_CLOSEDOCWITHMODALUP);
            return;
        }
    }

    CDataSource *pDataSource = (CDataSource*)COleDataSource::GetClipboardOwner();
    if (pDataSource && pDataSource->IsOwner(this))
    {
        // ----------------------------------------------------------------------
        // Force Render CF_EMBEDDEDOBJECT NOW!
        //
        // CF_OBJECTDESCIPTOR, CF_LINKSOURCE and CF_LINKSOURCEDESCRIPTOR
        // Not needed -- they are cached up front
        // ----------------------------------------------------------------------
        COleDataObject dataObject;
        if (dataObject.AttachClipboard() &&
            dataObject.IsDataAvailable(_oleData.cfEmbedSource))
        {
            STGMEDIUM stgMedium;
            FORMATETC FormatEtc;
            if (pDataSource->GetDataItem())
            {
                if (FALSE) // Render Embedded Only option
                {
                    SETDefFormatEtc(FormatEtc,_oleData.cfEmbedSource,TYMED_ISTORAGE);
                    ((COleServerItem* const)pDataSource->GetDataItem())->
                        GetEmbedSourceData(&stgMedium);
                    if (!pDataSource->ExchangeCacheStorage(FormatEtc,&stgMedium))
                        pDataSource->CacheData(_oleData.cfEmbedSource,&stgMedium);
                }
                else
                {
                    CImage* pImage = pDataSource->ExtractImage();
                    ASSERT (pImage);
                    if (pImage)
                    {
                        ::OpenClipboard(PictPubApp.Get_hWndAstral(TRUE));
                        ::EmptyClipboard();
                        ::CloseClipboard();
                        CDataItem *pDataItem = new CDataItem (this,pImage);
                        if (pDataItem)
                        {
                            pDataItem->IncludeEmbedded(FALSE); //no Embedded
                            pDataItem->CopyToClipboard(FALSE); //no Linked
                        }
                    }
                }
            }
        }
    }   
	OnClosePPDocumentEx(FALSE);
}

void CServerDoc::OnClosePPDocumentEx(BOOL fRegenrating)
{
	if (m_lpImage == NULL)
	{
	    COleServerDoc::OnCloseDocument();  // kills all the views
		return;
	}

	BOOL fFastbits = (!fRegenrating && (m_lpImage->DocumentType == IMG_DOCPIECE));
	LPFRAME lpFastbitsPreview = m_lpImage->lpFastbitsPreview;
	ITEMID idFileType = m_lpImage->FileType;
	RECT rFastbits = m_lpImage->rFastbitsGrid;
	FNAME szFileName;
	BOOL bClosingMdiFrame = FALSE;

	// determine if we are closing down... if we are then dont do fastbits
	CWnd *pMainWnd = ::AfxGetMainWnd();
	if (pMainWnd && pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrame)))
		bClosingMdiFrame = ((CMDIFrame*)pMainWnd)->m_fFrameClosing;


	lstrcpy(szFileName, m_lpImage->CurFile);
	// keep preview from being freed
	if (fFastbits && !bClosingMdiFrame)
		m_lpImage->lpFastbitsPreview = NULL;
		
	CMD_PARMS parms;
	PostCommand(m_lpImage->lpCmdList, IDS_CMD_CLOSE, &parms);
    COleServerDoc::OnCloseDocument();  // kills all the views

	// fastbits should open another fastbits 
	if (fFastbits && !bClosingMdiFrame)
	{
		DoFastbitsLoad(idFileType, szFileName, NULL, lpFastbitsPreview, &rFastbits, YES);
	}
}

void CServerDoc::LoadFromStorage()
{
    ASSERT(m_lpRootStg != NULL);

	m_fReadFromOLE10 = FALSE; // assume NOT OLE10 initially

    // open Contents stream
    COleStreamFile file;
    CFileException fe;
    if (!file.OpenStream(m_lpRootStg, szContents,
        CFile::modeReadWrite|CFile::shareExclusive, &fe))
	{
		// check to see if it is from OLE10 version
	    if (!file.OpenStream(m_lpRootStg, szOLE10Contents,
    	    CFile::modeReadWrite|CFile::shareExclusive, &fe))
	    {
    	    AfxThrowFileException(fe.m_cause, fe.m_lOsError);
	    }
		else
		{
			// --------------------------------------------------------------
			// it's an OLE1.0!! and should we convert it or merely emulated it?
			// --------------------------------------------------------------
			m_fReadFromOLE10 = TRUE;
			m_fConvert = (NOERROR==GetConvertStg(m_lpRootStg));

			// --------------------------------------------------------------
			// This is the type of storage that we're really messing with 
			// in TreatAs
			// --------------------------------------------------------------
			if (!m_fConvert)
				ReadClassStg(m_lpRootStg,&m_TreatAsClsid);
		}
	}

    // load it with CArchive (loads from Contents stream)
    CArchive loadArchive(&file, CArchive::load | CArchive::bNoFlushOnDelete);
    loadArchive.m_pDocument = this;
    loadArchive.m_bForceFlat = FALSE;

    TRY
    {
        Serialize(loadArchive);     // load main contents
        loadArchive.Close();
        file.Close();
    }
    CATCH_ALL(e)
    {
        file.Abort();   // will not throw an exception
        DeleteContents();   // removed failed contents
        loadArchive.CArchive::~CArchive();

        THROW_LAST();
    }
    END_CATCH_ALL
}

void CServerDoc::SaveToStorage(CObject* pObject)
{
	ASSERT_VALID(this);
	if (pObject != NULL)
		ASSERT_VALID(pObject);

	ASSERT(m_lpRootStg != NULL);

    // create Contents stream
    COleStreamFile file;
    CFileException fe;

	// --------------------------------------------------------------
	// if we are Emulating an OLE1.0 object then write out to as 'szOLE10Contents'
	// --------------------------------------------------------------
	if (!m_fConvert && m_fReadFromOLE10 )
	{
		WriteClassStg(m_lpRootStg, m_TreatAsClsid);

	    if (!file.CreateStream(m_lpRootStg, szOLE10Contents,
	        CFile::modeWrite|CFile::shareExclusive|CFile::modeCreate, &fe))
	    {
	        AfxThrowFileException(fe.m_cause, fe.m_lOsError);
	    }
	}
	else
	{
		if (m_pFactory != NULL)
			WriteClassStg(m_lpRootStg, m_pFactory->GetClassID());

	    if (!file.CreateStream(m_lpRootStg, szContents,
	        CFile::modeWrite|CFile::shareExclusive|CFile::modeCreate, &fe))
	    {
	        AfxThrowFileException(fe.m_cause, fe.m_lOsError);
	    }
	}

    // save to Contents stream
    CArchive saveArchive(&file, CArchive::store | CArchive::bNoFlushOnDelete);
    saveArchive.m_pDocument = this;
    saveArchive.m_bForceFlat = FALSE;

    TRY
    {
        // save the contents
        if (pObject != NULL)
            pObject->Serialize(saveArchive);
        else
            Serialize(saveArchive);
        saveArchive.Close();
        file.Close();

		// Do the following AFTER We know that the Save is successful...
		// Any time you write a different structure into the storage you need 
		// to clean up any elements that are now unused. In the Conversion case,
		// the 'szOLE10Contents' stream will no longer be used because the storage
		// now contains and OLE2.0 Object.
		if (m_fReadFromOLE10 && m_fConvert)
		{

WriteFmtUserTypeStg(m_lpRootStg, Control.cfImage, 
	(LPTSTR)"Picture Publisher 5.0 Image");

			SetConvertStg(m_lpRootStg,FALSE);
			m_fConvert = FALSE;
		
			// clean up the stream
			m_lpRootStg->DestroyElement(szOLE10Contents);
		}

        // commit the root storage
        m_lpRootStg->Commit(STGC_ONLYIFCURRENT);
    }
    CATCH_ALL(e)
    {
        file.Abort();   // will not throw an exception
        CommitItems(FALSE); // abort save in progress
        saveArchive.CArchive::~CArchive();

        THROW_LAST();
    }
    END_CATCH_ALL
}

void CServerDoc::OnDeactivateUI(BOOL bUndoable)
{
//	LPFRAME lpFrame = ImgGetBaseEditFrame(m_lpImage);
//
//	if (lpFrame) 
//	{
//		int x = FrameXSize(lpFrame);
//		int y = FrameYSize(lpFrame);
//		CRect rPosRect;
//		GetItemPosition((LPRECT)&rPosRect);
//CServerView *pView = GetActiveView();
//if (pView)
//{
//	/// check display struct here!	
//}
//		if (x != rPosRect.Width() || y != rPosRect.Height())
//		{
//			if (TRUE) //option #1 - snap to actual frame size
//			{
//				rPosRect.right = rPosRect.left + x - 1;
//				rPosRect.bottom = rPosRect.top + y - 1;
//				RequestPositionChange((LPCRECT)&rPosRect);
//			}
//			else // option #2 - Update the image ie) frame size -> new ItemRect
//			{
//			}
//		}
//	}	

	// Flag the Document as Closing.  This added measure is needed over the
	// usual testing of the ::IsInPlaceActive because there is a small window
	// between the time when we DON'T have a connect with the client and the
	// documnet still thinks we are Inplace active
	m_bIPDeactivating = TRUE;

	CServerView *pView = GetActiveView();
	CInPlaceFrame* pIPFrame = NULL;
	ASSERT(pView);
	if (pView)
	{
		pIPFrame = (CInPlaceFrame*)pView->GetParentFrame();        
		ASSERT(pIPFrame);
		if (pIPFrame)
		{
			if (pIPFrame->NeedCacheUpdated())
			{
				pIPFrame->SetUpdateCacheFlag(FALSE);
    	        UpdateAllItems(NULL);                    
			}
		}
	}

	//----------------------------------------------------
	// see comment in OnFrameWindowActivate for more information about the
	// m_bIPFrameActivate Flag.
	//----------------------------------------------------
	SetToolbar(GetActiveView(),!m_bIPFrameActive);
  	SetToolboxes(pView,	!m_bIPFrameActive);
	set_bubble_hintstate(FALSE);
 	if (pIPFrame)
	{
		// turn off the palette if it is up
		if (pIPFrame->ShowHidePalette(-1))
			pIPFrame->ShowHidePalette(FALSE);
		// Remove the Clipboard Browser if up
		pIPFrame->DestroyPasteFromFileDlg ();
	}
	// Unhook Zoom if up (ie. remove)
	UnhookZoom(GetActiveView()->GetSafeHwnd());

	//----------------------------------------------------
	// Delete or Render all promised clipboard formats if 
	// we own DataSource 
	//----------------------------------------------------
	CDataSource *pDataSource = (CDataSource *)COleDataSource::GetClipboardOwner();
	if (pDataSource && pDataSource->IsOwner(this))
	{
		char szBuffer[512];
		if (hInstStringLib && LoadString( hInstStringLib, IDS_OKTORENDERCLIPBOARD, 
			szBuffer, 512))
		{
			if (IDYES == ::MessageBox( PictPubApp.Get_hWndAstral(TRUE), 
				szBuffer, szAppName,MB_ICONQUESTION | MB_YESNO))
				COleDataSource::FlushClipboard();
			else
			{
				pDataSource->Empty();
				if (::OpenClipboard(PictPubApp.Get_hWndAstral(TRUE)))
				{
					::EmptyClipboard();
					::CloseClipboard();
				}
			}
		}
	}

	//----------------------------------------------------
	// Call Up to complete Deactivation
	//----------------------------------------------------

	COleServerDoc::OnDeactivateUI(bUndoable);
}

void CServerDoc::OnDocWindowActivate(BOOL bActivate)
{
	COleServerDoc::OnDocWindowActivate(bActivate);

	// We are toggling between Documents in a MDI container.
	m_bIPDeactivating = FALSE;
	SetToolbar(GetActiveView(),bActivate);
	SetToolboxes(GetActiveView(),bActivate);
	set_bubble_hintstate(bActivate);
}

void CServerDoc::OnFrameWindowActivate(BOOL bActivate)
// Called by foundation in an insitu case when the InPLace object's container
// is activated ot deactivated.  This may be as a result of the applicatoin being
// closed or because another app was merely activated.
{
	COleServerDoc::OnFrameWindowActivate(bActivate);

	PictPubApp.SetInPlaceActiveState(bActivate);

	// This flag is set here to allow OnDeactivateUI to determine how it
	// should behave. (ie. DeactivateUI because we are lossing app level 
	// activation or are we merely lossing Insitu activation because the user
	// clicked outside the object).
	m_bIPFrameActive = bActivate; 
	m_bIPDeactivating = FALSE;
// 8/16/84
//	SetToolbar(GetActiveView(),bActivate);
SetToolbar(GetActiveView(),TRUE);
	set_bubble_hintstate(bActivate);

	// set the initial position for the Toolbox here. ie) this is done to 
	// allow us to position the main toolbox below the ribbon area when
	// inplace active
	if (bActivate)
	{
		CServerView *pActiveView = GetActiveView();
		ASSERT(pActiveView != NULL);
		CInPlaceFrame* pIPFrame = (CInPlaceFrame*)pActiveView->GetParentFrame();        
		if (pIPFrame && pIPFrame->NeedSetInitToolbarPos())
			pIPFrame->InstallToolBox(TRUE);
	}
}

COleServerItem* CServerDoc::OnGetEmbeddedItem( )
{
    ASSERT(m_lpImage);
	CServerItem *pServerItem = NULL;
	if (m_lpImage)
	{
    	pServerItem = new CServerItem(this, m_lpImage);
	    ASSERT(pServerItem);
	}
    return pServerItem;
}

COleServerItem* CServerDoc::OnGetLinkedItem( LPCSTR lpszItemName )
{
	ASSERT(lpszItemName);
	CServerItem *pServerItem = NULL;

	// if we have ole items the see if we can locate one by ItemName
    POSITION pos = GetStartPosition(); 
	if (pos)
	{
		COleServerItem *pOleServerItem = 
			COleServerDoc::OnGetLinkedItem(lpszItemName);

		// Ensure that the Linked item, if found, IS NOT A DATAITEM Clipboard
		// instance.  If so force then to create a new one
		if (pOleServerItem) 
		{
			if (!pOleServerItem->IsKindOf(RUNTIME_CLASS(CDataItem)))
				pServerItem = (CServerItem*)pOleServerItem;
		}
	}
	if (!pServerItem)
	{
	    ASSERT(m_lpImage);
	
		CRect rRect;
	
		if ( sscanf( lpszItemName, "(%d,%d)-(%d,%d)", &rRect.left, &rRect.top,
			&rRect.right, &rRect.bottom ) == 4 )
		{
			// create and add masked area 
			{
				BOOL fNewMask = NO;
				RECT rUpdate;
				LPMASK lpMask;
				CPoly Poly;
				
				RemoveMaskEx(m_lpImage,0);
				MakeRectPoly(&Poly, &rRect, YES);
				lpMask = ImgGetMaskEx(m_lpImage, FALSE, TRUE, &fNewMask, &rUpdate);
				MaskAddPoly(lpMask, &Poly, SHAPE_ADD, NO /*bAntialias*/);
			}
	
		    pServerItem = new CServerItem(this, m_lpImage,(LPRECT)&rRect);
		}
	}
	return (pServerItem);
}

BOOL CServerDoc::OnNewDocument()
{       
	BOOL bSuccess = FALSE;

    if (!COleServerDoc::OnNewDocument())
        return bSuccess;

	DeleteContents(); // make sure we don't already have an CImage*

#ifndef _DESIGNERFIX
	if (!DesignerEditorInProgress ())
	{
#endif //_DESIGNERFIX
#if _MFC_VER == 0x0250
		CWnd *pParentWnd = ::AfxGetMainWnd();
		CServerView *pActiveView = GetActiveView();
		if (pActiveView && pActiveView->GetDocument()->IsInPlaceActive())
		{
			CInPlaceFrame* pIPFrame = (CInPlaceFrame*)pActiveView->GetParentFrame();        
			if (pIPFrame)
			{
				HWND hWnd = pIPFrame->GetClientFrameWnd();
				if (hWnd)
					pParentWnd = CWnd::FromHandle(hWnd);
			}
		}
		
		CInsertDialog Dlg (IDD_INSERTOLE, pParentWnd);
		if (IDOK == Dlg.DoModal())
		{
			switch (Dlg.m_ActionID)
			{
			case IDC_INSERTEXIST:
				if (m_lpImage = open_image_as_new(FALSE))
		        	bSuccess = TRUE;        
				break;
			case IDC_INSERTIMPORT:
				if (m_lpImage = open_image_as_new(TRUE))
		        	bSuccess = TRUE;        
				break;
			case IDC_INSERTNEW:
			{
				CWnd *pParent = CWnd::GetActiveWindow();
				if (!pParent)
					pParent = AfxGetMainWnd();
			    /* Bring up the modal 'New' box */
			    if (AstralDlg( NO, ::AfxGetInstanceHandle(), 
					/*::AfxGetMainWnd()*/pParent->GetSafeHwnd(),
					IDD_NEW, DlgNewProc ))
		    	{
			        NEW_PARMS parms;
			    
		        	parms.Type = (FRMDATATYPE)Control.NewDepth;
		        	parms.Width = Control.NewWidth;
		        	parms.Height = Control.NewHeight;
		        	parms.Resolution = Control.NewResolution;
		        	parms.lpCmdList = NULL;
			
		        	if (m_lpImage = create_new_image(parms))
				        bSuccess = TRUE;        
		    	}
				break;
			}
			default:
				break;
			};
		}
#else  // version 3.0 of MFC
	NEW_PARMS parms;
	
	parms.Type = FDT_RGBCOLOR;
	parms.Width = FGET(4,1);
	parms.Height = FGET(3,1);
	parms.Resolution = 72;
	parms.lpCmdList = NULL;
		
	if (m_lpImage = create_new_image(parms))
		bSuccess = TRUE;        
#endif // version 3.0 of MFC
#ifndef _DESIGNERFIX
	}
	// Designer is launching us as a " prefered editor. Create
	// temporary dummy 1by1 grayscale image.  They will be setting a
	// new image into us soon...
	else 
	{
		LPFRAME lpFrame = FrameOpen(FDT_GRAYSCALE, 2, 2, 72);
		if (lpFrame)
		{
			m_lpImage = CreateImage(NULL,lpFrame,NULL,NULL,
				IDN_PP,IDC_SAVECT,IMG_DOCUMENT,NULL);
			if (!m_lpImage)
				FrameClose(lpFrame);
			else
				bSuccess = TRUE;
		}
	}
#endif //_DESIGNERFIX

	if (bSuccess)
	{
	    CRect rBounds;
	    m_lpImage->pServerDoc = this;
		ImgGetImageRect(m_lpImage, &rBounds);
		m_sizeDoc = CSize(rBounds.Width(),rBounds.Height());
	}

    return (bSuccess);
}

BOOL CServerDoc::OnOpenDocument(const char* pszPathName )                    
{
    BOOL bResult = TRUE;
    
    SetModifiedFlag(FALSE);  //Startoff unmodified
    
	if (pszPathName == NULL)  // Embedded -- This will use the Serialize method      
    	return (COleServerDoc::OnOpenDocument(pszPathName));
	else  // Linked -- This will use the regular open file calls on the linked document                             
	{
		// read in the file by name   
		CReadImage read((LPTSTR)pszPathName);
		m_lpImage = read.ReadImage();     	
		ASSERT(m_lpImage); 
		
		Revoke();
#ifndef WIN32
		RegisterIfServerAttached(pszPathName);
#else
		RegisterIfServerAttached(pszPathName, FALSE);
#endif
		
    	AfxOleSetUserCtrl(TRUE);			
	}			

    if (m_lpImage)
	{
	    CRect rBounds;
	    m_lpImage->pServerDoc = this;
		ImgGetImageRect(m_lpImage, &rBounds);
		m_sizeDoc = CSize(rBounds.Width(),rBounds.Height());
	}

    return (bResult);
}    

BOOL CServerDoc::OnSavedDocument()
{
    ASSERT_VALID(this);
    ASSERT(m_lpImage);

	if (!m_lpImage)
		return(FALSE);

	// back to unmodified                            
    SetModifiedFlag(FALSE);
	m_lpImage->SetChanged(NO);

	// dont use document path for fastbits
	if (m_lpImage->DocumentType != IMG_DOCPIECE)
	{
	    m_lpImage->fUntitled = FALSE;                     
		m_FileType = m_lpImage->FileType;
		SetPathName(m_lpImage->CurFile);
		SetTitle(filename(m_lpImage->CurFile));
	}

    return (TRUE);  
}

BOOL CServerDoc::OnSaveDocument(const char* pszPathName)
{
    ASSERT_VALID(this);
    ASSERT(pszPathName == NULL || AfxIsValidString(pszPathName));
    ASSERT(m_lpImage);

	if (!m_lpImage)
		return(FALSE);

    BOOL bResult = TRUE;    

	UpdateAllItems(NULL);

    if (!IsEmbedded() && (pszPathName != NULL))
    {
	    bResult = AstralImageSave(	m_lpImage,
									m_FileType,
	            					m_lpImage->DataType,
									(LPTSTR)pszPathName );

		if (m_bRemember)
		{
			// update the moniker since the name has changed
			Revoke();
#ifndef WIN32
		   RegisterIfServerAttached(pszPathName);
#else
		   RegisterIfServerAttached(pszPathName, FALSE);
#endif
		}

	    if (pszPathName != NULL && m_bRemember)
	    {
	        if (GetPathName() == pszPathName)
	        {
	            // saved to same file
	            NotifySaved();
	        }
	        else
	        {
	            // saved to a different file
	            NotifyRename(pszPathName);
	        }
	    }
	}
	else
        bResult = COleServerDoc::OnSaveDocument(pszPathName);            

    if (bResult)        
    {
        SetModifiedFlag(FALSE);     // back to unmodified                            
		SetImgChanged(m_lpImage, NO);

		// dont use document path for fastbits 
		if (pszPathName)
		    lstrcpy(m_lpImage->CurFile,pszPathName);
		else
			m_lpImage->CurFile[0] = '\0';
    }       

    return (bResult);  
}

void CServerDoc::OnSetItemRects(LPCRECT lpPosRect, LPCRECT lpClipRect)
{
	// call base class to change the size of the window
	COleServerDoc::OnSetItemRects(lpPosRect,lpClipRect);

	// save our zoom factor 
	GetZoomFactor(&m_ZoomNum,&m_ZoomDenom);

	ASSERT(GetActiveView());
	UpdateInPlaceDisplay(GetActiveView());
}


void CServerDoc::ProcessStatus(CServerView *pActiveView,LPCSTR lpHintStr)
{
	if (!lpHintStr)
		return;

	if( Hints.fStatusHintsOn)
	{		
		// funky way to allow for static method  access
		if (pActiveView && pActiveView->GetDocument()->IsInPlaceActive())
		{
			if (!pActiveView->GetDocument() ||
				pActiveView->GetDocument()->m_bIPDeactivating)
				return;
			
			CInPlaceFrame* pIPFrame = (CInPlaceFrame*)pActiveView->GetParentFrame();        
			if (pIPFrame)
				pIPFrame->SetHintString(lpHintStr);
		}
		else
			do_status (lpHintStr);
	}
}

BOOL CServerDoc::SaveModified()
{   
	BOOL fFastbits;

    if (m_lpClientSite != NULL)
    {
        if (m_pInPlaceFrame == NULL)
        {
            UpdateModifiedFlag();   // check for modified items
            OnUpdateDocument();
        }
        return TRUE;
    }
    
    // determine if necessary to discard changes
    if (::InSendMessage())
    {
        POSITION pos = GetStartPosition();
        COleClientItem* pItem;
        while ((pItem = GetNextClientItem(pos)) != NULL)
        {
            ASSERT(pItem->m_lpObject != NULL);
            SCODE sc = GetScode(pItem->m_lpObject->IsUpToDate());
            if (sc != OLE_E_NOTRUNNING && FAILED(sc))
            {
                // inside inter-app SendMessage limits the user's choices
                CString name = m_strPathName;
                if (name.IsEmpty())
                    VERIFY(name.LoadString(AFX_IDS_UNTITLED));

                CString prompt;
                AfxFormatString1(prompt, AFX_IDP_ASK_TO_DISCARD, name);
                return AfxMessageBox(prompt, MB_OKCANCEL|MB_DEFBUTTON2,
                    AFX_IDP_ASK_TO_DISCARD) == IDOK;
            }
        }
    }

    // sometimes items change without a notification, so we have to
    //  update the document's modified flag before calling
    //  CDocument::SaveModified.
    UpdateModifiedFlag();

    if  (!IsModified())
        return TRUE;

    {
		
        CString name;
		fFastbits = m_lpImage->DocumentType == IMG_DOCPIECE;
		
		// fastbits are untitled but the do have a file.
		if (!fFastbits)
		{
        	name = m_strPathName;
        	if (name.IsEmpty())
        	{
        	    ASSERT(FALSE);
        	    VERIFY(name.LoadString(AFX_IDS_UNTITLED));
        	}
		}
		else
		{
			// for fastbits use fastbits file
			name = m_lpImage->CurFile;
		}
        CString prompt;
        AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
        switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
        {
        case IDCANCEL:
            return FALSE;       // don't continue
    
        case IDYES:
			// If so, either Save or Update, as appropriate
			if (fFastbits && (_access(m_lpImage->CurFile, 6) == 0))
			{
			    if (!do_save(m_lpImage->CurFile))
					return FALSE;
			}
			else if ((_access(m_strPathName, 6) != 0) || m_lpImage->fUntitled)
			{
			    // we do not have read-write access or the file does not (now) exist
			    if (!do_save(NULL))
					return FALSE;
			}
			else
			{
			    if (!do_save(GetPathName()))
					return FALSE;
			}
            break;
        
        case IDNO:
            // If not saving changes, revert the document
            break;
    
        default:
            ASSERT(FALSE);
            break;
        }
    }

    return TRUE;    // keep going
}

void CServerDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	FNAME szFileName;

	COleServerDoc::SetPathName(lpszPathName, bAddToMRU);
	lstrcpy(szFileName, lpszPathName);
	m_FileType = GetPPFileType(szFileName);
}

void CServerDoc::SetTitle(const char* pszTitle)
// lower case all titles 
{
    if (Control.WinVer == WV_WINDOWS95)
	    stripext( (char *)pszTitle );

    m_strTitle = pszTitle;
    ::AnsiLower((LPTSTR)m_strTitle.GetBuffer(m_strTitle.GetLength())); 
		// always lower case 
    UpdateFrameCounts();        // will cause name change in views
}

void CServerDoc::SetToolbar(CServerView *pActiveView,BOOL bShow,int idRibbon,
	DLGPROC lpRibbonProc)
{
	UINT id = (idRibbon != -1) ? idRibbon : 
		(Tool.bAltTool ? Tool.Alt.idRibbon : Tool.Prim.idRibbon);
	DLGPROC lpProc = (lpRibbonProc != NULL) ? lpRibbonProc : 
		(Tool.bAltTool ? Tool.Alt.lpRibbonProc : Tool.Prim.lpRibbonProc);
	BOOL bInPlaceActive = FALSE;

	// funky way to allow for static method  access
	if (pActiveView)
	{
		bInPlaceActive = pActiveView->GetDocument()->IsInPlaceActive();
		if (bInPlaceActive)
		{
			if (!pActiveView->GetDocument() ||
				pActiveView->GetDocument()->m_bIPDeactivating)
				return;

			CInPlaceFrame* pIPFrame = (CInPlaceFrame*)pActiveView->GetParentFrame();        
			if (pIPFrame)
				pIPFrame->InstallRibbon(id,lpProc,bShow);
		}
	}

	if ( (!bInPlaceActive) && id)
	{
//		if (id != Tool.idRibbon) // 08/15/94 Jake and Joe fix for global ribbon state
		if (!AstralDlgGet( id ))
		{
			HWND hOldDlg;
	
			// create new ribbon
			if ( lpProc )
			{
				if (!AstralDlg(DLG_MODELESS_TOPMOST, PictPubApp.GetResourceHandle(), 
					PictPubApp.Get_hWndAstral(FALSE), id, lpProc))
					return; // Unable to create Ribbon
			}

		    // close the old ribbon
			if ( hOldDlg = AstralDlgGet( Tool.idRibbon ) )
				AstralDlgEnd(hOldDlg, TRUE);
		}
		((CPPToolList*)OCToolList)->GetRibbon()->AttachRibbon(AstralDlgGet(id));
		Tool.idRibbon = id;
		Tool.hRibbon = AstralDlgGet( Tool.idRibbon );
		ASSERT(Tool.hRibbon);
		if ( Tool.hRibbon  )
		{
			// display the ribbons
			::ShowWindow(Tool.hRibbon,bShow ? SW_SHOW : SW_HIDE);
		}
	}
}

void CServerDoc::SetToolboxes(CServerView *pActiveView, BOOL bShow)
{
	// -----------------------------------
	// Handle the Insitu case
	// -----------------------------------
	if (pActiveView && pActiveView->GetDocument()->IsInPlaceActive())
	{
		CInPlaceFrame* pIPFrame = (CInPlaceFrame*)pActiveView->GetParentFrame();        
		if (pIPFrame)
			pIPFrame->InstallToolBox(bShow);
		return;
	}

	// -----------------------------------
	// Handle the NON-Insitu case
	// -----------------------------------
	CWnd *pMainWnd = ::AfxGetMainWnd();

	if (!pMainWnd)
		return;

    PictPubApp.GetCustomBoxes()->ShowBars(bShow);
}

void CServerDoc::Serialize(CArchive& ar)
// Stores Native Format
{   
    if (ar.IsStoring())
    {                     
        ASSERT(m_lpImage);    
        Store(ar,IsA(),m_lpImage, m_fReadFromOLE10 ,m_fConvert);
    }
    else
	{
    	DeleteContents();         
        m_lpImage = Load(ar,IsA(),m_fReadFromOLE10);
		if (m_lpImage)
		{
		    CRect rBounds;
		    m_lpImage->pServerDoc = this;
			ImgGetImageRect(m_lpImage, &rBounds);
			m_sizeDoc = CSize(rBounds.Width(),rBounds.Height());
		}
		else
	   	    AfxThrowMemoryException();
	}
}

LPSTORAGE CServerDoc::GetRootCompoundStorage(BOOL *pbTempStg) 
{
    if (pbTempStg)
    {
    	if (m_lpRootImageStorage)
        	*pbTempStg = m_bTempStg;    
    	else
        	*pbTempStg = TRUE; 
    }

    return m_lpRootImageStorage;
}

void CServerDoc::SetRootCompoundStorage(LPSTORAGE lpRootStg,BOOL bTempStg)
{
    if (m_lpRootImageStorage)
    {
        DWORD refCnt = ::ReleaseInterface((LPUNKNOWN*)&m_lpRootImageStorage);
        if ((refCnt < 0) && !bTempStg)
            //????? IF NOT TEMPORARY DO WE NEED TO DELETE THE FILE AS WELL???? - jtm
            ;
    }
    m_lpRootImageStorage = lpRootStg; 
    m_bTempStg = bTempStg;

    ReadSummaryInfo();        
};

BOOL CServerDoc::InitCompoundFile()
{
	BOOL bResult = TRUE;
	LPSTORAGE lpStorage = NULL;
	SCODE sc;
    BOOL bIsCompoundStg = FALSE;
	BOOL bIsNew = m_lpImage->fUntitled;

	// Is this existing file a structured storage
    if (!bIsNew)
    {
    	sc = StgIsStorageFile(GetPathName());
    	bIsCompoundStg = (sc==S_OK);
    }

    // ie.) New File or Non_Compound Old File - 
    // create temporary storage for the Mask management
	if (!bIsCompoundStg) 
	{
		// create new temporary docfile
		sc = ::StgCreateDocfile(NULL, 
			STGM_DELETEONRELEASE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE|STGM_CREATE,
			0, 
			&lpStorage);

		if (sc != NOERROR)
			return FALSE;

		ASSERT(lpStorage != NULL);
        SetRootCompoundStorage(lpStorage,TRUE);
	}
	else
	{
		TRY
		{
			// open new storage file
			sc = StgOpenStorage(
				GetPathName(),
				NULL,
				STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
				0, 
				0, 
				&lpStorage);

 			if (sc != NOERROR)
 				AfxThrowOleException(sc);

 			ASSERT(lpStorage != NULL);
            SetRootCompoundStorage(lpStorage,FALSE);

			bResult = TRUE;
		}
		CATCH_ALL(e)
		{
            ;
		}
		END_CATCH_ALL
	}

	return TRUE;
}

BOOL CServerDoc::DoSummaryInfo()
{
	CSummaryPropSheet	SummarySheet(this,IDS_SUMMARYTABDLG, AfxGetMainWnd(), 
        IDD_SUMMARYTABDLG);
	if (IDOK == SummarySheet.DoModal())
	{
        // mark the image and document as dirty.
		if (m_lpImage)
	        m_lpImage->SetChanged(TRUE);
        SetModifiedFlag(TRUE);
	}
    return TRUE;
}

BOOL CServerDoc::ReadSummaryInfo()
{
    BOOL bResult = TRUE;

    if (m_lpRootImageStorage)
    {   
    	if (bResult = m_SummaryInfo.Init())
            bResult = m_SummaryInfo.ReadSummInfo(m_lpRootImageStorage);
    }

    return bResult;
}

BOOL CServerDoc::WriteSummaryInfo()
{
    BOOL bResult = TRUE;

    if (m_lpRootImageStorage)
    	bResult = m_SummaryInfo.WriteSummInfo(m_lpRootImageStorage,TRUE);

    return bResult;
}

BOOL CServerDoc::SwapStorage(LPTSTR lpFileName)
{
    BOOL bTempStg;
    LPSTORAGE lpCurrentRootStg = GetRootCompoundStorage(&bTempStg);
    if (lpCurrentRootStg) 
    {
		// see if this storage has the right name
		if (!bTempStg && lpFileName)
		{
			STATSTG StatStg;
			LPMALLOC pMalloc;

			CoGetMalloc(MEMCTX_TASK, &pMalloc);
			if (pMalloc)
			{
				if (lpCurrentRootStg->Stat(&StatStg, STATFLAG_DEFAULT) == S_OK)
				{
					BOOL fSame = StringsEqual(StatStg.pwcsName, lpFileName);
					pMalloc->Free(StatStg.pwcsName);
			  		ReleaseInterface((LPUNKNOWN*)&pMalloc);
					if (fSame) // correct name, so bug out
						return(TRUE);
				}
			}
		}

        LPSTORAGE lpNewRootStg = NULL;
        SCODE sc = ::StgCreateDocfile(lpFileName, 
        	STGM_READWRITE|
            STGM_SHARE_EXCLUSIVE|STGM_CREATE,
        	0, 
        	&lpNewRootStg);
        
        if (sc != NOERROR)
        	return FALSE;
        ASSERT(lpNewRootStg != NULL);
    
		// copy everything into it from the temporary storage,
		// except the content stream
		struct
		{
			LPOLESTR	lpContents;
			LPOLESTR	lpNull;
			FNAME		szContents;
		} ContentsSNB;
		SNB snb = (SNB)&ContentsSNB;
		const TCHAR szContents[] = _T("Contents");

		ContentsSNB.lpContents = ContentsSNB.szContents;
		ContentsSNB.lpNull = NULL;
		lstrcpy(ContentsSNB.szContents, szContents);

        lpCurrentRootStg->CopyTo(NULL,NULL,snb,lpNewRootStg);
    
        // replace m_lpImage and mark it as Temporary or NON-Temporary ;
        // NOTE: SetRootCompoundStorage will release the current
        // storage.
        bTempStg = (lpFileName==NULL);
        SetRootCompoundStorage(lpNewRootStg,bTempStg);
    }
    return TRUE;
}

 
/******************************* EXPORTED Functions *****************************/
