//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "ctxhelp.h"
#include "defaults.h"
#include "svrview.h"
#include "calib.h"

// Static prototypes
static BOOL ImageCropToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg );
static BOOL AutoMeasure( LPIMAGE lpImage, LPRECT lpRect, LPTR lpStep1, LPTR lpStep2, LPTR lpStep3, LPTR lpStep4, int iCount, int iType );

// Static data
BOOL Global_fCalAll = TRUE;
BOOL Global_fUseRGBForGray; 
BOOL Global_fUseGrayForRGB;

#define GRIDSIZE 64 

BEGIN_MESSAGE_MAP(CPrintVisualDlg, CPPMapDlg)
    ON_WM_DESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_QUERYNEWPALETTE()
END_MESSAGE_MAP()                      

BEGIN_MESSAGE_MAP(CScanVisualDlg, CPPMapDlg)
    ON_WM_DESTROY()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_QUERYNEWPALETTE()
END_MESSAGE_MAP()                      

/************************************************************************/
static BOOL AutoMeasure( LPIMAGE lpImage, LPRECT lpRect, LPTR lpStep1, LPTR lpStep2, LPTR lpStep3, LPTR lpStep4, int iCount, ITEMID idType )
/************************************************************************/
{
    int x, y, npix, nlin, nPatchesInMask, nPatchesToMeasure, n, nColumns;
    RECT mRect, pRect;
    RGBS rgb;
    LFIXED ratio, fRatio, fRatio1, fRatio2, fError, flin;
    
    mRect = *lpRect;
    
    switch ( idType )
    {
    // The Scanner (Stouffer's) step scale has an aspect ratio of about 8.3
    // we will read 11 patches, but it is about 13 patches tall
    case IDS_OKTOMEASUREGRAY:
    case IDS_OKTOMEASURERGB:
        fRatio = FGET( 830, 100 );
        nColumns = 1;
        nPatchesInMask = 13;
        nPatchesToMeasure = 11;
        break;
    
    // The Gray printed step scale has an aspect ratio of about 6.6
    // we will read 11 patches
    case IDS_OKTOMEASUREBLACK:
        fRatio = FGET( 660, 100 );
        nColumns = 1;
        nPatchesInMask = 11;
        nPatchesToMeasure = 11;
        break;
    
    // The CMYK printed step scale has an aspect ratio of about 6.6 / 4 = 1.65
    // we will read 4 columns of 11 patches
    case IDS_OKTOMEASURECMYK:
        fRatio = FGET( 165, 100 );
        nColumns = 4;
        nPatchesInMask = 11;
        nPatchesToMeasure = 11;
        break;
    }
    
    fError = (fRatio / 8L); // About 12% error
    fRatio1 = fRatio - fError; // + or - 12% error
    fRatio2 = fRatio + fError;
    
    x = mRect.left;
    y = mRect.top;
    npix = RectWidth( &mRect );
    nlin = RectHeight( &mRect );
    ratio = FGET( nlin, npix );
    if ( ratio < fRatio1 || ratio > fRatio2 )
    {
    	Message( IDS_BADSCANMASK );
    	return( NO );
    }
    
    if ( AstralOKCancel(idType) == IDCANCEL )
    	return( NO );
    
    flin = FGET( nlin, nPatchesInMask );
    nlin = FROUND( flin );
    npix /= nColumns;
    for ( n=0; n<nColumns; n++ )
    {
    	// Start the measurements 2/9ths of the way from the left
    	pRect.left = x + ((2*npix)/9);
    	// Measure the next 2/9ths and ignore the 5/9ths on the right
    	pRect.right = pRect.left + ((2*npix)/9);
    	for ( iCount=0; iCount<nPatchesToMeasure; iCount++ )
    	{
    		// Start the measurements 1/4th of the way from the patch top
    		pRect.top = y + (nlin/4) + FMUL(iCount, flin);
    		// Measure the next 1/2 and ignore 1/4th on the bottom
    		pRect.bottom = pRect.top + (nlin/2);
    		MeasureRGB( lpImage, &pRect, &rgb );
    		if ( nColumns == 1 )
    		{ // Always gray steps
    			if ( lpStep1 )
    				*lpStep1++ = rgb.red;
    			if ( lpStep2 )
    				*lpStep2++ = rgb.green;
    			if ( lpStep3 )
    				*lpStep3++ = rgb.blue;
    			if ( lpStep4 )
    				*lpStep4++ = 255 - RGBtoL(&rgb);
    		}
    		else	
            { // Always CMYK color
    			if ( lpStep1 && n == 0 )
    				*lpStep1++ = 255 - rgb.red;
    			if ( lpStep2 && n == 1 )
    				*lpStep2++ = 255 - rgb.green;
    			if ( lpStep3 && n == 2 )
    				*lpStep3++ = 255 - rgb.blue;
    			if ( lpStep4 && n == 3 )
    				*lpStep4++ = 255 - RGBtoL(&rgb);
    		}
   		}
    	x += npix;
   	}
    
    return( YES );
}

//************************************************************************
void LoadMeasurementControls( HWND hDlg, LPTR lpStep1, LPTR lpStep2, LPTR lpStep3, LPTR lpStep4 )
//************************************************************************
{
    int i;
    
    for ( i=0; i<CALPOINTS; i++ )
   	{
    	SetDlgItemInt( hDlg, IDC_RMEASURE1+i, TOPERCENT( *lpStep1++ ), NO );
    	SetDlgItemInt( hDlg, IDC_GMEASURE1+i, TOPERCENT( *lpStep2++ ), NO );
    	SetDlgItemInt( hDlg, IDC_BMEASURE1+i, TOPERCENT( *lpStep3++ ), NO );
    	SetDlgItemInt( hDlg, IDC_XMEASURE1+i, TOPERCENT( *lpStep4++ ), NO );
   	}
}

/************************************************************************/
static BOOL ImageCropToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg )
/************************************************************************/
{
    POINT pt;
    static HDC shDC;
    static RECT SelectRect;
    static BOOL bTrack;
    static RECT rControl;
    ImageToolProcData *pData =
   	 (ImageToolProcData *)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROCDATA);
    ASSERT(pData);

    switch (msg)
    {
    case WM_SIZE: // Values used for Image2Control mapping
        rControl.right  = rControl.left + x - 1;
        rControl.bottom = rControl.top  + y - 1;
        SetImageControlMapping( pData->iImageWidth, pData->iImageHeight, &rControl );
        break;
    
    case WM_MOVE: // Values used for Image2Control mapping
        rControl.left = x;
        rControl.top = y;
        SetImageControlMapping( pData->iImageWidth, pData->iImageHeight, &rControl );
        break;
    
    case WM_LBUTTONDOWN:
        if (bTrack)
        	break;
        bTrack = YES;
        UpdateWindow( hWindow );
        shDC = GetDC( hWindow );
        SetClipDC( shDC, &rControl );
        DrawGrid( hWindow, shDC, &pData->rGridRect, SL_BOX ); // turn it off
        pt.x = x;
        pt.y = y;
        StartSelection( hWindow, shDC, &SelectRect, SL_BOX | SL_SPECIAL,
        	pt, 0L );
        pData->rGridRect = SelectRect;
        Control2Image( ( LPINT )&pData->rGridRect.left, 
            ( LPINT )&pData->rGridRect.top );
        Control2Image( ( LPINT )&pData->rGridRect.right, 
            ( LPINT )&pData->rGridRect.bottom );
        SEND_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0);
        break;
        
    case WM_LBUTTONUP:
        if (!bTrack)
        	break;
        bTrack = NO;
        EndSelection( hWindow, shDC, &SelectRect, SL_BOX, YES );
        pData->rGridRect = SelectRect;
        Control2Image( ( LPINT )&pData->rGridRect.left, 
            ( LPINT )&pData->rGridRect.top );
        Control2Image( ( LPINT )&pData->rGridRect.right, 
            ( LPINT )&pData->rGridRect.bottom );
        DrawGrid( hWindow, shDC, &pData->rGridRect, SL_BOX ); // turn it on
        if ( shDC )
        	ReleaseDC( hWindow, shDC );
        shDC = NULL;
        SEND_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0);
        break;
        
    case WM_MOUSEMOVE:
        if (!bTrack)
        	break;
        pt.x = x;
        pt.y = y;
        UpdateSelection( hWindow, shDC, &SelectRect, SL_BOX,
        	pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||RBUTTON,FROMCENTER);
        pData->rGridRect = SelectRect;
        Control2Image( ( LPINT )&pData->rGridRect.left, 
            ( LPINT )&pData->rGridRect.top );
        Control2Image( ( LPINT )&pData->rGridRect.right, 
            ( LPINT )&pData->rGridRect.bottom );
        SEND_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow), 
            hWindow, 0);
        break;
        
    case WM_LBUTTONDBLCLK:
        pData->rGridRect.left = pData->rGridRect.top = 0;
        pData->rGridRect.right = pData->iImageWidth - 1;
        pData->rGridRect.bottom = pData->iImageHeight - 1;
        InvalidateRect(hWindow, NULL, NO);
        UpdateWindow(hWindow);
        break;
        
    case WM_PAINT:
        SetClipDC( hDC, &rControl );
        DrawGrid( hWindow, hDC, &pData->rGridRect, SL_BOX );
        break;
    }
    
    return(TRUE);
}



////////////////////////////////////////////////////////////////////////////
// CCalibrateScanDlg Methods

CCalibrateScanDlg::CCalibrateScanDlg(CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd) 
    : CPPCalibDlg (pView, nIDTemplate,/*IDD_CALIBRATESCAN,*/ pParentWnd)
{
    m_bModified = FALSE;
}


BOOL CCalibrateScanDlg::OnInitDialog()
{
    HWND hDlg = this->GetSafeHwnd();
    HWND hImageControl;
    LPFRAME lpFrame;

    PreDoModal (NULL);
    CenterPopup( hDlg );
    AstralStrEx( IDS_UNTITLED, m_Map_Name, sizeof(m_Map_Name) );

    ResetMap( &m_RedMap, CALPOINTS, NO  );
    ResetMap( &m_GreenMap, CALPOINTS, NO  );
    ResetMap( &m_BlueMap, CALPOINTS, NO  );
    ResetMap( &m_BlackMap, CALPOINTS, YES );

    CalibrateScan_InitMaps();

    if ( GetImage() && (lpFrame = ImgGetBaseEditFrame(GetImage())) &&
    	 (hImageControl = ::GetDlgItem( hDlg,IDC_PREVIEW )) )
   	{
    	m_Data.iImageWidth  = FrameXSize(lpFrame);
    	m_Data.iImageHeight = FrameYSize(lpFrame);
    	SetWindowLong( hImageControl, GWL_IMAGE, (long)ImgGetBase(GetImage()) );
    	SetWindowLong( hImageControl, GWL_IMAGE_TOOLPROC,
	    	(long)ImageCropToolProc );
    	SetWindowLong( hImageControl, GWL_IMAGE_TOOLPROCDATA,
            (long)&m_Data);
	}
    SetRect( &m_Data.rGridRect, 0, 0, m_Data.iImageWidth-1, m_Data.iImageHeight-1 );
    CheckComboItem(hDlg, IDC_CALMETHOD, IDC_CALMEASURE, IDC_CALVISUAL, IDC_CALMEASURE+m_fScanCalVisual);
    m_bModified = NO;
    ControlEnable(hDlg, IDOK, m_bModified);

    CPPCalibDlg::OnInitDialog();
    return(TRUE);
}

void CCalibrateScanDlg::CalibrateScan_InitMaps()
{
	int i, m;
    HWND hDlg = this->GetSafeHwnd();

	for ( i=0; i<CALPOINTS; i++ )
 	{
		m = CALPOINTS - 1 - i; // RGB's measure upside down
		m_Rstep[i] = (BYTE)m_RedMap.Pnt[m].x;
		m_Gstep[i] = (BYTE)m_GreenMap.Pnt[m].x;
		m_Bstep[i] = (BYTE)m_BlueMap.Pnt[m].x;
		m_Xstep[i] = (BYTE)m_BlackMap.Pnt[i].x;
 	}
	LoadMeasurementControls( hDlg, m_Rstep, m_Gstep, m_Bstep, m_Xstep );
}

BOOL CCalibrateScanDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = this->GetSafeHwnd();
    BOOL bReturn = TRUE;
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);
    int i;
    BOOL Bool;

    switch (nID)
	{
	case IDC_CALMETHOD:
    	if ( !(nID = HandleCombo( hDlg, nID, nCode )) )
    		break;                  
    	m_fScanCalVisual = nID == IDC_CALVISUAL;
    	if (m_fScanCalVisual)
        {
            PostDoModal();
        	EndDialog(IDD_CALIBRATESCANVISUAL);
        }
    	break;

	case IDC_RMEASURE1:
	case IDC_RMEASURE2:
	case IDC_RMEASURE3:
	case IDC_RMEASURE4:
	case IDC_RMEASURE5:
	case IDC_RMEASURE6:
	case IDC_RMEASURE7:
	case IDC_RMEASURE8:
	case IDC_RMEASURE9:
	case IDC_RMEASURE10:
	case IDC_RMEASURE11:
	    if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
    		break;
    	if ( nCode != EN_CHANGE )
    		break;
    	i = GetDlgItemInt( nID, &Bool, NO );
    	m_Rstep[nID-IDC_RMEASURE1] = TOGRAY(i);
    	m_bModified = YES;
    	ControlEnable(hDlg, IDOK, m_bModified);
    	break;

	case IDC_GMEASURE1:
	case IDC_GMEASURE2:
	case IDC_GMEASURE3:
	case IDC_GMEASURE4:
	case IDC_GMEASURE5:
	case IDC_GMEASURE6:
	case IDC_GMEASURE7:
	case IDC_GMEASURE8:
	case IDC_GMEASURE9:
	case IDC_GMEASURE10:
	case IDC_GMEASURE11:
	    if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
    		break;
    	if ( nCode != EN_CHANGE )
    		break;
    	i = GetDlgItemInt( nID, &Bool, NO );
    	m_Gstep[nID-IDC_GMEASURE1] = TOGRAY(i);
    	m_bModified = YES;
    	ControlEnable(hDlg, IDOK, m_bModified);
    	break;

	case IDC_BMEASURE1:
	case IDC_BMEASURE2:
	case IDC_BMEASURE3:
	case IDC_BMEASURE4:
	case IDC_BMEASURE5:
	case IDC_BMEASURE6:
	case IDC_BMEASURE7:
	case IDC_BMEASURE8:
	case IDC_BMEASURE9:
	case IDC_BMEASURE10:
	case IDC_BMEASURE11:
    	if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
    		break;
    	if ( nCode != EN_CHANGE )
    		break;
    	i = GetDlgItemInt( nID, &Bool, NO );
    	m_Bstep[nID-IDC_BMEASURE1] = TOGRAY(i);
    	m_bModified = YES;
    	ControlEnable(hDlg, IDOK, m_bModified);
    	break;

	case IDC_XMEASURE1:
	case IDC_XMEASURE2:
	case IDC_XMEASURE3:
	case IDC_XMEASURE4:
	case IDC_XMEASURE5:
	case IDC_XMEASURE6:
	case IDC_XMEASURE7:
	case IDC_XMEASURE8:
	case IDC_XMEASURE9:
	case IDC_XMEASURE10:
	case IDC_XMEASURE11:
    	if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
    		break;
    	if ( nCode != EN_CHANGE )
    		break;
    	i = GetDlgItemInt( nID, &Bool, NO );
    	m_Xstep[nID-IDC_XMEASURE1] = TOGRAY(i);
    	m_bModified = YES;
    	ControlEnable(hDlg, IDOK, m_bModified);
	    break;

	case IDC_MEASURE:
    	if ( !GetImage() )
		{
    		Message( IDS_NEEDSCANMASK );
    		break;
		}
	    if ( FrameDepth(ImgGetBaseEditFrame(GetImage())) == 1 )
		{
    		if ( !AutoMeasure( GetImage(), &m_Data.rGridRect, NULL, NULL, NULL, m_Xstep,
    			CALPOINTS, IDS_OKTOMEASUREGRAY ) )
	    		break;
		}
	else
		{
    		if ( !AutoMeasure( GetImage(), &m_Data.rGridRect, m_Rstep, m_Gstep, m_Bstep, NULL,
	    		CALPOINTS, IDS_OKTOMEASURERGB ) )
		    	break;
		}
    	LoadMeasurementControls(hDlg, m_Rstep, m_Gstep, m_Bstep, m_Xstep);
    	m_bModified = YES;
    	ControlEnable(hDlg, IDOK, m_bModified);
    	break;

	case IDC_MAPLOAD:
        {
            CLoadDlg *pDlg = new CLoadDlg(IDN_SCANMAP, IDD_SCANMAP_LOAD, this,
                &m_MasterMap, &m_RedMap,  &m_GreenMap, &m_BlueMap, &m_BlackMap);
            if (pDlg)
            {
                int ret = pDlg->DoModal();
                delete pDlg;
                if (ret != IDOK)
                    break;
            }
    
        	if (m_BlackMap.Points != CALPOINTS ||
        		m_RedMap.Points != CALPOINTS ||
        		m_GreenMap.Points != CALPOINTS ||
        		m_BlueMap.Points != CALPOINTS)
        	{
        		Message(IDS_NOTMEASUREMENTMAP);
        		break;
    		}
    	    CalibrateScan_InitMaps();
           	m_bModified = YES;
        	ControlEnable(hDlg, IDOK, m_bModified);
        }
    	break;
    
	case IDC_RESET:
	    SetRect( &m_Data.rGridRect, 0, 0, m_Data.iImageWidth-1, m_Data.iImageHeight-1 );
    	AstralControlRepaint( hDlg, IDC_PREVIEW );
    	ResetMap( &m_RedMap, CALPOINTS, NO  );
    	ResetMap( &m_GreenMap, CALPOINTS, NO  );
    	ResetMap( &m_BlueMap, CALPOINTS, NO  );
    	ResetMap( &m_BlackMap, CALPOINTS, YES );
    
    	CalibrateScan_InitMaps();
    	m_bModified = NO;
    	ControlEnable(hDlg, IDOK, m_bModified);
    	break;
    
    default:
        bReturn = FALSE;
    	break;
    }

    return bReturn ? bReturn : CPPCalibDlg::OnCommand(wParam, lParam);

}

void CCalibrateScanDlg::OnOK()
{
    HWND hDlg = this->GetSafeHwnd();
    int i,m;
    FNAME szFileName;

	for ( i=0; i<CALPOINTS-1; i++ )
	{ // Make sure the points are in ascending order
		if ( m_Rstep[i] < m_Rstep[i+1] )
			break;
		if ( m_Gstep[i] < m_Gstep[i+1] )
			break;
		if ( m_Bstep[i] < m_Bstep[i+1] )
			break;
		if ( m_Xstep[i] > m_Xstep[i+1] )
			break;
	}

	if ( i != CALPOINTS-1 )
	{
		Message( IDS_ECALASC );
        OnCancel();
        return;
	}

	ResetMap( &m_RedMap, CALPOINTS, NO  ); m_RedMap.bStretch = NO;
	ResetMap( &m_GreenMap, CALPOINTS, NO  ); m_GreenMap.bStretch = NO;
	ResetMap( &m_BlueMap, CALPOINTS, NO  ); m_BlueMap.bStretch = NO;
	ResetMap( &m_BlackMap, CALPOINTS, YES ); m_BlackMap.bStretch = NO;

	for ( i=0; i<CALPOINTS; i++ )
	{
		m = CALPOINTS - 1 - i; // RGB's measure upside down
		m_RedMap.Pnt[m].x = m_Rstep[i];
		m_GreenMap.Pnt[m].x = m_Gstep[i];
		m_BlueMap.Pnt[m].x = m_Bstep[i];
		m_BlackMap.Pnt[i].x = m_Xstep[i];
	}

	MakeMap( &m_RedMap );
	MakeMap( &m_GreenMap );
	MakeMap( &m_BlueMap );
	MakeMap( &m_BlackMap );

	szFileName[0] = '\0';
	if ( !PromptForExtName( hDlg, m_Map_Name, szFileName,
		IDN_SCANMAP ) )
    {
        OnCancel();
        return;
    }

	if ( !SaveMap( &m_BlackMap, &m_RedMap, &m_GreenMap, &m_BlueMap, szFileName ))
    {
        OnCancel();
        return;
    }

	if ( !AddExtFile( m_Map_Name, szFileName, IDN_SCANMAP ) )
	{
		FileDelete( szFileName );
        OnCancel();
        return;
	}

    CPPCalibDlg::OnOK();
}
    
void CCalibrateScanDlg::OnCancel()
{
    CPPCalibDlg::OnCancel();
}    

////////////////////////////////////////////////////////////////////////////
// CCalibratePrintDlg Methods

CCalibratePrintDlg::CCalibratePrintDlg(CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd) 
    : CPPCalibDlg (pView, nIDTemplate/*IDD_CALIBRATEPRINT*/, pParentWnd)
{
    m_bModified = FALSE;
}


BOOL CCalibratePrintDlg::OnInitDialog()
{
    HWND hDlg = this->GetSafeHwnd();
    int i;
    HWND hImageControl;
    LPFRAME lpFrame;

    PreDoModal (NULL);
    CenterPopup( hDlg );
    AstralStrEx( IDS_UNTITLED, m_Map_Name, sizeof(m_Map_Name) );

    ResetMap( &m_RedMap, CALPOINTS, NO ); m_RedMap.bStretch = YES;
    ResetMap( &m_GreenMap, CALPOINTS, NO ); m_GreenMap.bStretch = YES;
    ResetMap( &m_BlueMap, CALPOINTS, NO ); m_BlueMap.bStretch = YES;
    ResetMap( &m_BlackMap, CALPOINTS, NO ); m_BlackMap.bStretch = YES;

    for ( i=0; i<CALPOINTS; i++ )
	{
    	m_Rstep[i] = (BYTE)m_RedMap.Pnt[i].x;
	    m_Gstep[i] = (BYTE)m_GreenMap.Pnt[i].x;
    	m_Bstep[i] = (BYTE)m_BlueMap.Pnt[i].x;
    	m_Kstep[i] = (BYTE)m_BlackMap.Pnt[i].x;
	}

    LoadMeasurementControls( hDlg, m_Rstep, m_Gstep, m_Bstep, m_Kstep );
    if ( GetImage() && (lpFrame = ImgGetBaseEditFrame(GetImage())) &&
    	 (hImageControl = ::GetDlgItem( hDlg,IDC_PREVIEW )) )
	{
    	m_Data.iImageWidth  = FrameXSize(lpFrame);
    	m_Data.iImageHeight = FrameYSize(lpFrame);
	    SetWindowLong( hImageControl, GWL_IMAGE, (long)ImgGetBase(GetImage()) );
    	SetWindowLong( hImageControl, GWL_IMAGE_TOOLPROC,
	    	(long)ImageCropToolProc );
    	SetWindowLong( hImageControl, GWL_IMAGE_TOOLPROCDATA,
            (long)&m_Data);
	}
    SetRect( &m_Data.rGridRect, 0, 0, m_Data.iImageWidth-1, m_Data.iImageHeight-1 );
    CheckComboItem(hDlg, IDC_CALMETHOD, IDC_CALMEASURE, IDC_CALVISUAL, IDC_CALMEASURE+Control.fPrintCalVisual);
    m_bModified = NO;
    ControlEnable(hDlg, IDOK, m_bModified);

    CPPCalibDlg::OnInitDialog();
    return(TRUE);
}

BOOL CCalibratePrintDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = this->GetSafeHwnd();
    BOOL bReturn = TRUE;
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);
    int      i;
    BOOL     Bool;

    switch (nID)
	{
   	case IDC_CALMETHOD:
    	if ( !(nID = HandleCombo( hDlg, nID, nCode )) )
    		break;
    	Control.fPrintCalVisual = nID == IDC_CALVISUAL;
    	if (Control.fPrintCalVisual)
        	EndDialog(IDD_CALIBRATEPRINTVISUAL);
    	break;

	case IDC_MAPLOAD:
        {
            CLoadDlg *pDlg = new CLoadDlg(IDN_CALMAP,IDD_CALMAP_LOAD,this,
                &m_MasterMap, &m_RedMap,  &m_GreenMap, &m_BlueMap, &m_BlackMap);
            if (pDlg)
            {
                int ret = pDlg->DoModal();
                delete pDlg;
                if (ret != IDOK)
                    break;
            }
    
        	if (m_RedMap.Points != CALPOINTS ||
        		m_GreenMap.Points != CALPOINTS ||
        		m_BlueMap.Points != CALPOINTS ||
        		m_BlackMap.Points != CALPOINTS)
    		{
        		Message(IDS_NOTMEASUREMENTMAP);
        		break;
        	}
    	    for ( i=0; i<CALPOINTS; i++ )
    		{
        		m_Rstep[i] = (BYTE)m_RedMap.Pnt[i].x;
        		m_Gstep[i] = (BYTE)m_GreenMap.Pnt[i].x;
        		m_Bstep[i] = (BYTE)m_BlueMap.Pnt[i].x;
        		m_Kstep[i] = (BYTE)m_BlackMap.Pnt[i].x;
    		}
    	    LoadMeasurementControls( hDlg, m_Rstep, m_Gstep, m_Bstep, m_Kstep );
        	m_bModified = YES;
        	ControlEnable(hDlg, IDOK, m_bModified);
        }
   	    break;
    
	case IDC_RMEASURE1:
	case IDC_RMEASURE2:
	case IDC_RMEASURE3:
	case IDC_RMEASURE4:
	case IDC_RMEASURE5:
	case IDC_RMEASURE6:
	case IDC_RMEASURE7:
	case IDC_RMEASURE8:
	case IDC_RMEASURE9:
	case IDC_RMEASURE10:
	case IDC_RMEASURE11:
		if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
			break;
		if ( nCode != EN_CHANGE )
			break;
		i = GetDlgItemInt( nID, &Bool, NO );
		m_Rstep[nID-IDC_RMEASURE1] = TOGRAY(i);
		m_bModified = YES;
		ControlEnable(hDlg, IDOK, m_bModified);
		break;

	case IDC_GMEASURE1:
	case IDC_GMEASURE2:
	case IDC_GMEASURE3:
	case IDC_GMEASURE4:
	case IDC_GMEASURE5:
	case IDC_GMEASURE6:
	case IDC_GMEASURE7:
	case IDC_GMEASURE8:
	case IDC_GMEASURE9:
	case IDC_GMEASURE10:
	case IDC_GMEASURE11:
		if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
			break;
		if ( nCode != EN_CHANGE )
			break;
		i = GetDlgItemInt( nID, &Bool, NO );
		m_Gstep[nID-IDC_GMEASURE1] = TOGRAY(i);
		m_bModified = YES;
		ControlEnable(hDlg, IDOK, m_bModified);
    	break;

	case IDC_BMEASURE1:
	case IDC_BMEASURE2:
	case IDC_BMEASURE3:
	case IDC_BMEASURE4:
	case IDC_BMEASURE5:
	case IDC_BMEASURE6:
	case IDC_BMEASURE7:
	case IDC_BMEASURE8:
	case IDC_BMEASURE9:
	case IDC_BMEASURE10:
	case IDC_BMEASURE11:
		if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
			break;
		if ( nCode != EN_CHANGE )
			break;
		i = GetDlgItemInt( nID, &Bool, NO );
		m_Bstep[nID-IDC_BMEASURE1] = TOGRAY(i);
		m_bModified = YES;
	    ControlEnable(hDlg, IDOK, m_bModified);
    	break;

	case IDC_XMEASURE1:
	case IDC_XMEASURE2:
	case IDC_XMEASURE3:
	case IDC_XMEASURE4:
	case IDC_XMEASURE5:
	case IDC_XMEASURE6:
	case IDC_XMEASURE7:
	case IDC_XMEASURE8:
	case IDC_XMEASURE9:
	case IDC_XMEASURE10:
	case IDC_XMEASURE11:
		if ( ::GetFocus() != ::GetDlgItem( hDlg,nID ) )
			break;
		if ( nCode != EN_CHANGE )
			break;
		i = GetDlgItemInt( nID, &Bool, NO );
		m_Kstep[nID-IDC_XMEASURE1] = TOGRAY(i);
		m_bModified = YES;
		ControlEnable(hDlg, IDOK, m_bModified);
    	break;

	case IDC_MEASURE:
		if ( !GetImage() )
		{
			Message( IDS_NEEDPRINTMASK );
			break;
		}
		if ( FrameDepth(ImgGetBaseEditFrame(GetImage())) == 1 )
		{
			if ( !AutoMeasure( GetImage(), &m_Data.rGridRect, NULL, NULL, NULL, 
                m_Kstep,CALPOINTS, IDS_OKTOMEASUREBLACK ) )
				break;
			// set all channels to K readings
			copy(m_Kstep, m_Rstep, sizeof(m_Kstep));
			copy(m_Kstep, m_Gstep, sizeof(m_Kstep));
			copy(m_Kstep, m_Bstep, sizeof(m_Kstep));
			}
		else
			{
			if ( !AutoMeasure( GetImage(), &m_Data.rGridRect, m_Rstep, m_Gstep, 
                m_Bstep, m_Kstep,CALPOINTS, IDS_OKTOMEASURECMYK ) )
				break;
			}
		LoadMeasurementControls(hDlg, m_Rstep, m_Gstep, m_Bstep, m_Kstep);
		m_bModified = YES;
		ControlEnable(hDlg, IDOK, m_bModified);
	break;

	case IDC_RESET:
		SetRect( &m_Data.rGridRect, 0, 0, m_Data.iImageWidth-1, m_Data.iImageHeight-1 );
		AstralControlRepaint( hDlg, IDC_PREVIEW );

		ResetMap( &m_RedMap, CALPOINTS, NO ); m_RedMap.bStretch = YES;
		ResetMap( &m_GreenMap, CALPOINTS, NO ); m_GreenMap.bStretch = YES;
		ResetMap( &m_BlueMap, CALPOINTS, NO ); m_BlueMap.bStretch = YES;
		ResetMap( &m_BlackMap, CALPOINTS, NO ); m_BlackMap.bStretch = YES;

		for ( i=0; i<CALPOINTS; i++ )
		{
			m_Rstep[i] = (BYTE)m_RedMap.Pnt[i].x;
			m_Gstep[i] = (BYTE)m_GreenMap.Pnt[i].x;
			m_Bstep[i] = (BYTE)m_BlueMap.Pnt[i].x;
			m_Kstep[i] = (BYTE)m_BlackMap.Pnt[i].x;
		}
		LoadMeasurementControls(hDlg, m_Rstep, m_Gstep, m_Bstep, m_Kstep);
		m_bModified = NO;
		ControlEnable(hDlg, IDOK, m_bModified);
    	break;

   default:
        bReturn = FALSE;
	    break;
   }

    return bReturn ? bReturn : CPPCalibDlg::OnCommand(wParam, lParam);
}

void CCalibratePrintDlg::OnOK()
{
    HWND hDlg = this->GetSafeHwnd();
    short int i;
    FNAME szFileName;

    for ( i=0; i<CALPOINTS-1; i++ )
  	{ // Make sure the points are in ascending order
		if ( m_Kstep[i] > m_Kstep[i+1] )
			break;
		if ( m_Rstep[i] > m_Rstep[i+1] )
			break;
		if ( m_Gstep[i] > m_Gstep[i+1] )
			break;
		if ( m_Bstep[i] > m_Bstep[i+1] )
			break;
  	}

	if ( i != CALPOINTS-1 )
  	{
		Message( IDS_ECALASC );
        OnCancel();
        return;
  	}

	ResetMap( &m_RedMap, CALPOINTS, NO ); m_RedMap.bStretch = YES;
	ResetMap( &m_GreenMap, CALPOINTS, NO ); m_GreenMap.bStretch = YES;
	ResetMap( &m_BlueMap, CALPOINTS, NO ); m_BlueMap.bStretch = YES;
	ResetMap( &m_BlackMap, CALPOINTS, NO ); m_BlackMap.bStretch = YES;

	for ( i=0; i<CALPOINTS; i++ )
	{
		m_RedMap.Pnt[i].x = m_Rstep[i];
		m_GreenMap.Pnt[i].x = m_Gstep[i];
		m_BlueMap.Pnt[i].x = m_Bstep[i];
		m_BlackMap.Pnt[i].x = m_Kstep[i];
	}

	MakeMap( &m_RedMap );
	MakeMap( &m_GreenMap );
	MakeMap( &m_BlueMap );
	MakeMap( &m_BlackMap );

	szFileName[0] = '\0';
	if ( !PromptForExtName( hDlg, m_Map_Name, szFileName,
		IDN_CALMAP ) )
    {
        OnCancel();
        return;
    }

	if ( !SaveMap( NULL, &m_RedMap, &m_GreenMap, &m_BlueMap, &m_BlackMap, szFileName))
    {
        OnCancel();
        return;
    }
	if ( !AddExtFile( m_Map_Name, szFileName, IDN_CALMAP ))
	{
		FileDelete( szFileName );
        OnCancel();
        return;
	}

    CPPCalibDlg::OnOK();
}

void CCalibratePrintDlg::OnCancel()
{
    CPPCalibDlg::OnCancel();
}

////////////////////////////////////////////////////////////////////////////
// CPrintVisualDlg Methods

CPrintVisualDlg::CPrintVisualDlg(CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd) 
    : CPPCalibDlg (pView, nIDTemplate/*IDD_CALIBRATEPRINTVISUAL*/, pParentWnd)
{
}

void CPrintVisualDlg::OnDestroy()
{	
	HandleMapDestroy();
}

void CPrintVisualDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	HandleMapMouseMove( point.x, point.y, nFlags);
}

void CPrintVisualDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	HandleMapLButtonUp( point.x, point.y, nFlags);
}

void CPrintVisualDlg::OnTimer(UINT nIDEvent)
{	
	if (!AutoPreview_OnTimer( nIDEvent))
		return;
		
	// got good timer
	UpdateCurrentMap();
	MakeMap( m_MapValues.pMap );
	MapPreview(m_pView->GetSafeHwnd());
}

BOOL CPrintVisualDlg::OnQueryNewPalette()
{
    return(HandleQueryNewPalette(GetSafeHwnd(), NO, NO));
}

BOOL CPrintVisualDlg::OnInitDialog()
{
    HWND hDlg = this->GetSafeHwnd();
	FRMDATATYPE type;

    PreDoModal (NULL);
    CenterPopup( hDlg );
	AstralStrEx( IDS_UNTITLED, m_Map_Name, sizeof(m_Map_Name) );

	CheckComboItem(hDlg, IDC_CALMETHOD, IDC_CALMEASURE, IDC_CALVISUAL, 
        IDC_CALMEASURE+Control.fPrintCalVisual);
	m_MapValues.fModified = FALSE;

	ResetAllMapsEx(NO, YES /*applyMtoK*/);

	// set up to edit the calibration maps
	ResetMapEx( &m_MasterMap, m_nPoints, REVERSE_LUTXY, YES );
	m_MasterMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_MasterMap);
	ResetMap( &m_RedMap, m_nPoints, REVERSE_LUTXY );
	m_RedMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_RedMap);
	ResetMap( &m_GreenMap, m_nPoints, REVERSE_LUTXY );
	m_GreenMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_GreenMap);
	ResetMap( &m_BlueMap, m_nPoints, REVERSE_LUTXY );
	m_BlueMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_BlueMap);
	ResetMap( &m_BlackMap, m_nPoints, REVERSE_LUTXY );
	m_BlackMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_BlackMap);

	m_MapValues.wChannel = IDC_DO_ALL;
	if (GetImage())
		type = FrameType(ImgGetBaseEditFrame(GetImage()));
	else
		type = FDT_CMYKCOLOR;
	HandleMapInit( type, TRUE);

    CPPCalibDlg::OnInitDialog();
	return(TRUE);
}

BOOL CPrintVisualDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = this->GetSafeHwnd();
    BOOL bReturn = TRUE;
   	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

	switch (nID)
	{
	case IDC_CALMETHOD:
		if ( !(nID = HandleCombo( hDlg, nID, nCode )) )
			break;
		Control.fPrintCalVisual = nID == IDC_CALVISUAL;
		if (!Control.fPrintCalVisual)
		{
            HandleMapEnd();
        	EndDialog(IDD_CALIBRATEPRINT);
		}
		break;

	case IDC_MAPLOAD:
        {
            CLoadDlg *pDlg = new CLoadDlg(IDN_CALMAP, IDD_CALMAP_LOAD, this,
                &m_MasterMap, &m_RedMap,  &m_GreenMap, &m_BlueMap, &m_BlackMap);
            if (pDlg)
            {
                int ret = pDlg->DoModal();
                delete pDlg;
                if (ret != IDOK)
                    break;
            }
    
    		m_MasterMap.Reverse |= REVERSE_LUTXY;
    		m_RedMap.Reverse |= REVERSE_LUTXY;
    		m_GreenMap.Reverse |= REVERSE_LUTXY;
    		m_BlueMap.Reverse |= REVERSE_LUTXY;
    		m_BlackMap.Reverse |= REVERSE_LUTXY;
    		ReverseMap(&m_RedMap, TRUE);
    		ReverseMap(&m_GreenMap, TRUE);
    		ReverseMap(&m_BlueMap, TRUE);
    		HandleMapLoad();
        }
		break;

    case IDOK: // alias SAVE
    case IDCANCEL:
        bReturn = FALSE; // let this be handled by the OnOK and OnCancel
        break;

   	default:
      	if (HandleMapCommand(nID, nCode))
			ControlEnable(hDlg, IDOK, m_MapValues.fModified);
		break;
   }
    return bReturn ? bReturn : CPPCalibDlg::OnCommand(wParam, lParam);
}

void CPrintVisualDlg::OnOK()
{
    HWND hDlg = this->GetSafeHwnd();
    FNAME szFileName;

    // strip off bit used for display only
	m_MasterMap.Reverse &= ~REVERSE_LUTXY;
	m_RedMap.Reverse &= ~REVERSE_LUTXY;
	m_GreenMap.Reverse &= ~REVERSE_LUTXY;
	m_BlueMap.Reverse &= ~REVERSE_LUTXY;
	m_BlackMap.Reverse &= ~REVERSE_LUTXY;

	// make maps work on CMY
	ReverseMap(&m_RedMap, FALSE);
	ReverseMap(&m_GreenMap, FALSE);
	ReverseMap(&m_BlueMap, FALSE);

	szFileName[0] = '\0';
	if ( !PromptForExtName( hDlg, m_Map_Name, szFileName,
		IDN_CALMAP ) )
    {
        OnCancel();
        return;
    }
            
	if ( !SaveMap( &m_MasterMap, &m_RedMap, &m_GreenMap, &m_BlueMap, 
        &m_BlackMap, szFileName))
    {
        OnCancel();
        return;
    }

	if ( !AddExtFile( m_Map_Name, szFileName, IDN_CALMAP ))
	{
		FileDelete( szFileName );
        OnCancel();
        return;
	}

    CPPCalibDlg::OnOK();            
}
    
void CPrintVisualDlg::OnCancel()
{
    HandleMapEnd();
    CPPCalibDlg::OnCancel();
};

//////////////////////////////////////////////////////////////////////////////////////
// CScanVisualDlg Methods

CScanVisualDlg::CScanVisualDlg(CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd) 
    : CPPCalibDlg (pView,nIDTemplate/*IDD_CALIBRATESCANVISUAL*/, pParentWnd)
{
}

void CScanVisualDlg::OnDestroy()
{	
	HandleMapDestroy();
}

void CScanVisualDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	HandleMapMouseMove(point.x, point.y, nFlags);
}

void CScanVisualDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	HandleMapLButtonUp(point.x, point.y, nFlags);
}

void CScanVisualDlg::OnTimer(UINT nIDEvent)
{	
	if (!AutoPreview_OnTimer(nIDEvent))
		return;
		
	// got good timer
	UpdateCurrentMap();
	MakeMap(m_MapValues.pMap);
	MapPreview(m_pView->GetSafeHwnd());
}

BOOL CScanVisualDlg::OnQueryNewPalette()
{
    return(HandleQueryNewPalette(GetSafeHwnd(), NO, NO));
}

BOOL CScanVisualDlg::OnInitDialog()
{
    HWND hDlg = this->GetSafeHwnd();

    PreDoModal (NULL);
    CenterPopup( hDlg );
	AstralStrEx( IDS_UNTITLED, m_Map_Name, sizeof(m_Map_Name) );

	CheckComboItem(hDlg, IDC_CALMETHOD, IDC_CALMEASURE, IDC_CALVISUAL, 
        IDC_CALMEASURE + m_fScanCalVisual);
	// set up to edit the calibration maps
	ResetAllMapsEx(NO, YES /*applyMtoK*/);

	ResetMap( &m_BlackMap, m_nPoints, NO );
	m_BlackMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_BlackMap);
	ResetMap( &m_RedMap, m_nPoints, NO );
	m_RedMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_RedMap);
	ResetMap( &m_GreenMap, m_nPoints, NO );
	m_GreenMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_GreenMap);
	ResetMap( &m_BlueMap, m_nPoints, NO );
	m_BlueMap.iCurves = m_bCurves ? m_nCurveType:0;
	MakeMap( &m_BlueMap);

	m_fCalAll = TRUE;
	HandleScanMapInit();

    CPPCalibDlg::OnInitDialog();
	return(TRUE);
}

BOOL CScanVisualDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    BOOL bReturn = TRUE;
    HWND hDlg = this->GetSafeHwnd();
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

	switch (nID)
	{
	case IDC_CALMETHOD:
		if ( !(nID = HandleCombo( hDlg, nID, nCode )) )
    		break;
    	m_fScanCalVisual = nID == IDC_CALVISUAL;
    	if (!m_fScanCalVisual)
		{
            PostDoModal();
			HandleMapEnd();
        	EndDialog(IDD_CALIBRATESCAN);
		}
    	break;

	case IDC_SCAN_CHANNELS:
		HandleScanChannels(nID, nCode);
	    break;

	case IDC_CALALLSAME:
    	HandleScanCalAllSame( nID, nCode);
    	break;

	case IDC_CALRGB:
    	m_fUseGrayForRGB = !m_fUseGrayForRGB;
		CheckDlgButton(IDC_CALRGB, m_fUseGrayForRGB);
    	break;
    
	case IDC_CALGRAY:
    	m_fUseRGBForGray = !m_fUseRGBForGray;
    	CheckDlgButton(IDC_CALGRAY, m_fUseRGBForGray);
    	break;

	case IDC_MAPLOAD:
        {
            CLoadDlg *pDlg = new CLoadDlg(IDN_SCANMAP, IDD_SCANMAP_LOAD,this,
                &m_MasterMap, &m_RedMap,  &m_GreenMap, &m_BlueMap, &m_BlackMap);

            if (pDlg)
            {
                int ret = pDlg->DoModal();
                delete pDlg;
                if (ret != IDOK)
                    break;
            }

	    	HandleScanMapLoad();
        }
	    break;

    case IDOK:
    case IDCANCEL:
        bReturn = FALSE; // let this be handled by the OnOK and OnCancel
        break;

	default:
		if (HandleMapCommand(nID, nCode))
			ControlEnable(hDlg, IDOK, m_MapValues.fModified);
	    break;
   }

    return bReturn ? bReturn : CPPCalibDlg::OnCommand(wParam, lParam);
}


void CScanVisualDlg::OnOK()
{
    HWND hDlg = this->GetSafeHwnd();
	if (HandleScanSave( ))
	{
		HandleMapEnd();
		CPPCalibDlg::OnOK();
        return ;
	}

    OnCancel();
}

void CScanVisualDlg::OnCancel()
{
	HandleMapEnd();
    CPPCalibDlg::OnCancel();
}


// This dialog is the print calibration file editor; edit Names.ScanMap

 ///////////////////////////////////////////////////////////////////////
// CScanEditDlg Methods

CScanEditDlg::CScanEditDlg(UINT nIDTemplate, CWnd *pParentWnd) : 
    CPPCalibDlg(NULL,nIDTemplate/*IDD_CALIBRATE*/, pParentWnd)
{
    m_fCalAll = FALSE;
}

BOOL CScanEditDlg::OnInitDialog()
{
    HWND hDlg = this->GetSafeHwnd();

    PreDoModal (NULL);
    CenterPopup( hDlg );
	LoadScanCalibrate( Names.ScanMap );

	m_Scan_type = FDT_RGBCOLOR;
	lstrcpy(m_Map_Name, Names.ScanMap);
	HandleScanMapLoad( );
	HandleScanMapInit( );
	CheckComboItem(hDlg, IDC_SCANMAPTYPE, IDC_SCANMAPGRAY, IDC_SCANMAPCOLOR,
			m_Scan_type == FDT_GRAYSCALE ? IDC_SCANMAPGRAY : IDC_SCANMAPCOLOR);

    CPPCalibDlg::OnInitDialog();
    return(TRUE);
}

BOOL CScanEditDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = this->GetSafeHwnd();
    BOOL bReturn = TRUE;
    UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

	switch (nID)
	{
	case IDC_SCANMAPTYPE:
	    if ( !(nID = HandleCombo( hDlg, nID, nCode )) )
    		break;
    	UpdateScanMaps( );
	    m_Scan_type = ((nID == IDC_SCANMAPGRAY) ? FDT_GRAYSCALE : FDT_RGBCOLOR);
    	HandleScanMapLoad( );
    	break;

	case IDC_SCAN_CHANNELS:
    	HandleScanChannels(nID, nCode);
    	break;

	case IDC_CALALLSAME:
    	HandleScanCalAllSame( nID, nCode );
    	break;

	case IDC_CALRGB:
    	m_fUseGrayForRGB = !m_fUseGrayForRGB;
    	CheckDlgButton(IDC_CALRGB, m_fUseGrayForRGB);
    	break;

	case IDC_CALGRAY:
    	m_fUseRGBForGray = !m_fUseRGBForGray;
    	CheckDlgButton(IDC_CALGRAY, m_fUseRGBForGray);
    	break;

    case IDOK:
    case IDCANCEL:
        bReturn = FALSE; // let this be handled by the OnOK and OnCancel
        break;

	default:
		if (HandleMapCommand(nID, nCode))
			ControlEnable(hDlg, IDOK, m_MapValues.fModified);
	    break;
   	}
    return bReturn ? bReturn : CPPCalibDlg::OnCommand(wParam, lParam);
}

void CScanEditDlg::OnOK()
{
    HWND hDlg = this->GetSafeHwnd();
   	if (!HandleScanSave( ))
    {
        CDialog::OnCancel();
        return ;
    }
	lstrcpy(Names.ScanMap, m_Map_Name);

    CDialog::OnOK();
}

void CScanEditDlg::OnCancel()
{
    // Load the old scan calibration file from disk
   	LoadScanCalibrate( Names.ScanMap );
    CDialog::OnCancel();
}


////////////////////////////////////////////////////////////////////////////
// CLoadDlg Methods

CLoadDlg::CLoadDlg(ITEMID MapType, UINT nIDTemplate, CWnd *pParentWnd,
    MAP *pMasterMap, MAP *pRedMap,  MAP *pGreenMap,  MAP *pBlueMap,  MAP *pBlackMap) :
    CPPCalibDlg(NULL,nIDTemplate/*IDD_CALMAP_LOAD*/, pParentWnd)
{
    m_MapType = MapType;
    m_pMasterMap = pMasterMap;
    m_pRedMap    = pRedMap;
    m_pGreenMap  = pGreenMap;
    m_pBlueMap   = pBlueMap;
    m_pBlackMap  = pBlackMap;
}

BOOL CLoadDlg::OnInitDialog()
{
    HWND hDlg = this->GetSafeHwnd();

    PreDoModal (NULL);
    CenterPopup( hDlg );
    lstrcpy(Names.Saved, m_Map_Name);
    InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, m_Map_Name, m_MapType );

    CPPCalibDlg::OnInitDialog();
    return(TRUE);
}

BOOL CLoadDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    BOOL bReturn = TRUE;
    HWND hDlg = this->GetSafeHwnd();
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

    switch (nID)
	{
	case IDC_EXTNAMES:
	    GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
    		m_Map_Name, m_MapType, nCode );
	    break;

	case IDC_EXTMANAGE:
    	PopupMenu( hDlg, nID, IDC_FILENAME );
	    break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
    	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, m_Map_Name,
	    	m_MapType, nID, NO );
    	break;

    default:
        bReturn = FALSE;    
    	break;
   }
    return bReturn ? bReturn : CPPCalibDlg::OnCommand(wParam, lParam);
}

void CLoadDlg::OnOK()
{
    FNAME szFileName;
    if (! LookupExtFile(m_Map_Name, szFileName, m_MapType) )
    {
        CDialog::OnCancel();
        return ;
    }

   	if (!LoadCalMap(m_pMasterMap,
   	    			m_pRedMap,
   		    		m_pGreenMap,
   			    	m_pBlueMap,
   				    m_pBlackMap, 
                    szFileName ))
    {
        CDialog::OnCancel();
        return ;
    }

    CDialog::OnOK();
}

void CLoadDlg::OnCancel()
{
  	lstrcpy(m_Map_Name, Names.Saved);
    CDialog::OnCancel();
}


////////////////////////////////////////////////////////////////////////////
// CCalibrateApplyDlg Methods

CCalibrateApplyDlg::CCalibrateApplyDlg(UINT nIDTemplate, CWnd* pParentWnd) 
    : CPPModalDlg ( nIDTemplate/*IDD_CALIBRATEAPPLY*/, pParentWnd)
{
    m_CalibrateApply_lpParms = NULL;
}

BOOL CCalibrateApplyDlg::OnInitDialog()
{
    ASSERT(m_CalibrateApply_lpParms!=NULL); // call PreDomodal w/ Calib parms
    HWND hDlg = this->GetSafeHwnd();

    CenterPopup( hDlg );
    InitExtNameN( hDlg, IDC_CALSCANNAMES, IDC_CALSCANFILE,
    	m_szScanMap, IDN_SCANMAP, IDS_NONE, NO );
    InitExtNameN( hDlg, IDC_CALPRINTNAMES, IDC_CALPRINTFILE,
    	m_szPrintMap, IDN_CALMAP, IDS_NONE, NO );
    SetWindowWord( ::GetDlgItem(hDlg,IDC_CALSCANMGR), GWW_ICONID,
    	IDC_EXTMANAGE );
    SetWindowWord( ::GetDlgItem(hDlg,IDC_CALPRINTMGR), GWW_ICONID,
    	IDC_EXTMANAGE );

    CPPModalDlg::OnInitDialog();
    return(TRUE);
}

BOOL CCalibrateApplyDlg::PreDoModal(LPVOID lpData)
{
    m_CalibrateApply_lpParms = (LPAPPLYCAL_PARMS)lpData;
    lstrcpy( m_szScanMap,  Names.ScanMap );
    lstrcpy( m_szPrintMap, PrintStyle.CalibrateName );
    
    return CPPModalDlg::PreDoModal(lpData);
}

BOOL CCalibrateApplyDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = this->GetSafeHwnd();
    BOOL bReturn = TRUE;
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

    switch (nID)
    {
    case IDC_CALSCANNAMES:
    	if ( nCode != CBN_SELCHANGE )
    		break;
    	GetExtNameN( hDlg, nID, IDC_CALSCANFILE, m_szScanMap,
    		IDN_SCANMAP, nCode, IDS_NONE, NO );
        break;
    
    case IDC_CALPRINTNAMES:
    	if ( nCode != CBN_SELCHANGE )
    		break;
    	GetExtNameN( hDlg, nID, IDC_CALPRINTFILE,
    		m_szPrintMap, IDN_CALMAP, nCode,
    		IDS_NONE, NO );
        break;
    
    case IDC_CALSCANMGR:
    	m_idListToManage = nID;
    	PopupMenuEx( hDlg, nID, IDC_EXTMANAGE, IDC_CALSCANFILE );
        break;
    
    case IDC_CALPRINTMGR:
    	m_idListToManage = nID;
    	PopupMenuEx( hDlg, nID, IDC_EXTMANAGE, IDC_CALPRINTFILE );
        break;
    
    case IDC_ADDEXT:
    case IDC_DELETEEXT:
    case IDC_RENAMEEXT:
    switch (m_idListToManage )
    	{
    	case IDC_CALSCANMGR:
    		ExtNameManager( hDlg, IDC_CALSCANNAMES, IDC_CALSCANFILE,
    			m_szScanMap, IDN_SCANMAP, nID, NO );
    		break;
    
    	case IDC_CALPRINTMGR:
    		ExtNameManager( hDlg, IDC_CALPRINTNAMES, IDC_CALPRINTFILE,
    			m_szPrintMap, IDN_CALMAP, nID, NO );
    		break;
    	}
        break;
    
    default:
        bReturn = FALSE;
        break;
   }

    return bReturn ? bReturn : CPPModalDlg::OnCommand(wParam, lParam);
}

void CCalibrateApplyDlg::OnOK()
{
    if ( StringsEqualID( m_szScanMap, IDS_NONE ) )
    	m_szScanMap[0] = 0;
    
    if ( StringsEqualID( m_szPrintMap, IDS_NONE ) )
    	m_szPrintMap[0] = 0;
    
    lstrcpy(m_CalibrateApply_lpParms->szScanMap, m_szScanMap);
    lstrcpy(m_CalibrateApply_lpParms->szPrintMap, m_szPrintMap);

    CDialog::OnOK();
}
    
void CCalibrateApplyDlg::OnCancel()
{
    CDialog::OnCancel();
}    

//////////////////////////////////////////////////////////////////////
// CPPCalibDlg:: methods
CPPCalibDlg::CPPCalibDlg (CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd)
    : CPPMapDlg (pView,nIDTemplate, pParentWnd)
{
}

BOOL CPPCalibDlg::PreDoModal(LPVOID lpVoid)
{
    m_bAutoPreview = GetDefaultInt(Map_bAuto_Str,YES);
	m_nPoints = GetDefaultInt(Map_nPoints_Str,3);
	m_bGrid = GetDefaultInt(Map_bGrid_Str,YES);
	m_bPercentages = GetDefaultInt(Map_bPercent_Str,YES);
	m_bIntensity = GetDefaultInt(Map_bIntensity_Str,NO);
	m_bCurves = GetDefaultInt(Map_bCurves_Str,YES);
	m_nCurveType = GetDefaultInt(Map_CurveType_Str,SMOOTH_NORMAL);

    m_fScanCalVisual = GetDefaultInt(fScanCalVisual_Str,NO);
    m_fUseRGBForGray = Global_fUseRGBForGray; 
    m_fUseGrayForRGB = Global_fUseGrayForRGB;
    m_fCalAll = Global_fCalAll;

	return CPPMapDlg::PreDoModal(lpVoid);
}

LPVOID CPPCalibDlg::PostDoModal()
{
    PutDefaultInt( Map_nPoints_Str, m_nPoints);
    PutDefaultInt( Map_bGrid_Str, m_bGrid);
    PutDefaultInt( Map_bPercent_Str, m_bPercentages);
    PutDefaultInt( Map_bIntensity_Str, m_bIntensity);
    PutDefaultInt( Map_bCurves_Str, m_bCurves);
    PutDefaultInt( Map_CurveType_Str, m_nCurveType);
    PutDefaultInt( Map_bAuto_Str, m_bAutoPreview);
    PutDefaultInt( fScanCalVisual_Str, m_fScanCalVisual );

    Global_fUseRGBForGray = m_fUseRGBForGray;   
    Global_fUseGrayForRGB = m_fUseGrayForRGB;
    Global_fCalAll = m_fCalAll;

	return CPPMapDlg::PostDoModal();
}

BOOL CPPCalibDlg::OnInitDialog()
{
    return CPPMapDlg::OnInitDialog();
}

void CPPCalibDlg::OnCancel()
{
	PostDoModal();
    CPPMapDlg::OnCancel();
}

void CPPCalibDlg::OnOK()
{
	PostDoModal();
    CPPMapDlg::OnOK();
}

void CPPCalibDlg::HandleScanMapInit( )
{
	LPIMAGE lpImage;

	m_Scan_type = FDT_RGBCOLOR;
	if (m_pView->GetSafeHwnd())
	{
		lpImage = m_pView->GetDocument()->m_lpImage;
		m_Scan_type = FrameType(ImgGetBaseEditFrame(lpImage));
	}

	if (m_Scan_type == FDT_GRAYSCALE || m_fCalAll)
		m_MapValues.wChannel = IDC_DO_ALL;
	else
	{
		m_Scan_type = FDT_RGBCOLOR;
		m_MapValues.wChannel = IDC_DO_RED;
	}

	HandleMapInit( m_Scan_type, TRUE);

	SetupScanMapControls( );
}

void CPPCalibDlg::HandleScanMapLoad( )
{
	BOOL fGraySame;

	m_fCalAll = TRUE;
	if (m_fCalAll = MemCompare((LPTR)&m_RedMap, (LPTR)&m_GreenMap, sizeof(MAP)))
		m_fCalAll = MemCompare((LPTR)&m_GreenMap, (LPTR)&m_BlueMap, sizeof(MAP));

	if (fGraySame = m_fCalAll)
		fGraySame = MemCompare((LPTR)&m_BlackMap, (LPTR)&m_RedMap, sizeof(MAP));

	if (m_Scan_type == FDT_GRAYSCALE)
	{
		m_fUseGrayForRGB = fGraySame;
		m_MasterMap = m_BlackMap;
		// we must invert the map because the mapping dialog assumes
		// the points are grayscale and then sets the reverse flag
		// to make it work on ink. A grayscale scan make has points
		// that are setup to work on ink, but then has a reverse flag
		// to make it work on grayscale.  If mapping stuff paid correct
		// attention to the reverse flag, this would not be a proble.
		InvertMap(&m_MasterMap);
		MakeMap(&m_MasterMap);
        m_MapValues.wChannel = IDC_DO_ALL;
	}
	else
	{
		m_fUseRGBForGray = fGraySame;
		if (m_fCalAll)
		{
			m_MasterMap = m_RedMap;
			// flip to make it work on ink
			ReverseMap(&m_MasterMap, TRUE);
			m_MapValues.wChannel = IDC_DO_ALL;
		}
		else
			m_MapValues.wChannel = IDC_DO_RED;
	}
	MapSetGradients();
	HandleMapLoad(); 
	SetupScanMapControls( );
}

void CPPCalibDlg::HandleScanChannels( int id, UINT codeNotify )
{
	if ( !(id = HandleCombo( GetSafeHwnd(), id, codeNotify )) )
		return;
	UpdateCurrentMap();
	m_MapValues.wChannel = id - IDC_SCAN_RED + IDC_DO_RED;
	MapSetGradients();
	m_MapValues.pMap = ResetMapChannel();
}

void CPPCalibDlg::HandleScanCalAllSame( int id, UINT codeNotify )
{
	m_fCalAll = !m_fCalAll;
	CheckDlgButton( IDC_CALALLSAME, m_fCalAll);

	if (m_fCalAll)
	{
		ResetAllMapsEx(NO, YES /*applyMtoK*/);
        m_MasterMap = *m_MapValues.pMap;
		ReverseMap(&m_MasterMap, TRUE);
	}
	else
	{
		ReverseMap(&m_MasterMap, TRUE);
		m_RedMap = m_MasterMap;
		m_GreenMap = m_MasterMap;
		m_BlueMap = m_MasterMap;
		ResetMapEx( &m_MasterMap, m_nPoints, NO, YES );
		m_MasterMap.iCurves = m_bCurves ? m_nCurveType:0;
		MakeMap( &m_MasterMap);
	}

	m_MapValues.wChannel = IDC_DO_RED;
	CheckComboItem(GetSafeHwnd(), IDC_SCAN_CHANNELS, IDC_SCAN_RED, IDC_SCAN_BLUE,
 				m_MapValues.wChannel - IDC_DO_RED + IDC_SCAN_RED);
	if (m_fCalAll)
		m_MapValues.wChannel = IDC_DO_ALL;

	MapSetGradients();
	m_MapValues.pMap = ResetMapChannel( );
	AutoPreview(m_MapValues.pMap, m_bAutoPreview, YES);

	ControlEnable(GetSafeHwnd(), IDC_CALGRAY, m_fCalAll);
	ControlEnable(GetSafeHwnd(), IDC_SCAN_CHANNELS, !m_fCalAll);
	ControlEnable(GetSafeHwnd(), IDC_RESETALL, !m_fCalAll && m_Scan_type != FDT_GRAYSCALE);
}

BOOL CPPCalibDlg::HandleScanSave( )
{
	UpdateScanMaps( );
	SaveScanCalibrate( m_Map_Name);
	return(TRUE);
}

void CPPCalibDlg::UpdateScanMaps( )
{
	ReverseMap(&m_MasterMap, TRUE);
	// make map work on ink instead of grayscale
	if (m_Scan_type == FDT_GRAYSCALE)
	{
		if (m_fUseGrayForRGB)
		{
			m_RedMap = m_MasterMap;
			m_GreenMap = m_MasterMap;
			m_BlueMap = m_MasterMap;
		}
		ReverseMap(&m_MasterMap, TRUE);
		InvertMap(&m_MasterMap);
		m_BlackMap = m_MasterMap;
	}
	else
	{
		if (m_fCalAll)
		{
			m_RedMap = m_MasterMap;
			m_GreenMap = m_MasterMap;
			m_BlueMap = m_MasterMap;
			if (m_fUseRGBForGray)
			{
				ReverseMap(&m_MasterMap, TRUE);
				InvertMap(&m_MasterMap);
				m_BlackMap = m_MasterMap;
			}
		}
	}
}
BOOL CPPCalibDlg::LoadScanCalibrate( LPSTR lpExtName )
{
    
    LPSTR lp;
    FNAME szFileName;
    int i;
    BOOL fDoCalibration;        

    if ( !LookupExtFile( lpExtName, szFileName, IDN_SCANMAP ) )
	{
	    if ( AstralStr( IDS_NONE, &lp ) )
    		lstrcpy( lpExtName, lp );
	}

    if ( !(fDoCalibration = !StringsEqualID( lpExtName, IDS_NONE )) )
    	szFileName[0] = '\0';

    ResetMap( &m_BlackMap, m_nPoints, NO  );
    ResetMap( &m_RedMap, m_nPoints, NO  );
    ResetMap( &m_GreenMap, m_nPoints, NO  );
    ResetMap( &m_BlueMap, m_nPoints, NO  );
   	m_BlackMap.iCurves = m_RedMap.iCurves = m_GreenMap.iCurves = 
       	m_BlueMap.iCurves = m_bCurves ? m_nCurveType:0;
    MakeMap( &m_BlackMap );
    MakeMap( &m_RedMap );
    MakeMap( &m_GreenMap );
    MakeMap( &m_BlueMap );


    if ( szFileName[0] )
    	LoadMap(&m_BlackMap, &m_RedMap, &m_GreenMap, &m_BlueMap, szFileName );

    return( fDoCalibration );
}


BOOL CPPCalibDlg::SaveScanCalibrate( LPSTR lpExtName )
{
    LPSTR lp;
    FNAME szFileName, szNewExtName;

    lstrcpy( szNewExtName, lpExtName );
    if ( StringsEqualID( szNewExtName, IDS_NONE ) && AstralStr( IDS_UNTITLED, &lp ) )
    	lstrcpy( szNewExtName, lp );

    szFileName[0] = '\0';
    if ( !PromptForExtName( GetSafeHwnd(), szNewExtName, szFileName, 
        IDN_SCANMAP ) )
    	return( FALSE );
    if ( !szFileName[0] )
    	return( FALSE );

    if ( !SaveMap( &m_BlackMap, &m_RedMap, &m_GreenMap,
    	&m_BlueMap, szFileName ) )
    		return( FALSE );

    if ( !AddExtFile( szNewExtName, szFileName, IDN_SCANMAP ) )
	{
    	FileDelete( szFileName );
    	return( FALSE );
	}
    lstrcpy( lpExtName, szNewExtName );
    return( TRUE );
}

void CPPCalibDlg::SetupScanMapControls( )
{
	ShowDlgItem(GetSafeHwnd(), IDC_CALRGB, m_Scan_type == FDT_GRAYSCALE);
	ShowDlgItem(GetSafeHwnd(), IDC_CALGRAY, m_Scan_type != FDT_GRAYSCALE);
	ShowDlgItem(GetSafeHwnd(), IDC_CALALLSAME, m_Scan_type != FDT_GRAYSCALE);

	CheckDlgButton(IDC_CALALLSAME, m_fCalAll);
	CheckDlgButton(IDC_CALGRAY, m_fUseRGBForGray);
	CheckDlgButton(IDC_CALRGB, m_fUseGrayForRGB);
	ControlEnable(GetSafeHwnd(), IDC_CALGRAY, m_fCalAll);
	ControlEnable(GetSafeHwnd(), IDC_RESETALL, !m_fCalAll && m_Scan_type != FDT_GRAYSCALE);

	if (m_Scan_type == FDT_GRAYSCALE)
	{
		CheckComboItem(GetSafeHwnd(), IDC_SCAN_CHANNELS, IDC_SCAN_GRAY, IDC_SCAN_GRAY,
							IDC_SCAN_GRAY);
		ControlEnable(GetSafeHwnd(), IDC_SCAN_CHANNELS, FALSE);
	}
	else
	{
		CheckComboItem(GetSafeHwnd(), IDC_SCAN_CHANNELS, IDC_SCAN_RED, IDC_SCAN_BLUE,
						IDC_SCAN_RED);
		ControlEnable(GetSafeHwnd(), IDC_SCAN_CHANNELS, !m_fCalAll);
	}                                                    
}

