// objpos.h : header file
//

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectPositionDlg dialog

class CObjectPositionDlg : public CPmwDialog
{
// Construction
public:
	CObjectPositionDlg(CPmwDoc *pDoc, RectPageObjectPtr object, CWnd* pParent = NULL);	// standard constructor

	PBOX& get_chosen_bound(VOID)
	{
		return m_bound;
	}

// Dialog Data
	//{{AFX_DATA(CObjectPositionDlg)
	enum { IDD = IDD_TYPE_IN_POSITION };
	CButton	m_preview;
	CString	m_height;
	CString	m_left;
	CString	m_top;
	CString	m_width;
	//}}AFX_DATA

	CPmwDoc *m_pDoc;
	PBOX m_world;
	RectPageObjectPtr m_object;
	PBOX m_bound;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnOK();

	VOID init_preview(VOID);
	VOID preview_position(LPDRAWITEMSTRUCT lpDrawItemStruct);
	VOID init_sizepos(VOID);
	VOID limit_control(UINT id, SHORT digits);

	// Generated message map functions
	//{{AFX_MSG(CObjectPositionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnChangeAnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
