// slpicdlg.h : header file
//

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectPictureDlg dialog

class CSelectPictureDlg : public CPmwDialog
{
protected:
	VOID build_object_list(VOID);
	VOID update_remove_button(BOOL something_selected);

// Construction
public:
	CSelectPictureDlg(CPmwDoc* pDoc,
 						CWordArray* pcwaTypes,
 						SHORT id = CSelectPictureDlg::IDD,
 						CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectPictureDlg)
	enum { IDD = IDD_SELECT_PICTURES };
	CListBox	m_picture_list;
	int		m_list_index;
	//}}AFX_DATA

	CPmwDoc *m_pDoc;
	CWordArray* m_pcwaTypes;
// Implementation

/* Set the selection state of the objects from the list. */
	void objects_from_list(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL IsShowableObject(PMGPageObject* pObject);

	// Generated message map functions
	//{{AFX_MSG(CSelectPictureDlg)
	afx_msg void OnPictureAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkPictureList();
	afx_msg void OnSelchangePictureList();
	afx_msg void OnClickedRemove();
	virtual void OnCancel();
	afx_msg void OnPictureAddOther();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
