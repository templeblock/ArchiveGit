#if !defined(AFX_MONACOVIEWDLG_H__6EECDFC2_EB46_11D0_9B8C_444553540000__INCLUDED_)
#define AFX_MONACOVIEWDLG_H__6EECDFC2_EB46_11D0_9B8C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ColorGamut.h"
#include "ColorControl.h"
#include "Gamma.h"
#include "GammaTable.h"
#include "dataconvert.h"
#include "SipCal.h"
#include "define.h"

/////////////////////////////////////////////////////////////////////////////
// MonacoViewDlg dialog

// max number of colors (used for array declarations)
#define kMaxColorCnt	64
#define	VALID_PATCH_NUM	(5)

typedef struct Msetup
{
	short ver;
	short gamma;
	short wx;
	short wy;
	short rx;
	short ry;
	short gx;
	short gy;
	short bx;
	short by;
} Msetup;

class CColorGamut;

class MonacoViewDlg : public CDialog
{
public:
	MonacoViewDlg(CWnd* pParent = NULL);	// standard constructor
	~MonacoViewDlg();	// standard destructor

	void SetPointColor(int a, int b);
	void SetGamma(CString& );
	void SetWhite(CString& );
	BOOL gamutbound(LabColorType *aLabColor);

// Dialog Data
	CString m_gamma;
	CString m_white;
	HICON m_hIcon;

	//{{AFX_DATA(MonacoViewDlg)
	enum { IDD = IDD_MONACOVIEW };
	CStatic	m_patch_warn;
	CColorControl m_patch;
	CColorGamut	m_color_view;
	CScrollBar m_color_scroll;
	CString	m_strDeltaE;
	CString	m_strOutL;
	CString	m_strOutB;
	CString	m_strOutA;
	//}}AFX_DATA

	//flags
	short	m_calibrationStatus;
	BOOL	m_stateChanged;
	BOOL	m_ProfileSaved;
	BOOL	m_has_mea_data;
	BOOL	m_has_val_data;
	BOOL	m_has_new_cal;

	//data for saving
//Measurement data
	MonitorData	m_monitorData;
	short		m_port;
	short		m_CurrentTargetWhitePoint;
	short		m_CurrentTargetGamma;
	short		m_CurrentGamutControl;
	BOOL		m_UseCalibration;
	BOOL		m_save_photo_table;
	BOOL		m_save_icc_table;

	int			m_in_l;
	int			m_in_a;
	int			m_in_b;
	long		m_valid_num;
	double		m_deltaE[5];
	double		m_lab[5][3];

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	Gamma		m_Gamma;
	GammaTable	m_gammaTable;
	DataConvert	m_dataConvert;

	void		_Init(void);
	BOOL		saveProfile(void);
	BOOL		SavePhotoShopProfile(CString szProfileName, Msetup* msetup);
	BOOL		SaveICCProfile(CString szProfileName, double* monitor);
	BOOL		readPreference(); 
	BOOL		savePreference(); 

	// stores the copy of the image,
	// will be used for manipulation

	//get the measurement data
	short GetData(COLORREF *rgbLevel, dYxy *YxyIntensity, 
					short colorCount, MonitorData *theMonitor);
	BOOL LoadGammaTable(BOOL calibrated);
	BOOL handleGammaChoice(int item); 
	BOOL handleWhiteChoice(int item); 

	BOOL IsInGamut(LabColorType *aLabColor, COLORREF *aRgbColor);
	BOOL getchromaticity(double *r, double *g, double *b);

	// repaint color if l,a,b is changed
	void RepaintColor(bool bChangedL); // if l has changed
	void DrawXY(void);
	void DrawWarnPatch(void);

	// Generated message map functions
	//{{AFX_MSG(MonacoViewDlg)
	afx_msg void OnFileCalibrateOn();
	afx_msg void OnFileCalibrate();
	afx_msg void OnFileMeasure();
	afx_msg void OnFileValidate();
	afx_msg void OnFilePref();
	afx_msg void OnFileQuit();
	afx_msg void OnFileSave();
	afx_msg void OnTools(UINT nID);
	afx_msg void OnUpdateToolsUI(CCmdUI *pCmdUI);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCalibrate();
	afx_msg void OnMeasure();
	afx_msg void OnValidate();
	afx_msg void OnChangeInL();
	afx_msg void OnChangeInA();
	afx_msg void OnChangeInB();
	afx_msg void OnKillfocusInL();
	afx_msg void OnKillfocusInA();
	afx_msg void OnKillfocusInB();
	afx_msg void OnSelchangeGamma();
	afx_msg void OnSelchangeWhite();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONACOVIEWDLG_H__6EECDFC2_EB46_11D0_9B8C_444553540000__INCLUDED_)
