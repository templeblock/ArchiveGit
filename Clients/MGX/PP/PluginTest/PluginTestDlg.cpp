// PluginTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PluginTest.h"
#include "PluginTestDlg.h"
#include "pitypes.h"
#include "pifilter.h"
#include "piexport.h"
#include "piacquire.h"
#include "io.h"
#include "CProgressDlg.h"
#include "dib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProgressDlg* CPluginTestDlg::pProgressDlg = NULL;

/////////////////////////////////////////////////////////////////////////////
CPluginTestDlg::CPluginTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPluginTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////////////
CPluginTestDlg::~CPluginTestDlg()
{
	FreeUpPluginList();
}

/////////////////////////////////////////////////////////////////////////////
void CPluginTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPluginTestDlg)
	DDX_Control(pDX, IDC_SEARCHPATH, m_editSearchPath);
	DDX_Control(pDX, IDC_PLUGINLIST, m_listPlugins);
	DDX_Control(pDX, IDC_SEARCH, m_btnSearch);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPluginTestDlg, CDialog)
	//{{AFX_MSG_MAP(CPluginTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_BN_CLICKED(IDC_ACQUIRE, OnAcquire)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CPluginTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, true);			// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon
	
	// TODO: Add extra initialization here
	m_editSearchPath.SetWindowText("D:\\Source\\PluginTest\\Plugins\\");;
	
	return true;  // return TRUE  unless you set the focus to a control
}

typedef void ( FAR pascal *LPADOBEENTRYPROC )( short selector, FilterRecordPtr stuff, long FAR *data, short FAR *result );
typedef void ( FAR pascal *LPEXPORTENTRYPROC )( short selector, ExportRecordPtr stuff, long FAR *data, short FAR *result );
typedef void ( FAR pascal *LPACQUIREENTRYPROC )( short selector, AcquireRecordPtr stuff, long FAR *data, short FAR *result );

typedef char STRING[256];
typedef char FNAME[256];
typedef struct ADOBEPLUGIN_tag
{
	STRING szFilter;
	STRING szFileName;
	int    bOldStyle;
	WORD   iListIndex;
	PLUGIN_TYPE piType;
	long	lData;
	struct ADOBEPLUGIN_tag FAR *lpNext;
}
ADOBEPLUGIN, FAR *LPADOBEPLUGIN;

// Globals
LPADOBEPLUGIN m_lpPluginList;
Boolean m_bAborted;

/////////////////////////////////////////////////////////////////////////////
BOOL CPluginTestDlg::AddFilter( CString& lpFilter, CString& lpFileName,
					bool bOldStyle, PLUGIN_TYPE piType, int iListIndex )
{
	// Allocate the memory for the new plugin
	LPADOBEPLUGIN lpNewPlugin = new ADOBEPLUGIN;

	if ( !lpNewPlugin )
		return( false );

	// Stuff the data members...
	lstrcpy( lpNewPlugin->szFilter, lpFilter );
	lstrcpy( lpNewPlugin->szFileName, lpFileName );
	lpNewPlugin->bOldStyle    = bOldStyle;
	lpNewPlugin->iListIndex   = iListIndex;
	lpNewPlugin->piType		  = piType;
	lpNewPlugin->lData		  = 0;
	lpNewPlugin->lpNext       = NULL;

	// Special case the first plugin
	if ( !m_lpPluginList )
	{
		m_lpPluginList = lpNewPlugin;
		return( true );
	}

	// Add the plugin to the end of the list
	LPADOBEPLUGIN lpPluginWalker = m_lpPluginList;

	while ( lpPluginWalker->lpNext )
		lpPluginWalker = lpPluginWalker->lpNext;

	lpPluginWalker->lpNext = lpNewPlugin;

	return( true );
}

/////////////////////////////////////////////////////////////////////////////
void CPluginTestDlg::FreeUpPluginList()
{
	// This is a cleanup call; Freeup the plugin list and return
	LPADOBEPLUGIN lpPluginWalker = m_lpPluginList;
	while ( lpPluginWalker )
	{
		LPADOBEPLUGIN lpDeadMeat = lpPluginWalker;
		lpPluginWalker = lpPluginWalker->lpNext;
		delete lpDeadMeat;
	}

	m_lpPluginList = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CPluginTestDlg::OnSearch() 
{
	// If it has already been setup, get rid of it
	if ( m_lpPluginList )
		FreeUpPluginList();

	m_listPlugins.ResetContent();

	int iListIndex = 0;
		
	// Loop through the plugin search path, and add any plugins to the list
	CString szFileSpec;
	m_editSearchPath.GetWindowText(szFileSpec);
	if (szFileSpec.GetAt(szFileSpec.GetLength()-1) != '\\')
		szFileSpec += "\\";
	szFileSpec += "*.8b?";

	CFileFind FileFind;
	int bOK = FileFind.FindFile(szFileSpec);
	if (bOK)
		bOK = FileFind.FindNextFile();

	// Walk the directory...
	BeginWaitCursor();
	while (bOK)
	{
		if (FileFind.IsDirectory())
		{ 
            // Skip over directories
			bOK = FileFind.FindNextFile();
			continue;
		}

		CString szName = FileFind.GetFileName();
		char LastChar = szName.GetAt(szName.GetLength()-1);
		if ( isupper( LastChar ))
			LastChar = tolower( LastChar );

		// Process the matching file
		CString szDLL = FileFind.GetFilePath();

		HMODULE hModule;
		if ( hModule = (HMODULE)LoadLibrary( szDLL ))
		{
			// Do module specifics here...
			STRING szResType;
			szResType[0] = '\0';
			PLUGIN_TYPE piType = PIT_FILTER;

			if ( LastChar == 'f' )
			{
				strcpy( szResType, "_8BFM" );
				piType = PIT_FILTER;

			}
            else
			if ( LastChar == 'e' )
			{
				strcpy( szResType, "_8BEM" );
				piType = PIT_EXPORT;
			}
            else
			if ( LastChar == 'a' )
			{
				strcpy( szResType, "_8BAM" );
				piType = PIT_ACQUIRE;
			}
			else
			{
			}

			HRSRC hResource = FindResource( hModule, (LPCSTR)"#1", (LPCSTR)szResType );

			// if hResource != NULL, then the plugin is the OLD style
			bool bOldStyle;
			CString szFilterName;
			if ( hResource )
			{
				bOldStyle = true;
				HGLOBAL hResData  = LoadResource( hModule, hResource );
				LPSTR lpResData = (LPSTR)LockResource( hResData );
				szFilterName = (LPSTR)&lpResData[2];
				UnlockResource( hResData );
				FreeResource( hResData );
			}
			else
			{
				if (hResource = FindResource( hModule, MAKEINTRESOURCE(16000), (LPCSTR)"PIPL" ))
				{
					bOldStyle = false;
					HGLOBAL hResData  = LoadResource( hModule, hResource );
					LPSTR lpResData = (LPSTR)LockResource( hResData );
					szFilterName = (LPSTR)&lpResData[47];
					UnlockResource( hResData );
					FreeResource( hResData );
				}
			}

			// If we found the resource we were looming for...
			if (hResource)
			{
				AddFilter( szFilterName, szDLL, bOldStyle, piType, iListIndex++ );
				m_listPlugins.AddString(szName);
			}

			FreeLibrary( hModule );
		}

		bOK = FileFind.FindNextFile();
	}

	FileFind.Close();
	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
Boolean MyAbortProc()
{
	m_bAborted = false;
	if (CPluginTestDlg::pProgressDlg)
		m_bAborted = CPluginTestDlg::pProgressDlg->CheckCancelButton();

	return m_bAborted;
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
void MyProgressProc( long iSoFar, long iTotal )
{
	if (CPluginTestDlg::pProgressDlg)
	{
		CPluginTestDlg::pProgressDlg->SetRange(1, iTotal);
		CPluginTestDlg::pProgressDlg->SetPos(iSoFar);
	}
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
short MyDisplayPixelsProc( const PSPixelMap FAR *source, const VRect FAR *srcRect,
			int32 dstRow, int32 dstCol, void* platformContext )
{
	HDC hDC = (HDC)platformContext;
	if (source->imageMode != plugInModeRGBColor)
		return( 1 );

	// only support chunky data
	long planes = 3;
	if ( source->planeBytes != 1 || source->colBytes != planes )
		return( 1 );

	// allocate buffer used for display
	long lSize = ( srcRect->right - srcRect->left ) * planes;

	// go to start of data in pixel map
	LPSTR hpSrc = (LPSTR)source->baseAddr;
	hpSrc += ( srcRect->top * source->rowBytes ) + srcRect->left;

	// setup SuperBlt
	RECT rDest;
	rDest.top = dstRow;
	rDest.left = dstCol;
	rDest.bottom = rDest.top + ( srcRect->bottom - srcRect->top ) - 1;
	rDest.right = rDest.left + ( srcRect->right - srcRect->left ) - 1;

	for (int y = rDest.top; y <= rDest.bottom; ++y)
	{
//j		copy( hpSrc, lpDispBuffer, lSize);
		hpSrc += source->rowBytes;
	}

	return( 0 );
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
OSErr MyAllocateBufferProc(int32 size, BufferID* bufferID)
{
	HGLOBAL hMemory = GlobalAlloc(GHND, size);
	*bufferID = (BufferID)hMemory;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
Ptr MyLockBufferProc(BufferID bufferID, Boolean /*moveHigh*/)
{
	HGLOBAL hMemory = (HGLOBAL)bufferID;
	Ptr pLockData = (Ptr)GlobalLock(hMemory);
	return pLockData;
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
void MyUnlockBufferProc(BufferID bufferID)
{
	HGLOBAL hMemory = (HGLOBAL)bufferID;
	GlobalUnlock(hMemory);
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
void MyFreeBufferProc(BufferID bufferID)
{
	HGLOBAL hMemory = (HGLOBAL)bufferID;
	GlobalFree(hMemory);
}

/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) 
int32 MyBufferSpaceProc(void)
{
	MEMORYSTATUS MemoryStatus;
	GlobalMemoryStatus(&MemoryStatus);
	// dwLength
	// dwMemoryLoad
	// dwTotalPhys
	// dwAvailPhys
	// dwTotalPageFile
	// dwAvailPageFile
	// dwTotalVirtual
	// dwAvailVirtual

	return MemoryStatus.dwAvailPageFile;
}

/////////////////////////////////////////////////////////////////////////////
void CPluginTestDlg::OnAcquire() 
{
	int iListIndex = m_listPlugins.GetCurSel();
	if ( iListIndex == LB_ERR )
		return;

	// Find the plugin that matches the command ID from the plugin list
	LPADOBEPLUGIN lpPlugin = m_lpPluginList;
	while (lpPlugin)
	{
		if ( lpPlugin->iListIndex == iListIndex )
			break;
		lpPlugin = lpPlugin->lpNext;
	}

	// Get out if you can't find it
	if ( !lpPlugin )
		return;

//j	if ( lpPlugin->piType != PIT_ACQUIRE )
//j		return;

	// Load the acquisition DLL
	HMODULE hModule;
	if ( !( hModule = LoadLibrary(lpPlugin->szFileName)))
		return;

	CWnd* hFocus = GetFocus();

	bool bDone = false;
	m_bAborted = false;

	// Get the entry point into the DLL
	LPACQUIREENTRYPROC lpEntryPoint;
	if (!(lpEntryPoint = (LPACQUIREENTRYPROC)GetProcAddress( hModule,
		(lpPlugin->bOldStyle ? "ENTRYPOINT1" : "ENTRYPOINT") )))
		goto BadStart;

	// Set the FilterRecord Info
	PlatformData MyPlatformData;
	memset(&MyPlatformData, 0, sizeof(MyPlatformData));
	MyPlatformData.hwnd = (long)GetSafeHwnd();

	BufferProcs MyBufferProcs;
	memset(&MyBufferProcs, 0, sizeof(MyBufferProcs));
	MyBufferProcs.bufferProcsVersion = kCurrentBufferProcsVersion;
	MyBufferProcs.numBufferProcs = kCurrentBufferProcsCount;
	MyBufferProcs.allocateProc = MyAllocateBufferProc;
	MyBufferProcs.lockProc = MyLockBufferProc;
	MyBufferProcs.unlockProc = MyUnlockBufferProc;
	MyBufferProcs.freeProc = MyFreeBufferProc;
	MyBufferProcs.spaceProc = MyBufferSpaceProc;

	AcquireRecord Record;
	memset(&Record, 0, sizeof(Record));
	Record.abortProc       = MyAbortProc;
	Record.progressProc    = MyProgressProc;
	Record.maxData         = MyBufferSpaceProc();
	Record.planeMap[0]     = 0;
	Record.planeMap[1]     = 1;
	Record.planeMap[2]     = 2;
	Record.planeMap[3]     = 3;
	Record.platformData    = &MyPlatformData;
	Record.canTranspose    = false;
	Record.needTranspose   = false;
	Record.canReadBack     = false;
	Record.canFinalize	   = true;
	Record.diskSpace       = -1;
	Record.spaceProc       = NULL;
	Record.bufferProcs     = NULL; //j &MyBufferProcs;
	Record.resourceProcs   = NULL;
	Record.processEvent    = NULL;
	Record.displayPixels   = MyDisplayPixelsProc;
	Record.handleProcs     = NULL;
	Record.hostProc	       = NULL;	// Host specific callback procedure
	Record.hostModes	   = (1<<plugInModeRGBColor);

	Record.hostSig         =
		( long )'M' << 24 | ( long )'C' << 16 | ( long )'O' << 8 | ( long )'S';

//j	Record.monitor.gamma   = 65536L;
//j	Record.monitor.redX    = 40960L;
//j	Record.monitor.redY    = 22282L;
//j	Record.monitor.greenX  = 18350L;
//j	Record.monitor.greenY  = 38993L;
//j	Record.monitor.blueX   = 10158L;
//j	Record.monitor.blueY   = 4587L;
//j	Record.monitor.whiteX  = 20493L;
//j	Record.monitor.whiteY  = 21561L;
//j	Record.monitor.ambient = 32768L;

	LPLONG lpData;
	lpData = &lpPlugin->lData;
	short sErrCode;
	sErrCode = true;

	// The acquireSelectorPrepare function sets up the acquire operation.
	try
	{
		(*lpEntryPoint)( (short)acquireSelectorPrepare, (AcquireRecordPtr FAR)&Record, (long* FAR)lpData, (short* FAR)&sErrCode );
	}
	catch(...)
	{
		sErrCode = true;
	}

	if ( sErrCode )
	{
		m_bAborted = true;
		goto BadStart;
	}

	// The acquireSelectorStart function informs the host of the image type, 
	// mode, and resolution. Plugin dialogs are displayed during this call. 
	try
	{
		(*lpEntryPoint)( (short)acquireSelectorStart, (AcquireRecordPtr FAR)&Record, (long* FAR)lpData, (short* FAR)&sErrCode );
	}
	catch(...)
	{
		sErrCode = true;
	}

	if ( sErrCode )
	{
		m_bAborted = true;
	}

	bool bRGBColor;
	bRGBColor = (Record.depth == 8 && Record.planes == 3);
	if ( !bRGBColor || Record.imageMode != plugInModeRGBColor )
	{
		m_bAborted = true;
	}

	// Only support chunky data
	if ( Record.planeBytes != 1 || Record.colBytes != Record.planes )
	{
		m_bAborted = true;
	}

	// Create a DIB to catch the image data
	CDib* pDib;
	if (pDib = new CDib())
	{
		pDib->Create(8 * Record.planes, Record.imageSize.h, Record.imageSize.v);
	}
	else
	{
		m_bAborted = true;
	}

	// Create a progress dialog
	if (CPluginTestDlg::pProgressDlg = new CProgressDlg)
	{
		CPluginTestDlg::pProgressDlg->Create(this);
		CPluginTestDlg::pProgressDlg->SetStatus((const char*)Record.filename);
		CPluginTestDlg::pProgressDlg->SetStep(1);
	}

	// start the processing of the data
	while ( !bDone && !m_bAborted )
	{
		// The acquireSelectorContinue function returns an area of the image to the host.
		try
		{
			(*lpEntryPoint)( (short)acquireSelectorContinue, (AcquireRecordPtr FAR)&Record, (long* FAR)lpData, (short* FAR)&sErrCode );
		}
		catch(...)
		{
			sErrCode = true;
		}

		if ( sErrCode )
		{
			m_bAborted = true;
			break;
		}

		if ( !Record.data )  // no more data
		{
			bDone = true;
			break;
		}

		if (pDib)
		{
			long lRowBytes;
			if ( Record.rowBytes < 1 )
				lRowBytes = ( Record.theRect.right - Record.theRect.left ) * Record.planes; 
			else
				lRowBytes = Record.rowBytes;

			// Write the data into the DIB
			HPTR pData;
			pData = (HPTR)Record.data;
			for (int y=Record.theRect.top; y<Record.theRect.bottom; y++)
			{ 
					HPTR pRow = pDib->GetXY(Record.theRect.left, y);
					if (pRow)
						memcpy(pRow, pData, lRowBytes);
					pData += lRowBytes;
			} //j watch: Record.theRect, Record.loPlane, Record.hiPlane
		}
	}

	if (CPluginTestDlg::pProgressDlg)
		delete pProgressDlg;

	if (pDib)
	{
		pDib->WriteFile("c:\\windows\\desktop\\test.dib");
		delete pDib;
	}

	// The acquireSelectorFinish function must be called when exiting at any 
	// point after acquireSelectorStart is called without error.
	try
	{
		(*lpEntryPoint)( (short)acquireSelectorFinish, (AcquireRecordPtr FAR)&Record, (long* FAR)lpData, (short* FAR)&sErrCode );
	}
	catch(...)
	{
		sErrCode = true;
	}

	if ( sErrCode )
		m_bAborted = true;

	// The acquireSelectorFinalize function must be called if asked for
	if (Record.wantFinalize)
	{
		try
		{
			(*lpEntryPoint)( (short)acquireSelectorFinalize, (AcquireRecordPtr FAR)&Record, (long* FAR)lpData, (short* FAR)&sErrCode );
		}
		catch(...)
		{
			sErrCode = true;
		}
	
		if ( sErrCode )
			m_bAborted = true;
	}

	BadStart:

	if ( hModule )
		FreeLibrary( hModule );

	if ( hFocus )
		hFocus->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CPluginTestDlg::OnPaint() 
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

/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPluginTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
