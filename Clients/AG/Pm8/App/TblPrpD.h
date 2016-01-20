/*
// $Workfile: TblPrpD.h $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TblPrpD.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 5     6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 4     6/02/98 3:00p Jayn
// 
// 3     4/14/98 4:41p Jayn
// Changes for the new table object.
// 
//    Rev 1.0   14 Aug 1997 15:21:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:04   Fred
// Initial revision.
// 
//    Rev 1.0   26 Mar 1997 15:17:42   Fred
// Initial revision.
*/

#ifndef __TBLPRPD_H__
#define __TBLPRPD_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTableObject;
class CTableFormatControl;

/////////////////////////////////////////////////////////////////////////////
// CTablePropertiesDlg dialog

class CTablePropertiesDlg : public CDialog
{
// Construction
public:
	CTablePropertiesDlg(CTableObject* pObject, CWnd* pParent = NULL, UINT uID = IDD);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTablePropertiesDlg)
	enum { IDD = IDD_AUTO_FORMAT };
	CListBox m_FormatList;
	//}}AFX_DATA

	CTableFormatControl* m_pTableControl;
	UINT m_uLastCellData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTablePropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CTableObject* m_pTable;

	// Build the format list.
	void BuildFormatList(void);
	void UpdateFormatPreview(void);
	void SetTableDefaults(CTableObject* pTable);

	// Generated message map functions
	//{{AFX_MSG(CTablePropertiesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeFormats();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTableCreateDialog dialog

class CTableCreateDialog : public CTablePropertiesDlg
{
private:
// Construction
public:
	CTableCreateDialog(CTableObject* pTable, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTableCreateDialog)
	enum { IDD = IDD_TABLE_CREATE_DLG };
	CSpinButtonCtrl	m_ctlColumns;
	CSpinButtonCtrl	m_ctlRows;
	int		m_nColumns;
	int		m_nRows;
	//}}AFX_DATA

	CTableFormatControl* m_pTableControl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableCreateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableCreateDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
