#if !defined(AFX_TuningDlg_H__26149F99_19E4_11D3_91E8_00A02459C447__INCLUDED_)
#define AFX_TuningDlg_H__26149F99_19E4_11D3_91E8_00A02459C447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TuningDlg.h : header file
//

#include "ColorControl.h"
#include "Qd3dGamutTweak.h"
#include "Qd3dControl.h"

enum kTOOL { kNone, kMove, kSize, kColorDesired, kColorGamut, kColorPrint };

#define MAX_NUM_TWEAKS 40 //j

/////////////////////////////////////////////////////////////////////////////
// CTuningDlg dialog

class CProfileDoc;

class CTuningDlg : public CDialog
{
// Construction
public:
	CTuningDlg(CWnd* pParent = NULL);   // standard constructor
	~CTuningDlg();

// Dialog Data
	//{{AFX_DATA(CTuningDlg)
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
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTuningDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_inited;
	BOOL m_ingamut;
	McoHandle m_gamut;

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
		
public:
	BOOL IsDlgOkToShow(void);
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

	// Generated message map functions
	//{{AFX_MSG(CTuningDlg)
	//}}AFX_MSG
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

#endif // !defined(AFX_TuningDlg_H__26149F99_19E4_11D3_91E8_00A02459C447__INCLUDED_)
