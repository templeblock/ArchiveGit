/*
// $Workfile: ADDRIMP.H $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDRIMP.H $
// 
// 1     3/03/99 6:01p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:38   Fred
// Initial revision.
// 
//    Rev 1.11   07 Sep 1996 17:33:34   Fred
// New import code
// 
//    Rev 1.10   01 Sep 1996 17:28:16   Fred
// Changed some dialogs from CPmwDialog to CDialog becuase of missing sounds
// 
//    Rev 1.9   28 Aug 1996 21:26:22   Fred
// CAddressBookImportError::Throw() is no longer inline
// 
//    Rev 1.8   28 Aug 1996 15:22:44   Fred
// Enforces maximum number of names
// 
//    Rev 1.7   05 Aug 1996 18:55:30   Fred
//  
// 
//    Rev 1.6   01 Aug 1996 09:22:32   Fred
// More 16-bit changes
// 
//    Rev 1.5   31 Jul 1996 11:56:26   Fred
// 16-bit compatibility
// 
//    Rev 1.4   16 Jul 1996 08:51:20   Fred
// Hooked up address/sender book
// 
//    Rev 1.3   11 Jul 1996 08:37:32   Fred
// More address book features
// 
//    Rev 1.2   09 Jul 1996 16:21:22   Fred
// Import names feature
// 
//    Rev 1.1   08 Jul 1996 07:14:10   Fred
// Import Names Wizard
// 
//    Rev 1.0   05 Jul 1996 09:47:00   Fred
// Initial revision.
*/

class CAddressBookImportReader;
class CAddressBookImportWizard;
class CAddressBookImportWizardPage;
class CAddressBookImportIntroduction;
class CAddressBookImportChooseFile;
class CAddressBookImportChooseBook;
class CAddressBookImportImporting;
class CImportingNamesProgressDialog;
class CImportingNamesCongratulationsDialog;

#ifndef __ADDRIMP_H__
#define __ADDRIMP_H__

#include "progress.h"
#include "pmwwiz.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportError
// Exception class for reporting errors.

class CAddressBookImportError : public CException
{
	DECLARE_DYNAMIC(CAddressBookImportError);
public:
	CAddressBookImportError(DWORD dwError)
		{ m_dwError = dwError; }
	DWORD GetError(void)
		{ return m_dwError; }
protected:
	DWORD m_dwError;

public:
	static void Throw(DWORD dwError);
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportReader
// Class for reading a file of names to be imported.

class CAddressBookImportReader
{
public:
	// Error numbers.
	enum
	{
		ERROR_UserAbort					=	1,
		ERROR_FileNameEmpty				=	2,
		ERROR_CantAccessFile				=	3,
		ERROR_UnknownFormat				=	4,
		ERROR_BlankFieldNames			=	5,
		ERROR_FieldNumberMismatch		=	6,
		ERROR_AddressBookNameEmpty		=	7,
		ERROR_CantAccessAddressBook	=	8,
		ERROR_TooManyImportNames		=	9
	};

	// Small class for collecting and managing field statistics.
	class CFieldStatistics
	{
	public:
		CFieldStatistics();
		virtual ~CFieldStatistics();

	public:
		DWORD GetMaximumLength(void)
			{ return m_dwMaximumLength; }
		void AdjustMaximumLength(DWORD dwLength);
		DWORD GetMinimumLength(void)
			{ return m_dwMinimumLength; }
		void AdjustMinimumLength(DWORD dwLength);
		DWORD GetEmptyCount(void)
			{ return m_dwEmptyCount; }
		void AdjustEmptyCount(DWORD dwLength);

	protected:
		DWORD m_dwMaximumLength;
		DWORD m_dwMinimumLength;
		DWORD m_dwEmptyCount;
	};

public:
	CAddressBookImportReader();
	virtual ~CAddressBookImportReader();

public:
	void Init(LPCSTR pszFileName);
	void SetAddressBookName(LPCSTR pszAddressBookName);
	BOOL ImportNames(CWnd* pParent);

	LPCSTR GetFileName(void)
		{ return m_csFileName; }
	LPCSTR GetAddressBookName(void)
		{ return m_csAddressBookName; }
	LPCSTR GetAddressBookBaseName(void)
		{ return m_csAddressBookBaseName; }

	DWORD GetFieldCount(void)
		{ return m_FieldNames.GetSize(); }
	CFieldStatistics* GetFieldStatistics(DWORD dwFieldNumber);
	LPCSTR GetFieldName(DWORD dwFieldNumber);
	DWORD GetImportFieldLength(DWORD dwFieldNumber);
	DWORD GetImportFieldIndexType(DWORD dwFieldNumber);

	DWORD GetRecordCount()
		{ return m_dwRecordCount; }
	DWORD GetValueCount(void)
		{ return m_FieldValues.GetSize(); }

protected:
	void CollectStatistics(void);
	void ChooseNameFields(void);
	DWORD FindNameField(LPCSTR pszName, ...);
	void ReadFieldNames(void);
	BOOL ReadFieldValues(void);
	void ReadElement(int nSeparator, int& nSeparatorFound);
	BOOL ReadLine(CStringArray& StringArray);

protected:
	CString m_csFileName;
	CString m_csAddressBookName;
	CString m_csAddressBookBaseName;
	FILE* m_pFile;
	int m_nSeparator;
	CStringArray m_FieldNames;
	CStringArray m_FieldValues;
	DWORD m_dwRecordCount;
	CFieldStatistics* m_pStatistics;
	DWORD m_dwValueOffset;
	CString m_csFirstNameField;
	CString m_csLastNameField;
	CString m_csFullNameField;

	// Element buffer.
	enum
	{
		MAXIMUM_ELEMENT_LENGTH	=	4096
	};

	char m_szElement[MAXIMUM_ELEMENT_LENGTH+1];
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportWizard

class CAddressBookImportWizard : public CPmwWizard
{
	INHERIT(CAddressBookImportWizard, CPmwWizard)
//	DECLARE_DYNAMIC(CAddressBookImportWizard)

// Construction
public:
	CAddressBookImportWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAddressBookImportWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	virtual void InitDialog(void);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookImportWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAddressBookImportWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAddressBookImportWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CommonConstruct(void);

public:
	static BOOL Run(CWnd* pWnd, CString& csAddressBookName);

public:
	CAddressBookImportReader* GetReader(void)
		{ return m_pReader; }
	void CreateReader(LPCSTR pszFileName);
	void ReportReaderError(CException* pException);

public:
	virtual CBitmap* GetBitmap(void)
		{ return &m_Bitmap; }

protected:
	CAddressBookImportReader* m_pReader;
	
protected:
	CBitmap m_Bitmap;
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportWizardPage

class CAddressBookImportWizardPage : public CPmwWizardPage
{
	INHERIT(CAddressBookImportWizardPage, CPmwWizardPage)
//	DECLARE_DYNCREATE(CAddressBookImportWizardPage)

// Construction
public:
	CAddressBookImportWizardPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CAddressBookImportWizardPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
//	CAddressBookImportWizardPage();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookImportWizardPage)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAddressBookImportWizardPage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAddressBookImportWizardPage)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// Inherited access functions.
	CAddressBookImportWizard* GetWizard(void)
		{ return (CAddressBookImportWizard*)GetParent(); }
	CAddressBookImportReader* GetReader(void)
		{ return GetWizard()->GetReader(); }
	void ReportReaderError(CException* pException)
		{ GetWizard()->ReportReaderError(pException); }
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportIntroduction dialog

class CAddressBookImportIntroduction : public CAddressBookImportWizardPage
{
	INHERIT(CAddressBookImportIntroduction, CAddressBookImportWizardPage)
//	DECLARE_DYNCREATE(CAddressBookImportIntroduction)

// Construction
public:
	CAddressBookImportIntroduction();
	~CAddressBookImportIntroduction();

// Dialog Data
	//{{AFX_DATA(CAddressBookImportIntroduction)
	enum { IDD = IDD_ADDRESS_BOOK_IMPORT_INTRODUCTION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookImportIntroduction)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookImportIntroduction)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportChooseFile dialog

class CAddressBookImportChooseFile : public CAddressBookImportWizardPage
{
	INHERIT(CAddressBookImportChooseFile, CAddressBookImportWizardPage)
//	DECLARE_DYNCREATE(CAddressBookImportChooseFile)

// Construction
public:
	CAddressBookImportChooseFile();
	~CAddressBookImportChooseFile();

// Dialog Data
	//{{AFX_DATA(CAddressBookImportChooseFile)
	enum { IDD = IDD_ADDRESS_BOOK_IMPORT_CHOOSE_FILE };
	CEdit	m_FileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookImportChooseFile)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookImportChooseFile)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportChooseBook dialog

class CAddressBookImportChooseBook : public CAddressBookImportWizardPage
{
	INHERIT(CAddressBookImportChooseBook, CAddressBookImportWizardPage)
//	DECLARE_DYNCREATE(CAddressBookImportChooseBook)

// Construction
public:
	CAddressBookImportChooseBook();
	~CAddressBookImportChooseBook();

// Dialog Data
	//{{AFX_DATA(CAddressBookImportChooseBook)
	enum { IDD = IDD_ADDRESS_BOOK_IMPORT_CHOOSE_BOOK };
	CEdit	m_AddressBookName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookImportChooseBook)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookImportChooseBook)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookImportImporting dialog

class CAddressBookImportImporting : public CAddressBookImportWizardPage
{
	INHERIT(CAddressBookImportImporting, CAddressBookImportWizardPage)
//	DECLARE_DYNCREATE(CAddressBookImportImporting)

// Construction
public:
	CAddressBookImportImporting();
	~CAddressBookImportImporting();

// Dialog Data
	//{{AFX_DATA(CAddressBookImportImporting)
	enum { IDD = IDD_ADDRESS_BOOK_IMPORT_IMPORTING };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookImportImporting)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookImportImporting)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnWizardFinish();
protected:
	CString m_csImportingTitleFormat;
};

/////////////////////////////////////////////////////////////////////////////
// CImportingNamesProgressDialog dialog

class CImportingNamesProgressDialog : public CDialog
{
	INHERIT(CImportingNamesProgressDialog, CDialog)

// Construction
public:
	CImportingNamesProgressDialog(CAddressBookImportReader* pReader, CWnd* pParent = NULL);
	virtual ~CImportingNamesProgressDialog();

// Dialog Data
	//{{AFX_DATA(CImportingNamesProgressDialog)
	enum { IDD = IDD_IMPORTING_NAMES_PROGRESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportingNamesProgressDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportingNamesProgressDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetPosition(DWORD dwPosition);
	BOOL CheckForAbort(void);

protected:
	CMlsProgressBar m_ProgressBar;
	CAddressBookImportReader* m_pReader;
	BOOL m_fAbort;
};

/////////////////////////////////////////////////////////////////////////////
// CImportingNamesCongratulationsDialog dialog

class CImportingNamesCongratulationsDialog : public CDialog
{
	INHERIT(CImportingNamesCongratulationsDialog, CDialog)

// Construction
public:
	CImportingNamesCongratulationsDialog(CAddressBookImportReader* pReader, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CImportingNamesCongratulationsDialog)
	enum { IDD = IDD_IMPORTING_NAMES_SUCCESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportingNamesCongratulationsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportingNamesCongratulationsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CAddressBookImportReader* m_pReader;
};

/////////////////////////////////////////////////////////////////////////////

#endif
