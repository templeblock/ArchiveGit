// DialogMain.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogMain dialog
#include "MPlusSetUp.h"


#include "led.h"



class CDialogMain : public CDialog
{
// Construction
public:
	CDialogMain(CWnd* pParent = NULL);   // standard constructor
	CMPlusSetUp SetUpThisMeter;
	CMEASUREBOX MeasureThisMonitor;
// Dialog Data
	//{{AFX_DATA(CDialogMain)
	enum { IDD = IDD_DIALOG_MAIN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	  

	// Generated message map functions
	//{{AFX_MSG(CDialogMain)
	afx_msg void OnButtonSetup();
	afx_msg void OnButtonSavevalue();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonMeasure();
   	afx_msg void DisplayMeasurement(dYxy * meas, LEDdisplay *LEDptr, SetUpProfile *profile);
	virtual void OnOK();
	afx_msg void OnRadio1();
	afx_msg void OnDisplayct();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
