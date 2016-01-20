// MPlusSetUp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMPlusSetUp dialog
#include "MEASUREBOX.H"

class CMPlusSetUp : public CDialog
{
// Construction
public:
	CMPlusSetUp(CWnd* pParent = NULL);   // standard constructor
    CMEASUREBOX MeasureThisMonitor;
// Dialog Data
	//{{AFX_DATA(CMPlusSetUp)
	enum { IDD = IDD_DIALOG_SETUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMPlusSetUp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMPlusSetUp)
	afx_msg void OnRadioFast();
	afx_msg void OnRadioMedium();
	afx_msg void OnRadioSlow();
	afx_msg void OnRadioLuv();
	afx_msg void OnRadioYxy();
	afx_msg void OnRadioRgbMatch();
	afx_msg void OnRadioFtlamberts();
	afx_msg void OnRadioCandelas();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonMeasFreq();
	virtual void OnOK();
	afx_msg void OnSavereference();
	afx_msg void OnClickSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheck1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
