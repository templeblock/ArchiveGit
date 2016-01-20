// prnstyle.cpp : implementation file
//

#include "pp.h"
#include "prnstyle.h"
#include "ppafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define BLACKLUT_TIMER 	1
#define CALLUT_TIMER 	2
#define REDISPLAY_WAIT	500

void LOCAL MapSetGradients(HWND hDlg, BOOL bIntensity, WORD wChannel);


/////////////////////////////////////////////////////////////////////////////
// CSeparatePage property page

IMPLEMENT_DYNCREATE(CSeparatePage, CPPPropertyPage)
 
CSeparatePage::CSeparatePage() : CPPPropertyPage(CSeparatePage::IDD)
{
}

CSeparatePage::~CSeparatePage()
{
}

void CSeparatePage::InitBlackLuts()
{
	HWND	hDlg = GetSafeHwnd();

	int i;
	CFrameTypeConvert TypeConvert;
	FRMTYPEINFO inType, outType;
	RGBS rgb;

	InitPrintStyle(&Separate, &Calibrate);
	LPRGB lpRGB = (LPRGB)Alloc(256L*3L);
	LPCMYK lpCMYK = (LPCMYK)Alloc(256L*4L);
	FrameSetTypeInfo(&inType, FDT_RGBCOLOR);
	FrameSetTypeInfo(&outType, FDT_CMYKCOLOR);
	if (lpRGB && lpCMYK && 
		TypeConvert.Init(inType, outType, 256))
	{
		for (i = 0; i < 256; ++i)
			lpRGB[i].red = lpRGB[i].green = lpRGB[i].blue = 255-i;
		TypeConvert.ConvertData((LPTR)lpRGB, (LPTR)lpCMYK, 0, 256);
		Lut_SetLut(::GetDlgItem(hDlg, IDC_MAP), 4, (LPTR)lpCMYK);
		rgb.red = 0; rgb.green = rgb.blue = 255;
		Lut_SetColor(::GetDlgItem(hDlg, IDC_MAP), 0, &rgb); 
		rgb.red = rgb.blue = 255; rgb.green = 0;
		Lut_SetColor(::GetDlgItem(hDlg, IDC_MAP), 1, &rgb); 
		rgb.red = rgb.green = 255; rgb.blue = 0;
		Lut_SetColor(::GetDlgItem(hDlg, IDC_MAP), 2, &rgb); 
		rgb.red = rgb.green = rgb.blue = 0;
		Lut_SetColor(::GetDlgItem(hDlg, IDC_MAP), 3, &rgb); 
	}
	if (lpRGB)
		FreeUp(lpRGB);
	if (lpCMYK)
		FreeUp(lpCMYK);
}

void CSeparatePage::SepChanged()
{
	if (m_idTimer)
		KillTimer(m_idTimer);
	m_idTimer = SetTimer(BLACKLUT_TIMER, REDISPLAY_WAIT, NULL);
	if (!m_idTimer)
		InitBlackLuts();
}

BEGIN_MESSAGE_MAP(CSeparatePage, CPPPropertyPage)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeparatePage message handlers

BOOL CSeparatePage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	// black generation
	SetWindowWord( ::GetDlgItem(hDlg,IDC_BLACKMGR), GWW_ICONID,
			 		IDC_EXTMANAGE );

	InitExtNameN( hDlg, IDC_BLACKNAMES, IDC_BLACKFILE, Separate.BGName,
				IDN_BGMAP, IDS_NONE, NO);
	SetWindowLong( ::GetDlgItem( hDlg, IDC_CORRECT_RED ), 0, RGB(255,0,0) );
	SetWindowLong( ::GetDlgItem( hDlg, IDC_CORRECT_GREEN ), 0, RGB(0,255,0) );
	SetWindowLong( ::GetDlgItem( hDlg, IDC_CORRECT_BLUE ), 0, RGB(0,0,255) );
	::SetDlgItemInt( hDlg, IDC_CORRECT_RED, Separate.MfromR, YES );
	::SetDlgItemInt( hDlg, IDC_CORRECT_GREEN, Separate.YfromG, YES );
	::SetDlgItemInt( hDlg, IDC_CORRECT_BLUE, Separate.CfromB, YES );
	InitDlgItemSpin( hDlg, IDC_M_IN_R, bound(100+Separate.MfromR,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_Y_IN_R, bound(100-Separate.MfromR,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_Y_IN_G, bound(100+Separate.YfromG,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_C_IN_G, bound(100-Separate.YfromG,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_C_IN_B, bound(100+Separate.CfromB,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_M_IN_B, bound(100-Separate.CfromB,0,100), NO,
		0, 100 );

	InitDlgItemSpin( hDlg, IDC_UCR, Separate.UCR, NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_BLACKLIMIT, Separate.BlackLimit, NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_TOTALINK, Separate.TotalInk, NO, 0, 400 );
	InitDlgItemSpin( hDlg, IDC_SATBOOST, Separate.SatBoost, NO, 0, 100 );

	// initialize the lut control
	Lut_Init(::GetDlgItem(hDlg, IDC_MAP), 64, 64);
	InitBlackLuts();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSeparatePage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL 	Bool;
	int    	i;
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);
	CBlackGenerateDlg dlg(PictPubApp.GetActiveView());


	switch (id)
	{
		case IDC_CORRECT_RED:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			Separate.MfromR = ::GetDlgItemInt(hDlg, id, &Bool, YES);
			SetDlgItemSpin( hDlg, IDC_M_IN_R,
				bound(100+Separate.MfromR,0,100), NO );
			SetDlgItemSpin( hDlg, IDC_Y_IN_R,
				bound(100-Separate.MfromR,0,100), NO );
			SepChanged();
			SetModified();
		break;

		case IDC_CORRECT_GREEN:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			Separate.YfromG = ::GetDlgItemInt(hDlg, id, &Bool, YES);
			SetDlgItemSpin( hDlg, IDC_Y_IN_G,
				bound(100+Separate.YfromG,0,100), NO );
			SetDlgItemSpin( hDlg, IDC_C_IN_G,
				bound(100-Separate.YfromG,0,100), NO );
			SepChanged();
			SetModified();
		break;

		case IDC_CORRECT_BLUE:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			Separate.CfromB = ::GetDlgItemInt(hDlg, id, &Bool, YES);
			SetDlgItemSpin( hDlg, IDC_C_IN_B,
				bound(100+Separate.CfromB,0,100), NO );
			SetDlgItemSpin( hDlg, IDC_M_IN_B,
				bound(100-Separate.CfromB,0,100), NO );
			SepChanged();
			SetModified();
		break;

		case IDC_M_IN_R:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.MfromR = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			Separate.MfromR = bound( Separate.MfromR-100, -100, 100 );
			i = bound( 100-Separate.MfromR, 0, 100 );
			SetDlgItemSpin( hDlg, IDC_Y_IN_R, i, NO );
			::SetDlgItemInt( hDlg, IDC_CORRECT_RED, Separate.MfromR, YES );
			AstralControlRepaint( hDlg, IDC_CORRECT_RED );
			SepChanged();
			SetModified();
		break;

		case IDC_Y_IN_R:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.MfromR = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			Separate.MfromR = bound( 100-Separate.MfromR, -100, 100 );
			i = bound( 100+Separate.MfromR, 0, 100 );
			SetDlgItemSpin( hDlg, IDC_M_IN_R, i, NO );
			::SetDlgItemInt( hDlg, IDC_CORRECT_RED, Separate.MfromR, YES );
			AstralControlRepaint( hDlg, IDC_CORRECT_RED );
			SepChanged();
			SetModified();
		break;

		case IDC_Y_IN_G:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.YfromG = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			Separate.YfromG = bound( Separate.YfromG-100, -100, 100 );
			i = bound( 100-Separate.YfromG, 0, 100 );
			SetDlgItemSpin( hDlg, IDC_C_IN_G, i, NO );
			::SetDlgItemInt( hDlg, IDC_CORRECT_GREEN, Separate.YfromG, YES );
			AstralControlRepaint( hDlg, IDC_CORRECT_GREEN );
			SepChanged();
			SetModified();
		break;

		case IDC_C_IN_G:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.YfromG = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			Separate.YfromG = bound( 100-Separate.YfromG, -100, 100 );
			i = bound( 100+Separate.YfromG, 0, 100 );
			SetDlgItemSpin( hDlg, IDC_Y_IN_G, i, NO );
			::SetDlgItemInt( hDlg, IDC_CORRECT_GREEN, Separate.YfromG, YES );
			AstralControlRepaint( hDlg, IDC_CORRECT_GREEN );
			SepChanged();
			SetModified();
		break;

		case IDC_C_IN_B:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.CfromB = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			Separate.CfromB = bound( Separate.CfromB-100, -100, 100 );
			i = bound( 100-Separate.CfromB, 0, 100 );
			SetDlgItemSpin( hDlg, IDC_M_IN_B, i, NO );
			::SetDlgItemInt( hDlg, IDC_CORRECT_BLUE, Separate.CfromB, YES );
			AstralControlRepaint( hDlg, IDC_CORRECT_BLUE );
			SepChanged();
			SetModified();
		break;

		case IDC_M_IN_B:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.CfromB = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			Separate.CfromB = bound( 100-Separate.CfromB, -100, 100 );
			i = bound( 100+Separate.CfromB, 0, 100 );
			SetDlgItemSpin( hDlg, IDC_C_IN_B, i, NO );
			::SetDlgItemInt( hDlg, IDC_CORRECT_BLUE, Separate.CfromB, YES );
			AstralControlRepaint( hDlg, IDC_CORRECT_BLUE );
			SepChanged();
			SetModified();
		break;

		case IDC_UCR:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.UCR = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SepChanged();
			SetModified();
		break;

		case IDC_BLACKLIMIT:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.BlackLimit = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SepChanged();
			SetModified();
		break;

		case IDC_TOTALINK:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.TotalInk = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SepChanged();
			SetModified();
		break;

		case IDC_SATBOOST:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Separate.SatBoost = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SepChanged();
			SetModified();
		break;

		case IDC_BLACKNAMES:
			if ( codeNotify != CBN_SELCHANGE )
	 			break;
			GetExtNameN( hDlg, id, IDC_BLACKFILE, Separate.BGName,
						IDN_BGMAP, codeNotify, IDS_NONE, NO);
			LoadBlackGeneration(&Separate);
			InitBlackLuts();
			SetModified();
		break;

		case IDC_BLACKMGR:
			PopupMenuEx( hDlg, id, IDC_EDITEXTMANAGE, IDC_BLACKFILE );
			SetModified();
		break;

		case IDC_ADDEXT:
		case IDC_DELETEEXT:
		case IDC_RENAMEEXT:
			ExtNameManager( hDlg, IDC_BLACKNAMES, IDC_BLACKFILE,
					Separate.BGName, IDN_BGMAP, id, IDS_NONE );
			SetModified();
		break;

		case IDC_EDITEXT:
//			AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_BLACKGENERATE,
//				DlgBlackGenerateProc );
			dlg.DoModal();
			InitExtNameN( hDlg, IDC_BLACKNAMES, IDC_BLACKFILE,
						Separate.BGName, IDN_BGMAP, IDS_NONE, NO);
			LoadBlackGeneration(&Separate);
			InitBlackLuts();
			SetModified();
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	 return(TRUE);
}


void CSeparatePage::HandleOK()
{
	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = 0;
	}
	CPPPropertyPage::HandleOK();
}
   
void CSeparatePage::HandleCancel()
{
	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = 0;
	}
	CPPPropertyPage::HandleCancel();
}
   
void CSeparatePage::OnTimer(UINT nIDEvent)
{
	KillTimer(m_idTimer);
	InitBlackLuts();
	m_idTimer = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CCMSPrintPage property page

IMPLEMENT_DYNCREATE(CCMSPrintPage, CPPPropertyPage)
 
CCMSPrintPage::CCMSPrintPage() : CPPPropertyPage(CCMSPrintPage::IDD)
{
}

CCMSPrintPage::~CCMSPrintPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CCMSPrintPage message handlers

BOOL CCMSPrintPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
    CmsPrintSetup_OnInitDialog(hDlg);
    CmsPrintSetupInit( hDlg );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCMSPrintPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
 	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_STATUSINFO:
			AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg,
				IDD_CMS_DSTINFO, DlgCmsPrintSetupInfoProc, ( LPARAM )hDlg );
		break;

		case IDC_PREVIEW_DST:
			CmsPrintSetupProcessMsg( hDlg, id );
		break;

		case IDC_CMS_DST:
			if( codeNotify != CBN_SELCHANGE )
				break;
			CmsPrintSetupProcessMsg( hDlg, id );
			SetModified();
		break;

		case IDC_CMS_DSTLIST:
			if( codeNotify != LBN_SELCHANGE )
				break;
			CmsPrintSetupProcessMsg( hDlg, id );
			SetModified();
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}


void CCMSPrintPage::HandleOK()
{
	HWND hDlg = GetSafeHwnd();
	CmsPrintSetupProcessMsg( hDlg, IDOK );
	CPPPropertyPage::HandleOK();
}
   
void CCMSPrintPage::HandleCancel()
{
	HWND hDlg = GetSafeHwnd();
	CmsPrintSetupProcessMsg( hDlg, IDCANCEL );
	CPPPropertyPage::HandleCancel();
}
   

/////////////////////////////////////////////////////////////////////////////
// CHalftonePage property page

IMPLEMENT_DYNCREATE(CHalftonePage, CPPPropertyPage)
 
CHalftonePage::CHalftonePage() : CPPPropertyPage(CHalftonePage::IDD)
{
}

CHalftonePage::~CHalftonePage()
{
}

void CHalftonePage::EnableHalftone()
{
	HWND	hDlg = GetSafeHwnd();
	int nItems, i;
	ITEMID idCtls[] = {	IDC_DOTSHAPES, IDC_HILIGHT, IDC_SHADOW,
						IDC_SCRULING0, IDC_SCRULING1, IDC_SCRULING2,
						IDC_SCRULING3, IDC_ANGLE0, IDC_ANGLE1,
						IDC_ANGLE2, IDC_ANGLE3};

	nItems = sizeof(idCtls) / sizeof(ITEMID);
	for (i = 0; i < nItems; ++i)
		ControlEnable(hDlg, idCtls[i], Halftone.DoHalftoning);
}

/////////////////////////////////////////////////////////////////////////////
// CHalftonePage message handlers

BOOL CHalftonePage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	::CheckDlgButton(hDlg, IDC_USEPRINTERHALFTONE, !Halftone.DoHalftoning);
	CheckComboItem( hDlg, IDC_DOTSHAPES, IDC_CIRCLEDOT, IDC_ELLIPSEDOT,
		Halftone.DotShape );
	InitDlgItemSpin( hDlg, IDC_HILIGHT, TOPERCENT(Calibrate.Hilight),
		NO,	0, 100 );
	InitDlgItemSpin( hDlg, IDC_SHADOW, TOPERCENT(Calibrate.Shadow),
		NO,	0, 100 );
	AstralSetDlgItemFixed( hDlg, IDC_SCRULING0, Halftone.ScreenRuling[0],
		YES );
	AstralSetDlgItemFixed( hDlg, IDC_SCRULING1, Halftone.ScreenRuling[1],
		YES );
	AstralSetDlgItemFixed( hDlg, IDC_SCRULING2, Halftone.ScreenRuling[2],
		YES );
	AstralSetDlgItemFixed( hDlg, IDC_SCRULING3, Halftone.ScreenRuling[3],
		YES );
	AstralSetDlgItemFixed( hDlg, IDC_ANGLE0, Halftone.ScreenAngle[0], YES);
	AstralSetDlgItemFixed( hDlg, IDC_ANGLE1, Halftone.ScreenAngle[1], YES );
	AstralSetDlgItemFixed( hDlg, IDC_ANGLE2, Halftone.ScreenAngle[2], YES );
	AstralSetDlgItemFixed( hDlg, IDC_ANGLE3, Halftone.ScreenAngle[3], YES );
	EnableHalftone();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CHalftonePage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL 	Bool;
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_USEPRINTERHALFTONE:
			Halftone.DoHalftoning = !Halftone.DoHalftoning;
			::CheckDlgButton(hDlg, IDC_USEPRINTERHALFTONE, !Halftone.DoHalftoning);
			EnableHalftone();
			SetModified();
		break;

		case IDC_DOTSHAPES:
//		case IDC_CIRCLEDOT:
//		case IDC_SQUAREDOT:
//		case IDC_ELLIPSEDOT:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			Halftone.DotShape = id;
			SetModified();
		break;

		case IDC_SCRULING0:
		case IDC_SCRULING1:
		case IDC_SCRULING2:
		case IDC_SCRULING3:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Halftone.ScreenRuling[ id - IDC_SCRULING0 ] = 
				AstralGetDlgItemFixed( hDlg, id, &Bool, YES );
			SetModified();
		break;

		case IDC_ANGLE0:
		case IDC_ANGLE1:
		case IDC_ANGLE2:
		case IDC_ANGLE3:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Halftone.ScreenAngle[ id - IDC_ANGLE0 ] = 
				AstralGetDlgItemFixed( hDlg, id, &Bool, YES );
			SetModified();
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}


void CHalftonePage::HandleOK()
{
	CPPPropertyPage::HandleOK();
}
   
void CHalftonePage::HandleCancel()
{
	CPPPropertyPage::HandleCancel();
}
   

/////////////////////////////////////////////////////////////////////////////
// CCalibratePage property page

IMPLEMENT_DYNCREATE(CCalibratePage, CPPPropertyPage)
 
CCalibratePage::CCalibratePage() : CPPPropertyPage(CCalibratePage::IDD)
{
}

CCalibratePage::~CCalibratePage()
{
}

void CCalibratePage::InitCalLuts()
{
	HWND	hDlg = GetSafeHwnd();
	RGBS rgb;
	BOOL fDoCal;

	// initialize to a black ramp
	ResetMap(&m_MasterMap, MAPPOINTS, NO);
	fDoCal = DoCalibrate(&Calibrate);
	if (fDoCal)
	{
		if (m_wChannel == IDC_CAL_CYAN)
	 		CreateCMYKCalLut(&Calibrate, 0, m_MasterMap.Lut, NULL);
		else
		if (m_wChannel == IDC_CAL_MAGENTA)
	 		CreateCMYKCalLut(&Calibrate, 1, m_MasterMap.Lut, NULL);
		else
		if (m_wChannel == IDC_CAL_YELLOW)
	 		CreateCMYKCalLut(&Calibrate, 2, m_MasterMap.Lut, NULL);
		else
		if (m_wChannel == IDC_CAL_BLACK)
	 		CreateCMYKCalLut(&Calibrate, 3, m_MasterMap.Lut, NULL);
	}
	rgb.red = rgb.green = rgb.blue = 0;
	Lut_SetLut(::GetDlgItem(hDlg, IDC_MAP), 1, m_MasterMap.Lut);
	Lut_SetColor(::GetDlgItem(hDlg, IDC_MAP), 0, &rgb); 
}

void CCalibratePage::OnDotGain(ITEMID id, UINT codeNotify)
{
	HWND	hDlg = GetSafeHwnd();
	BOOL Bool;

	if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		return;
	if ( codeNotify != EN_CHANGE )
		return;
	Calibrate.DotGain = ::GetDlgItemSpin(hDlg, id,  &Bool, NO);
	CalChanged();
}

void CCalibratePage::OnHilight(ITEMID id, UINT codeNotify)
{
	HWND	hDlg = GetSafeHwnd();
	BOOL Bool;

	if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		return;
	if ( codeNotify != EN_CHANGE )
		return;
	Calibrate.Hilight = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
	Calibrate.Hilight = TOGRAY(Calibrate.Hilight);
	CalChanged();
}

void CCalibratePage::OnShadow(ITEMID id, UINT codeNotify)
{
	HWND	hDlg = GetSafeHwnd();
	BOOL Bool;

	if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		return;
	if ( codeNotify != EN_CHANGE )
		return;
	Calibrate.Shadow = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
	Calibrate.Shadow = TOGRAY(Calibrate.Shadow);
	CalChanged();
}

void CCalibratePage::CalChanged()
{
	if (m_idTimer)
		KillTimer(m_idTimer);
	m_idTimer = SetTimer(CALLUT_TIMER, REDISPLAY_WAIT, NULL);
	if (!m_idTimer)
		InitCalLuts();
}

BEGIN_MESSAGE_MAP(CCalibratePage, CPPPropertyPage)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalibratePage message handlers

BOOL CCalibratePage::OnInitDialog() 
{					  
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	SetWindowWord( ::GetDlgItem(hDlg,IDC_CALIBRATEMGR), GWW_ICONID,
			 		IDC_EXTMANAGE );
	InitExtNameN( hDlg, IDC_CALIBRATENAMES, IDC_CALIBRATEFILE,
				PrintStyle.CalibrateName, IDN_CALMAP, IDS_NONE, NO );

	m_wChannel = IDC_CAL_CYAN;
	CheckComboItem( hDlg, IDC_CALVIEW_CHANNELS, IDC_CALVIEW_CYAN, IDC_CALVIEW_BLACK,
				m_wChannel-IDC_CAL_CHANNELS-1+IDC_CALVIEW_CHANNELS );

	InitDlgItemSpin( hDlg, IDC_HILIGHT, TOPERCENT(Calibrate.Hilight),
		NO,	0, 100 );
	InitDlgItemSpin( hDlg, IDC_SHADOW, TOPERCENT(Calibrate.Shadow),
		NO,	0, 100 );
	InitDlgItemSpin( hDlg, IDC_DOTGAIN, Calibrate.DotGain, NO, 0, 100 );
	::CheckRadioButton( hDlg, IDC_CALONCONVERT, IDC_NOCALONCONVERT,
			Calibrate.DoCalOnConvert ? IDC_CALONCONVERT : IDC_NOCALONCONVERT);

	// initialize the lut control
	MapSetGradients(hDlg, TRUE, m_wChannel);
	Lut_Init(::GetDlgItem(hDlg, IDC_MAP), 64, 64);
	InitCalLuts();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCalibratePage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);
	CCalibratePrintEditDlg dlg(PictPubApp.GetActiveView());

	switch (id)
	{
		case IDC_CALVIEW_CHANNELS:
		//case IDC_CALVIEW_CYAN:
		//case IDC_CALVIEW_MAGENTA:
		//case IDC_CALVIEW_YELLOW:
		//case IDC_CALVIEW_BLACK:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			m_wChannel = id-IDC_CALVIEW_CHANNELS;
			m_wChannel += IDC_CAL_CHANNELS+1;
			MapSetGradients(hDlg, TRUE, m_wChannel);
			InitCalLuts();
			SetModified();
		break;

		case IDC_DOTGAIN:
			OnDotGain(id, codeNotify);
			SetModified();
		break;

		case IDC_HILIGHT:
			OnHilight(id, codeNotify);
			SetModified();
		break;

		case IDC_SHADOW:
			OnShadow(id, codeNotify);
			SetModified();
		break;

		case IDC_NOCALONCONVERT:
		case IDC_CALONCONVERT:
			Calibrate.DoCalOnConvert = id == IDC_CALONCONVERT;
			::CheckRadioButton( hDlg, IDC_CALONCONVERT, IDC_NOCALONCONVERT,
				Calibrate.DoCalOnConvert ? IDC_CALONCONVERT : IDC_NOCALONCONVERT);
			SetModified();
		break;

		case IDC_CALIBRATENAMES:
			if ( codeNotify != CBN_SELCHANGE )
				break;
			GetExtNameN( hDlg, id, IDC_CALIBRATEFILE, PrintStyle.CalibrateName,
						IDN_CALMAP, codeNotify, IDS_NONE, NO );
			LoadCalibrateStyle( PrintStyle.CalibrateName );
			InitCalLuts();
			SetModified();
		break;

		case IDC_CALIBRATEMGR:
			PopupMenuEx( hDlg, id, IDC_EDITEXTMANAGE, IDC_CALIBRATEFILE );
			SetModified();
		break;

		case IDC_ADDEXT:
		case IDC_DELETEEXT:
		case IDC_RENAMEEXT:
		 	ExtNameManager( hDlg, IDC_CALIBRATENAMES, IDC_CALIBRATEFILE,
 					PrintStyle.CalibrateName, IDN_CALMAP, id, IDS_NONE );
			SetModified();
		break;

		case IDC_EDITEXT:
//			AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_CALIBRATEPRINTEDIT,
//				DlgCalPrintEditProc );
			dlg.DoModal();
			InitExtNameN( hDlg, IDC_CALIBRATENAMES, IDC_CALIBRATEFILE,
				PrintStyle.CalibrateName, IDN_CALMAP, IDS_NONE, NO );
			InitCalLuts();
			SetModified();
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}


void CCalibratePage::HandleOK()
{
	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = 0;
	}
	CPPPropertyPage::HandleOK();
}
   
void CCalibratePage::HandleCancel()
{
	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = 0;
	}
	CPPPropertyPage::HandleCancel();
}
   
void CCalibratePage::OnTimer(UINT nIDEvent)
{
	KillTimer(m_idTimer);
	InitCalLuts();
	m_idTimer = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CExtrasPage property page

IMPLEMENT_DYNCREATE(CExtrasPage, CPPPropertyPage)
 
CExtrasPage::CExtrasPage() : CPPPropertyPage(CExtrasPage::IDD)
{
}

CExtrasPage::~CExtrasPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CExtrasPage message handlers

BOOL CExtrasPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	::CheckDlgButton( hDlg, IDC_TRIMMARKS,      PrintStyle.TrimMarks );
	::CheckDlgButton( hDlg, IDC_REGMARKS,       PrintStyle.RegMarks );
   	::CheckDlgButton( hDlg, IDC_LABELS,         PrintStyle.Labels );
   	::CheckDlgButton( hDlg, IDC_STEPSCALE,      PrintStyle.StepScale );
	::CheckDlgButton( hDlg, IDC_NEGATIVE,       PrintStyle.Negative );
	::CheckDlgButton( hDlg, IDC_EMULSIONDOWN,   PrintStyle.EmulsionDown );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CExtrasPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_TRIMMARKS:
			PrintStyle.TrimMarks = !PrintStyle.TrimMarks;
			::CheckDlgButton( hDlg, id, PrintStyle.TrimMarks );
			SetModified();
		break;

		case IDC_REGMARKS:
			PrintStyle.RegMarks = !PrintStyle.RegMarks;
			::CheckDlgButton( hDlg, id, PrintStyle.RegMarks );
			SetModified();
		break;

		case IDC_LABELS:
    		PrintStyle.Labels = !PrintStyle.Labels;
	    	::CheckDlgButton( hDlg, id, PrintStyle.Labels );
			SetModified();
		break;

		case IDC_STEPSCALE:
			PrintStyle.StepScale = !PrintStyle.StepScale;
			::CheckDlgButton( hDlg, id, PrintStyle.StepScale );
			SetModified();
		break;

		case IDC_NEGATIVE:
			PrintStyle.Negative = !PrintStyle.Negative;
			::CheckDlgButton( hDlg, id, PrintStyle.Negative );
			SetModified();
		break;

		case IDC_EMULSIONDOWN: // Postscript only
			PrintStyle.EmulsionDown = !PrintStyle.EmulsionDown;
			::CheckDlgButton( hDlg, id, PrintStyle.EmulsionDown );
			Message(IDS_PSONLY);
			SetModified();
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}


void CExtrasPage::HandleOK()
{
	CPPPropertyPage::HandleOK();

}
   
void CExtrasPage::HandleCancel()
{
	CPPPropertyPage::HandleCancel();
}
   
/////////////////////////////////////////////////////////////////////////////
// CPrintStylePropSheet

IMPLEMENT_DYNAMIC(CPrintStylePropSheet, CPPPropertySheet)

int CPrintStylePropSheet::m_nLastPage = 0;

CPrintStylePropSheet::CPrintStylePropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT nIDHelp)
	:CPPPropertySheet(nIDCaption, pParentWnd, nIDHelp, GetSelectPage())
{
	CommonConstruct();
}

CPrintStylePropSheet::CPrintStylePropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT nIDHelp)
	:CPPPropertySheet(pszCaption, pParentWnd, nIDHelp, GetSelectPage())
{
	CommonConstruct();
}

void CPrintStylePropSheet::CommonConstruct()
{
    LoadPrintStyle( Names.PrintStyle );
	if (Control.Retail)
		AddPage(&m_SeparatePage);
	AddPage(&m_HalftonePage);
	if (Control.Retail)
		AddPage(&m_CalibratePage);
	AddPage(&m_ExtrasPage);
	if (Control.CMSEnabled)
		AddPage(&m_CMSPrintPage);
}

CPrintStylePropSheet::~CPrintStylePropSheet()
{
	m_nLastPage = m_nCurPage;
}

UINT CPrintStylePropSheet::GetSelectPage()
{
	int nPageCount = 5;
	if (!Control.Retail)
		nPageCount -= 2;
	if (!Control.CMSEnabled)
		nPageCount -= 1;
	if (nPageCount <= m_nLastPage)
		m_nLastPage = nPageCount - 1;
	return(m_nLastPage);
}

void CPrintStylePropSheet::HandleOK()
{
	HWND hDlg = GetSafeHwnd();
	CPPPropertySheet::HandleOK();
	if ( SavePrintStyle( hDlg, Names.PrintStyle ) )
		InitPrintStyle(&Separate, &Calibrate);
}

void CPrintStylePropSheet::HandleCancel()
{
	CPPPropertySheet::HandleCancel();
	LoadPrintStyle( Names.PrintStyle );
}

//************************************************************************
//		MapSetGradient.
// DESCRIPTION:
//		sets the given gradient control based on the channel.
//************************************************************************
void LOCAL MapSetGradients(HWND hDlg, BOOL bIntensity, WORD wChannel)
//************************************************************************
{
	HWND hBar;
	RGBS rgb1;
	RGBS rgb2;
	RGBS rgbTemp;

	switch (wChannel)
	{
		case IDC_DO_RED:		
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = 255;
			rgb2.green = rgb2.blue = 0;
		break;
		case IDC_DO_GREEN:	
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.green = 255;
			rgb2.red = rgb2.blue = 0;
		break;
		case IDC_DO_BLUE:		
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = 0;
			rgb2.blue = 255;
		break;
		case IDC_CAL_CYAN:
		case IDC_DO_CYAN:		
			rgb2.red = 0;
			rgb2.green = rgb2.blue = 255;
			rgb1.red = rgb1.green = rgb1.blue = 255;
		break;
		case IDC_CAL_MAGENTA:
		case IDC_DO_MAGENTA:
			rgb2.green = 0;
			rgb2.red = rgb2.blue = 255;
			rgb1.red = rgb1.green = rgb1.blue = 255;
		break;
		case IDC_CAL_YELLOW:
		case IDC_DO_YELLOW:	
			rgb2.blue = 0;
			rgb2.red = rgb2.green = 255;
			rgb1.red = rgb1.green = rgb1.blue = 255;
		break;
		case IDC_CAL_BLACK:
		case IDC_DO_BLACK:	
			bIntensity = !bIntensity;
		case IDC_CAL_MASTER:
		case IDC_DO_ALL:
		default:
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = rgb2.blue = 255;
		break;
	}
	
	if (bIntensity)
	{
		rgbTemp = rgb1;
		rgb1 = rgb2;
		rgb2 = rgbTemp;
	}
		
	if (hBar = GetDlgItem(hDlg, IDC_MAP_HBAR))
	{
		Gradient_SetRGB(hBar, &rgb2, &rgb1);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
	if (hBar = GetDlgItem(hDlg,IDC_MAP_VBAR))
	{
		Gradient_SetRGB(hBar, &rgb1, &rgb2);
		Gradient_SetType(hBar, GT_RGB);																	  
	}
}
