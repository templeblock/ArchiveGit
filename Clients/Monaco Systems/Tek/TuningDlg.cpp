// TuningDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tek.h"
#include "TuningDlg.h"
#include "Message.h"
#include "Qd3dExtras.h"
#include "interpolate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
class ProfileDocFiles : public CDocument
{
public:
	int copyOutOfPrintData() { return MCO_SUCCESS; }
public:
	void* _labtableH;
	void* _out_of_gamutH;
};

/////////////////////////////////////////////////////////////////////////////
static ProfileDocFiles* GetDoc(CDialog* p)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
class CProfileDoc : public CDocument
{
public:
	int GetTweaks(Tweak_Element** pTweaks, int nTweaks) { return 0; };
	void PutTweaks(Tweak_Element** pTweaks, int nTweaks) {};
	ProfileDocFiles _profiledocfiles;
};

/////////////////////////////////////////////////////////////////////////////
static CProfileDoc* GetProfileDoc(CDialog* p)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
static void ConvertToGamutC(ProfileDocFiles* doc, double* lab, double* labc)
{
	if (!doc)
		return;
	double* labTable = (double*)McoLockHandle(doc->_labtableH);
	interpolate_33(lab,labc,labTable);
	McoUnlockHandle(doc->_labtableH);
}

/////////////////////////////////////////////////////////////////////////////
static void ConvertToPrint(ProfileDocFiles* doc, double* lab, double* labo)
{
	if (!doc)
		return;
//j	doc->tweak->eval(lab, labo, 1);
}

/////////////////////////////////////////////////////////////////////////////
static void ConvertToGamut(ProfileDocFiles* doc, double* lab, double* labo)
{
	if (!doc)
		return;
	ConvertToGamutC(doc, lab, labo);
//j	doc->gtweak->eval(lab, labo, labo, 1);
}

/////////////////////////////////////////////////////////////////////////////
static void SetupTweaks(ProfileDocFiles* doc)
{
	if (!doc)
		return;
	if ( doc->copyOutOfPrintData() != MCO_SUCCESS )
		return;

//j	doc->tweak->Init_Table();
//j	doc->tweak->Modify_Table(doc->printerData->num_tweaks, doc->printerData->tweaks);
//j	doc->gtweak->Init_Table();
//j	doc->gtweak->Modify_Table(doc->printerData->num_tweaks, doc->printerData->tweaks);
}

/////////////////////////////////////////////////////////////////////////////
// CTuningDlg dialog


// Message Map
BEGIN_MESSAGE_MAP(CTuningDlg, CDialog)
	//{{AFX_MSG_MAP(CTuningDlg)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_TWEAK_ADD, OnTweakAdd)
	ON_CBN_EDITCHANGE(IDC_TWEAK_SELECT, OnTweakSelectEditchange)
	ON_CBN_SELCHANGE(IDC_TWEAK_SELECT, OnTweakSelectSelchange)
	ON_BN_CLICKED(IDC_TWEAK_DELETE, OnTweakDelete)
	ON_BN_CLICKED(IDC_TWEAK_MOVETOOL, OnTweakMoveTool)
	ON_BN_CLICKED(IDC_TWEAK_SIZETOOL, OnTweakSizeTool)
	ON_BN_CLICKED(IDC_TWEAK_PRINTRADIO, OnTweakPrintRadio)
	ON_BN_CLICKED(IDC_TWEAK_GAMUTRADIO, OnTweakGamutRadio)
	ON_EN_CHANGE(IDC_TWEAK_LDESIRED, OnTweakChangeLdesired)
	ON_EN_CHANGE(IDC_TWEAK_ADESIRED, OnTweakChangeAdesired)
	ON_EN_CHANGE(IDC_TWEAK_BDESIRED, OnTweakChangeBdesired)
	ON_BN_CLICKED(IDC_TWEAK_CDESIRED, OnTweakCdesired)
	ON_EN_CHANGE(IDC_TWEAK_LGAMUT, OnTweakChangeLgamut)
	ON_EN_CHANGE(IDC_TWEAK_AGAMUT, OnTweakChangeAgamut)
	ON_EN_CHANGE(IDC_TWEAK_BGAMUT, OnTweakChangeBgamut)
	ON_BN_CLICKED(IDC_TWEAK_CGAMUT, OnTweakCgamut)
	ON_EN_CHANGE(IDC_TWEAK_LPRINT, OnTweakChangeLprint)
	ON_EN_CHANGE(IDC_TWEAK_APRINT, OnTweakChangeAprint)
	ON_EN_CHANGE(IDC_TWEAK_BPRINT, OnTweakChangeBprint)
	ON_BN_CLICKED(IDC_TWEAK_CPRINT, OnTweakCprint)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CTuningDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CTuningDlg)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CTuningDlg::CTuningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTuningDlg::IDD, pParent)
{
	for (int i = 0; i< MAX_NUM_TWEAKS; i++)
		m_pTweaks[i] = NULL;
	m_pCurrentTweak = NULL;
	m_nCurrentTweak = 0;
	m_nTweaks = 0;

	m_pControl = NULL;
	m_gamut = NULL;
	m_ingamut = true;

	m_colorDesired	= RGB(255,0,0);
	m_colorDesired2	= RGB(255,0,0);
	m_colorGamut	= RGB(255,0,0);
	m_colorGamut2	= RGB(255,0,0);
	m_colorPrint	= RGB(255,0,0);
	m_colorPrint2	= RGB(255,0,0);
	m_inited = FALSE;
	m_Qd3dGamut.Document.fModel = NULL;
	m_bMouseDown = false;
	m_kTool = kNone;
	//{{AFX_DATA_INIT(CTuningDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CTuningDlg::~CTuningDlg()
{
	// Delete any existing local tweaks
	for (int i = 0; i< m_nTweaks; i++)
	{
		delete m_pTweaks[i];
		m_pTweaks[i] = NULL;
	}

	if (m_pControl)
		delete m_pControl;
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTuningDlg)
	DDX_Control(pDX, IDC_TWEAK_GAMUT, m_wndGamut);
	DDX_Control(pDX, IDC_TWEAK_LRSLIDER, m_sldLightRange);
	DDX_Control(pDX, IDC_TWEAK_LRVALUE, m_staLightRange);
	DDX_Control(pDX, IDC_TWEAK_CRSLIDER, m_sldColorRange);
	DDX_Control(pDX, IDC_TWEAK_CRVALUE, m_staColorRange);
	DDX_Control(pDX, IDC_TWEAK_LSCROLL, m_scrLightness);
	DDX_Control(pDX, IDC_TWEAK_LVALUE, m_staLightness);
	DDX_Control(pDX, IDC_TWEAK_SELECT, m_comboTweaks);
	DDX_Control(pDX, IDC_TWEAK_MOVETOOL, m_radioMoveTool);
	DDX_Control(pDX, IDC_TWEAK_SIZETOOL, m_radioSizeTool);
	DDX_Control(pDX, IDC_TWEAK_GAMUTRADIO, m_radioGamut);
	DDX_Control(pDX, IDC_TWEAK_PRINTRADIO, m_radioPrint);
	DDX_Control(pDX, IDC_TWEAK_LDESIRED, m_editLDesired);
	DDX_Control(pDX, IDC_TWEAK_ADESIRED, m_editADesired);
	DDX_Control(pDX, IDC_TWEAK_BDESIRED, m_editBDesired);
	DDX_Control(pDX, IDC_TWEAK_CDESIRED, m_colrCDesired);
	DDX_Control(pDX, IDC_TWEAK_LDESIRED2, m_staLDesired2);
	DDX_Control(pDX, IDC_TWEAK_ADESIRED2, m_staADesired2);
	DDX_Control(pDX, IDC_TWEAK_BDESIRED2, m_staBDesired2);
	DDX_Control(pDX, IDC_TWEAK_CDESIRED2, m_colrCDesired2);
	DDX_Control(pDX, IDC_TWEAK_LGAMUT, m_editLGamut);
	DDX_Control(pDX, IDC_TWEAK_AGAMUT, m_editAGamut);
	DDX_Control(pDX, IDC_TWEAK_BGAMUT, m_editBGamut);
	DDX_Control(pDX, IDC_TWEAK_CGAMUT, m_colrCGamut);
	DDX_Control(pDX, IDC_TWEAK_LGAMUT2, m_staLGamut2);
	DDX_Control(pDX, IDC_TWEAK_AGAMUT2, m_staAGamut2);
	DDX_Control(pDX, IDC_TWEAK_BGAMUT2, m_staBGamut2);
	DDX_Control(pDX, IDC_TWEAK_CGAMUT2, m_colrCGamut2);
	DDX_Control(pDX, IDC_TWEAK_LPRINT, m_editLPrint);
	DDX_Control(pDX, IDC_TWEAK_APRINT, m_editAPrint);
	DDX_Control(pDX, IDC_TWEAK_BPRINT, m_editBPrint);
	DDX_Control(pDX, IDC_TWEAK_CPRINT, m_colrCPrint);
	DDX_Control(pDX, IDC_TWEAK_LPRINT2, m_staLPrint2);
	DDX_Control(pDX, IDC_TWEAK_APRINT2, m_staAPrint2);
	DDX_Control(pDX, IDC_TWEAK_BPRINT2, m_staBPrint2);
	DDX_Control(pDX, IDC_TWEAK_CPRINT2, m_colrCPrint2);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTuningDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	
	m_bMouseDown = FALSE;
	m_ingamut = TRUE;

	SetQd3dErrorHandler();

	m_sldLightRange.SetRange(0, 100, FALSE/*bRedraw*/);
	m_sldLightRange.SetTicFreq(10);

	m_sldColorRange.SetRange(0, 100, FALSE/*bRedraw*/);
	m_sldColorRange.SetTicFreq(10);

	m_scrLightness.SetScrollRange(0, 100, FALSE/*bRedraw*/);

	m_comboTweaks.ResetContent();
	m_comboTweaks.SetCurSel(0);

	m_colrCDesired.SetRGBColor(RGB(255,0,0));
	m_colrCDesired2.SetRGBColor(RGB(255,0,0));
	m_colrCGamut.SetRGBColor(RGB(255,0,0));
	m_colrCGamut2.SetRGBColor(RGB(255,0,0));
	m_colrCPrint.SetRGBColor(RGB(255,0,0));
	m_colrCPrint2.SetRGBColor(RGB(255,0,0));

	m_radioMoveTool.SetBitmap( ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MOVEPLANE)) );
	m_radioSizeTool.SetBitmap( ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MOVEDEPTH)) );

	DoTweakSetTool(kColorDesired);
		
	SetTweakRadio(FALSE/*bGamutMapping*/);
	Update3dGamut(50); // Default value for gamut view (slice)

	Init3D();
	GetDataFromDoc(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Dlgs should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::GetDataFromDoc(CProfileDoc* pDoc) 
{
	if (!pDoc)
		pDoc = GetProfileDoc(this);
	if (!pDoc || !IsWindow(this->m_hWnd))
		return;

	m_nTweaks = pDoc->GetTweaks(m_pTweaks, m_nTweaks);
	m_nCurrentTweak = 0;
	m_pCurrentTweak = m_pTweaks[m_nCurrentTweak];

	EnableAllButAdd(NULL, m_nTweaks > 0 /*bEnable*/);

	ReloadTweakList();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::PutTweaksIntoDoc(void) 
{
	CProfileDoc* pDoc = GetProfileDoc(this);
	if (pDoc)
		pDoc->PutTweaks(m_pTweaks, m_nTweaks);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::ReloadTweakList(void) 
{
	m_comboTweaks.ResetContent();
	for (int i = 0; i < m_nTweaks; i++)
		m_comboTweaks.AddString(m_pTweaks[i]->name);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);

	// Update all of the other controls
	SetControlsForCurrentTweak();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::SetControlsForCurrentTweak(void) 
{
	Tweak_Element* pTweak = m_pCurrentTweak;

	Tweak_Element EmptyTweak("");
	memset( &EmptyTweak, 0, sizeof(Tweak_Element));
	if (!pTweak)
		pTweak = &EmptyTweak;

	CString sz;
	sz.Format("%.0f", pTweak->lab_d[0]);	m_editLDesired.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_d[1]);	m_editADesired.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_d[2]);	m_editBDesired.SetWindowText(sz);
	m_colrCDesired.SetLABColor(pTweak->lab_d);
	m_colrCDesired.UpdateWindow();
	m_editLDesired.UpdateWindow();
	m_editADesired.UpdateWindow();
	m_editBDesired.UpdateWindow();
	
	sz.Format("%.0f", pTweak->lab_p[0]);	m_editLPrint.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_p[1]);	m_editAPrint.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_p[2]);	m_editBPrint.SetWindowText(sz);
	m_colrCPrint.SetLABColor(pTweak->lab_p);
	m_colrCPrint.UpdateWindow();
	m_editLPrint.UpdateWindow();
	m_editAPrint.UpdateWindow();
	m_editBPrint.UpdateWindow();

	sz.Format("%.0f", pTweak->lab_g[0]);	m_editLGamut.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_g[1]);	m_editAGamut.SetWindowText(sz);
	sz.Format("%.0f", pTweak->lab_g[2]);	m_editBGamut.SetWindowText(sz);
	m_colrCGamut.SetLABColor(pTweak->lab_g);
	m_colrCGamut.UpdateWindow();
	m_editLGamut.UpdateWindow();
	m_editAGamut.UpdateWindow();
	m_editBGamut.UpdateWindow();

	m_sldLightRange.SetPos(pTweak->Lr);
	sz.Format("%d", (int)pTweak->Lr);	m_staLightRange.SetWindowText(sz);

	m_sldColorRange.SetPos(pTweak->Cr);
	sz.Format("%d", (int)pTweak->Cr);	m_staColorRange.SetWindowText(sz);

	SetTweakRadio(pTweak->type == GamutComp_Tweak);
	switch(m_kTool)
	{
		case(kColorGamut):
			Update3dGamut(pTweak->lab_g[0]);
			break;

		case(kColorPrint):
			Update3dGamut(pTweak->lab_p[0]);
			break;

		default:
			Update3dGamut(pTweak->lab_d[0]);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::SetTweakRadio(BOOL bGamutMapping) 
{
	if (m_pCurrentTweak)
		m_pCurrentTweak->type = (bGamutMapping ? GamutComp_Tweak : Calibrate_Tweak);

	m_radioGamut.SetCheck(bGamutMapping);
	m_radioPrint.SetCheck(!bGamutMapping);

	BOOL bEnablePrint = (!bGamutMapping && m_nTweaks>0);
	BOOL bEnableGamut = (bGamutMapping && m_nTweaks>0);
	int nCmdShowPrint = (bEnablePrint ? SW_SHOW : SW_HIDE);
	int nCmdShowGamut = (bEnablePrint ? SW_HIDE : SW_SHOW);

	m_editLPrint.ShowWindow(nCmdShowPrint);
	m_editAPrint.ShowWindow(nCmdShowPrint);
	m_editBPrint.ShowWindow(nCmdShowPrint);
	m_colrCPrint.ShowWindow(nCmdShowPrint);
	m_colrCPrint.Invalidate();

	m_staLPrint2.ShowWindow(nCmdShowPrint);
	m_staAPrint2.ShowWindow(nCmdShowPrint);
	m_staBPrint2.ShowWindow(nCmdShowPrint);
	m_colrCPrint2.ShowWindow(nCmdShowPrint);
	m_colrCPrint2.Invalidate();

/*
	m_editLGamut.EnableWindow(bEnableGamut);
	m_editAGamut.EnableWindow(bEnableGamut);
	m_editBGamut.EnableWindow(bEnableGamut);
	m_colrCGamut.EnableWindow(bEnableGamut);
	m_colrCGamut.Invalidate();
*/

	m_staLGamut2.ShowWindow(nCmdShowGamut);
	m_staAGamut2.ShowWindow(nCmdShowGamut);
	m_staBGamut2.ShowWindow(nCmdShowGamut);
	m_colrCGamut2.ShowWindow(nCmdShowGamut);
	m_colrCGamut2.Invalidate();

	// Change to color tool if we are hiding or showing the matching color patches
	if (m_kTool == kColorGamut && nCmdShowGamut == SW_HIDE)
		DoTweakSetTool(kColorPrint);
	else
	if (m_kTool == kColorPrint && nCmdShowPrint == SW_HIDE)
		DoTweakSetTool(kColorGamut);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::Update3dGamut(int iLvalue) 
{
	// Set the slider
	if (iLvalue >= 0)
	{
		m_scrLightness.SetScrollPos(100 - iLvalue, TRUE/*bRedraw*/);
		// Set the readout
		CString sz;
		sz.Format("%d", iLvalue);
		m_staLightness.SetWindowText(sz);
	}
	else
		iLvalue = 100 - m_scrLightness.GetScrollPos();

	// Set the Gamut window, if we have been initialized
	if (m_Qd3dGamut.Document.fModel)
		m_Qd3dGamut.ChangeTweakData(m_pCurrentTweak, iLvalue);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	int iValue = pSlider->GetPos();

	CString sz;
	sz.Format("%d", iValue);

	switch (pSlider->GetDlgCtrlID()) 
	{
		case IDC_TWEAK_LRSLIDER:
			m_staLightRange.SetWindowText(sz);
			if (m_pCurrentTweak)
				m_pCurrentTweak->Lr = iValue;
			Update3dGamut(-1);
			PutTweaksIntoDoc(); // Keep the doc up to date
			SetupTweaks(GetDoc(this));
			break;
		case IDC_TWEAK_CRSLIDER:
			m_staColorRange.SetWindowText(sz);
			if (m_pCurrentTweak)
				m_pCurrentTweak->Cr = iValue;
			Update3dGamut(-1);
			PutTweaksIntoDoc(); // Keep the doc up to date
			SetupTweaks(GetDoc(this));
			break;
	}	

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetFocus();

	int iValue = 100 - pScrollBar->GetScrollPos();
	if (nSBCode == SB_LINEUP)			iValue++;
	if (nSBCode == SB_LINEDOWN)			iValue--;
	if (nSBCode == SB_PAGEUP)			iValue += 10;
	if (nSBCode == SB_PAGEDOWN)			iValue -= 10;
	if (nSBCode == SB_THUMBPOSITION)	iValue = 100 - nPos;
	if (nSBCode == SB_THUMBTRACK)		iValue = 100 - nPos;
	if (nSBCode == SB_TOP)				iValue = 100;
	if (nSBCode == SB_BOTTOM)			iValue = 0;
	if (nSBCode == SB_ENDSCROLL)		return;

	int iMinPos, iMaxPos;
	pScrollBar->GetScrollRange(&iMinPos, &iMaxPos);
	if (iValue < iMinPos) iValue = iMinPos;
	if (iValue > iMaxPos) iValue = iMaxPos;

	switch (pScrollBar->GetDlgCtrlID()) 
	{
		case IDC_TWEAK_LSCROLL:
			Update3dGamut(iValue);
			break;
	}	

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	BOOL bStatus = CDialog::OnSetCursor(pWnd, nHitTest, message);

	CWnd* pWndGamut = &m_wndGamut;
	if (pWndGamut && pWndGamut->GetTopWindow())
		pWndGamut = pWndGamut->GetTopWindow();

	if (pWndGamut != pWnd)
		return bStatus;

	static POINT ptLast;
	POINT pt;
	GetCursorPos(&pt);
	GetDesktopWindow()->MapWindowPoints(&m_wndGamut, &pt, 1);

	if (message == WM_MOUSEMOVE)
	{
		if (m_bMouseDown)
			OnTweakMouseDown(pt, ptLast);
		else
			OnTweakMouseMove(pt);
	}
	else
	if (message == WM_LBUTTONDOWN)
	{
		m_bMouseDown = TRUE;
		OnTweakMouseDown(pt, ptLast = pt);
	}
	else
	if (message == WM_LBUTTONUP)
		m_bMouseDown = FALSE;

	ptLast = pt;
	
	return bStatus;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::IsColorInGamut(double* lab)
{
	ProfileDocFiles* doc = GetDoc(this);
	if (!doc) return 0;

	if (lab[1] < -128) lab[1] = -128;
	if (lab[1] > 127) lab[1] = 127;

	if (lab[2] < -128) lab[2] = -128;
	if (lab[2] > 127) lab[2] = 127;

	int j = 33*lab[0]/100;
	int k = 33*(lab[1]+128)/255;
	int l = 33*(lab[2]+128)/255;

	unsigned char* out_gamut = (unsigned char*)McoLockHandle(doc->_out_of_gamutH);
	BOOL bInGamut;
	if (out_gamut[l+k*33+j*33*33] == 255)
		bInGamut = 0;
	else
		bInGamut = 1;

	McoUnlockHandle(doc->_out_of_gamutH);
	return bInGamut;
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakMouseMove(POINT pt) 
{
	if (!m_pCurrentTweak)
		return;

	double lab_d[3] = {100.0 - m_scrLightness.GetScrollPos(), 0.0, 0.0};
	McoStatus status;
//	BOOL ingamut = 0;
	if (m_pCurrentTweak->type == GamutComp_Tweak)
	{
		status = m_Qd3dGamut.Get3Dinfo(pt, lab_d, 0/*GamutCheck*/, &m_ingamut);
//		ingamut = 1 - ingamut;
	}
	else
	if (m_pCurrentTweak->type == Calibrate_Tweak)
	{
		status = m_Qd3dGamut.Get3Dinfo(pt, lab_d, 1/*GamutCheck*/, &m_ingamut);
	}

	if (status == MCO_FAILURE)
		return;

	double lab_p[3];
	double lab_g[3];

	// Display the color regardless of whether it was in the gamut or not.
	// This allows Get3Dinfo() to return black outside the gamut

	ProfileDocFiles* doc = GetDoc(this);
	if (doc)
	{
		ConvertToGamut(doc, lab_d, lab_g);
		ConvertToGamutC(doc, lab_d, lab_p);
		ConvertToPrint(doc, lab_p, lab_p);
	}
	else
	{
		lab_p[0] = lab_d[0];
		lab_p[1] = lab_d[1];
		lab_p[2] = lab_d[2];

		lab_g[0] = lab_d[0];
		lab_g[1] = lab_d[1];
		lab_g[2] = lab_d[2];
	}

	CString sz;
	sz.Format("%.0f", lab_d[0]); m_staLDesired2.SetWindowText(sz); m_staLDesired2.UpdateWindow();
	sz.Format("%.0f", lab_d[1]); m_staADesired2.SetWindowText(sz); m_staADesired2.UpdateWindow();
	sz.Format("%.0f", lab_d[2]); m_staBDesired2.SetWindowText(sz); m_staBDesired2.UpdateWindow();
	m_colrCDesired2.SetLABColor(lab_d); m_colrCDesired2.UpdateWindow();

	sz.Format("%.0f", lab_p[0]); m_staLPrint2.SetWindowText(sz); m_staLPrint2.UpdateWindow();
	sz.Format("%.0f", lab_p[1]); m_staAPrint2.SetWindowText(sz); m_staAPrint2.UpdateWindow();
	sz.Format("%.0f", lab_p[2]); m_staBPrint2.SetWindowText(sz); m_staBPrint2.UpdateWindow();
	m_colrCPrint2.SetLABColor(lab_p); m_colrCPrint2.UpdateWindow();

	sz.Format("%.0f", lab_g[0]); m_staLGamut2.SetWindowText(sz); m_staLGamut2.UpdateWindow();
	sz.Format("%.0f", lab_g[1]); m_staAGamut2.SetWindowText(sz); m_staAGamut2.UpdateWindow();
	sz.Format("%.0f", lab_g[2]); m_staBGamut2.SetWindowText(sz); m_staBGamut2.UpdateWindow();
	m_colrCGamut2.SetLABColor(lab_g); m_colrCGamut2.UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakMouseDown(POINT pt, POINT ptLast)
{
	if (m_kTool == kMove)
		DoTweakMoveTool(pt, ptLast);
	else
	if (m_kTool == kSize)
		DoTweakSizeTool(pt, ptLast);
	else
		DoTweakColorPickTool(pt, m_kTool);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakColorPickTool(POINT pt, kTOOL kTool)
{
	// Copy the color values into the tweak
	Tweak_Element* pTweak = m_pCurrentTweak;
	if (!pTweak)
		return;

	CString sz;
	if (kTool == kColorDesired)
	{
		m_staLDesired2.GetWindowText(sz); pTweak->lab_d[0] = atof(sz);
		m_staADesired2.GetWindowText(sz); pTweak->lab_d[1] = atof(sz);
		m_staBDesired2.GetWindowText(sz); pTweak->lab_d[2] = atof(sz);

		ProfileDocFiles* doc = GetDoc(this);
		if (doc)
		{
			ConvertToGamutC(doc, pTweak->lab_d, pTweak->lab_g);
		}
		else
		{
			pTweak->lab_g[0] = pTweak->lab_d[0];
			pTweak->lab_g[1] = pTweak->lab_d[1];
			pTweak->lab_g[2] = pTweak->lab_d[2];
		}
		pTweak->lab_p[0] = pTweak->lab_g[0];
		pTweak->lab_p[1] = pTweak->lab_g[1];
		pTweak->lab_p[2] = pTweak->lab_g[2];

		
		//should check which Radio button is selected
		//if inside the gamut, the printing button should be selected
		//if outside the gamut, the mapping button should be selected
		if (m_pCurrentTweak)
		{
			if(m_ingamut && (m_pCurrentTweak->type == GamutComp_Tweak) )
			{
				SetTweakRadio(FALSE);	
			}
			else if(!m_ingamut && (m_pCurrentTweak->type == Calibrate_Tweak) )
			{
				SetTweakRadio(TRUE);
			}			
		}
	}

//	if (kTool == kColorGamut || kTool == kColorDesired)
	if (kTool == kColorGamut && m_ingamut)
	{
		m_staLDesired2.GetWindowText(sz); pTweak->lab_g[0] = atof(sz);
		m_staADesired2.GetWindowText(sz); pTweak->lab_g[1] = atof(sz);
		m_staBDesired2.GetWindowText(sz); pTweak->lab_g[2] = atof(sz);

		//m_staLGamut2.GetWindowText(sz); pTweak->lab_g[0] = atof(sz);
		//m_staAGamut2.GetWindowText(sz); pTweak->lab_g[1] = atof(sz);
		//m_staBGamut2.GetWindowText(sz); pTweak->lab_g[2] = atof(sz);
	}

//	if (kTool == kColorPrint || kTool == kColorDesired)
	if (kTool == kColorPrint && m_ingamut)
	{
		m_staLDesired2.GetWindowText(sz); pTweak->lab_p[0] = atof(sz);
		m_staADesired2.GetWindowText(sz); pTweak->lab_p[1] = atof(sz);
		m_staBDesired2.GetWindowText(sz); pTweak->lab_p[2] = atof(sz);

		//m_staLPrint2.GetWindowText(sz); pTweak->lab_p[0] = atof(sz);
		//m_staAPrint2.GetWindowText(sz); pTweak->lab_p[1] = atof(sz);
		//m_staBPrint2.GetWindowText(sz); pTweak->lab_p[2] = atof(sz);
	}

	// Update all of the other controls
	SetControlsForCurrentTweak();

	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakRotateTool(POINT pt, POINT ptLast)
{
	// Rotate the 3d model
	double dx = (pt.x - ptLast.x);
	double dy = (pt.y - ptLast.y);
	m_Qd3dGamut.DoTransform(WE_Rotate, dy/100, dx/100, 0);
	m_Qd3dGamut.MakeCross(FALSE);
	m_Qd3dGamut.DocumentDraw3DData();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakMoveTool(POINT pt, POINT ptLast)
{
	// Translate the 3d model
	double dx = (pt.x - ptLast.x);
	double dy = (pt.y - ptLast.y);
	m_Qd3dGamut.DoTransform(WE_Translate, dx/175, dy/175, 0);
	m_Qd3dGamut.MakeCross(FALSE);
	m_Qd3dGamut.DocumentDraw3DData();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakSizeTool(POINT pt, POINT ptLast)
{
	// Scale the 3d model
	double dy = (pt.y - ptLast.y);
	m_Qd3dGamut.DoTransform(WE_Scale, dy/200, 0, 0);
	m_Qd3dGamut.MakeCross(FALSE);
	m_Qd3dGamut.DocumentDraw3DData();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakMoveTool() 
{
	DoTweakSetTool(m_kTool == kMove ? kColorDesired : kMove);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakSizeTool() 
{
	DoTweakSetTool(m_kTool == kSize ? kColorDesired : kSize);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::DoTweakSetTool(kTOOL kTool)
/////////////////////////////////////////////////////////////////////////////
{
	m_kTool = kTool;
	m_radioMoveTool.SetCheck(m_kTool == kMove);
	m_radioSizeTool.SetCheck(m_kTool == kSize);
	m_colrCDesired.SetCheck(m_kTool == kColorDesired);
	m_colrCGamut.SetCheck(m_kTool == kColorGamut);
	m_colrCPrint.SetCheck(m_kTool == kColorPrint);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakPrintRadio() 
{
	ProfileDocFiles* doc = GetDoc(this);
	if(doc && m_pCurrentTweak)
	{
		ConvertToGamutC(doc, m_pCurrentTweak->lab_d, m_pCurrentTweak->lab_g);
	}

	SetTweakRadio(FALSE/*bGamutMapping*/);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakGamutRadio() 
{
	SetTweakRadio(TRUE/*bGamutMapping*/);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakSelectEditchange() 
{
	DWORD dw = m_comboTweaks.GetEditSel();

	CString szName;
	m_comboTweaks.GetWindowText(szName);
	memcpy(m_pCurrentTweak->name, szName, sizeof(m_pCurrentTweak->name));
	m_comboTweaks.DeleteString(m_nCurrentTweak);
	m_comboTweaks.InsertString(m_nCurrentTweak, szName);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);
	m_comboTweaks.SetEditSel(LOWORD(dw), HIWORD(dw));
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakSelectSelchange() 
{
	m_nCurrentTweak = m_comboTweaks.GetCurSel();
	m_pCurrentTweak = m_pTweaks[m_nCurrentTweak];

	// Update all of the other controls
	SetControlsForCurrentTweak();
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::SetupTweak(Tweak_Element* tweak) 
{	
	ProfileDocFiles* pDoc = GetDoc(this);
	if(!pDoc || !tweak)	return; 

	if(tweak->type == Calibrate_Tweak)
	{
		ConvertToGamutC(pDoc, tweak->lab_d, tweak->lab_g);	
	}

}

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::IsDlgOkToShow(void)
{
	if (m_inited)
		return TRUE;

	m_inited = true; //j BuildGamut();

	return m_inited;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::BuildGamut()
{
//	if (!m_Qd3dGamut.Document.fModel)
//	{
//	}

	CProfileDoc* pDoc = GetProfileDoc(this);
	if (!pDoc)
		return FALSE;

	if( pDoc->_profiledocfiles.copyOutOfPrintData() != MCO_SUCCESS)
	{
		Message("Wrong DryJet data.");
		return FALSE;
	}
			
//j	CBuildProgress	buildprogress( (long)&pDoc->_profiledocfiles, 
//j		(long)&m_gamut, Process_GammutCompress, pDoc->GetTitle());
//j	if (buildprogress.DoModal() == IDCANCEL)
//j		return FALSE;

	m_ingamut = TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTuningDlg::Init3D()
{
	RECT rect;
	m_wndGamut.GetClientRect(&rect);

	m_pControl = new CQd3dControl(rect, &m_wndGamut, &m_Qd3dGamut, -1/*id*/);
	if (!m_pControl)	return FALSE;

	BeginWaitCursor();

	McoStatus status = m_Qd3dGamut.InitFromDoc(m_gamut, m_pControl->m_hWnd);
	if(status != MCO_SUCCESS)
	{
		Message("Can't load QuickDraw3D.");
		return FALSE;
	}

	Update3dGamut(-1);
	EndWaitCursor();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakAdd() 
{
	// Create a gamut control window to display the 3d data, and to catch all mouse moves and button clicks
	// Initialize the control and the attached 3d data, the first time a tweak is added
	// This process can be moved to another convenient location, but was done here so
	// the Tuning tab can be displayed initially without a wait

//	Init3D();

	if (m_nTweaks >= MAX_NUM_TWEAKS)
		return; 

	// Create a new tweak
	CString szTweakName;
	szTweakName.Format("Tweak Number %d", m_nTweaks);
	Tweak_Element* pTweak = new Tweak_Element((char*)(LPCTSTR)szTweakName);
	if (!pTweak)
		return;

	// Set in the default values
	pTweak->lab_d[0] = 50;	//(((long)rand() * 100) / RAND_MAX);
	pTweak->lab_d[1] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	pTweak->lab_d[2] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	
	pTweak->lab_p[0] = 50;	//(((long)rand() * 100) / RAND_MAX);
	pTweak->lab_p[1] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	pTweak->lab_p[2] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	
	pTweak->lab_g[0] = 50;	//(((long)rand() * 100) / RAND_MAX);
	pTweak->lab_g[1] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;
	pTweak->lab_g[2] = 0;	//(((long)rand() * 255) / RAND_MAX) - 128;

	pTweak->Lr = 10;		//(((long)rand() * 100) / RAND_MAX); //50;
	pTweak->Cr = 10;		//(((long)rand() * 100) / RAND_MAX); //50;
	pTweak->type = Calibrate_Tweak;

	// Add it to the list
	m_pTweaks[m_nTweaks] = pTweak;
	m_pCurrentTweak = pTweak;
	m_nCurrentTweak = m_nTweaks;

	// Increment the number of tweaks
	m_nTweaks++;
	EnableAllButAdd(NULL, m_nTweaks > 0 /*bEnable*/);

	// Update the combobox list
	m_comboTweaks.AddString(m_pCurrentTweak->name);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);
	
	// Update all of the other controls
	SetControlsForCurrentTweak();

	SetButtonStyle(1);
	PutTweaksIntoDoc(); // Keep the doc up to date

	//setup the compressed lab
	SetupTweak(m_pCurrentTweak);
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakDelete() 
{
	if (!m_pCurrentTweak)
		return;

	// Delete the current tweak
	delete m_pCurrentTweak;
	int nDeletedTweak = m_nCurrentTweak;

	// Remove it from the list
	for (int j = m_nCurrentTweak+1; j < m_nTweaks; j++)
	{
		m_pTweaks[j-1] = m_pTweaks[j];
		m_pTweaks[j] = NULL;
	}

	// Decrement the number of tweaks
	m_nTweaks--;
	EnableAllButAdd(NULL, m_nTweaks > 0 /*bEnable*/);

	// Make sure the current tweak is still valid
	if (m_nCurrentTweak >= m_nTweaks)
		m_nCurrentTweak = m_nTweaks - 1;

	// Reset the current tweak pointer
 	m_pCurrentTweak = m_pTweaks[m_nCurrentTweak];

	// Update the combobox list
	m_comboTweaks.DeleteString(nDeletedTweak);
	m_comboTweaks.SetCurSel(m_nCurrentTweak);

	// Update all of the other controls
	SetControlsForCurrentTweak();

	PutTweaksIntoDoc(); // Keep the doc up to date

	//setup the compressed lab
	SetupTweak(m_pCurrentTweak);
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeLdesired() 
{
	if (GetFocus() != &m_editLDesired) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editLDesired.GetWindowText(szValue);
	m_pCurrentTweak->lab_d[0] = atof(szValue);
	m_colrCDesired.SetLABColor(m_pCurrentTweak->lab_d);
	Update3dGamut(m_pCurrentTweak->lab_d[0]);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeAdesired() 
{
	if (GetFocus() != &m_editADesired) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editADesired.GetWindowText(szValue);
	m_pCurrentTweak->lab_d[1] = atof(szValue);
	m_colrCDesired.SetLABColor(m_pCurrentTweak->lab_d);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeBdesired() 
{
	if (GetFocus() != &m_editBDesired) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editBDesired.GetWindowText(szValue);
	m_pCurrentTweak->lab_d[2] = atof(szValue);
	m_colrCDesired.SetLABColor(m_pCurrentTweak->lab_d);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeLgamut() 
{
	if (GetFocus() != &m_editLGamut) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editLGamut.GetWindowText(szValue);
	m_pCurrentTweak->lab_g[0] = atof(szValue);
	m_colrCGamut.SetLABColor(m_pCurrentTweak->lab_g);
	Update3dGamut(m_pCurrentTweak->lab_g[0]);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeAgamut() 
{
	if (GetFocus() != &m_editAGamut) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editAGamut.GetWindowText(szValue);
	m_pCurrentTweak->lab_g[1] = atof(szValue);
	m_colrCGamut.SetLABColor(m_pCurrentTweak->lab_g);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeBgamut() 
{
	if (GetFocus() != &m_editBGamut) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editBGamut.GetWindowText(szValue);
	m_pCurrentTweak->lab_g[2] = atof(szValue);
	m_colrCGamut.SetLABColor(m_pCurrentTweak->lab_g);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeLprint() 
{
	if (GetFocus() != &m_editLPrint) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editLPrint.GetWindowText(szValue);
	m_pCurrentTweak->lab_p[0] = atof(szValue);
	m_colrCPrint.SetLABColor(m_pCurrentTweak->lab_p);
	Update3dGamut(m_pCurrentTweak->lab_p[0]);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeAprint() 
{
	if (GetFocus() != &m_editAPrint) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editAPrint.GetWindowText(szValue);
	m_pCurrentTweak->lab_p[1] = atof(szValue);
	m_colrCPrint.SetLABColor(m_pCurrentTweak->lab_p);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakChangeBprint() 
{
	if (GetFocus() != &m_editBPrint) return;
	if (!m_pCurrentTweak) return;
	CString szValue;
	m_editBPrint.GetWindowText(szValue);
	m_pCurrentTweak->lab_p[2] = atof(szValue);
	m_colrCPrint.SetLABColor(m_pCurrentTweak->lab_p);
	Update3dGamut(-1);
	PutTweaksIntoDoc(); // Keep the doc up to date
	SetupTweaks(GetDoc(this));
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakCdesired()
{
	SetButtonStyle(1);
	DoTweakSetTool(kColorDesired);
	Update3dGamut(m_pCurrentTweak->lab_d[0]);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakCgamut() 
{
	if (!m_pCurrentTweak)	return;

	if (m_pCurrentTweak->type == Calibrate_Tweak)	return;

	SetButtonStyle(2);
	DoTweakSetTool(kColorGamut);
	Update3dGamut(m_pCurrentTweak->lab_g[0]);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::OnTweakCprint() 
{
	//BOOL test = m_colrCPrint.ModifyStyleEx(WS_EX_DLGMODALFRAME, WS_EX_CLIENTEDGE);
	SetButtonStyle(3);
	DoTweakSetTool(kColorPrint);
	Update3dGamut(m_pCurrentTweak->lab_p[0]);
}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::SetButtonStyle(int which)
{
	BOOL	test;
	switch(which)
	{
		case(1):
		test = m_colrCDesired.ModifyStyleEx(1, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
		test = m_colrCGamut.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		test = m_colrCPrint.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		break;

		case(2):
		m_colrCDesired.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		m_colrCGamut.ModifyStyleEx(1, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
		m_colrCPrint.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		break;

		case(3):
		m_colrCDesired.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		m_colrCGamut.ModifyStyleEx(WS_EX_CLIENTEDGE, 1);
		m_colrCPrint.ModifyStyleEx(1, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
		break;
	}

	CRect rect;

	m_colrCDesired.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
	m_colrCGamut.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
	m_colrCPrint.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);

}

/////////////////////////////////////////////////////////////////////////////
void CTuningDlg::EnableAllButAdd(CWnd* pControl, BOOL bEnable)
{
	if (!pControl)
		pControl = GetTopWindow();

	while (pControl)
	{
		int i = (int)(short)pControl->GetDlgCtrlID();
		if (i != IDC_TWEAK_ADD)
		{
			if (!pControl->IsWindowEnabled() != !bEnable)
			{
				pControl->EnableWindow(bEnable);
				pControl->Invalidate();
			}
		}

		pControl = pControl->GetNextWindow();
	}
}
