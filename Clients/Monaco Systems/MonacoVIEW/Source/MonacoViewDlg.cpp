#include "stdafx.h"
#include <math.h>
#include "fstream.h"
#include "monacoview.h"
#include "CalibrateDlg.h"
#include "MonacoViewDlg.h"
#include "PreferenceDlg.h"
#include "MeasureDlg.h"
#include "Splash.h"
#include "Message.h"
#include "colortran.h"
#include "winmisc.h"
#include "fileformat.h"
#include "csprofile.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Gamma menu
#define GM_07			0
#define GM_10	 		1
#define GM_12	 		2
#define GM_14	 		3
#define GM_16	 		4
#define GM_18	 		5
#define GM_20	 		6
#define GM_22	 		7
#define GM_28	 		8

// White point menu
#define WM_5000 		0
#define WM_5500 		1
#define WM_6500 		2
#define WM_7500 		3
#define WM_9300 		4
#define WM_MONITOR		6
#define WM_CUSTOM		8

#define CM_NORMAL		1
#define CM_WIDE			2

// Scroll bar values
#define SCROLL_MIN		0
#define SCROLL_MAX		100
#define SCROLL_PAGE		10

/////////////////////////////////////////////////////////////////////////////
// MonacoViewDlg dialog

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(MonacoViewDlg, CDialog)
	//{{AFX_MSG_MAP(MonacoViewDlg)
	ON_COMMAND(ID_FILE_CAL, OnFileCalibrateOn)
	ON_COMMAND(ID_FILE_CALIBRATE, OnFileCalibrate)
	ON_COMMAND(ID_FILE_MEASURE, OnFileMeasure)
	ON_COMMAND(ID_FILE_VALIDATE, OnFileValidate)
	ON_COMMAND(ID_FILE_PREF, OnFilePref)
	ON_COMMAND(ID_FILE_QUIT, OnFileQuit)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND_RANGE(ID_TOOLS_GAMMA_07, ID_TOOLS_WHITE_MON, OnTools)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TOOLS_GAMMA_07, ID_TOOLS_WHITE_MON, OnUpdateToolsUI)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CALIBRATE, OnCalibrate)
	ON_BN_CLICKED(IDC_MEASURE, OnMeasure)
	ON_BN_CLICKED(IDC_VALIDATE, OnValidate)
	ON_EN_CHANGE(IDC_IN_L, OnChangeInL)
	ON_EN_CHANGE(IDC_IN_A, OnChangeInA)
	ON_EN_CHANGE(IDC_IN_B, OnChangeInB)
	ON_EN_KILLFOCUS(IDC_IN_L, OnKillfocusInL)
	ON_EN_KILLFOCUS(IDC_IN_A, OnKillfocusInA)
	ON_EN_KILLFOCUS(IDC_IN_B, OnKillfocusInB)
	ON_CBN_SELCHANGE(IDC_GAMMA, OnSelchangeGamma)
	ON_CBN_SELCHANGE(IDC_WHITE, OnSelchangeWhite)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
MonacoViewDlg::MonacoViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MonacoViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(MonacoViewDlg)
	m_strDeltaE = _T("");
	m_gamma = _T("");
	m_white = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strOutL = _T("");
	m_strOutB = _T("");
	m_strOutA = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

/////////////////////////////////////////////////////////////////////////////
MonacoViewDlg::~MonacoViewDlg() // standard destructor
{
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MonacoViewDlg)
	DDX_Control(pDX, IDC_PATCH_WARN, m_patch_warn);
	DDX_Control(pDX, IDC_PATCH, m_patch);
	DDX_Control(pDX, IDC_COLOR_VIEW, m_color_view);
	DDX_Control(pDX, IDC_COLOR_SCROLL, m_color_scroll);
	DDX_Text(pDX, IDC_D_E, m_strDeltaE);
	DDX_CBString(pDX, IDC_GAMMA, m_gamma);
	DDX_CBString(pDX, IDC_WHITE, m_white);
	DDX_Text(pDX, IDC_OUT_L, m_strOutL);
	DDX_Text(pDX, IDC_OUT_B, m_strOutB);
	DDX_Text(pDX, IDC_OUT_A, m_strOutA);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::OnInitDialog() 
{
	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this, 5000);

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	// Initialize the data
	_Init();

	// Initialize the interface
	SetDlgItemInt(IDC_IN_L, m_in_l, false);
	SetDlgItemInt(IDC_IN_A, m_in_a, true);
	SetDlgItemInt(IDC_IN_B, m_in_b, true);

	CComboBox* pCombo;
	if (pCombo = (CComboBox*)GetDlgItem(IDC_WHITE))
		pCombo->SetCurSel(m_CurrentTargetWhitePoint);

	if (pCombo = (CComboBox*)GetDlgItem(IDC_GAMMA))
		pCombo->SetCurSel(m_CurrentTargetGamma);

	SCROLLINFO sinfo;
	sinfo.cbSize = sizeof(sinfo);
	sinfo.fMask = SIF_ALL; 
	sinfo.nMin = SCROLL_MIN; 
    sinfo.nMax = SCROLL_MAX;
    sinfo.nPage = SCROLL_PAGE; 
    sinfo.nPos = 50; 
    sinfo.nTrackPos = 50; 
	m_color_scroll.SetScrollInfo(&sinfo);
	
	m_color_view.Init();

	// See if the display card support gamma ramp changes
	CDC* pDC = GetDC();
	if (pDC)
	{
		if (!m_gammaTable.isGammaAvailable(pDC))
			Message("Your display device or driver does not support gamma correction");

		m_gammaTable.Init(pDC); 
	
		ReleaseDC(pDC);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::_Init(void)
{
	m_calibrationStatus = kNeverCalibrated;
	m_stateChanged 	= FALSE;
	m_ProfileSaved = TRUE;
	m_has_mea_data = FALSE;
	m_has_val_data = FALSE;
	m_has_new_cal = FALSE;

	m_port = 1;
	m_CurrentTargetWhitePoint 	= WM_5000;	
	m_CurrentTargetGamma 		= GM_18;
	m_CurrentGamutControl		= CM_NORMAL;
	m_UseCalibration = TRUE;
	m_save_photo_table = TRUE;
	m_save_icc_table = TRUE;

	m_in_l = 50;
	m_in_a = 0;
	m_in_b = 0;

	m_valid_num = VALID_PATCH_NUM;

	// Initialize the validation patch data
	for (int i=0; i<5; i++)
		m_deltaE[i] = 6;

	m_lab[0][0] = 40;
	m_lab[0][1] = 20;
	m_lab[0][2] = -20;

	m_lab[1][0] = 50;
	m_lab[1][1] = 20;
	m_lab[1][2] = 20;

	m_lab[2][0] = 60;
	m_lab[2][1] = -20;
	m_lab[2][2] = 20;

	m_lab[3][0] = 70;
	m_lab[3][1] = 0;
	m_lab[3][2] = -20;

	m_lab[4][0] = 80;
	m_lab[4][1] = -20;
	m_lab[4][2] = 0;	

	// Read the preference file
	if (readPreference())
	{
		m_stateChanged = TRUE;
		m_calibrationStatus = kWasCalibrated;
		LoadGammaTable(TRUE);
		m_dataConvert.reset(&m_monitorData);
	}
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// CG: The following line was added by the Splash Screen component.
		CSplashWnd::ShowSplashScreen(this);

		// Use the splash screen instead of the about dialog
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void MonacoViewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR MonacoViewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iLuminance = m_in_l;
	int newpos = -1;
	if (nSBCode == SB_LINEDOWN)
	{
		if (iLuminance + 1 <= SCROLL_MAX)
			newpos = iLuminance + 1;
	}
	else
	if (nSBCode == SB_LINEUP)
	{
		if (iLuminance - 1 >= SCROLL_MIN)
			newpos = iLuminance - 1;
	}
	else
	if (nSBCode == SB_PAGEDOWN)
	{
		if (iLuminance + SCROLL_PAGE <= SCROLL_MAX)
			newpos = iLuminance + SCROLL_PAGE;
		else
			newpos = SCROLL_MAX;
	}
	else
	if (nSBCode == SB_PAGEUP)
	{
		if (iLuminance - SCROLL_PAGE >= SCROLL_MIN)
			newpos = iLuminance - SCROLL_PAGE;
		else
			newpos = SCROLL_MIN;
	}
	else
	if (nSBCode == SB_THUMBTRACK)
	{
		newpos = nPos;
	}
	else
	if (nSBCode == SB_THUMBPOSITION)
	{
		newpos = nPos;
	}

	if (newpos != -1)
	{
		if (pScrollBar)
			pScrollBar->SetScrollPos(newpos);

		m_in_l = newpos;
		SetDlgItemInt(IDC_IN_L, m_in_l, false);

		RepaintColor(true);
	}

	CDialog::OnVScroll(nSBCode, newpos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::RepaintColor(bool bChangedL)
{
	if (bChangedL)
		m_color_view.RenderColor(m_in_l);

	double lab[3] = {m_in_l, m_in_a, m_in_b};
	m_patch.SetLABColor(lab);
	m_patch.UpdateWindow();
	DrawXY();
	DrawWarnPatch();
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::getchromaticity(double *r, double *g, double *b)
{
	if(m_calibrationStatus == kNeverCalibrated){
		r[0] = 0.63;
		r[1] = 0.33;
		g[0] = 0.29;
		g[1] = 0.60;
		b[0] = 0.15;
		b[1] = 0.07;
	}
	else{
		r[0] = m_monitorData.redCurve[NUM_INTENSITY_POINTS-1].intensity.x;
		r[1] = m_monitorData.redCurve[NUM_INTENSITY_POINTS-1].intensity.y;
		g[0] = m_monitorData.greenCurve[NUM_INTENSITY_POINTS-1].intensity.x;
		g[1] = m_monitorData.greenCurve[NUM_INTENSITY_POINTS-1].intensity.y;
		b[0] = m_monitorData.blueCurve[NUM_INTENSITY_POINTS-1].intensity.x;
		b[1] = m_monitorData.blueCurve[NUM_INTENSITY_POINTS-1].intensity.y;		
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::DrawWarnPatch(void)
{	
	LabColorType labcolor;
	COLORREF		rgbcolor;

	labcolor.L = (double)m_in_l;
	labcolor.a = (double)m_in_a;
	labcolor.b = (double)m_in_b;
	
	if(IsInGamut(&labcolor, &rgbcolor))
	{
		m_patch_warn.ShowWindow(SW_HIDE);
	}	
	else
	{
		m_patch_warn.ShowWindow(SW_SHOW);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::DrawXY(void)
{
	double r[3], g[3], b[3];

	if(getchromaticity(r, g, b))
	{
		CString sz;
		sz.Format("%5.3f", r[0]);
		SetDlgItemText(IDC_RED_X, sz );

		sz.Format("%5.3f", r[1]);
		SetDlgItemText(IDC_RED_Y, sz );

		sz.Format("%5.3f", g[0]);
		SetDlgItemText(IDC_GREEN_X, sz );

		sz.Format("%5.3f", g[1]);
		SetDlgItemText(IDC_GREEN_Y, sz );
		
		sz.Format("%5.3f", b[0]);
		SetDlgItemText(IDC_BLUE_X, sz );

		sz.Format("%5.3f", b[1]);
		SetDlgItemText(IDC_BLUE_Y, sz );
	}	
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnCalibrate() 
{
	CCalibrateDlg CalibrateDlg;
	CalibrateDlg.DoModal();

	COLORREF RGBlist[33] =
	{ 
		RGB(255,  0,  0), RGB(223,  0,  0), RGB(191,  0,  0),	RGB(159,  0,  0),
		RGB(128,  0,  0), RGB( 96,  0,  0),	RGB( 64,  0,  0),	RGB( 32,  0,  0),
		RGB(  0,255,  0), RGB(  0,223,  0), RGB(  0,191,  0),	RGB(  0,  0,  0),	
		RGB(  0,128,  0), RGB(  0, 96,  0),	RGB(  0, 64,  0),	RGB(  0, 32,  0),
		RGB(  0,  0,255), RGB(  0,  0,223), RGB(  0,  0,191),	RGB(  0,  0,159),	
		RGB(  0,  0,128), RGB(  0,  0, 96),	RGB(  0,  0, 64),	RGB(  0,  0, 32),
		RGB(255,255,255), RGB(223,223,223), RGB(191,191,191),	RGB(159,159,159),	
		RGB(128,128,128), RGB( 96, 96, 96),	RGB( 64, 64, 64),	RGB( 32, 32, 32),
		RGB(  0,  0,  0)
	};

	dYxy YxyMeasureList[33];

	CMeasureDlg MeasureDlg(RGBlist, YxyMeasureList, sizeof(RGBlist)/sizeof(COLORREF));
	if(MeasureDlg.DoModal() == IDOK)
	{
		short result = 0;
		
		result = GetData(RGBlist, YxyMeasureList, 
						sizeof(RGBlist)/sizeof(COLORREF), &m_monitorData);
		if(LoadGammaTable(TRUE))
		{
			m_dataConvert.reset(&m_monitorData);
			m_calibrationStatus = kJustBeenCalibrated;
			m_ProfileSaved = FALSE;
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::LoadGammaTable(BOOL calibrated)
{
	CDC* pDC = GetDC();

	if(calibrated)
	{
		m_Gamma.ComputeGamma(&m_monitorData);
		m_gammaTable.setCurrentTable(&m_Gamma); 
		m_gammaTable.loadCalibratedTable(pDC); 
	}
	else
	{
		m_gammaTable.loadOriginalTable(pDC); 
	}
	
	ReleaseDC(pDC);

	return TRUE;
}	

/////////////////////////////////////////////////////////////////////////////
// Extract the monitor intensity curves from the measured data.
// Note: The RGB values and color intensities are in decreasing order from 
// maximum to zero saturation for each of the primaries, plus white levels and black.
// I.e. R(max),...,R(min),G(max),...,G(min),B(max),...,B(min),W(max),...,W(min),K.
short MonacoViewDlg::GetData(COLORREF *rgbLevel, dYxy *YxyIntensity, 
					short colorCount, MonitorData *theMonitor)
{
#ifdef DEBUG
	ofstream outf("extractMonitor.out",ios::out|ios::trunc);
	outf << "theMonitor->targetWhitePoint.Y = " << theMonitor->targetWhitePoint.Y << "\n";
	outf << "theMonitor->targetWhitePoint.x = " << theMonitor->targetWhitePoint.x << "\n";
	outf << "theMonitor->targetWhitePoint.y = " << theMonitor->targetWhitePoint.y << "\n";
	outf << "theMonitor->targetGamma.red    = " << theMonitor->targetGamma.red << "\n";
	outf << "theMonitor->targetGamma.green = " << theMonitor->targetGamma.green << "\n";
	outf << "theMonitor->targetGamma.blue   = " << theMonitor->targetGamma.blue << "\n";
#endif

	int numStepsPerColor = (colorCount - 1)/4;  // number of steps for each color wedge

#ifdef DEBUG
	outf << "numStepsPerColor = " << numStepsPerColor << "\n";
#endif

	// Save black point info, which is the same for all three primaries.
	theMonitor->redCurve[0].index = 0.0; 
	theMonitor->redCurve[0].intensity.Y = YxyIntensity[colorCount -1].Y;
	theMonitor->redCurve[0].intensity.x = YxyIntensity[colorCount -1].x;
	theMonitor->redCurve[0].intensity.y = YxyIntensity[colorCount -1].y;
	theMonitor->greenCurve[0].index = 0.0; 
	theMonitor->greenCurve[0].intensity.Y = YxyIntensity[colorCount -1].Y;
	theMonitor->greenCurve[0].intensity.x = YxyIntensity[colorCount -1].x;
	theMonitor->greenCurve[0].intensity.y = YxyIntensity[colorCount -1].y;
	theMonitor->blueCurve[0].index = 0.0; 
	theMonitor->blueCurve[0].intensity.Y = YxyIntensity[colorCount -1].Y;
	theMonitor->blueCurve[0].intensity.x = YxyIntensity[colorCount -1].x;
	theMonitor->blueCurve[0].intensity.y = YxyIntensity[colorCount -1].y;

	int i;
	for (i=1; i<(numStepsPerColor+1); i++) {
		theMonitor->redCurve[i].index = 
			pow(((double) (rgbLevel[numStepsPerColor  - i] & 0x000000ff)) / 255.0, 
			INIT_CORRECTION_RED); 
		theMonitor->redCurve[i].intensity.Y = YxyIntensity[numStepsPerColor - i].Y;
		theMonitor->redCurve[i].intensity.x = YxyIntensity[numStepsPerColor - i].x;
		theMonitor->redCurve[i].intensity.y = YxyIntensity[numStepsPerColor - i].y;

		theMonitor->greenCurve[i].index = 
			pow(((double) ((rgbLevel[2*numStepsPerColor - i] & 0x0000ff00) >> 8))/ 255.0,
			INIT_CORRECTION_GREEN); 
		theMonitor->greenCurve[i].intensity.Y = YxyIntensity[2*numStepsPerColor - i].Y;
		theMonitor->greenCurve[i].intensity.x = YxyIntensity[2*numStepsPerColor - i].x;
		theMonitor->greenCurve[i].intensity.y = YxyIntensity[2*numStepsPerColor - i].y;

		theMonitor->blueCurve[i].index = 
			pow(((double) ((rgbLevel[3*numStepsPerColor - i] & 0x00ff0000) >> 16))/ 255.0,
			INIT_CORRECTION_BLUE); 
		theMonitor->blueCurve[i].intensity.Y = YxyIntensity[3*numStepsPerColor - i].Y;
		theMonitor->blueCurve[i].intensity.x = YxyIntensity[3*numStepsPerColor - i].x;
		theMonitor->blueCurve[i].intensity.y = YxyIntensity[3*numStepsPerColor - i].y;
	}


#ifdef DEBUG
	outf << "theMonitor->redCurve: \n";
	for (i=0; i<NUM_INTENSITY_POINTS; i++) {
		outf << "index = " <<  theMonitor->redCurve[i].index << "\t"; 
		outf << "intensity = ";
		outf << theMonitor->redCurve[i].intensity.Y << "\t";
		outf << theMonitor->redCurve[i].intensity.x << "\t";
		outf << theMonitor->redCurve[i].intensity.y << "\n";
	}
	outf << "theMonitor->greenCurve: \n";
	for (i=0; i<NUM_INTENSITY_POINTS; i++) {
		outf << "index = " <<  theMonitor->greenCurve[i].index << "\t"; 
		outf << "intensity = ";
		outf << theMonitor->greenCurve[i].intensity.Y << "\t";
		outf << theMonitor->greenCurve[i].intensity.x << "\t";
		outf << theMonitor->greenCurve[i].intensity.y << "\n";
	}
	outf << "theMonitor->blueCurve: \n";
	for (i=0; i<NUM_INTENSITY_POINTS; i++) {
		outf << "index = " <<  theMonitor->blueCurve[i].index << "\t"; 
		outf << "intensity = ";
		outf << theMonitor->blueCurve[i].intensity.Y << "\t";
		outf << theMonitor->blueCurve[i].intensity.x << "\t";
		outf << theMonitor->blueCurve[i].intensity.y << "\n";
	}
#endif

	RGBNumber gamma;
	m_Gamma.calcGamma((*theMonitor), &gamma);

#ifdef DEBUG
		outf << "measured gamma[i].red= " << gamma.red << "\n";
		outf << "measured gamma[i].green= " << gamma.green << "\n";
		outf << "measured gamma[i].blue= " << gamma.blue << "\n";
#endif

	// Sanity checks:
	
	short result =  dataOK;

	// Monotonicity check:
	for (i=1; i<NUM_INTENSITY_POINTS; i++) {
/*	
		if (theMonitor->redCurve[i].intensity.Y < 
			theMonitor->redCurve[i-1].intensity.Y) 	result = result | dataNotMonotonic;

		if (theMonitor->greenCurve[i].intensity.Y < 
			theMonitor->greenCurve[i-1].intensity.Y) 	result = result | dataNotMonotonic;

		if (theMonitor->blueCurve[i].intensity.Y < 
			theMonitor->blueCurve[i-1].intensity.Y) 	result = result | dataNotMonotonic;
*/

		if (theMonitor->redCurve[i].intensity.Y < 
			theMonitor->redCurve[i-1].intensity.Y)
			theMonitor->redCurve[i].intensity.Y = theMonitor->redCurve[i-1].intensity.Y;
			
		if (theMonitor->greenCurve[i].intensity.Y < 
			theMonitor->greenCurve[i-1].intensity.Y)
			theMonitor->greenCurve[i].intensity.Y = theMonitor->greenCurve[i-1].intensity.Y;

		if (theMonitor->blueCurve[i].intensity.Y < 
			theMonitor->blueCurve[i-1].intensity.Y)
			theMonitor->blueCurve[i].intensity.Y = theMonitor->blueCurve[i-1].intensity.Y;
	}

	// Range check:
	if (ABS((theMonitor->redCurve[NUM_INTENSITY_POINTS-1].intensity.Y - 
			  theMonitor->redCurve[0].intensity.Y)) < 2.0) 
		result = result | dataRangeBad;
		
	if (ABS((theMonitor->greenCurve[NUM_INTENSITY_POINTS-1].intensity.Y - 
			  theMonitor->greenCurve[0].intensity.Y)) < 2.0) 
		result = result | dataRangeBad;
		
	if (ABS((theMonitor->blueCurve[NUM_INTENSITY_POINTS-1].intensity.Y - 
			  theMonitor->blueCurve[0].intensity.Y)) < 0.8) 
		result = result | dataRangeBad;
		
	
#ifdef DEBUG
	outf.close();
#endif

	return(result);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnMeasure() 
{
	COLORREF rgb;
	dYxy YxyMeasure;

	LabColorType labcolor;
	labcolor.L = (double)m_in_l;
	labcolor.a = (double)m_in_a;
	labcolor.b = (double)m_in_b;

	if(!IsInGamut(&labcolor, &rgb))
	{
		AfxMessageBox(IDS_ERR_OUT_OF_GAMUT, MB_OK);
	}
	else
	{
		CMeasureDlg MeasureDlg(&rgb, &YxyMeasure, 1);
		if(MeasureDlg.DoModal() == IDOK)
		{

			LabColorType mealabcolor;

			m_dataConvert.Yxytolab((YxyColorType*)&YxyMeasure, &mealabcolor, 1);
			
			double deltaE = 0;
			deltaE += (mealabcolor.L - labcolor.L)*(mealabcolor.L - labcolor.L);
			deltaE += (mealabcolor.a - labcolor.a)*(mealabcolor.a - labcolor.a);
			deltaE += (mealabcolor.b - labcolor.b)*(mealabcolor.b - labcolor.b);
			deltaE = sqrt(deltaE);

			m_strDeltaE.Format("%5.3f", deltaE);
			m_strOutL.Format("%5.3f", mealabcolor.L);
			m_strOutA.Format("%5.3f", mealabcolor.a);
			m_strOutB.Format("%5.3f", mealabcolor.b);
			UpdateData(FALSE);		
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::IsInGamut(LabColorType *aLabColor, COLORREF *aRgbColor) 
{
	RGBColor		rgbcolor;
	long			outgamut;

	m_dataConvert.labtorgb(aLabColor, &rgbcolor, &outgamut, 1);
	if(outgamut)	return FALSE;
	
	*aRgbColor = RGB(rgbcolor.red >> 8, rgbcolor.green >> 8, rgbcolor.blue >> 8);
	return TRUE;
}	

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnValidate() 
{
	int i;
	COLORREF RGBlist[5];
	RGBColor		rgbcolor;
	long			outgamut[5];

	long valid_num = 0;
	for ( i=0; i<5; i++)
	{
		m_dataConvert.labtorgb((LabColorType*)m_lab[i], &rgbcolor, &outgamut[i], 1);
		if(!outgamut[i])
		{
			RGBlist[valid_num] = RGB(rgbcolor.red >> 8, rgbcolor.green >> 8, rgbcolor.blue >> 8);
			valid_num++;
		}
	}

	if(valid_num <= 0)	return;

	dYxy YxyMeasureList[5];
	CMeasureDlg MeasureDlg(RGBlist, YxyMeasureList, valid_num);
	if(MeasureDlg.DoModal() == IDOK)
	{
		LabColorType mealabcolor;
		double deltaE = 0;
		double temp;

		for(i = 0; i < 5; i++)
		{
			temp = 0;
			if(!outgamut[i])
			{
				m_dataConvert.Yxytolab((YxyColorType*)&YxyMeasureList[i], &mealabcolor, 1);
		
				temp += (m_lab[i][0] - mealabcolor.L)*(m_lab[i][0] - mealabcolor.L);
				temp += (m_lab[i][1] - mealabcolor.a)*(m_lab[i][1] - mealabcolor.a);
				temp += (m_lab[i][2] - mealabcolor.b)*(m_lab[i][2] - mealabcolor.b);
				deltaE += sqrt(temp);
			}
		}

		deltaE	/= valid_num;
		m_strDeltaE.Format("%5.3f", deltaE);
		UpdateData(FALSE);		
	}		

}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnChangeInL()
{
	BOOL b;
	m_in_l = GetDlgItemInt(IDC_IN_L, &b, false);
	if (m_in_l <= 0)
		m_in_l = 0;
	else
	if (m_in_l > 100)
		m_in_l = 100;

	RepaintColor(true);

	m_color_scroll.SetScrollPos(m_in_l);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnChangeInA() 
{
	BOOL b;
	m_in_a = GetDlgItemInt(IDC_IN_A, &b, true);
	if (m_in_a <= -200)
		m_in_a  = -200;
	else
	if (m_in_a > 200)
		m_in_a = 200;

	RepaintColor(false);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnChangeInB() 
{
	BOOL b;
	m_in_b = GetDlgItemInt(IDC_IN_B, &b, true);
	if (m_in_b <= -200)
		m_in_b  = -200;
	else
	if (m_in_b > 200)
		m_in_b = 200;

	RepaintColor(false);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnKillfocusInL()
{
	SetDlgItemInt(IDC_IN_L, m_in_l, false);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnKillfocusInA()
{
	SetDlgItemInt(IDC_IN_A, m_in_a, true);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnKillfocusInB()
{
	SetDlgItemInt(IDC_IN_B, m_in_b, true);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::SetPointColor(int a, int b)
{
	m_in_a = a;
	m_in_b = b;

	SetDlgItemInt(IDC_IN_A, m_in_a, true);
	SetDlgItemInt(IDC_IN_B, m_in_b, true);

	RepaintColor(false);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::SetGamma(CString& s)
{
	m_gamma = s;
	CComboBox *the_control = (CComboBox *)GetDlgItem(IDC_GAMMA);
//	the_control->GetLBText(the_control->GetCurSel(), m_gamma);
	handleGammaChoice(the_control->GetCurSel());
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::SetWhite(CString& s)
{
	m_white = s;
	CComboBox *the_control = (CComboBox *)GetDlgItem(IDC_WHITE);
//	the_control->GetLBText(the_control->GetCurSel(), m_white);
	handleWhiteChoice(the_control->GetCurSel());
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnSelchangeGamma() 
{
	CComboBox *the_control = (CComboBox *)GetDlgItem(IDC_GAMMA);
	the_control->GetLBText(the_control->GetCurSel(), m_gamma);
	handleGammaChoice(the_control->GetCurSel());
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnSelchangeWhite() 
{
	CComboBox *the_control = (CComboBox *)GetDlgItem(IDC_WHITE);
	the_control->GetLBText(the_control->GetCurSel(), m_white);
	handleWhiteChoice(the_control->GetCurSel());
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnTools(UINT nID)
{
	CMenu *menu = GetMenu();
	ASSERT(menu);
	CString mystring;
	menu->GetMenuString( nID, mystring, MF_BYCOMMAND); 

	if (nID <= ID_TOOLS_GAMMA_28)
		SetGamma(mystring);
	else
		SetWhite(mystring);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnUpdateToolsUI(CCmdUI *pCmdUI)
{
	CMenu *menu = GetMenu();
	ASSERT(menu);

	CString mystring;
	menu->GetMenuString( pCmdUI->m_nID, mystring, MF_BYCOMMAND); 

	if (pCmdUI->m_nID <= ID_TOOLS_GAMMA_28)
		pCmdUI->SetCheck(m_gamma == mystring ? true : false);
	else
		pCmdUI->SetCheck(m_white == mystring ? true : false);
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnOK() 
{
	saveProfile();
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnCancel() 
{
	int iResult = MessageOKCancel("Do you want to update your monitor profile?");
	if (iResult == IDOK)
		savePreference();

	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnFileCalibrateOn()
{
	m_UseCalibration = !m_UseCalibration;
	GetMenu()->CheckMenuItem(ID_FILE_CAL, (m_UseCalibration ? MF_CHECKED : MF_UNCHECKED));
	if(m_UseCalibration)
	{
		LoadGammaTable(TRUE);
	}
	else
	{
		LoadGammaTable(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnFileCalibrate()
{
	OnCalibrate();
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnFileMeasure()
{
	OnMeasure();
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnFileValidate()
{
	OnValidate();
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnFilePref() 
{
	CPreferenceDlg PreferenceDlg(&m_lab[0][0], &m_deltaE[0]);
	PreferenceDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnFileQuit() 
{
	OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
void MonacoViewDlg::OnFileSave()
{
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::handleGammaChoice(int item) 
{
	double gamma[] = 
	{ 0.7,	1.0,	1.2,	1.4,	1.6,	1.8,	2.0,	2.2,	2.8};

	if(item < GM_07 || item > GM_28)
	{
		item = GM_18;
	}

	m_monitorData.targetGamma.red = gamma[item];
	m_monitorData.targetGamma.green = gamma[item];
	m_monitorData.targetGamma.blue = gamma[item];
	
	if (m_CurrentTargetGamma != item) 
	{
		m_CurrentTargetGamma = item;
		m_stateChanged = TRUE;
		m_ProfileSaved = FALSE;
	}
	
	if( m_stateChanged && m_UseCalibration && (m_calibrationStatus != kNeverCalibrated))
	{ 
		LoadGammaTable(TRUE);
		m_stateChanged = FALSE;
		m_dataConvert.reset(&m_monitorData);
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::handleWhiteChoice(int item) 
{
	double monitorY[] =
	{100.0, 100.0, 100.0, 100.0, 100.0, -100.0};
	double monitorx[] =
	{0.3457, 0.3324, 0.3127, 0.2990, 0.2848, 0.33};
	double monitory[] = 
	{0.3585, 0.3474, 0.3290, 0.3149, 0.2932, 0.33};

	if(item < WM_5000 || item > WM_MONITOR)
	{
		item = WM_6500;
	}

	m_monitorData.targetWhitePoint.Y =  monitorY[item];	
	m_monitorData.targetWhitePoint.x =  monitorx[item];
	m_monitorData.targetWhitePoint.y =  monitory[item];

	if (m_CurrentTargetWhitePoint != item) 
	{
		m_CurrentTargetWhitePoint = item;
		m_stateChanged = TRUE;
		m_ProfileSaved = FALSE;
	}

	//not supported yet
	//case WM_CUSTOM:
			
	if( m_stateChanged && m_UseCalibration && (m_calibrationStatus != kNeverCalibrated))
	{ 
		LoadGammaTable(TRUE);
		m_stateChanged = FALSE;
		m_dataConvert.reset(&m_monitorData);
	}
	return TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::saveProfile(void)
{
	McoStatus status = MCO_SUCCESS;
	Msetup msetup;

	msetup.ver = 2;
	// The three channels should have the same gamma value:
	// The 0.5 is there for rounding.  Somehow the cast just truncates.
	msetup.gamma = (short)(100.0*(m_monitorData.targetGamma.red + 
		m_monitorData.targetGamma.green + m_monitorData.targetGamma.blue)/3.0 + 0.5);
		
	// if user specifies uncorrected white point, then
	// save the monitor white point in the profiles
	if (m_monitorData.targetWhitePoint.Y < 0.0) {  
		XYZColorType r, g, b, w, k;
		YxyColorType whitePoint;
		
		convertYxyToXYZ(m_monitorData.redCurve[NUM_INTENSITY_POINTS-1].intensity,&r);
		convertYxyToXYZ(m_monitorData.greenCurve[NUM_INTENSITY_POINTS-1].intensity,&g);
		convertYxyToXYZ(m_monitorData.blueCurve[NUM_INTENSITY_POINTS-1].intensity,&b);
		convertYxyToXYZ(m_monitorData.redCurve[0].intensity,&k);
		
		w.X = r.X + g.X + b.X - k.X - k.X;
		w.Y = r.Y + g.Y + b.Y - k.Y - k.Y;
		w.Z = r.Z + g.Z + b.Z - k.Z - k.Z;
		convertXYZToYxy(w,&whitePoint);

		msetup.wx = (short)(10000*whitePoint.x);
		msetup.wy = (short)(10000*whitePoint.y);
	}
	else 
	{  // otherwise save the specified target
		msetup.wx = (short)(10000*m_monitorData.targetWhitePoint.x);
		msetup.wy = (short)(10000*m_monitorData.targetWhitePoint.y);
	}
	
	// Normal gamut = gamut of calibrated monitor
	msetup.rx = (short) (10000 * 
		m_monitorData.redCurve[NUM_INTENSITY_POINTS-1].intensity.x);
	msetup.ry = (short) (10000 * 
		m_monitorData.redCurve[NUM_INTENSITY_POINTS-1].intensity.y);
	msetup.gx = (short) (10000 * 
		m_monitorData.greenCurve[NUM_INTENSITY_POINTS-1].intensity.x);
	msetup.gy = (short) (10000 * 
		m_monitorData.greenCurve[NUM_INTENSITY_POINTS-1].intensity.y);
	msetup.bx = (short) (10000 * 
		m_monitorData.blueCurve[NUM_INTENSITY_POINTS-1].intensity.x);
	msetup.by = (short) (10000 * 
		m_monitorData.blueCurve[NUM_INTENSITY_POINTS-1].intensity.y);
		
	// save Photo Shop profile
	if(m_save_photo_table)
	{
		CFileDialog FileDialog(
			/*bOpenFileDialog*/	FALSE,
			/*lpszDefExt*/		"icc",
			/*lpszFileName*/	"monitorphotoshop.icc",
			/*dwFlags*/			OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
			/*lpszFilter*/		"ICC profiles (*.icc)|*.icc|All Files (*.*)|*.*||"
		//	/*pParentWnd*/		this
			);

		if (FileDialog.DoModal() != IDOK)
			return FALSE;

		SavePhotoShopProfile(FileDialog.GetPathName(), &msetup);
	}

// save ICC profile:
	if(m_save_icc_table)
	{
		double	monitor[12];
		monitor[0] = (double)msetup.gamma/100.0;
		monitor[1] = (double)msetup.wx/10000.0;
		monitor[2] = (double)msetup.wy/10000.0;
		monitor[3] = (double)msetup.rx/10000.0;
		monitor[4] = (double)msetup.ry/10000.0;
		monitor[5] = (double)msetup.gx/10000.0;
		monitor[6] = (double)msetup.gy/10000.0;
		monitor[7] = (double)msetup.bx/10000.0;
		monitor[8] = (double)msetup.by/10000.0;

		char dirName[256];
		int dirSize = GetSystemDirectory(dirName, 256);
		if(!dirSize)
		{
			AfxMessageBox(IDS_ERR_NO_ICM_DIR, MB_OK);
			return FALSE;
		}
		strcat(dirName, "\\Color");
		BOOL exist = SetDirectory((LPCSTR)dirName);
		if(!exist)
		{
			AfxMessageBox(IDS_ERR_NO_ICM_DIR, MB_OK);
			return FALSE;
		}

		CFileDialog FileDialog(
			/*bOpenFileDialog*/	FALSE,
			/*lpszDefExt*/		"icc",
			/*lpszFileName*/	"monitorprofile.icc",
			/*dwFlags*/			OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT,
			/*lpszFilter*/		"ICC profiles (*.icc)|*.icc|All Files (*.*)|*.*||"
		//	/*pParentWnd*/		this
			);

		if (FileDialog.DoModal() != IDOK)
			return TRUE;

		SaveICCProfile(FileDialog.GetPathName(), monitor);		
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::SavePhotoShopProfile(CString szProfileName, Msetup* msetup)
{
	McoStatus status = MCO_SUCCESS;
	FileFormat	photoshop;

	status = photoshop.createFile((char*)(LPCSTR)szProfileName, O_CREAT | _O_TRUNC | _O_BINARY | O_WRONLY /*| _O_RDWR*/);
	if (status != MCO_SUCCESS)
		return FALSE;

	status = photoshop.relWrite(sizeof(Msetup), (char*)msetup);
	if (status != MCO_SUCCESS)
		return FALSE;

	photoshop.closeFile();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::SaveICCProfile(CString szProfileName, double* monitor)
{
	McoStatus status = MCO_SUCCESS;

	char szFileName[255];
	strcpy( szFileName, szProfileName );
	CsFormat csformat;

//convert the data to ICC format
	double xyztorgb[9], rgbtoxyz[9];
	status = newbuildrgbxyz(monitor, xyztorgb, rgbtoxyz);
	if(status != MCO_SUCCESS)	return FALSE;

	//scale to D50 because of ICC
	//D50 is 0.9642, 1, 0.8249
	double wX = monitor[1]/monitor[2];
	double wY = 1;
	double wZ = (1.0 - monitor[1] - monitor[2])/monitor[2];
	 
	for(int i = 0; i < 3; i++){
		rgbtoxyz[i] *= 0.9642/wX;
		rgbtoxyz[i+6] *= 0.8249/wZ;
	}

//save to ICC profile
	status = csformat.createFile((char*)(LPCSTR)szProfileName, O_CREAT | _O_TRUNC | _O_BINARY | O_WRONLY /*| _O_RDWR*/);
	if (status != MCO_SUCCESS)
		return FALSE;

	// Set header
	CM2Header header;
	header.CMMType = MONACO_CMM;
	header.profileVersion = (MCOP_MAJOR_VERSION + MCOP_MINOR_VERSION);
	header.profileClass = cmDisplayClass;
	header.dataColorSpace = cmRGBData;
	header.profileConnectionSpace = cmXYZData;
	header.CS2profileSignature = cmMagicNumber;	//?
	header.platform = MCOP_DEFAULT_PLATFORM;
	header.flags = MCOP_DEFAULT_FLAGS;
	header.deviceManufacturer = MCOP_DEFAULT_MANUFACTURER;
	header.deviceModel = MCOP_DEFAULT_MODEL;
	header.deviceAttributes[0] = MCOP_DEFAULT_ATTRIBUTES;
	header.deviceAttributes[1] = MCOP_DEFAULT_ATTRIBUTES;
	header.renderingIntent = MCOP_DEFAULT_RENDERINGINTENT;
	header.white.X = MCOP_DEFAULT_WHITE_X;
	header.white.Y = MCOP_DEFAULT_WHITE_Y;
	header.white.Z = MCOP_DEFAULT_WHITE_Z;

	status = csformat.setHeader(&header, cmSigLut8Type);
	if (status != MCO_SUCCESS)
		return FALSE;

	CMXYZType xyz;
	xyz.typeDescriptor = cmSigXYZType;
	xyz.reserved = 0x00000000;
	xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[0]*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[3]*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[6]*65536.0+0.5);
	status = csformat.setXYZtypedata(cmRedColorantTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[1]*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[4]*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[7]*65536.0+0.5);
	status = csformat.setXYZtypedata(cmGreenColorantTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	xyz.XYZ[0].X = (unsigned long)(rgbtoxyz[2]*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(rgbtoxyz[5]*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(rgbtoxyz[8]*65536.0+0.5);
	status = csformat.setXYZtypedata(cmBlueColorantTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	xyz.XYZ[0].X = (unsigned long)(wX*65536.0+0.5);
	xyz.XYZ[0].Y = (unsigned long)(wY*65536.0+0.5);
	xyz.XYZ[0].Z = (unsigned long)(wZ*65536.0+0.5);
	status = csformat.setXYZtypedata(cmMediaWhitePointTag, &xyz);
	if (status != MCO_SUCCESS)
		return FALSE;

	unsigned short gamma = (unsigned short)(monitor[0]*256);
	CMCurveType rh;
	rh.typeDescriptor = cmSigCurveType;
	rh.reserved = 0x00000000;
	rh.countValue = 1;
	status = csformat.setCurvetypedata(cmRedTRCTag, &rh, &gamma);
	if (status != MCO_SUCCESS)
		return FALSE;

	rh.countValue = 1;
	status = csformat.setCurvetypedata(cmGreenTRCTag, &rh, &gamma);
	if (status != MCO_SUCCESS)
		return FALSE;

	rh.countValue = 1;
	status = csformat.setCurvetypedata(cmBlueTRCTag, &rh, &gamma);
	if (status != MCO_SUCCESS)
		return FALSE;

//copyright tag
	if( (status = csformat.setCprttypedata(MCOP_CPRT, strlen(MCOP_CPRT))) != MCO_SUCCESS )
		return status;

//description tag
	//save the file name, not path name
	const char*	description = strrchr( (LPCSTR)szFileName, '\\' );
	if( description == NULL )
		description = (LPCSTR)szFileName;
	else
		description++;

	status = csformat.setDesctypedata((char*)description, strlen(description)+1);
	if (status != MCO_SUCCESS)
		return FALSE;

		
	status = csformat.writeTag(1);
	csformat.closeFile();

	if (status != MCO_SUCCESS)
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Read preferences and results of previous calibrations.
// This Code Warrior c++ implementation really sucks -
BOOL MonacoViewDlg::readPreference() 
{
	McoStatus 	status;
	FileFormat 	prefFile;  
	int i;

	// Does file exist?
	CString szDirName((LPCSTR)IDS_PREF_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)szDirName);
	if(!exist)	return FALSE;

	char szFileName[256] = { "\\MonacoVIEW1.0Pref"};
	exist = GetDefaultFile(szFileName);
	if(!exist)	return FALSE;

	status = prefFile.createFile(szFileName, _O_BINARY | O_RDONLY);
	if (status != MCO_SUCCESS) 
	{
		prefFile.closeFile();
		return FALSE;
	}
	
	// Get monitor data
	status = prefFile.relRead(sizeof(MonitorData), (char*)&m_monitorData); 
	if (status != MCO_SUCCESS)	goto bail;
	
	// Get state variables
	status = prefFile.relRead(sizeof(short), (char*)&m_port);
	if (status != MCO_SUCCESS)	goto bail;
	
	status = prefFile.relRead(sizeof(short), (char*) (&m_CurrentTargetWhitePoint));
	if (status != MCO_SUCCESS)	goto bail;
	
	status = prefFile.relRead(sizeof(short), (char*) (&m_CurrentTargetGamma));
	if (status != MCO_SUCCESS)	goto bail;

	status = prefFile.relRead(sizeof(short), (char*) (&m_CurrentGamutControl));
	if (status != MCO_SUCCESS)	goto bail;

	status = prefFile.relRead(sizeof(BOOL), (char*) (&m_UseCalibration));
	if (status != MCO_SUCCESS)	goto bail;

//following are the things added by Peter
	status = prefFile.relRead(sizeof(BOOL), (char*) (&m_save_photo_table));
	if (status != MCO_SUCCESS)	goto bail;

	status = prefFile.relRead(sizeof(BOOL), (char*) (&m_save_icc_table));
	if (status != MCO_SUCCESS)	goto bail;

	char lab[3];
	status = prefFile.relRead(sizeof(char)*3, lab);
	if (status != MCO_SUCCESS)	goto bail;
	
	m_in_l = (int)lab[0];
	m_in_a = (int)lab[1];
	m_in_b = (int)lab[2];

//following are the data added by Peter for validation
	status = prefFile.relRead(sizeof(long), (char*)&m_valid_num);
	if (status != MCO_SUCCESS)	goto bail;

	double valid_lab[3*5];
	status = prefFile.relRead(sizeof(double)*3*m_valid_num, (char*)valid_lab);
	if (status != MCO_SUCCESS)	goto bail;
	
	double deltaE;
	status = prefFile.relRead(sizeof(double), (char*)&deltaE);
	if (status != MCO_SUCCESS)	goto bail;

	// Initialize the validation patch data
	for (i=0; i<5; i++)
	{
		m_lab[i][0] = valid_lab[i*3+0];
		m_lab[i][1] = valid_lab[i*3+1];
		m_lab[i][2] = valid_lab[i*3+2];
		m_deltaE[i] = deltaE;
	}

/*	
	if( m_CurrentTargetWhitePoint == WM_CUSTOM)
		_show = 0;
	else
		_show = 1;
*/			
	prefFile.closeFile();
	return TRUE;

bail:
	prefFile.closeFile();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//should always save preference before quit
// Save preferences.
BOOL MonacoViewDlg::savePreference() 
{
	McoStatus 		status;
	FileFormat 	prefFile;
	int i;

	if(m_calibrationStatus == kNeverCalibrated)
		return FALSE;
	
	if(!m_UseCalibration)	return FALSE;

	CString szDirName((LPCSTR)IDS_PREF_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)szDirName);

	char szFileName[256] = {"\\MonacoVIEW1.0Pref"};
	exist = GetDefaultFile(szFileName);
	if(!exist)	return FALSE;
	
	// Create preference file.
	status = prefFile.createFile((char*)(LPCSTR)szFileName, O_CREAT | _O_TRUNC | _O_BINARY | O_WRONLY /*| _O_RDWR*/);
	if (status != MCO_SUCCESS)	goto bail;
	
	// save monitor data
	status = prefFile.relWrite(sizeof(MonitorData), (char*)&m_monitorData); 
	if (status != MCO_SUCCESS)	goto bail;
	
	// save state variables
	status = prefFile.relWrite(sizeof(short), (char*) (&m_port));
	if (status != MCO_SUCCESS)	goto bail;
	
	status = prefFile.relWrite(sizeof(short), (char*) (&m_CurrentTargetWhitePoint));
	if (status != MCO_SUCCESS)	goto bail;
	
	status = prefFile.relWrite(sizeof(short), (char*) (&m_CurrentTargetGamma));
	if (status != MCO_SUCCESS)	goto bail;
	
	status = prefFile.relWrite(sizeof(short), (char*) (&m_CurrentGamutControl));
	if (status != MCO_SUCCESS)	goto bail;
	
	status = prefFile.relWrite(sizeof(BOOL), (char*) (&m_UseCalibration));
	if (status != MCO_SUCCESS)	goto bail;

//following are the things added by Peter
	status = prefFile.relWrite(sizeof(BOOL), (char*) (&m_save_photo_table));
	if (status != MCO_SUCCESS)	goto bail;

	status = prefFile.relWrite(sizeof(BOOL), (char*) (&m_save_icc_table));
	if (status != MCO_SUCCESS)	goto bail;

	char lab[3];
	lab[0] = (char)m_in_l;
	lab[1] = (char)m_in_a;
	lab[2] = (char)m_in_b;

	status = prefFile.relWrite(sizeof(char)*3, lab);
	if (status != MCO_SUCCESS)	goto bail;

//added by Peter for handling the validation process
	status = prefFile.relWrite(sizeof(long), (char*)&m_valid_num);
	if (status != MCO_SUCCESS)	goto bail;

	double valid_lab[3*5];
	for (i=0; i<5; i++)
	{
		valid_lab[i*3+0] = m_lab[i][0];
		valid_lab[i*3+1] = m_lab[i][1];
		valid_lab[i*3+2] = m_lab[i][2];
	}

	status = prefFile.relWrite(sizeof(double)*3*m_valid_num, (char*)valid_lab);
	if (status != MCO_SUCCESS)	goto bail;

	status = prefFile.relWrite(sizeof(double), (char*)&m_deltaE[0]);
	if (status != MCO_SUCCESS)	goto bail;
		
	prefFile.closeFile();

	return TRUE;

bail:
	prefFile.closeFile();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL MonacoViewDlg::gamutbound(LabColorType *aLabColor)
{
	LabColorType inbound, outbound, curbound;
	COLORREF	rgbcolor;
	double delta = 10000;

	inbound.L = aLabColor->L;
	inbound.a = 0;
	inbound.b = 0;
		
	BOOL yes = IsInGamut(&inbound, &rgbcolor);
	if(!yes){
		aLabColor->a = 0;
		aLabColor->b = 0;
		return FALSE;
	}	
	
	outbound.L = aLabColor->L;
	outbound.a = aLabColor->a;
	outbound.b = aLabColor->b;
	
	curbound.L = aLabColor->L;
	while(delta > 4){
		curbound.a = (outbound.a + inbound.a)/2;
		curbound.b = (outbound.b + inbound.b)/2;
		yes = IsInGamut(&curbound, &rgbcolor);
		if(yes){	//inside the boundary
			inbound.a = curbound.a;
			inbound.b = curbound.b;
		}
		else{
			outbound.a = curbound.a;
			outbound.b = curbound.b;
		}
			
		delta = (outbound.a - inbound.a)*(outbound.a - inbound.a) + 
				(outbound.b - inbound.b)*(outbound.b - inbound.b);
	}	
	
	aLabColor->a = (outbound.a + inbound.a)/2;
	aLabColor->b = (outbound.b + inbound.b)/2;	
	return TRUE;	
}
