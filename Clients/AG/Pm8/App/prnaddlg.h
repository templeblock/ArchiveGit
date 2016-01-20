#if !defined(AFX_ADDRPRNT_H__37F35B21_90B9_11D1_8541_444553540000__INCLUDED_)
#define AFX_ADDRPRNT_H__37F35B21_90B9_11D1_8541_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Addrprnt.h : header file

#include "pmwdoc.h"
#include "pmwprint.h"
#include "util.h"

// Special combo box for this dialog
class CJComboBox : public CComboFileNames
{
public:
   CJComboBox(void);

   CString
   GetFriendlyName (LPCSTR str);
protected:
   CString        mINIFileName;
};
/////////////////////////////////////////////////////////////////////////////
// CPrintAddressBookDlg dialog
class CPrintAddressBookDlg : public CPmwPrint
{
// Construction
public:
	CPrintAddressBookDlg(CPmwDoc* pDoc, UINT nRecs);
   // Build a full name for what was selected
   LPCSTR
   FullSelectedName (void)
   {
      mFullName = mPath;
      mFullName += mSelectedName;
      return mFullName;
   }
   // Returns TRUE if no print templates found
   BOOL
   DidYouNotFindAny (void)
   {
      return mAborted;
   }

// Dialog Data
	//{{AFX_DATA(CPrintAddressBookDlg)
	enum { IDD = IDD_PRINT_ADDRESS_BOOK };
	CEdit	      mToControl;
	CEdit	      mFromControl;
	CJComboBox	m_Format;
	int		   m_Copies;
	CString	   m_FormatValue;
	UINT	      mFrom;
	UINT	      mTo;
	int		   m_PrintAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintAddressBookDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrintAddressBookDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
   afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CString        mPath, mFullName, mSelectedName;
   BOOL           mAborted;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDRPRNT_H__37F35B21_90B9_11D1_8541_444553540000__INCLUDED_)
