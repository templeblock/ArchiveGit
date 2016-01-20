#pragma once

#include "ProfilerDoc.h"

// PropertyPages.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalibratePage dialog

#include "grid.h"
#include "CurveWindow.h"
#include "DeviceControl.h"
#include "FirstDialog.h"


class CCalibratePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCalibratePage)

// Construction
public:
	CCalibratePage();
	~CCalibratePage();

	BOOL m_bShowCurveCyan;
	BOOL m_bShowCurveMagenta;
	BOOL m_bShowCurveYellow;
	BOOL m_bShowCurveBlack;
	CCurveWindow* m_pCurveWindow;
	CGrid* m_pGrid;

	//paper density
	double m_paper[4];
	BOOL   m_haspaper;
	CFirstDialog m_firstdialog;

//measurement stuff
	//current patch number and total patch number
	int m_currentPatch;
	int	m_totalPatches;

	int m_dataStart;
	int m_cyanStartPatches;
	int m_magentaStartPatches;
	int m_yellowStartPatches;
	int m_blackStartPatches;

	//check to see the device is reading data
	int	m_runningstatus;

	int m_currentRow;
	int m_currentCol;

	//device object
	CDeviceControl *m_pdevice;
	DeviceSetup	m_deviceSetup;

	int m_measureFlag;

//end of measurement stuff

// Dialog Data
	//{{AFX_DATA(CCalibratePage)
	enum { IDD = IDD_CALIBRATE };
	CButton	m_bMeasure;
	CEdit m_editDescription;
	CGrid m_ctlGrid;
	CEdit m_editLevelIn;
	CEdit m_editLevelOut;
	CString	m_date;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCalibratePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetDataFromDoc(CProfileDoc* pDoc);
	void SetCheck(BOOL enable);
protected:

	void SetPaperDensity(double* cmyk);
	BOOL GetPaperDensity();
	BOOL HasData();
	int	Todo();
	void ClearData(); 
	BOOL InitReading();
	void CleanupDevice();
	void ReadNextPatch();
	void SetParamForGrid();
	void PutonGrid(int row, int startcol, int endcol, double *data);
	BOOL AdjustDensity(double *density);

	afx_msg LRESULT OnRequestComplete(WPARAM wParam, LPARAM lParam);

	BOOL	OnMeasureDone(void);

	// Generated message map functions
	//{{AFX_MSG(CCalibratePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBlack();
	afx_msg void OnCyan();
	afx_msg void OnMagenta();
	afx_msg void OnYellow();
	afx_msg void OnReset();
	afx_msg void OnGridClick();
	afx_msg void OnGridKeyDown(short* pKeyCode, short Shift);
	afx_msg BOOL OnCurveChange(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDataReady(WPARAM wParam, LPARAM lParam);
	afx_msg void OnImport();
	afx_msg void OnMeasure();
	afx_msg void OnExport();
	afx_msg void OnDescriptionChange();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CToneAdjustPage dialog

#include "grid.h"
#include "CurveWindow.h"

class CToneAdjustPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CToneAdjustPage)

// Construction
public:
	CToneAdjustPage();
	~CToneAdjustPage();

	BOOL m_bShowCurveCyan;
	BOOL m_bShowCurveMagenta;
	BOOL m_bShowCurveYellow;
	BOOL m_bShowCurveBlack;
	CCurveWindow* m_pCurveWindow;
	CGrid* m_pGrid;

// Dialog Data
	//{{AFX_DATA(CToneAdjustPage)
	enum { IDD = IDD_TONEADJUST };
	CEdit m_editDescription;
	CGrid m_ctlGrid;
	CEdit m_editLevelIn;
	CEdit m_editLevelOut;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CToneAdjustPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetDataFromDoc(CProfileDoc* pDoc);
protected:
	// Generated message map functions
	//{{AFX_MSG(CToneAdjustPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBlack();
	afx_msg void OnCyan();
	afx_msg void OnMagenta();
	afx_msg void OnYellow();
	afx_msg void OnReset();
	afx_msg void OnGridClick();
	afx_msg void OnGridKeyDown(short* pKeyCode, short Shift);
	afx_msg BOOL OnCurveChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnDescriptionChange();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CColorDataPage dialog

#include "grid.h"
#include "ViewGamut.h"
#include "Qd3dExtras.h"

class CColorDataPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CColorDataPage)

// Construction
public:
	CColorDataPage();
	~CColorDataPage();

	int	m_whichData;
	CViewGamut* m_pViewGamutDialogSrc;
	CViewGamut* m_pViewGamutDialogDst;

// Dialog Data
	//{{AFX_DATA(CColorDataPage)
	enum { IDD = IDD_COLORDATA };
	CEdit m_editDescription;
	CEdit m_editDescriptionDry;
	CStatic m_bmpDataOKSource;
	CStatic m_bmpDataOKDryjet;
	CString	m_srcDate;
	CString	m_dryDate;
	//}}AFX_DATA

	#ifdef NO_COLORDATA_GRIDS
	// To restore grids, move these lines into the AFX_DATA block above
	CGrid m_ctlGridSrc;
	CGrid m_ctlGridDst;
	#endif NO_COLORDATA_GRIDS

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorDataPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetDataFromDoc(CProfileDoc* pDoc);
	void SetCheck(int which, BOOL enable);
protected:
	BOOL LoadDataFromGrid(PatchFormat *patchdata, CGrid *pGrid);
	void GetSourceDataFromDoc(CProfileDoc* pDoc);
	void GetDryjetDataFromDoc(CProfileDoc* pDoc);
	// Generated message map functions
	//{{AFX_MSG(CColorDataPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnGridSrcClick();
	afx_msg void OnGridSrcKeyDown(short* pKeyCode, short Shift);
	afx_msg void OnGridDstClick();
	afx_msg void OnGridDstKeyDown(short* pKeyCode, short Shift);
	afx_msg void OnMeasure();
	afx_msg void OnMeasureDry();
	afx_msg void OnExport();
	afx_msg void OnExportDry();
	afx_msg void OnImport();
	afx_msg void OnImportDry();
	afx_msg void OnViewData();
	afx_msg void OnViewDataDry();
	afx_msg void OnViewGamut();
	afx_msg void OnViewGamutDry();
	afx_msg void OnDescriptionChange();
	afx_msg void OnDescriptionDryChange();
	afx_msg BOOL OnDataReady(WPARAM wParam, LPARAM lParam);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage dialog

class CSettingsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSettingsPage)

// Construction
public:
	CSettingsPage();
	~CSettingsPage();

// Dialog Data
	//{{AFX_DATA(CSettingsPage)
	enum { IDD = IDD_SETTINGS };
	CComboBox	m_comboMode;
	CButton		m_radioTextEnhOn;
	CButton		m_radioTextEnhOff;
	CButton		m_radioHighLightOn;
	CButton		m_radioHighLightOff;
	CButton		m_radioCopyPaperOn;
	CButton		m_radioCopyPaperOff;
	CSliderCtrl	m_sldBalanceCutoff;
	CStatic		m_staBalanceCutoff;
	CSliderCtrl	m_sldContrast;
	CStatic		m_staContrast;
	CSliderCtrl	m_sldSaturation;
	CStatic		m_staSaturation;
	CSliderCtrl	m_sldSmoothing;
	CStatic		m_staSmoothing;
	//}}AFX_DATA

	BOOL	m_automatic;	//TRUE
	BOOL	m_textEnh;		//TRUE
	BOOL	m_copyPaper;	//TRUE
	BOOL	m_highLightOn;	//TRUE
	int		m_colorBalanceCutoff;	//94
	int		m_colorBalance;			//100
	int		m_inkNeutralize;		//1
	int		m_contrast;		//80
 	int		m_saturation;	//80
	int		m_smoothSimu;	//0
			

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetDataFromDoc(CProfileDoc* pDoc);
	BOOL SetDataToDoc(CProfileDoc* pDoc);
protected:
	void SetDataToControls(BOOL bAuto);
	void EnableStatics(BOOL bEnable);
	// Generated message map functions
	//{{AFX_MSG(CSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnModeSelected(BOOL bSave = TRUE);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTextEnhanceOff();
	afx_msg void OnTextEnhanceOn();
	afx_msg void OnCopyPaperOn();
	afx_msg void OnCopyPaperOff();
	afx_msg void OnHighLightOn();
	afx_msg void OnHighLightOff();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CTuningPage dialog

#include "ColorControl.h"
#include "Qd3dGamutTweak.h"
#include "Qd3dControl.h"
#include "profileview.h"

enum kTOOL { kNone, kMove, kSize, kColorDesired, kColorGamut, kColorPrint };

class CTuningPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTuningPage)

// Construction
public:
	CTuningPage(CProfileView *pView);
	CTuningPage();
	~CTuningPage();

	CProfileView *m_pView;
	BOOL	m_inited;
	BOOL	m_ingamut;
	McoHandle	m_gamut;


//device data
	CDeviceControl *m_pdevice;
	DeviceSetup	m_deviceSetup;
	BOOL   m_whiteCalibrated;
	CFirstDialog m_firstdialog;


//tweak data
	Tweak_Element* m_pTweaks[MAX_NUM_TWEAKS];
	Tweak_Element* m_pCurrentTweak;
	int m_nTweaks;
	int m_nCurrentTweak;
	COLORREF m_colorDesired;
	COLORREF m_colorDesired2;
	COLORREF m_colorGamut;
	COLORREF m_colorGamut2;
	COLORREF m_colorPrint;
	COLORREF m_colorPrint2;
	CQd3dGamutTweak m_Qd3dGamut;
	CQd3dControl* m_pControl;
	BOOL m_bMouseDown;
	kTOOL m_kTool;
		
// Dialog Data
	//{{AFX_DATA(CTuningPage)
	enum { IDD = IDD_TUNING };
	CStatic		m_wndGamut;
	CSliderCtrl	m_sldLightRange;
	CStatic		m_staLightRange;
	CSliderCtrl	m_sldColorRange;
	CStatic		m_staColorRange;
	CScrollBar	m_scrLightness;
	CStatic		m_staLightness;
	CComboBox	m_comboTweaks;
	CButton		m_radioMoveTool;
	CButton		m_radioSizeTool;
	CButton		m_radioGamut;
	CButton		m_radioPrint;
	CEdit		m_editLDesired;
	CEdit		m_editADesired;
	CEdit		m_editBDesired;
	CColorControl	m_colrCDesired;
	CStatic		m_staLDesired2;
	CStatic		m_staADesired2;
	CStatic		m_staBDesired2;
	CColorControl	m_colrCDesired2;
	CEdit		m_editLGamut;
	CEdit		m_editAGamut;
	CEdit		m_editBGamut;
	CColorControl	m_colrCGamut;
	CStatic		m_staLGamut2;
	CStatic		m_staAGamut2;
	CStatic		m_staBGamut2;
	CColorControl	m_colrCGamut2;
	CEdit		m_editLPrint;
	CEdit		m_editAPrint;
	CEdit		m_editBPrint;
	CColorControl	m_colrCPrint;
	CStatic		m_staLPrint2;
	CStatic		m_staAPrint2;
	CStatic		m_staBPrint2;
	CColorControl	m_colrCPrint2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTuningPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsPageOkToShow(void);
	void GetDataFromDoc(CProfileDoc* pDoc);
protected:
	void SetButtonStyle(int which);
	BOOL BuildGamut();
	BOOL Init3D();
	BOOL IsColorInGamut(double* lab);
	void SetupTweak(Tweak_Element* tweak); 
	void PutTweaksIntoDoc(void);
	void ReloadTweakList(void);
	void SetControlsForCurrentTweak(void);
	void SetTweakRadio(BOOL bGamutMapping);
	void Update3dGamut(int iLvalue);
	void OnTweakMouseMove(POINT pt);
	void OnTweakMouseDown(POINT pt, POINT ptLast);
	void DoTweakSetTool(kTOOL kTool);
	void DoTweakColorPickTool(POINT pt, kTOOL kTool);
	void DoTweakRotateTool(POINT pt, POINT ptLast);
	void DoTweakMoveTool(POINT pt, POINT ptLast);
	void DoTweakSizeTool(POINT pt, POINT ptLast);
	void EnableAllButAdd(CWnd* pTopWnd, BOOL bEnable);

//device functions
//	void ReadNextPatch();
//	afx_msg LRESULT OnRequestComplete(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CTuningPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTweakAdd();
	afx_msg void OnTweakSelectEditchange();
	afx_msg void OnTweakSelectSelchange();
	afx_msg void OnTweakDelete();
	afx_msg void OnTweakMoveTool();
	afx_msg void OnTweakSizeTool();
	afx_msg void OnTweakPrintRadio();
	afx_msg void OnTweakGamutRadio();
	afx_msg void OnTweakChangeLdesired();
	afx_msg void OnTweakChangeAdesired();
	afx_msg void OnTweakChangeBdesired();
	afx_msg void OnTweakCdesired();
	afx_msg void OnTweakChangeLgamut();
	afx_msg void OnTweakChangeAgamut();
	afx_msg void OnTweakChangeBgamut();
	afx_msg void OnTweakCgamut();
	afx_msg void OnTweakChangeLprint();
	afx_msg void OnTweakChangeAprint();
	afx_msg void OnTweakChangeBprint();
	afx_msg void OnTweakCprint();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
