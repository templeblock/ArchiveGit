/*
// $Workfile: DBCONVRT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DBCONVRT.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:02   Fred
// Initial revision.
// 
//    Rev 1.1   18 Jul 1996 14:53:06   FRED
//  
// 
//    Rev 1.0   17 Jul 1996 11:21:46   Fred
// Initial revision.
*/

#include "stdafx.h"  
#include "util.h"
#include "addrdata.h"
#include "cbdedb.h"
#include "dbconvrt.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
// The system heap.
*/

HeapPtr system_heap = NULL;

static void CheckProcessorType();
BOOL g_fProcessorIs386Compatible = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CTestApp

BEGIN_MESSAGE_MAP(CDbconvertApp, CWinApp)
	//{{AFX_MSG_MAP(CDbconvertApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDbconvertApp construction

CDbconvertApp::CDbconvertApp()
{
	system_heap = new StandardHeap;
	m_pDialog = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDbconvertApp object

CDbconvertApp near theApp;

/////////////////////////////////////////////////////////////////////////////
// CDbconvertApp initialization

BOOL CDbconvertApp::InitInstance()
{
	::SetHandleCount(100);
	
	// Get the directory names.
	if (!GetDirectoryNames())
	{
		return FALSE;
	}

	if (system_heap == NULL)
	{
	/* We really need a heap... */
		return FALSE;
	}

	CheckProcessorType();

	Init3DControls();

	// Attempt to initialize IDAPI.
	if (OpenBDE())
	{
		TRY
		{
			m_pDialog = new CDbconvertDlg();
			m_pMainWnd = m_pDialog;
		}
		END_TRY
		
		ConvertOldAddressBooks(); 
		
		delete m_pDialog;
		m_pDialog = NULL;
	}
	
	CloseBDE();

	UINT msgConvertFinished = RegisterWindowMessage("PmwConvertOldAddressBooksFinished");
	if (msgConvertFinished != 0)
	{
		// Let any who's interested know that we're finished.
		PostMessage(HWND_BROADCAST, msgConvertFinished, 0, 0);
	}
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/*
// Initialize the 3D controls.
*/

void CDbconvertApp::Init3DControls(void)
{
	SetDialogBkColor();        // set dialog background color to gray

	TRY
	{
		m_ctl3d.Startup(AfxGetInstanceHandle(), NULL);
	}
	END_TRY
}

static void CheckProcessorType()
{
#ifdef _WIN32
	g_fProcessorIs386Compatible = TRUE;
#else
	DWORD dwWinFlags;

	dwWinFlags = GetWinFlags();

	g_fProcessorIs386Compatible =
		((dwWinFlags & (WF_CPU386 | WF_CPU486 | WF_ENHANCED)) != 0);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CDbconvertDlg dialog

CDbconvertDlg::CDbconvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbconvertDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDbconvertDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Create(CDbconvertDlg::IDD, NULL);
	m_fAbort = FALSE;
}                                   

CDbconvertDlg::~CDbconvertDlg()
{
	if (m_hWnd != NULL)
	{
		DestroyWindow();
	}
}
 
void CDbconvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbconvertDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDbconvertDlg, CDialog)
	//{{AFX_MSG_MAP(CDbconvertDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDbconvertDlg::CheckForAbort()
{
	if (!m_fAbort)
	{
		MSG Msg;
		while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!AfxGetApp()->PumpMessage())
			{
				::PostQuitMessage(0);
				m_fAbort = TRUE;
				break;
			}
		}
	}

	return m_fAbort;
}
	
void CDbconvertDlg::SetName(LPCSTR pszName)
{
	CWnd* pName = GetDlgItem(IDC_NAME);
	if (pName != NULL)
	{
		pName->SetWindowText(pszName);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDbconvertDlg message handlers

BOOL CDbconvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	Util::CenterWindowInWindow(this);

	m_ProgressBar.Initialize(IDC_PROGRESS, this);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDbconvertDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDbconvertDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDbconvertDlg::OnCancel()
{
	if (!m_fAbort)
	{
		if (AfxMessageBox(IDS_CONFIRM_ABORT, MB_YESNO) == IDYES)
		{
			m_fAbort = TRUE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// BDE support.

static CBdeEngine MyBdeEngine;

/*
// Open the BDE system.
*/

BOOL CDbconvertApp::OpenBDE(void)
{                                 
	// No error messages!
	CBdeException::m_fQuiet = TRUE;
	
	if (!MyBdeEngine.IsOpen())
	{
		TRY
		{
			MyBdeEngine.Open();
		}
		CATCH_ALL(e)
		{
			return FALSE;
		}
		END_CATCH_ALL
	}
	return TRUE;
}

/*
// Close the BDE system.
*/

void CDbconvertApp::CloseBDE(void)
{
	TRY
	{
		MyBdeEngine.Close();
	}
	END_TRY
}

// Open (and create) an address book sender database.
// No exceptions.
ERRORCODE CDbconvertApp::OpenAddressBookDatabase(
	LPCSTR pszFullName,
	StorageFile*& pDatabaseFile,
	CFlatFileDatabase*& pDatabase,
	BOOL fCreate /*=FALSE*/,
	LPCSTR pszDescription /*=NULL*/,
	CFlatFileFieldDescription* pFieldDescriptions /*=NULL*/,
	CFlatFileIndexDescription* pIndexDescriptions /*=NULL*/)
{
	ERRORCODE Error = ERRORCODE_None;

	// Clean up any existing data.
	CloseAddressBookDatabase(pDatabaseFile, pDatabase);

	CString csFullName;
	CString csPath;
	TRY
	{
		// Make sure the directory exists.
		csFullName = pszFullName;
		Util::SplitPath(csFullName, &csPath, NULL);
		Util::MakeDirectory(csPath);
		
		// Create the file object.
		pDatabaseFile = new StorageFile(pszFullName);

		// Create the database object.
		pDatabase = new CFlatFileDatabase(pDatabaseFile);

		// Try to open the database exclusively and perform validation.
		pDatabaseFile->set_open_flags(CFile::modeReadWrite | CFile::shareExclusive);
		Error = pDatabase->read_state(TRUE, TRUE);
		pDatabaseFile->flush();

		// If the file does not exist, attempt to intialize the database.
		if ((Error == ERRORCODE_DoesNotExist) && (fCreate))
		{
			// Create the file.
			if ((Error = pDatabaseFile->initialize()) == ERRORCODE_None)
			{
				// Assign defaults.
				if (pszDescription == NULL)
				{
					pszDescription = szStandardDatabaseDescription;
				}
				if (pFieldDescriptions == NULL)
				{
					pFieldDescriptions = StandardDatabaseFieldDescriptions;
				}
				if (pIndexDescriptions == NULL)
				{
					pIndexDescriptions = StandardDatabaseIndexDescriptions;
				}

				// Make the creation data.
				CFlatFileHeaderRecord::CCreationData CreationData(
					(LPVOID)(LPSTR)pszDescription,
					strlen(pszDescription)+1,
					pFieldDescriptions,
					pIndexDescriptions);

				// Initialize the database and then close it.
				if (((Error = pDatabase->initialize(0, &CreationData)) != ERRORCODE_None)
				 || ((Error = pDatabase->flush()) != ERRORCODE_None)
				 || ((Error = pDatabase->write_state()) != ERRORCODE_None)
				 || ((Error = pDatabaseFile->flush()) != ERRORCODE_None))
				{
					// Got the file, but database error. Delete the file and return the error.
					pDatabaseFile->zap();
				}
			}
		}

		if (Error == ERRORCODE_None)
		{
			// Try to open the database in sharing mode.
			pDatabaseFile->set_open_flags(CFile::modeReadWrite | CFile::shareDenyNone);
			Error = pDatabase->read_state();
		}
	}
	CATCH_ALL(e)
	{
		if (Error == ERRORCODE_None)
		{
			Error = ERRORCODE_IntError;
		}
	}
	END_CATCH_ALL

	if (Error != ERRORCODE_None)
	{
		CloseAddressBookDatabase(pDatabaseFile, pDatabase);
	}

	return Error;
}

// Open (and create) an address book sender database.
// No exceptions.
ERRORCODE CDbconvertApp::OpenAddressBookSenderDatabase(
		LPCSTR pszFullName,
		StorageFile*& pDatabaseFile,
		CFlatFileDatabase*& pDatabase,
		BOOL fCreate /*=FALSE*/)
{
	// Use OpenAddressBookDatabase() with the proper default field and index descriptions.
	return OpenAddressBookDatabase(
		pszFullName,
		pDatabaseFile,
		pDatabase,
		fCreate,
		szStandardSenderDatabaseDescription,
		StandardSenderDatabaseFieldDescriptions,
		StandardSenderDatabaseIndexDescriptions);
}

// Close an address book database.
// No exceptions.
void CDbconvertApp::CloseAddressBookDatabase(StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase)
{
	// Close and delete the database object.
	TRY
	{
		if (pDatabase != NULL)
		{
			pDatabase->flush();
			pDatabase->write_state();
			delete pDatabase;
		}
	}
	END_TRY

	// Close and delete the file object.
	TRY
	{
		delete pDatabaseFile;
	}
	END_TRY

	// Always NULL the pointers.
	pDatabase = NULL;
	pDatabaseFile = NULL;
}

/*
// Parse a parameter in a command line. A Parameter always begins
// at the start of the command line or after a space. No spaces
// are allowed within parameters, but quoted strings are handled
// properly and may contain spaces. All comparisons are case-insensitive.
*/

LPSTR CDbconvertApp::LocateParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
	LPSTR lpszResult = NULL;
	
	LPSTR lpszCommandLine = m_lpCmdLine;
	int nParamNameLength = lstrlen(lpszParamName);
	int nCommandLineLength = lstrlen(lpszCommandLine);
	BOOL fInQuote = FALSE;
	BOOL fStartOfParameter = TRUE;
	BOOL fFoundParameter = FALSE;
	
	while ((!fFoundParameter) && (nCommandLineLength >= nParamNameLength))
	{
		if (fStartOfParameter)
		{
			// A parameter can start here, check for a match.
			
			int i = 0;
			
			for (;;)
			{
				char ch1 = lpszCommandLine[i];
				if ((ch1 > 'a') && (ch1 < 'z')) ch1 = (char)(ch1+'A'-'a');
				
				char ch2 = lpszParamName[i];
				if ((ch2 > 'a') && (ch2 < 'z')) ch2 = (char)(ch2+'A'-'a');
				
				if (ch2 == 0)
				{
					fFoundParameter = TRUE;
					break;
				}
				
				if (ch1 != ch2)
				{
					fStartOfParameter = FALSE;
					break;
				}
				
				i++;
			}
		}
		
		else
		{
			if (*lpszCommandLine == ' ')
			{
				fStartOfParameter = !fInQuote;
			}
			
			else if (*lpszCommandLine == '"')
			{
				fInQuote = !fInQuote;
			}
			
			lpszCommandLine++;
			nCommandLineLength--;
		}
	}
	
	if (fFoundParameter)
	{
		lpszResult = lpszCommandLine;
		
		if (lpszValue != NULL)
		{
			*lpszValue = 0;
			
			lpszCommandLine += nParamNameLength;

			if (*lpszCommandLine == '=')
			{
				lpszCommandLine++;
			}
			
			fInQuote = FALSE;
			
			for (;;)
			{
				if (*lpszCommandLine == 0)
				{
					break;
				}
				
				if ((!fInQuote) && (*lpszCommandLine == ' '))
				{
					break;
				}
				
				if (*lpszCommandLine == '"')
				{
					fInQuote = !fInQuote;
				}
				
				else
				{
					*(lpszValue++) = *lpszCommandLine;
				}
				
				lpszCommandLine++;
			}
			
			*lpszValue = 0;
		}
	}
	
	return lpszResult;
}

BOOL CDbconvertApp::FindParameter(LPSTR lpszParamName)
{
	return LocateParameter(lpszParamName, NULL) != NULL;
}

LPSTR CDbconvertApp::GetParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
	if (LocateParameter(lpszParamName, lpszValue) == NULL)
	{
		return NULL;
	}
	
	else
	{
		return lpszValue;
	}
}

// The main function which does the conversion.
void CDbconvertApp::ConvertOldAddressBooks(void)
{
	CString csOldAddressBookPath;
	CString csOldSenderBookPath;
	CString csNewAddressBookPath;
	CString csNewSenderBookPath;
	CFileIterator FileIterator;
	CString csSearch;
	CString csOldName;
	CString csNewName;
	CString csDrive;
	CString csDirectory;
	CString csFile;
	TRY
	{
		// Compute the full address book and sender book path names for the old installation.
		csOldAddressBookPath = m_csOldDirectory;
		Util::AppendBackslashToPath(csOldAddressBookPath);
		csOldAddressBookPath += "BOOKS\\";
		csOldSenderBookPath = csOldAddressBookPath;
		csOldSenderBookPath += "SENDER\\";
		TRACE("OldAddressBookPath = %s\n", (LPCSTR)csOldAddressBookPath);
		TRACE("OldSenderBookPath = %s\n", (LPCSTR)csOldSenderBookPath);
		
		// Compute the full address book and sender book path names for the old installation.
		csNewAddressBookPath = m_csNewDirectory;
		Util::AppendBackslashToPath(csNewAddressBookPath);
		csNewAddressBookPath += "BOOKS\\";
		csNewSenderBookPath = csNewAddressBookPath;
		csNewSenderBookPath += "SENDER\\";
		TRACE("NewAddressBookPath = %s\n", (LPCSTR)csNewAddressBookPath);
		TRACE("NewSenderBookPath = %s\n", (LPCSTR)csNewSenderBookPath);
		
		// Convert the address books.
		csSearch = csOldAddressBookPath+"*.DB";
		BOOL fContinue = FileIterator.FindFirst(csSearch, _A_NORMAL);
		while (fContinue)
		{
			if (CheckForAbort())
			{
				break;
			}
			
			// Compute the old and new names.
			csOldName = csOldAddressBookPath+FileIterator.Name();
			Util::SplitPath(csOldName, &csDrive, &csDirectory, &csFile, NULL);
			csNewName = csNewAddressBookPath;
			csNewName += csFile;
			csNewName += ADDRESS_BOOK_EXTENSION;
			ConvertAddressBook(csOldName, csNewName);
			
			fContinue = FileIterator.FindNext();
		}
		
		// Convert the sender books.
		csSearch = csOldSenderBookPath+"*.DB";
		fContinue = FileIterator.FindFirst(csSearch, _A_NORMAL);
		while (fContinue)
		{
			if (CheckForAbort())
			{
				break;
			}
			
			// Compute the old and new names.
			csOldName = csOldSenderBookPath+FileIterator.Name();
			Util::SplitPath(csOldName, &csDrive, &csDirectory, &csFile, NULL);
			csNewName = csNewSenderBookPath;
			csNewName += csFile;
			csNewName += ADDRESS_BOOK_EXTENSION;
			ConvertSenderBook(csOldName, csNewName);
			
			fContinue = FileIterator.FindNext();
		}
	}
	END_TRY
}

void CDbconvertApp::ConvertAddressBook(LPCSTR pszOldName, LPCSTR pszNewName)
{
	TRACE("Convert Address Book from %s to %s\n", (LPCSTR)pszOldName, (LPCSTR)pszNewName);
	
	BOOL fSuccess = FALSE;
			
	ASSERT(pszOldName != NULL);
	ASSERT(pszNewName != NULL);
	
	ShowDialog();
	
	CBdeDatabase Database;
	CBdeCursor Cursor;
	StorageFile* pDatabaseFile = NULL;
	CFlatFileDatabase* pDatabase = NULL;
	CFlatFileDatabaseCursor* pDatabaseCursor = NULL;
	CString csFullName;
	CString csFile;

	TRY
	{
		// Initialize the progress dialog.
		csFullName = pszOldName;
		Util::SplitPath(csFullName, NULL, NULL, &csFile, NULL);
		SetName(csFile);
		SetPosition(0);
		
		// Open the old database.
		Database.Open(dbiREADONLY);
		Cursor.Open(&Database, pszOldName, dbiREADONLY);
		
		// Figure out the number of records in the old database.
		DWORD dwRecordCount;
		DbiGetRecordCount(Cursor.Handle(), &dwRecordCount);
		TRACE("%lu Records\n", dwRecordCount);
		SetRange(0, dwRecordCount);
		SetPosition(0);
		
		// Create and open the new address book database.
		TRY
		{
			CFile::Remove(pszNewName);
		}
		END_TRY
		
		ERRORCODE Error = OpenAddressBookDatabase(pszNewName, pDatabaseFile, pDatabase, TRUE);
		if (Error != ERRORCODE_None)
		{
			ThrowErrorcodeException(Error);
		}
		
		// Create a cursor on the database.
		pDatabaseCursor = new CFlatFileDatabaseCursor;
		pDatabaseCursor->Attach(pDatabase);
		pDatabaseCursor->SetIndex((DWORD)0);
		
		// Convert each record.
		DWORD dwPosition = 0;
		Cursor.ToStart();
		while (Cursor.GetNextRecord())
		{
			// Check for abort.
			if (CheckForAbort())
			{
				AfxThrowUserException();
			}
			
			// Initialize the new record.
			pDatabaseCursor->New();
			pDatabaseCursor->SetCounters(TRUE);
			ConvertField(Cursor, "FirstName", pDatabaseCursor, "First Name");
			ConvertField(Cursor, "LastName", pDatabaseCursor, "Last Name");
			ConvertField(Cursor, "Relationship", pDatabaseCursor, "Relationship");
			ConvertField(Cursor, "Title", pDatabaseCursor, "Title");
			ConvertField(Cursor, "Company", pDatabaseCursor, "Company");
			ConvertField(Cursor, "Address1", pDatabaseCursor, "Address1");
			ConvertField(Cursor, "Address2", pDatabaseCursor, "Address2");
			ConvertField(Cursor, "City", pDatabaseCursor, "City");
			ConvertField(Cursor, "State", pDatabaseCursor, "State");
			ConvertField(Cursor, "Zip", pDatabaseCursor, "Zip");
			ConvertField(Cursor, "Phone", pDatabaseCursor, "Phone");
			ConvertField(Cursor, "Country", pDatabaseCursor, "Country");
			ConvertField(Cursor, "Fax", pDatabaseCursor, "Fax");
			ConvertField(Cursor, "Email", pDatabaseCursor, "Email");
			ConvertField(Cursor, "Birthday", pDatabaseCursor, "Birthday");
			ConvertField(Cursor, "Anniversary", pDatabaseCursor, "Anniversary");
			ConvertField(Cursor, "UserDefinedInfo", pDatabaseCursor, "User Defined Info");
			ConvertField(Cursor, "UserDefinedDate", pDatabaseCursor, "User Defined Date");
			
			// Insert the record into the new database.
			pDatabaseCursor->Insert();

			// Set position based on record we just finished converting.
			dwPosition++;
			SetPosition(dwPosition);
		}
		
		fSuccess = TRUE;
	}
	CATCH_ALL(e)
	{
		fSuccess = FALSE;
	}
	END_CATCH_ALL
	
	delete pDatabaseCursor;
	CloseAddressBookDatabase(pDatabaseFile, pDatabase);
	
	Cursor.Close();
	Database.Close();
	
	if (!fSuccess)
	{
		CFile::Remove(pszNewName);
	}
}

void CDbconvertApp::ConvertSenderBook(LPCSTR pszOldName, LPCSTR pszNewName)
{
	TRACE("Convert Sender Book from %s to %s\n", (LPCSTR)pszOldName, (LPCSTR)pszNewName);
			
	BOOL fSuccess = FALSE;
	
	ASSERT(pszOldName != NULL);
	ASSERT(pszNewName != NULL);
	
	ShowDialog();
			
	CBdeDatabase Database;
	CBdeCursor Cursor;
	StorageFile* pDatabaseFile = NULL;
	CFlatFileDatabase* pDatabase = NULL;
	CFlatFileDatabaseCursor* pDatabaseCursor = NULL;
	CString csFullName;
	CString csFile;

	TRY
	{
		// Initialize the progress dialog.
		csFullName = pszOldName;
		Util::SplitPath(csFullName, NULL, NULL, &csFile, NULL);
		SetName(csFile);
		SetPosition(0);
		
		// Open the old database.
		Database.Open(dbiREADONLY);
		Cursor.Open(&Database, pszOldName, dbiREADONLY);
		
		// Figure out the number of records in the old database.
		DWORD dwRecordCount;
		DbiGetRecordCount(Cursor.Handle(), &dwRecordCount);
		TRACE("%lu Records\n", dwRecordCount);
		SetRange(0, dwRecordCount);
		SetPosition(0);
		
		// Create and open the new address book database.
		TRY
		{
			CFile::Remove(pszNewName);
		}
		END_TRY
		
		ERRORCODE Error = OpenAddressBookSenderDatabase(pszNewName, pDatabaseFile, pDatabase, TRUE);
		if (Error != ERRORCODE_None)
		{
			ThrowErrorcodeException(Error);
		}
		
		// Create a cursor on the database.
		pDatabaseCursor = new CFlatFileDatabaseCursor;
		pDatabaseCursor->Attach(pDatabase);
		pDatabaseCursor->SetIndex((DWORD)0);
		
		// Convert each record.
		DWORD dwPosition = 0;
		Cursor.ToStart();
		while (Cursor.GetNextRecord())
		{
			// Check for abort.
			if (CheckForAbort())
			{
				AfxThrowUserException();
			}
			
			// Initialize the new record.
			pDatabaseCursor->New();
			pDatabaseCursor->SetCounters(TRUE);
			ConvertField(Cursor, "FirstName", pDatabaseCursor, "First Name");
			ConvertField(Cursor, "LastName", pDatabaseCursor, "Last Name");
			ConvertField(Cursor, "Title", pDatabaseCursor, "Title");
			ConvertField(Cursor, "Company", pDatabaseCursor, "Company");
			ConvertField(Cursor, "Address1", pDatabaseCursor, "Address1");
			ConvertField(Cursor, "Address2", pDatabaseCursor, "Address2");
			ConvertField(Cursor, "City", pDatabaseCursor, "City");
			ConvertField(Cursor, "State", pDatabaseCursor, "State");
			ConvertField(Cursor, "Zip", pDatabaseCursor, "Zip");
			ConvertField(Cursor, "Phone", pDatabaseCursor, "Phone");
			ConvertField(Cursor, "Country", pDatabaseCursor, "Country");
			ConvertField(Cursor, "Fax", pDatabaseCursor, "Fax");
			ConvertField(Cursor, "Email", pDatabaseCursor, "Email");
			ConvertField(Cursor, "Birthday", pDatabaseCursor, "Birthday");
			ConvertField(Cursor, "Anniversary", pDatabaseCursor, "Anniversary");
			ConvertField(Cursor, "UserDefinedInfo", pDatabaseCursor, "User Defined Info");
			ConvertField(Cursor, "UserDefinedDate", pDatabaseCursor, "User Defined Date");
			
			// Insert the record into the new database.
			pDatabaseCursor->Insert();

			// Set position based on record we just finished converting.
			dwPosition++;
			SetPosition(dwPosition);
		}
		
		fSuccess = TRUE;
	}
	CATCH_ALL(e)
	{
		fSuccess = FALSE;
	}
	END_CATCH_ALL
	
	delete pDatabaseCursor;
	CloseAddressBookDatabase(pDatabaseFile, pDatabase);
	
	Cursor.Close();
	Database.Close();
	
	if (!fSuccess)
	{
		CFile::Remove(pszNewName);
	}
}

void CDbconvertApp::ConvertField(CBdeCursor& Cursor, LPCSTR pszOldFieldName, CFlatFileDatabaseCursor* pDatabaseCursor, LPCSTR pszNewFieldName)
{
	// Get the description of the field in the old database.
	CString csValue;
	const FLDDesc* pDescription = NULL;
	TRY
	{
		pDescription = Cursor.FieldDescription(pszOldFieldName);
		if (pDescription != NULL)
		{
			WORD wLength = pDescription->iLen;
			if (wLength > 0)
			{
				LPSTR pBuffer = csValue.GetBuffer(wLength+1);
				if (pBuffer != NULL)
				{
					Cursor.GetField(pszOldFieldName, (BYTE*)pBuffer, NULL);
					csValue.ReleaseBuffer();
					CFlatFileDatabaseField* pField = pDatabaseCursor->GetField(pszNewFieldName);
					if (pField != NULL)
					{
//						TRACE("Convert string field '%s' to '%s' value is '%s'\n", pszOldFieldName, pszNewFieldName, (LPCSTR)csValue);
						pField->SetAsString(csValue);
					}
				}
			}
		}
	}
	END_TRY
}

BOOL CDbconvertApp::GetDirectoryNames(void)
{      
	BOOL fResult = FALSE;
	
	TRY
	{
		GetParameter("/O", m_csOldDirectory.GetBuffer(_MAX_PATH));
		GetParameter("/N", m_csNewDirectory.GetBuffer(_MAX_PATH));
		fResult = TRUE;
	}
	END_TRY
	
	m_csOldDirectory.ReleaseBuffer();
	m_csNewDirectory.ReleaseBuffer();
	
	return fResult;
}

void CDbconvertApp::ShowDialog(void)
{
	if (!m_pDialog->IsWindowVisible())
	{
		m_pDialog->ShowWindow(SW_SHOW);
		m_pDialog->UpdateWindow();
	}
}
