// CompDegree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompDegree dialog
#ifndef _COMPDEGREE_H_
#define _COMPDEGREE_H_

class CCompDegree : public CDialog
{
// Construction
public:
	CCompDegree(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CCompDegree)
	enum { IDD = DIALOG_EDIT_COMPENSATION_DEGREE };
	float	m_flCompDegree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompDegree)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompDegree)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif//_COMPDEGREE_H_