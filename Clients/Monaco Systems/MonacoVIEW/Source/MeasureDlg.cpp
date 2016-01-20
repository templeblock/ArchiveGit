#include "stdafx.h"
#include "monacoview.h"
#include "MeasureDlg.h"
#include "SipCal.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	BEGIN_STATE	1
#define	DONE_STATE	2

BEGIN_MESSAGE_MAP(CMeasureDlg, CDialog)
	//{{AFX_MSG_MAP(CMeasureDlg)
	ON_BN_CLICKED(ID_BEGIN, OnMeasure)
	ON_BN_CLICKED(IDC_MEAS_FREQ, OnMeasureFrequency)
	ON_BN_CLICKED(IDC_SET_INTTIME, OnSetIntegrationTime)
	ON_BN_CLICKED(IDC_SET_NUM_PERIODS, OnSetNumPeriods)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasureDlg dialog

char szVersion[128];

/////////////////////////////////////////////////////////////////////////////
CMeasureDlg::CMeasureDlg(COLORREF* pRGBlist, dYxy* pYxyMeasure, int iCount, CWnd* pParent /*=NULL*/) : CDialog(CMeasureDlg::IDD, pParent)
{
	m_pRGBlist = pRGBlist;
	m_pYxyMeasure = pYxyMeasure;
	m_iCount = iCount;
	//{{AFX_DATA_INIT(CMeasureDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CMeasureDlg::~CMeasureDlg()
{
	sipCloseCalibrator();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMeasureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	BeginWaitCursor();

	long lResult = sipOpenCalibrator(1/*COM1:*/, szVersion, NULL);
	if (lResult != SUCCESS)
	{
		lResult = sipOpenCalibrator(2/*COM1:*/, szVersion, NULL);
		if (lResult != SUCCESS)
			Message("Error finding the calibrator on COM1 or COM2 %ld", lResult);
	}

	SetDlgItemText(IDC_CAL_VERSION, szVersion);

	double dIntTime;
	sipGetIntegrationTime(&dIntTime);
	CString szString;
	szString.Format("%8.5lf", dIntTime);
	SetDlgItemText(IDC_INTTIME, szString);

	EndWaitCursor();

	m_state = BEGIN_STATE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeasureDlg)
	DDX_Control(pDX, ID_BEGIN, m_btnBegin);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
void CMeasureDlg::DisplayColorPatch(CWnd* pWnd, COLORREF lColor)
{
	if (!pWnd)
		return;

	CDC* pMainDC = pWnd->GetDC();
	if (!pMainDC)
		return;

	HBRUSH hBrush = CreateSolidBrush (lColor);
	hBrush = (HBRUSH)pMainDC->SelectObject(hBrush);

	HPEN hPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
	hPen = (HPEN)pMainDC->SelectObject(hPen);

	RECT MeasurementRect;
	pWnd->GetClientRect(&MeasurementRect);
	pMainDC->Ellipse(&MeasurementRect);

	DeleteObject(pMainDC->SelectObject(hBrush)); 
	DeleteObject(pMainDC->SelectObject(hPen));

	ReleaseDC(pMainDC);
}

/////////////////////////////////////////////////////////////////////////////
double CMeasureDlg::TakeMeasurement(dYxy* pYxyMeasure)
{
	if (!pYxyMeasure)
		return 0;

	double dStart = GetTickCount();

	long lResult;
	unsigned short usUnits = (IsDlgButtonChecked(IDC_UNITS_FTLTS) ? FOOT_LAMBERTS : CANDELAS);
	if (IsDlgButtonChecked(IDC_AUTOMEAS_CHECK))
		lResult = sipAutoMeasureYxy(usUnits, pYxyMeasure);
	else
		lResult = sipMeasureYxy(usUnits, pYxyMeasure);

	double dEnd = GetTickCount();

	MessageBeep(UINT(-1));

	if (lResult != SUCCESS)
		return 0;

	return (dEnd - dStart) * (1e-3);
}

/////////////////////////////////////////////////////////////////////////////
void CMeasureDlg::OnMeasure() 
{
	if(m_state == BEGIN_STATE)
	{
		SetDlgItemText(IDC_CAL_VERSION, szVersion);

		CWnd* pWnd = GetDlgItem(IDC_SENSOR);
		if (!pWnd)
			return;

		BeginWaitCursor();

		for (int i=0; i<m_iCount; i++)
		{
			DisplayColorPatch(pWnd, m_pRGBlist[i]);
			dYxy* pYxy = m_pYxyMeasure + i;
			double dTimeToMeasure = TakeMeasurement(pYxy);
			if (dTimeToMeasure)
			{
				// Update the integration time field
				double dIntTime;
				if (sipGetIntegrationTime(&dIntTime) == SUCCESS)
				{
					CString szString;
					szString.Format("%8.5lf", dIntTime);
					SetDlgItemText(IDC_INTTIME, szString);
				}

				// Update the time field
				CString szString;
				szString.Format("%8.5lf", dTimeToMeasure);
				SetDlgItemText(IDC_MEAS_TIME, szString);

				// Update themeasurement fields
				szString.Format("%6.3lf", pYxy->Y);
				SetDlgItemText(IDC_DISPLAY_L, szString);
				szString.Format("%6.4lf", pYxy->x);
				SetDlgItemText(IDC_DISPLAY_x, szString);
				szString.Format("%6.4lf", pYxy->y);
				SetDlgItemText(IDC_DISPLAY_y, szString);

				UpdateWindow();
			}
			else
			{
	//j			Message("Error taking the measurement %ld", lResult);
	//j			break;
				Sleep(1000);
			}
		}

		EndWaitCursor();

		m_state = DONE_STATE;
		m_btnBegin.SetWindowText("Done");

		// Redraw the original window
		pWnd->Invalidate(FALSE);
	}
	else
		CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CMeasureDlg::OnMeasureFrequency() 
{
//	if (!gCalState)
//	{
//		MessageBeep((UINT)-1);
//		return;;
//	}

	char szNumber[64];
	GetDlgItemText(IDC_NUM_PERIODS, szNumber, 10);
	short nper = atoi(szNumber);
	double dMonFreq = 0;
	long lResult = sipMeasMonFreqAndSetIntTime(nper, &dMonFreq);
	if (lResult == ERR_FF_COULD_NOT_SYNC)
		Message("Error measuring the monitor frequency; Could not sync to monitor");
	else
	if (lResult != SUCCESS)
		Message("Error measuring the monitor frequency %ld", lResult);

	CString szString;
	szString.Format("%6.3lf", dMonFreq);
	SetDlgItemText(IDC_MONFREQ, szString);

	// read int time set in calibrator
	double dIntTime;
	if ((lResult = sipGetIntegrationTime(&dIntTime)) != SUCCESS)
	{
		Message("Error getting the integration time %ld", lResult);
		return;
	}

	szString.Format("%8.5lf", dIntTime);
	SetDlgItemText(IDC_INTTIME, szString);

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CMeasureDlg::OnSetIntegrationTime() 
{
//	if (!gCalState)
//	{
//		MessageBeep((UINT)-1);
//		return;;
//	}

	// get the int time entered in the edit box
	char szNumber[64];
	GetDlgItemText(IDC_INTTIME, (LPSTR)szNumber, 10);
	double dIntTime = (double)atof(szNumber);

	// set int time in calibrator
	long lResult;
	if ((lResult = sipSetIntegrationTime(dIntTime)) != SUCCESS)
	{
		Message("Error setting the integration time %ld", lResult);
		return;
	}

	// read int time as set in calibrator
	if ((lResult = sipGetIntegrationTime(&dIntTime)) != SUCCESS)
	{
		Message("Error getting the integration time %ld", lResult);
		return;
	}

	CString szString;
	szString.Format("%8.5lf", dIntTime);
	SetDlgItemText(IDC_INTTIME, szString);

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CMeasureDlg::OnSetNumPeriods() 
{
	char szNumber[64];
	GetDlgItemText(IDC_NUM_PERIODS, (LPSTR)szNumber, 10);
	short nper = atoi(szNumber);
	sipSetNumPeriodsToMeasure(nper);
}

#ifdef NOTUSED
/////////////////////////////////////////////////////////////////////////////
void CW32dtestDlg::OnDarkMeasure() 
{
	HCURSOR hSaveCursor;
	unsigned char ucRed, ucGreen, ucBlue;
	short stemp;
	double dStart, dEnd, dTime;

//	if (!gCalState)
//	{
//		MessageBeep((UINT)-1);
//		return;;
//	}

	CString szString;
	GetDlgItemText(IDC_DARK_RED_PERS, szString, 10);
	stemp = atoi(szString);
	if (stemp > 255)
	{
		Message("Invalid Parameter");
		return;
	}
	ucRed = (unsigned char)atoi(szString);

	GetDlgItemText(IDC_DARK_GREEN_PERS, szString, 10);
	stemp = atoi(szString);
	if (stemp > 255)
	{
		Message("Invalid Parameter");
		return;
	}
	ucGreen = (unsigned char)atoi(szString);

	GetDlgItemText(IDC_DARK_BLUE_PERS, szString, 10);
	stemp = atoi(szString);
	if (stemp > 255)
	{
		Message("Invalid Parameter");
		return;
	}
	ucBlue = (unsigned char)atoi(sztemp);

	hSaveCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));

	dStart = GetTickCount();
	dYxy Yxymeas;
	long lResult = sipDarkMeasureYxy((IsDlgButtonChecked(IDC_UNITS_FTLTS) ? FOOT_LAMBERTS : CANDELAS),
			ucRed, ucGreen, ucBlue, (IsDlgButtonChecked(IDC_DARK_RGB_SIMUL) ? 1 : 0), &Yxymeas);
	dEnd = GetTickCount();

	MessageBeep(UINT(-1));
	SetCursor(hSaveCursor);
	if (lResult != SUCCESS)
	{
		Message("Error doing the dark measurement %ld", lResult);
		return;
	}

	dTime = (dEnd-dStart)*1e-3;
	CString szString;
	szString.Format("%6.3lf", dTime);
	SetDlgItemText(IDC_MEAS_TIME, szString);

	// update display with measurement data
	szString.Format("%6.3lf", Yxymeas.Y);
	SetDlgItemText(IDC_DISPLAY_L, szString);
	szString.Format("%6.4lf", Yxymeas.x);
	SetDlgItemText(IDC_DISPLAY_x, szString);
	szString.Format("%6.4lf", Yxymeas.y);
	SetDlgItemText(IDC_DISPLAY_y, szString);
}
#endif NOTUSED

