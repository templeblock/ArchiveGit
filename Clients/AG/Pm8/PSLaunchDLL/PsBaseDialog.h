#if !defined(AFX_PSBASEDIALOG_H__EED06472_48CE_11D2_BD02_006008473DD0__INCLUDED_)
#define AFX_PSBASEDIALOG_H__EED06472_48CE_11D2_BD02_006008473DD0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PsBaseDialog.h : header file
//

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CPSBaseDialog dialog

#pragma pack( push, 1 )
typedef struct tagProjectMapEntry
{
	WORD	m_wType;
	WORD	m_wHeaderID;
	WORD	m_wStartDlgID;
	WORD	m_wFormatDlgID;
	WORD	m_wPaperMapID;

} SProjectMapEntry;
#pragma pack( pop )

class CPSBaseDialog : public CDialog
{
// Construction
public:
			CPSBaseDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

	UINT	GetTemplateID() { return m_nIDTemplate; }

// Dialog Data
	//{{AFX_DATA(CPSBaseDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSBaseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	UINT	GetBaseDialogID( UINT nDialogID );

	void	LoadButtonData();
	void	LoadListData();
	void	LoadPaperList();
	void	LoadProjectMap();

	// Generated message map functions
	//{{AFX_MSG(CPSBaseDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	typedef CMap<UINT, UINT, SProjectMapEntry, SProjectMapEntry&> CProjectMap;
	static CProjectMap m_cProjectMap;

	UINT m_nIDTemplate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSBASEDIALOG_H__EED06472_48CE_11D2_BD02_006008473DD0__INCLUDED_)
