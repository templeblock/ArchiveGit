// monitor.cpp : implementation file
//

#include "pp.h"
#include "monitor.h"
#include "ppafx.h"
#include "svrview.h"

extern LPSTR RGamma_Str;
extern LPSTR GGamma_Str;
extern LPSTR BGamma_Str;
extern LPSTR GammaLock_Str;
extern LPSTR GammaDisable_Str;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_GAMMA 400

/////////////////////////////////////////////////////////////////////////////
// CMonitorGammaPage property page

IMPLEMENT_DYNCREATE(CMonitorGammaPage, CPPPropertyPage)
 
CMonitorGammaPage::CMonitorGammaPage() : CPPPropertyPage(CMonitorGammaPage::IDD)
{
	//{{AFX_DATA_INIT(CMonitorGammaPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	BLTSETTINGS Settings;

	m_lpGammaImage = NULL;
	GetGammaSettings(&Settings);

    m_RGammaCurr=Settings.RGamma;
    m_GGammaCurr=Settings.GGamma;
    m_BGammaCurr=Settings.BGamma;
	m_fGammaDisable=Settings.fGammaDisable;

	m_RGamma = m_RGammaCurr;
	m_GGamma = m_GGammaCurr;
	m_BGamma = m_BGammaCurr;
	m_fGammaLock=GetDefaultInt(GammaLock_Str,NO);
}

CMonitorGammaPage::~CMonitorGammaPage()
{
	if (PageEverActive()) // have we ever used this page
	{
		if (m_lpGammaImage)
		{
			// unplug the palette begin used
			HWND hWnd = GetActiveDoc();
			if (hWnd)
			{
				CServerView *pView = (CServerView *)CWnd::FromHandle(hWnd);
				if (pView)
					pView->SetupImageDisplayPalette();
			}
			else
			{
				SetNewPalette(lpBltScreen,
							lpBltScreen->ColorPaletteType,
							lpBltScreen->hColorPal,
							lpBltScreen->lpColorPaletteLUT,
							lpBltScreen->ColorPalette,
							lpBltScreen->nColorEntries, FALSE /* fApplyGamma */);
			}

			delete m_lpGammaImage;
			m_lpGammaImage = NULL;
		}
		SetDisplayGamma(lpBltScreen);
	}
}

void CMonitorGammaPage::SetupGammaImage()
{
	FNAME szFileName;
	FRMTYPEINFO TypeInfo;
	int i;
	WORD wRGB;
	LPCOLORMAP lpColorMap;
	LPTR lpPaletteLUT;

	lstrcpy(szFileName, Control.ProgHome);
	lstrcat(szFileName, "ppgamma.tif");
	CReadImage read(szFileName);
	m_lpGammaImage = read.ReadImage();
	if (m_lpGammaImage)
	{
		m_lpGammaImage->GetTypeInfo(&TypeInfo);
		// if we are on a 8-bit display and the gamma
		// image is a palette color image and
		// it has more colors than we can display
		// set the colors to the max colors we can display
		// assumning that the important colors are in the
		// front of the color table.  We do this so important
		// colors don't get reduced out of the palette
		if (lpBltScreen->BitMapInfo.bmiHeader.biBitCount == 8 &&
			TypeInfo.DataType == FDT_PALETTECOLOR &&
			TypeInfo.ColorMap->NumEntries > MAX8BITCOLORS)
		{
			TypeInfo.ColorMap->NumEntries = MAX8BITCOLORS;
		}
		// set up the palettes and stuff for the image
		ImgInitDisplay(m_lpGammaImage, FALSE);
		// we want to create the palette lookup table ourselves
		// se that we can do it fast, cause we only care about
		// our colors
		if (m_lpGammaImage->lpPaletteLUT)
		{
			// make everybody map to entry 0
			lclr(m_lpGammaImage->lpPaletteLUT, 32768L);
			// setup the entries for our image only, everybody
			// else should paint black or the color of the
			// first entry
			lpColorMap = TypeInfo.ColorMap;
			lpPaletteLUT = m_lpGammaImage->lpPaletteLUT;
			for (i = 0; i < lpColorMap->NumEntries; ++i)
			{
				wRGB = RGBtoMiniRGB(&lpColorMap->RGBData[i]);
				lpPaletteLUT[wRGB] = i;
			}
		}

		if (m_lpGammaImage->hPal)
		{
			
			SetNewPalette(lpBltScreen, m_lpGammaImage->PaletteType, m_lpGammaImage->hPal,
						m_lpGammaImage->lpPaletteLUT, m_lpGammaImage->Palette,
						m_lpGammaImage->nPaletteEntries, FALSE /* fApplyGamma */);
			SetSuperBltGamma( lpBltScreen, FUNITY, FUNITY, FUNITY, TRUE);
		}
	  	SetWindowLong(GetDlgItem(IDC_GAMMA)->GetSafeHwnd(), GWL_IMAGE, (long)m_lpGammaImage->GetBaseEditFrame());
	  	AstralControlRepaint(GetSafeHwnd(), IDC_GAMMA);
	}
}

void CMonitorGammaPage::ModifyGamma()
{
	LPPALETTEENTRY lpEntry, lpNewEntry;
	PALETTEENTRY TempEntries[256+1];
	int i;
	HDC hDC;

	if (!m_lpGammaImage)
		return;

	HWND hDlg = GetSafeHwnd();
	if (lpBltScreen->hPal && lpBltScreen->bPaletteGamma)
 	{
		BuildGammaLut(m_RGamma, m_RedMap.Lut);
		BuildGammaLut(m_GGamma, m_GreenMap.Lut);
		BuildGammaLut(m_BGamma, m_BlueMap.Lut);

		lpEntry = m_lpGammaImage->Palette;
		lpNewEntry = TempEntries;
		for ( i=0; i<m_lpGammaImage->nPaletteEntries; i++ )
 		{
			lpNewEntry->peFlags = lpEntry->peFlags;
			lpNewEntry->peRed = m_RedMap.Lut[lpEntry->peRed];
			lpNewEntry->peGreen = m_GreenMap.Lut[lpEntry->peGreen];
			lpNewEntry->peBlue = m_BlueMap.Lut[lpEntry->peBlue];
			lpEntry++;
			lpNewEntry++;
 		}
		hDC = ::GetDC(hDlg);
 		SelectPalette(hDC, m_lpGammaImage->hPal, FALSE);
		RealizePalette(hDC);
		::ReleaseDC(hDlg, hDC);
		AnimatePalette(m_lpGammaImage->hPal, 0, m_lpGammaImage->nPaletteEntries, TempEntries);
 	}
	else
 	{
		SetSuperBltGamma( lpBltScreen, m_RGamma, m_GGamma,
                  	m_BGamma, m_fGammaDisable );
		AstralControlRepaint( hDlg, IDC_GAMMA );
 	}
}				  

void CMonitorGammaPage::SetupNew(int idCtl, LFIXED fGamma )
{
	HWND hDlg = GetSafeHwnd();
	if (m_fGammaLock)
		m_RGamma = m_GGamma = m_BGamma = fGamma;

	if ((idCtl != IDC_RGAMMA_EDIT && m_fGammaLock) ||
			idCtl == IDC_RGAMMA_SCROLL)
		SetDlgItemSpinFixed( hDlg, IDC_RGAMMA_EDIT, m_RGamma, NO );
	if ((idCtl != IDC_GGAMMA_EDIT && m_fGammaLock) ||
			idCtl == IDC_GGAMMA_SCROLL)
		SetDlgItemSpinFixed( hDlg, IDC_GGAMMA_EDIT, m_GGamma, NO );
	if ((idCtl != IDC_BGAMMA_EDIT && m_fGammaLock) ||
			idCtl == IDC_BGAMMA_SCROLL)
		SetDlgItemSpinFixed( hDlg, IDC_BGAMMA_EDIT, m_BGamma, NO );

	if ((idCtl != IDC_RGAMMA_SCROLL && m_fGammaLock) ||
			idCtl == IDC_RGAMMA_EDIT)
		SetSlide( hDlg, IDC_RGAMMA_SCROLL, (int)FMUL(m_RGamma,100) );
	if ((idCtl != IDC_GGAMMA_SCROLL && m_fGammaLock) ||
			idCtl == IDC_GGAMMA_EDIT)
		SetSlide( hDlg, IDC_GGAMMA_SCROLL, (int)FMUL(m_GGamma,100) );
	if ((idCtl != IDC_BGAMMA_SCROLL && m_fGammaLock) ||
			idCtl == IDC_BGAMMA_EDIT)
		SetSlide( hDlg, IDC_BGAMMA_SCROLL, (int)FMUL(m_BGamma,100) );
	GetDlgItem(IDC_RGAMMA_EDIT)->UpdateWindow();
	GetDlgItem(IDC_GGAMMA_EDIT)->UpdateWindow();
	GetDlgItem(IDC_BGAMMA_EDIT)->UpdateWindow();
	GetDlgItem(IDC_RGAMMA_SCROLL)->UpdateWindow();
	GetDlgItem(IDC_GGAMMA_SCROLL)->UpdateWindow();
	GetDlgItem(IDC_BGAMMA_SCROLL)->UpdateWindow();

	ModifyGamma();
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorGammaPage message handlers

BOOL CMonitorGammaPage::HandleQueryNewPalette()
{
	HDC hDC;
	HWND hDlg = GetSafeHwnd();

	if (m_lpGammaImage && m_lpGammaImage->hPal)
	{
		SetNewPalette(lpBltScreen, m_lpGammaImage->PaletteType, m_lpGammaImage->hPal,
					m_lpGammaImage->lpPaletteLUT, m_lpGammaImage->Palette,
					m_lpGammaImage->nPaletteEntries, FALSE /* fApplyGamma */);
		hDC = ::GetDC(hDlg);
		SelectPalette(hDC, m_lpGammaImage->hPal, FALSE);
		RealizePalette(hDC);
		::ReleaseDC(hDlg, hDC);

		return(TRUE);
	}
	else
		return CPPPropertyPage::OnQueryNewPalette();
}

BOOL CMonitorGammaPage::OnInitDialog() 
{
 	CPPPropertyPage::OnInitDialog();
	
	SetupGammaImage();
	ModifyGamma();

	HWND hDlg = GetSafeHwnd();
	InitSlide( hDlg, IDC_RGAMMA_SCROLL,
		(int)FMUL(m_RGamma,100), 1, MAX_GAMMA );
	InitSlide( hDlg, IDC_GGAMMA_SCROLL,
		(int)FMUL(m_GGamma,100), 1, MAX_GAMMA );
	InitSlide( hDlg, IDC_BGAMMA_SCROLL,
		(int)FMUL(m_BGamma,100), 1, MAX_GAMMA );
	::CheckRadioButton(hDlg, IDC_UNLOCK, IDC_LOCK,
		m_fGammaLock ? IDC_LOCK : IDC_UNLOCK);
	InitDlgItemSpinFixed( hDlg, IDC_RGAMMA_EDIT, m_RGamma, NO, 0L, UNITY*4L );
	InitDlgItemSpinFixed( hDlg, IDC_GGAMMA_EDIT, m_GGamma, NO, 0L, UNITY*4L );
	InitDlgItemSpinFixed( hDlg, IDC_BGAMMA_EDIT, m_BGamma, NO, 0L, UNITY*4L );

	SetDlgItemSpinStep( hDlg, IDC_RGAMMA_EDIT, 1 ); // Use 1/10
	SetDlgItemSpinStep( hDlg, IDC_GGAMMA_EDIT, 1 ); // Use 1/10
	SetDlgItemSpinStep( hDlg, IDC_BGAMMA_EDIT, 1 ); // Use 1/10

	::CheckDlgButton(hDlg, IDC_GAMMA_DISABLE, m_fGammaDisable);
	ControlEnable(hDlg, IDC_RGAMMA_SCROLL, !m_fGammaDisable);
	ControlEnable(hDlg, IDC_RGAMMA_EDIT, !m_fGammaDisable);
	ControlEnable(hDlg, IDC_GGAMMA_SCROLL, !m_fGammaDisable);
	ControlEnable(hDlg, IDC_GGAMMA_EDIT, !m_fGammaDisable);
	ControlEnable(hDlg, IDC_BGAMMA_SCROLL, !m_fGammaDisable);
	ControlEnable(hDlg, IDC_BGAMMA_EDIT, !m_fGammaDisable);
	ControlEnable(hDlg, IDC_LOCK, !m_fGammaDisable);
	ControlEnable(hDlg, IDC_UNLOCK, !m_fGammaDisable);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMonitorGammaPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL 	Bool;
	int    	i;
	LFIXED 	fGamma;
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
		{
		case IDC_GAMMA_DISABLE:
			m_fGammaDisable = !m_fGammaDisable;
			CheckDlgButton(IDC_GAMMA_DISABLE, m_fGammaDisable);
			ControlEnable(hDlg, IDC_RGAMMA_SCROLL, !m_fGammaDisable);
			ControlEnable(hDlg, IDC_RGAMMA_EDIT, !m_fGammaDisable);
			ControlEnable(hDlg, IDC_GGAMMA_SCROLL, !m_fGammaDisable);
			ControlEnable(hDlg, IDC_GGAMMA_EDIT, !m_fGammaDisable);
			ControlEnable(hDlg, IDC_BGAMMA_SCROLL, !m_fGammaDisable);
			ControlEnable(hDlg, IDC_BGAMMA_EDIT, !m_fGammaDisable);
			ControlEnable(hDlg, IDC_LOCK, !m_fGammaDisable);
			ControlEnable(hDlg, IDC_UNLOCK, !m_fGammaDisable);
			ModifyGamma();
			SetModified();
		break;

		case IDC_LOCK:
		case IDC_UNLOCK:
			if ( GetFocus() != GetDlgItem( id ) )
				break;
			m_fGammaLock = (id == IDC_LOCK) ? TRUE : FALSE;
			CheckRadioButton(IDC_UNLOCK, IDC_LOCK,
				m_fGammaLock ? IDC_LOCK : IDC_UNLOCK);
			if (m_fGammaLock)
				SetupNew(id, m_RGamma);
			SetModified();
		break;

		case IDC_RGAMMA_EDIT:
		if ( GetFocus() != GetDlgItem( id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_RGamma =
			GetDlgItemSpinFixed( hDlg, id, &Bool, NO);
		SetupNew(id, m_RGamma);
			SetModified();
		break;

		case IDC_RGAMMA_SCROLL:
		if ( GetFocus() != GetDlgItem( id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, NULL );
		fGamma = FGET(i, 100);
		if (fGamma == m_RGamma)
			break;
		m_RGamma = fGamma;
		SetupNew(id, m_RGamma);
			SetModified();
		break;

		case IDC_GGAMMA_EDIT:
		if ( GetFocus() != GetDlgItem( id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_GGamma =
			GetDlgItemSpinFixed( hDlg, id, &Bool, NO);
		SetupNew(id, m_GGamma);
			SetModified();
		break;

		case IDC_GGAMMA_SCROLL:
		if ( GetFocus() != GetDlgItem( id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, NULL );
		fGamma = FGET(i, 100);
		if (fGamma == m_GGamma)
			break;
		m_GGamma = fGamma;
		SetupNew(id, m_GGamma);
			SetModified();
		break;

		case IDC_BGAMMA_EDIT:
		if ( GetFocus() != GetDlgItem( id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_BGamma =
			GetDlgItemSpinFixed( hDlg, id, &Bool, NO);
		SetupNew(id, m_BGamma);
			SetModified();
		break;

		case IDC_BGAMMA_SCROLL:
		if ( GetFocus() != GetDlgItem( id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, NULL );
		fGamma = FGET(i, 100);
		if (fGamma == m_BGamma)
			break;
		m_BGamma = fGamma;
		SetupNew(id, m_BGamma);
			SetModified();
		break;

   	default:
		return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}


void CMonitorGammaPage::HandleOK()
{
	char t[50];

	m_RGammaCurr = m_RGamma;
	m_GGammaCurr = m_GGamma;
	m_BGammaCurr = m_BGamma;
	PutDefaultString(RGamma_Str,FixedAscii(m_RGammaCurr,t,-1));
	PutDefaultString(GGamma_Str,FixedAscii(m_GGammaCurr,t,-1));
	PutDefaultString(BGamma_Str,FixedAscii(m_BGammaCurr,t,-1));
	PutDefaultInt( GammaLock_Str, m_fGammaLock );
	PutDefaultInt( GammaDisable_Str, m_fGammaDisable );
	CPPPropertyPage::HandleOK();
}
   
void CMonitorGammaPage::HandleCancel()
{
	CPPPropertyPage::HandleCancel();
}
   
/////////////////////////////////////////////////////////////////////////////
// CMonitorCMSPage property page

IMPLEMENT_DYNCREATE(CMonitorCMSPage, CPPPropertyPage)

CMonitorCMSPage::CMonitorCMSPage() : CPPPropertyPage(CMonitorCMSPage::IDD)
{
	//{{AFX_DATA_INIT(CMonitorCMSPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMonitorCMSPage::~CMonitorCMSPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorCMSPage message handlers
BOOL CMonitorCMSPage::OnInitDialog() 
{
	CPPPropertyPage::OnInitDialog();

	CmsMonitorSetup_OnInitDialog( GetSafeHwnd() );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMonitorCMSPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
		{
		case IDC_CMS_DST:
		case IDC_CMS_DSTLIST:
			if( codeNotify != CBN_SELCHANGE )
				break;
			CmsMonitorSetupProcessMsg( hDlg, id );
			SetModified();
		break;

		case IDC_STATUSINFO:
			AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg,
					IDD_CMS_DSTINFO, DlgCmsMonitorSetupInfoProc, ( LPARAM )hDlg );
		break;

   	default:
		return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CMonitorCMSPage::HandleOK()
{
	HWND	hDlg = GetSafeHwnd();
	CmsMonitorSetupProcessMsg( hDlg, IDOK );
	CPPPropertyPage::HandleOK();
}

void CMonitorCMSPage::HandleCancel()
{
	HWND	hDlg = GetSafeHwnd();
	CmsMonitorSetupProcessMsg( hDlg, IDCANCEL );
	CPPPropertyPage::HandleCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorPropSheet

IMPLEMENT_DYNAMIC(CMonitorPropSheet, CPPPropertySheet)

int CMonitorPropSheet::m_nLastPage = 0;

CMonitorPropSheet::CMonitorPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT nIDHelp)
	:CPPPropertySheet(nIDCaption, pParentWnd, nIDHelp, GetSelectPage())
{
	CommonConstruct();
}

CMonitorPropSheet::CMonitorPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT nIDHelp)
	:CPPPropertySheet(pszCaption, pParentWnd, nIDHelp, GetSelectPage())
{
	CommonConstruct();
}

void CMonitorPropSheet::CommonConstruct()
{
	AddPage(&m_GammaPage);
	if (Control.CMSEnabled)
		AddPage(&m_CMSPage);
}

CMonitorPropSheet::~CMonitorPropSheet()
{
	m_nLastPage = m_nCurPage;
}

UINT CMonitorPropSheet::GetSelectPage()
{
	int nPageCount = 2;
	if (!Control.CMSEnabled)
		nPageCount -= 1;
	if (nPageCount <= m_nLastPage)
		m_nLastPage = nPageCount - 1;
	return(m_nLastPage);
}

BEGIN_MESSAGE_MAP(CMonitorPropSheet, CPPPropertySheet)
	ON_WM_QUERYNEWPALETTE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMonitorPropSheet message handlers

afx_msg BOOL CMonitorPropSheet::OnQueryNewPalette()
{
	if (m_GammaPage.m_hWnd)
		return m_GammaPage.HandleQueryNewPalette();
	else
		return CPPPropertySheet::OnQueryNewPalette();
}													 

void GetGammaSettings(LPBLTSETTINGS lpSettings)
{
	char t[50];

    GetDefaultString(RGamma_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
    lpSettings->RGamma=AsciiFixed(t);
    GetDefaultString(GGamma_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
    lpSettings->GGamma=AsciiFixed(t);
    GetDefaultString(BGamma_Str,FixedAscii(FUNITY, t, 4),t,sizeof(t));
    lpSettings->BGamma=AsciiFixed(t);
	lpSettings->fGammaDisable=GetDefaultInt(GammaDisable_Str,NO);
}

BOOL SetDisplayGamma(LPBLT lpBlt)
{
	BLTSETTINGS Settings;

	GetGammaSettings(&Settings);
  	SetSuperBltGamma( lpBltScreen, Settings.RGamma, Settings.GGamma, Settings.BGamma,
						Settings.fGammaDisable);
	POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
	while (TemplatePos)
	{
		CDocTemplate * pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
		POSITION DocPos = pTemplate->GetFirstDocPosition();
		while (DocPos)
		{
    		CServerDoc *pDoc = 
				(CServerDoc *)pTemplate->GetNextDoc (DocPos);    
    		if (pDoc)
				pDoc->UpdateAllViews(NULL);
   		}
	}
	return(TRUE);
}

  
