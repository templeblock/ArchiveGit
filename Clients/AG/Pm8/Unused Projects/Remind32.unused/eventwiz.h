/*
// $Workfile: eventwiz.h $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/eventwiz.h $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:04a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:50   Fred
// Initial revision.
// 
//    Rev 1.2   26 Feb 1997 13:52:48   Fred
// Fixed some problems with sender books
// 
//    Rev 1.1   25 Feb 1997 09:54:22   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:56:18   Fred
// Initial revision.
*/

class CEventWizardPage;
class CEventWizardAddressBook;
class CEventWizardType;
class CEventWizardName;
class CEventWizardDays;
class CEventWizard;

#ifndef __EVENTWIZ_H__
#define __EVENTWIZ_H__

#include "pmwwiz.h"
#include "addrbook.h"


/////////////////////////////////////////////////////////////////////////////
// CAddressBookNameArray
// Class for holding an array of name ID's

class CAddressBookIdArray : public CDWordArray
{
public:
	CAddressBookIdArray();
	virtual ~CAddressBookIdArray();
};


/////////////////////////////////////////////////////////////////////////////
// CAddressNameListBox
//

typedef BOOL (*GetNameCallback)(DWORD dwData, DWORD dwId, CString& csName);

class CAddressNameListBox : public CListBox
{
// Construction
public:
	CAddressNameListBox();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
// Attributes
public:

// Operations
public:
	BOOL FontMetrics(TEXTMETRIC& Metrics);
	void SetGetNameCallback(GetNameCallback pCallback, DWORD dwData);

// Implementation
public:
	virtual ~CAddressNameListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressNameListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	virtual void DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct);

protected:
	GetNameCallback m_pCallback;
	DWORD m_dwCallbackData;
};


/////////////////////////////////////////////////////////////////////////////
// CEventWizardError
// Exception class for reporting errors.

class CEventWizardError : public CException
{
	DECLARE_DYNAMIC(CEventWizardError);
public:
	CEventWizardError(DWORD dwError)
		{ m_dwError = dwError; }
	DWORD GetError(void)
		{ return m_dwError; }
protected:
	DWORD m_dwError;

public:
	static void Throw(DWORD dwError);
};


/////////////////////////////////////////////////////////////////////////////
// CEventWizardPage

class CEventWizardPage : public CPmwWizardPage
{
	INHERIT(CEventWizardPage, CPmwWizardPage)

// Construction
public:
	CEventWizardPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CEventWizardPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookImportWizardPage)
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEventWizardPage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAddressBookImportWizardPage)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual void UpdateButtons(void);
	virtual void UpdateInterface(void);
	
	// Inherited access functions (defined as inline at end of this file.)
	CEventWizard* GetWizard(void) const;
	void ReportError(CException* pException) const;
	CFlatFileDatabaseCursor* GetIdCursor(void) const;
	CFlatFileDatabaseCursor* GetNameCursor(void) const;
	BOOL GetNameFromId(DWORD dwId, CString& csName) const;
	BOOL GetNameFromCursor(CFlatFileDatabaseCursor* pCursor, CString& csName) const;
	BOOL GetIsStandardFormat(void) const;
	BOOL GetHasNameFields(void) const;
	DWORD GetIdField(void) const;
	DWORD GetFirstNameField(void) const;
	DWORD GetLastNameField(void) const;
	DWORD GetFullNameField(void) const;
	void SetAddressBookUserName(LPCSTR pszAddressBookUserName, BOOL fIsSenderDatabase) const;
	const CString& GetAddressBookUserName(void) const;
	BOOL GetIsSenderDatabase(void) const;
	void SetEventTypeId(DWORD dwEventType) const;
	DWORD GetEventTypeId(void) const;
	void GetEventType(CString& csEventType) const;
	void SetNameId(DWORD dwId) const;
	DWORD GetNameId(void) const;
	void SetDays(int nDays) const;
	int GetDays(void) const;
};


/////////////////////////////////////////////////////////////////////////////
// CEventWizardAddressBook dialog

class CEventWizardAddressBook : public CEventWizardPage
{
	INHERIT(CEventWizardAddressBook, CEventWizardPage)
	
// Construction
public:
	CEventWizardAddressBook();
	virtual ~CEventWizardAddressBook();

// Dialog Data
	//{{AFX_DATA(CEventWizardAddressBook)
	enum { IDD = IDD_ADD_NEW_EVENT_ADDRESS_BOOK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventWizardAddressBook)
	public:
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEventWizardAddressBook)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	virtual void UpdateButtons(void);
	virtual void UpdateInterface(void);
	CAddressBookListBox m_AddressBookList;
	BOOL m_fUpdateInterface;
};

/////////////////////////////////////////////////////////////////////////////
// CEventWizardType dialog

class CEventWizardType : public CEventWizardPage
{
	INHERIT(CEventWizardType, CEventWizardPage)
	
// Construction
public:
	CEventWizardType();
	virtual ~CEventWizardType();

// Dialog Data
	//{{AFX_DATA(CEventWizardType)
	enum { IDD = IDD_ADD_NEW_EVENT_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventWizardType)
	public:
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEventWizardType)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	virtual void UpdateButtons(void);
	virtual void UpdateInterface(void);
	CListBox m_TypeList;
	CString m_csAddressBookUserName;
};

/////////////////////////////////////////////////////////////////////////////
// CEventWizardName dialog

class CEventWizardName : public CEventWizardPage
{
	INHERIT(CEventWizardName, CEventWizardPage)
	
// Construction
public:
	CEventWizardName();
	virtual ~CEventWizardName();

// Dialog Data
	//{{AFX_DATA(CEventWizardName)
	enum { IDD = IDD_ADD_NEW_EVENT_NAME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventWizardName)
	public:
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEventWizardName)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	static DoGetNameFromId(DWORD dwData, DWORD dwId, CString& csName);
	virtual void UpdateButtons(void);
	virtual void UpdateInterface(void);
	CAddressNameListBox m_NameList;
	CString m_csAddressBookUserName;
};

/////////////////////////////////////////////////////////////////////////////
// CEventWizardDays dialog

class CEventWizardDays : public CEventWizardPage
{
	INHERIT(CEventWizardDays, CEventWizardPage)
	
// Construction
public:
	CEventWizardDays();
	virtual ~CEventWizardDays();

// Dialog Data
	//{{AFX_DATA(CEventWizardDays)
	enum { IDD = IDD_ADD_NEW_EVENT_DAYS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventWizardDays)
	public:
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEventWizardDays)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeDays();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	virtual void UpdateButtons(void);
	virtual void UpdateInterface(void);
};

/////////////////////////////////////////////////////////////////////////////
// CEventWizardCongratulations dialog

class CEventWizardCongratulations : public CEventWizardPage
{
	INHERIT(CEventWizardCongratulations, CEventWizardPage)
	
// Construction
public:
	CEventWizardCongratulations();
	virtual ~CEventWizardCongratulations();

// Dialog Data
	//{{AFX_DATA(CEventWizardCongratulations)
	enum { IDD = IDD_ADD_NEW_EVENT_CONGRATULATIONS };
	CStatic m_cStaticText;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventWizardCongratulations)
	public:
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEventWizardCongratulations)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CFont* m_pFont;
protected:
	virtual void UpdateButtons(void);
	virtual void UpdateInterface(void);
};

/////////////////////////////////////////////////////////////////////////////
// CEventWizard - This is the main wizard (dialog).

class CEventWizard : public CPmwWizard
{
	INHERIT(CEventWizard, CPmwWizard)

public:
	// Error numbers.
	enum
	{
		ERROR_CantAccessAddressBook	=	1,
		ERROR_NoFieldNames				=	2
	};
	
// Construction
public:
	CEventWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CEventWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	virtual void InitDialog(void);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEventWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEventWizard)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CommonConstruct(void);
public:
	static CEvent* Run(CWnd* pParent);
	static CEvent* Run(CWnd* pParent, CEvent* pOldEvent);

public:
	virtual CBitmap* GetBitmap(void)
		{ return &m_Bitmap; }

	CEventWizardAddressBook m_AddressBookPage;
	CEventWizardType m_TypePage;
	CEventWizardName m_NamePage;
	CEventWizardDays m_DaysPage;
	CEventWizardCongratulations m_CongratulationsPage;
	
public:
	void ReportError(CException* pException) const;
	
	CAddressBook* GetAddressBook(void)
		{ return AddressBook; }		// use the application's address book object
	CFlatFileDatabaseCursor* GetIdCursor(void)
		{ return GetAddressBook()->GetIdCursor(); }
	CFlatFileDatabaseCursor* GetNameCursor(void)
		{ return GetAddressBook()->GetNameCursor(); }
	BOOL GetNameFromId(DWORD dwId, CString& csName)
		{ return GetAddressBook()->GetNameFromId(dwId, csName); }
	BOOL GetNameFromCursor(CFlatFileDatabaseCursor* pCursor, CString& csName)
		{ return GetAddressBook()->GetNameFromCursor(pCursor, csName); }
	BOOL GetIsStandardFormat(void)
		{ return GetAddressBook()->GetIsStandardFormat(); }
	BOOL GetHasNameFields(void)
		{ return GetAddressBook()->GetHasNameFields(); }
	DWORD GetIdField(void)
		{ return GetAddressBook()->GetIdField(); }
	DWORD GetFirstNameField(void)
		{ return GetAddressBook()->GetFirstNameField(); }
	DWORD GetLastNameField(void)
		{ return GetAddressBook()->GetLastNameField(); }
	DWORD GetFullNameField(void)
		{ return GetAddressBook()->GetFullNameField(); }
		
	void SetAddressBookUserName(LPCSTR pszAddressBookUserName, BOOL fIsSenderDatabase);
	const CString& GetAddressBookUserName(void) const
		{ return m_csAddressBookUserName; }
	BOOL GetIsSenderDatabase(void) const
		{ return m_fIsSenderDatabase; }
		
	void SetEventTypeId(DWORD dwEventType);
	DWORD GetEventTypeId(void) const
		{ return m_dwEventTypeId; }
	void GetEventType(CString& csEventType);
	
	void SetNameId(DWORD dwId);
	DWORD GetNameId(void) const
		{ return m_dwNameId; }
		
	void SetDays(int nDays)
		{ m_nDays = nDays; }
	int GetDays(void) const
		{ return m_nDays; }
		
protected:
	CString m_csAddressBookUserName;
	BOOL m_fIsSenderDatabase;
	DWORD m_dwEventTypeId;
	DWORD m_dwNameId;
	int m_nDays;
	
protected:
	CBitmap m_Bitmap;
};

// Inline functions.

inline CEventWizard* CEventWizardPage::GetWizard(void) const
	{ return (CEventWizard*)GetParent(); }
inline void CEventWizardPage::ReportError(CException* pException) const
	{ GetWizard()->ReportError(pException); }
inline CFlatFileDatabaseCursor* CEventWizardPage::GetIdCursor(void) const
	{ return GetWizard()->GetIdCursor(); }
inline CFlatFileDatabaseCursor* CEventWizardPage::GetNameCursor(void) const
	{ return GetWizard()->GetNameCursor(); }
inline BOOL CEventWizardPage::GetNameFromId(DWORD dwId, CString& csName) const
	{ return GetWizard()->GetNameFromId(dwId, csName); }
inline BOOL CEventWizardPage::GetNameFromCursor(CFlatFileDatabaseCursor* pCursor, CString& csName) const
	{ return GetWizard()->GetNameFromCursor(pCursor, csName); }
inline BOOL CEventWizardPage::GetIsStandardFormat(void) const
	{ return GetWizard()->GetIsStandardFormat(); }
inline BOOL CEventWizardPage::GetHasNameFields(void) const
	{ return GetWizard()->GetHasNameFields(); }
inline DWORD CEventWizardPage::GetIdField(void) const
	{ return GetWizard()->GetIdField(); }
inline DWORD CEventWizardPage::GetFirstNameField(void) const
	{ return GetWizard()->GetFirstNameField(); }
inline DWORD CEventWizardPage::GetLastNameField(void) const
	{ return GetWizard()->GetLastNameField(); }
inline DWORD CEventWizardPage::GetFullNameField(void) const
	{ return GetWizard()->GetFullNameField(); }
inline void CEventWizardPage::SetAddressBookUserName(LPCSTR pszAddressBookUserName, BOOL fIsSenderDatabase) const
	{ GetWizard()->SetAddressBookUserName(pszAddressBookUserName, fIsSenderDatabase); }
inline const CString& CEventWizardPage::GetAddressBookUserName(void) const
	{ return GetWizard()->GetAddressBookUserName(); }
inline BOOL CEventWizardPage::GetIsSenderDatabase(void) const
	{ return GetWizard()->GetIsSenderDatabase(); }
inline void CEventWizardPage::SetEventTypeId(DWORD dwEventType) const
	{ GetWizard()->SetEventTypeId(dwEventType); }
inline DWORD CEventWizardPage::GetEventTypeId(void) const
	{ return GetWizard()->GetEventTypeId(); }
inline void CEventWizardPage::GetEventType(CString& csEventType) const
	{ GetWizard()->GetEventType(csEventType); }
inline void CEventWizardPage::SetNameId(DWORD dwId) const
	{ GetWizard()->SetNameId(dwId); }
inline DWORD CEventWizardPage::GetNameId(void) const
	{ return GetWizard()->GetNameId(); }
inline void CEventWizardPage::SetDays(int nDays) const
	{ GetWizard()->SetDays(nDays); }
inline int CEventWizardPage::GetDays(void) const
	{ return GetWizard()->GetDays(); }
	
#endif
