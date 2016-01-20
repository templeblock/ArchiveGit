// MEASUREBOX.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMEASUREBOX dialog

class CMEASUREBOX : public CDialog
{
// Construction
public:
	CMEASUREBOX(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMEASUREBOX)
	enum { IDD = IDD_DIALOG_MEASURE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMEASUREBOX)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMEASUREBOX)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBegin();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  CBitmap *theImage;

};
