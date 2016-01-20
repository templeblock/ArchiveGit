// ****************************************************************************
//
//  File Name:			StandaloneApplicationBase.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RStandaloneApplication class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/StandaloneApplication.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"StandaloneApplication.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIDs.h"
#include "StandaloneDocument.h"
#include "StandaloneView.h"
#include "ComponentDocument.h"
#include "View.h"
#include "ComponentApplication.h"
#include "EditMemFailure.h"
#include "EditFileFailure.h"
#include "StreamScript.h"
#include "ChunkyStorage.h"
#include "FileOpenAction.h"
#include "FileCloseAction.h"
#include "FileSaveAction.h"
#include "FilePrintAction.h"
#include	"Frame.h"
#include "NewHandler.h"
#include "ApplicationGlobals.h"
#include "MemoryMappedStream.h"
#include "Menu.h"
#include "ResourceManager.h"
#include "IdleProcessing.h"
#include "FileFormat.h"
#include "FileType.h"
#include "CorrectSpell.h"
#include "Intellifinder.h"
#include "ComponentView.h"
#include "Mainframe.h"
#include "ClipboardDataTransfer.h"
#include	"PrintManager.h"
#include "PrintQueue.h"
#include "ComponentManager.h"
#include "Configuration.h"


//	By having this value defined, the /BlankPage command can be sent to Renaissance
//		when there is already a copy running and it will just open a tall sign in 
//		the already running copy.	-	Neat Huh...
#define	BLANKPAGE_FIX_WHEN_RUNNING		1

// Command Map
RCommandMap<RStandaloneApplication, RApplication> RStandaloneApplication::m_CommandMap;

RCommandMap<RStandaloneApplication, RApplication>::RCommandMap( )
	{
	HandleCommand( COMMAND_MENU_FILE_NEW, RStandaloneApplication::OnNew );
	HandleCommand( COMMAND_MENU_FILE_QUIT, RStandaloneApplication::OnQuit );
	HandleCommand( COMMAND_MENU_FILE_OPEN, RStandaloneApplication::OnOpen );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_SCRIPT_RECORD, RStandaloneApplication::OnScriptRecord, RStandaloneApplication::OnUpdateScriptRecord );
	HandleAndUpdateCommand( COMMAND_MENU_DEBUG_SCRIPT_PLAYBACK, RStandaloneApplication::OnScriptPlayback, RStandaloneApplication::OnUpdateScriptPlayback );
	HandleCommand( COMMAND_MENU_DEBUG_FILEDIALOG, RStandaloneApplication::OnFileFailDialog );
	HandleCommand( COMMAND_MENU_DEBUG_FILE_SUCCEED, RStandaloneApplication::OnFileSucceed );
	HandleCommandRange( COMMAND_MENU_DEBUG_FILE_FAIL1, COMMAND_MENU_DEBUG_FILE_FAIL4, RStandaloneApplication::OnFileFail );
	HandleCommand( COMMAND_MENU_DEBUG_MEMDIALOG, RStandaloneApplication::OnMemoryFailDialog );
	HandleCommand( COMMAND_MENU_DEBUG_MEM_SUCCEED, RStandaloneApplication::OnMemorySucceed );
	HandleCommandRange( COMMAND_MENU_DEBUG_MEM_FAIL1, COMMAND_MENU_DEBUG_MEM_FAIL4, RStandaloneApplication::OnMemoryFail );
	HandleCommand( COMMAND_MENU_DEBUG_MEM_PURGE, RStandaloneApplication::OnMemoryPurge );
	HandleCommand( COMMAND_MENU_FILE_PRINTTEST, RStandaloneApplication::CalibratePrinters );
	}
	
// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::RStandaloneApplication( )
//
//  Description:		Constructor. The parameters are not used on the Mac, and
//							may be anything.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneApplication::RStandaloneApplication( YMenuIndex componentSubMenuIndex, YResourceId uwDocumentResourceID )
	: FrameworkApplication( ),
	  RApplication( ),
	  m_uwDocumentResourceID( uwDocumentResourceID ),
	  m_fPlayback( FALSE ),
	  m_pApplicationGlobals( NULL ),
	  m_ComponentSubMenuIndex( componentSubMenuIndex ),
	  m_pApplicationMenu( NULL ),
	  m_fLastClosedWindowWasMaximized( TRUE ),
	  m_fLaunchedForPrinting( FALSE )

	{
	// Create the application globals
	m_pApplicationGlobals = new RApplicationGlobals( this );

	// Set them up globally
	m_gsApplicationGlobals = m_pApplicationGlobals;

	#ifdef	_WINDOWS
		// DDE Support
		m_dwDDEInstance = 0;
	#endif // _WINDOWS

#ifdef	TPSDEBUG
#ifdef	USE_SMARTHEAP
		//	Setup a trace handler for smartheap - must be after ApplicationGlobals are created
		void __stdcall RenaissanceSmartHeapTraceFn(MEM_ERROR_INFO MEM_FAR * errorInfo, unsigned long /* longValue */ );
		dbgMemSetEntryHandler( RenaissanceSmartHeapTraceFn );
#else		//	USE_SMARTHEAP
		int __cdecl RenaissanceAllocHook( int nAllocType, void* pvData, size_t nSize, int nBlockUse, 
											 long lRequest, const unsigned char * szFileName, int nLine );
	   _CrtSetAllocHook( (_CRT_ALLOC_HOOK)RenaissanceAllocHook );
#endif	//	USE_SMARTHEAP
#endif	//	TPSDEBUG
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::~RStandaloneApplication( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneApplication::~RStandaloneApplication( )
	{
	delete m_pApplicationGlobals;
	delete m_pApplicationMenu;
	delete m_pPrintQueue;

	#ifdef	_WINDOWS
		if ( m_lpWindowClassName != NULL )
			free( m_lpWindowClassName );
	#endif // _WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::Initialize( )
//
//  Description:		Initializes the application instance. Called by MFC in
//							WinMain, just before CWinApp::Run( ).
//
//							We just delegate to our Initialize routine
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
BOOLEAN RStandaloneApplication::Initialize( RApplicationGlobals* pGlobals )
	{
	try
		{
		// Start up the print manager. This will be done in background thread.
		RPrintManager::Startup( );

		// Call the base method
		if( !RApplication::Initialize( pGlobals ) )
			return FALSE;

		//	Initialize the Correct Spell and Intellifinder Engines
		GetCorrectSpell( ).Initialize( TRUE, TRUE );
		GetIntelliFinder( ).Initialize( );

		// Load the main menu
		LoadMainMenu( );

		//	Initialize the Stream library
		RStream::SystemStartup( );

		// Add ourself to the resource manager
		GetResourceManager( ).AddResourceFile( MacWinDos( CurResFile( ), AfxGetInstanceHandle(), xxx ) );

		// Initialize the bitmap toolbox
		InitializeBitmapToolbox( );

		// Enable 3d controls in case we are running on NT 3.51
		Enable3dControls( );

  		// Load standard INI file options (including MRU)
		LoadStdProfileSettings( );

		// Create the print queue. Must be done after the main window exists
		m_pPrintQueue = new RPrintQueue;
		}

	catch( ... )
		{
		return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::CalibratePrinters( )
//
//  Description:		Start the printer calibration process
//
//  Exceptions:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::CalibratePrinters( )
	{
	RPrintManager::GetPrintManager( ).TestPrinter( RPrintManager::GetPrintManager( ).GetDefaultPrinter( ) );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::LoadMainMenu( )
//
//  Description:		Loads the main application menu
//
//  Exceptions:		FALSE if the app should terminate
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::LoadMainMenu( )
	{
#ifdef _WINDOWS

	// Load the application menu
	m_hApplicationMenu = ::LoadMenu( AfxGetResourceHandle( ), MAKEINTRESOURCE( m_uwDocumentResourceID ) );

	if( !m_hApplicationMenu )
		{
		RAlert( ).AlertUser( STRING_ERROR_GENERAL_STARTUP );
		throw FALSE;
		}

	// Put it into an RMenu
	m_pApplicationMenu = new RMenuBar( m_hApplicationMenu );

#endif
	//
	//	REVIEW BRD(mdh)
	//	If we are not in a debug mode, we need to remove the debug menu
	//		This logic ASSUMES that the debug menu is count - 2
	if ( !::GetDebugMode( ) )
		{
		YMenuIndex	debugIndex	= m_pApplicationMenu->GetItemCount( ) - 2;
		m_pApplicationMenu->RemoveItemByIndex( debugIndex );
		}
	}

#ifdef	_WINDOWS
WINDOWPLACEMENT initialWindowPlacement;
#endif	//	_WINDOWS
// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::PositionAndShowMainWindow( )
//
//  Description:		Loads the main window position, and shows it
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::PositionAndShowMainWindow( )
	{
	if( m_fLaunchedForPrinting )
		return;

#ifdef _WINDOWS

		// Retrieve and set the window placement
		WINDOWPLACEMENT windowPlacement;		
		
		// If the placement information exists, set it			
		uLONG uDataLength;
		RMainframeConfiguration rConfig;		
		if(rConfig.GetBinaryValue(RMainframeConfiguration::kPosition, &windowPlacement, uDataLength))
			{
			TpsAssert(uDataLength == sizeof(WINDOWPLACEMENT), "Actual data size does not equal expected data size.");
			
			// If the window is minimized, maximize it instead
			if( windowPlacement.showCmd == SW_SHOWMINIMIZED )
				windowPlacement.showCmd = SW_MAXIMIZE;
			else
				{
				// check for a visible portion of title bar in main app
				// if positioned off the desktop, maximize window
				CRect	rcDesktop;
				CWnd*	pDesktopWnd = m_pMainWnd->GetDesktopWindow();
				pDesktopWnd->GetWindowRect(rcDesktop);
				CRect	rcAppTitle(windowPlacement.rcNormalPosition);
				rcAppTitle.DeflateRect(::GetSystemMetrics(SM_CXBORDER), ::GetSystemMetrics(SM_CYBORDER));
				rcAppTitle.bottom = rcAppTitle.top + ::GetSystemMetrics(SM_CYCAPTION); 				
				CRect	rcIntersect;
				rcIntersect = rcDesktop & rcAppTitle;
				if(rcIntersect.right <= 0 || rcIntersect.bottom <= 0)
					{
					// set normal position to the desktop
					windowPlacement.rcNormalPosition.left = 0;
					windowPlacement.rcNormalPosition.top = 0;
					windowPlacement.rcNormalPosition.right = rcDesktop.right;
					windowPlacement.rcNormalPosition.bottom = rcDesktop.bottom;
					windowPlacement.showCmd = SW_MAXIMIZE;
					}
				}
			}

		// Otherwise, default to maximized
		else
			{
			windowPlacement			= initialWindowPlacement;
			windowPlacement.showCmd = SW_MAXIMIZE;
			}
		
		m_pMainWnd->SetWindowPlacement( &windowPlacement );
		m_pMainWnd->ShowWindow( SW_SHOW );
		m_pMainWnd->SetFocus( );
		m_pMainWnd->UpdateWindow( );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::PositionAndHideMainWindow( )
//
//  Description:		Loads the main window position, and hides it
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::PositionAndHideMainWindow( )
	{
	if( m_fLaunchedForPrinting )
		return;

#ifdef _WINDOWS

	//	Save the initial location of the MainFrame
	m_pMainWnd->GetWindowPlacement( &initialWindowPlacement );
	// move the application window entirely offscreen to give the illusion
	// of hiding the app on startup. project picker dialog thus appears to
	// be the app initially.
	// the application window must remain 'visible' so that the
	// app is represented in the task bar.
	// the selected coordinates for positioning should guarantee that the 
	// application window is offscreen!
	m_pMainWnd->SetWindowPos(NULL, 10000, 10000, 1280, 1024,
		SWP_NOREDRAW);
	m_pMainWnd->ShowWindow( SW_SHOW );
	m_pMainWnd->UpdateWindow();

#endif
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::Shutdown( )
//
//  Description:		Terminates the application instance
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::Shutdown( )
	{
	// Shutdown the print manager - if initialized
	if ( RPrintManager::IsPrintManagerInitialized( ) )
		RPrintManager::GetPrintManager( ).Shutdown( );

	// free bitmap subsystem
	if( m_pApplicationGlobals->GetGlobalBitmapPool( ) )
		tbitShutdown( );							

	//	shutdown the Stream library
	RStream::SystemShutdown();

	// Remove ourself from the reource manager
	GetResourceManager().RemoveResourceFile( MacWinDos( CurResFile( ), AfxGetInstanceHandle(), xxx ) );

	// Call the base class
	RApplication::Shutdown( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::SendAction( RAction* pAction )
//
//  Description:		Component API function to send an action to a Component
//
//  Returns:			TRUE: we wrote the action to the current script successfully
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneApplication::SendAction( RAction* pAction )
	{
	BOOLEAN					fReturnValue	= FALSE;

	//	First Always, try to write the script as it is in its state before the 
	//	Do occurs. This will allow us to Create the Action and then call Do
	//	in the script processing.
	try
		{
		if ( GetRecordingScript( ) )
			{
			RScript&	script	= *GetRecordingScript( );
			script << pAction->GetActionId( );
			pAction->WriteScript( script );
			fReturnValue = TRUE;
			}
		}
	catch( YException exception )
		{
		::ReportException( exception );///xxx	TODO: Script Dumping Failed... Alert user and continue
		}

	//	actions that get here will not be on any action tracking lists so delete them
	delete pAction;

	return fReturnValue;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::CreateScriptAction( )
//
//  Description:		Try to create an action from the script
//
//  Returns:			The pointer to the action or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RStandaloneApplication::CreateScriptAction( const YActionId& actionId, RScript& script )
	{
	if ( actionId == RFileOpenAction::m_ActionId )
		return new RFileOpenAction( script );

	else if ( actionId == RFileCloseAction::m_ActionId )
		return new RFileCloseAction( script );
	
	else if ( actionId == RFilePrintAction::m_ActionId )
		return new RFilePrintAction( script );

	else if ( actionId == RFileSaveAction::m_ActionId )
		return new RFileSaveAction( script );

	// Apparently no one wants this command. Return FALSE.
	return RApplication::CreateScriptAction( actionId, script );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::InitializeBitmapToolbox( )
//
//  Description:		Initializes the bitmap toolbox
//
//  Returns:			Nothing
//
//  Exceptions:		FALSE if the app should terminate
//
// ****************************************************************************
//
void RStandaloneApplication::InitializeBitmapToolbox( )
	{
	HNATIVE	hnColors = NULL;

	//	Initialize the bitmap toolbox - needs palette, so must be after AddResourceFile
	try
		{
			YResourceType		trt;
			SetResourceType(trt, 'P','L','T','T');
			if ( NULL == (hnColors = GetResourceManager().GetResource(trt, PALETTE_APPLICATION_256)) )
				throw kResource;

			if (!tbitStartup (3, 0, hnColors))		//	initialize with 3 ports for any depth
				throw kMemory;

			GetResourceManager().FreeResource(hnColors);
			hnColors = NULL;								//	in case of exception
		}
	catch( YException /* exception */ )
		{
			if(hnColors != NULL)
				GetResourceManager().FreeResource(hnColors);

			RAlert( ).AlertUser( STRING_ERROR_GENERAL_STARTUP );

			throw FALSE;
		}
	}

#ifdef	_WINDOWS

#ifdef	TPSDEBUG
#ifdef	USE_SMARTHEAP
// ****************************************************************************
//
//  Function Name:	RenaissanceSmartHeapTraceFn
//
//  Description:		Smartheap trace function callback. We use this to implement
//							forced memory errors.
//
//  Returns:			Nothing
//
//  Exceptions:		RForcedMemoryException
//
// ****************************************************************************
//
void __stdcall RenaissanceSmartHeapTraceFn(MEM_ERROR_INFO MEM_FAR * errorInfo, unsigned long /* longValue */ )
	{
	switch (errorInfo->errorAPI)
		{
		case MEM_MEMALLOC :
		case MEM_MEMREALLOC :
		case MEM_MEMALLOCPTR :
		case MEM_MEMREALLOCPTR :
		case MEM_MEM_MALLOC :
		case MEM_MEM_CALLOC :
		case MEM_MEM_REALLOC :
		case MEM_NEW :
			{
			//	If no file name, assume it is stdlib stuff
			if ( !errorInfo->file )
				break;
			char chFileName = errorInfo->file[0];
			if ( (chFileName < 'A') || (chFileName > 'Z') )
				break;

			sLONG&	slMemSuccessCount = GetMemSuccessCount();
			sLONG&	slMemFailureCount	= GetMemFailureCount();
			//	If we are in a count down
			if ( slMemSuccessCount > 0 )
				--slMemSuccessCount;
			else if (slMemSuccessCount == 0)
				{
				//	If we are at at zero, throw an error
				if (slMemFailureCount-- > 0)
					throw RForcedMemoryException( (char*)errorInfo->file, errorInfo->line, errorInfo->argSize );
				--slMemSuccessCount;	//	Sets counter to -1 to stop errors
				}
			}
			break;

		default:
			break;
		}
	}
#else		// USE_SMARTHEAP
/* ALLOCATION HOOK FUNCTION
   -------------------------
   An allocation hook function can have many, many different
   uses.
*/

int __cdecl RenaissanceAllocHook( int nAllocType, void* , size_t nSize, int nBlockUse, 
											 long , const unsigned char * szFileName, int nLine )
{
	const	int	kMaxAllocBeforeTest	= 1024;

   if ( nBlockUse == _CRT_BLOCK )   // Ignore internal C runtime library allocations
      return TRUE ;

	sLONG&	slMemSuccessCount = GetMemSuccessCount();
	sLONG&	slMemFailureCount	= GetMemFailureCount();

	if (nAllocType == _HOOK_ALLOC || nAllocType == _HOOK_REALLOC)
	{
		if ( nSize < kMaxAllocBeforeTest )
			return TRUE;
		//	If we are in a count down
		if ( slMemSuccessCount > 0 )
			--slMemSuccessCount;
		else if (slMemSuccessCount == 0)
			{
			//	If we are at at zero, throw an error
			if (slMemFailureCount-- > 0)
				throw RForcedMemoryException( (char*)szFileName, nLine, nSize );
			--slMemSuccessCount;	//	Sets counter to -1 to stop errors
			}
	}

   return TRUE ;         // Allow the memory operation to proceed
}

#endif	//	USE_SMARTHEAP

#endif	//	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	InitialiseDDEServers
//
//  Description:	Creates a new DDE Server called "PrintShop6Server"
//					and calls AlreadyRunning to see whether another instance
//					of PSD is executing.	
//
//  Returns:		TRUE if Application startup should continue.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneApplication::InitialiseDDEServers()
{
	BOOL bDDEInitialised = FALSE;

	bDDEInitialised = ( ::DdeInitialize( &m_dwDDEInstance, (PFNCALLBACK) &DdeCallback, APPCLASS_STANDARD, 0 ) == DMLERR_NO_ERROR );

	if ( bDDEInitialised != FALSE )
	{
		// use Windows ANSI code page
		HSZ hServName = ::DdeCreateStringHandle( m_dwDDEInstance, _T("PrintShop6Server"), CP_WINANSI );

		if ( hServName != 0L )
		{
			if (	(AlreadyRunning( hServName ) != FALSE ) || 
					(::DdeNameService( m_dwDDEInstance, hServName, 0, DNS_REGISTER ) <= 0 ))
						bDDEInitialised = FALSE;

			// free server name
			::DdeFreeStringHandle( m_dwDDEInstance, hServName );
		}
	}
	
	if ( bDDEInitialised == FALSE )
	{
		// DDE Initialization failed. The app. will terminate.
		TRACE( "DDE could not be initialized!" );
		return FALSE;
	}

	return TRUE;
}


// ****************************************************************************
//
//  Function Name:	ShutdownDDEServer
//
//  Description:		Frees all DDE resource and closes DDE server.
//					
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::ShutdownDDEServer()
{
	// shut down the DDE server and clean-up.
	::DdeNameService( m_dwDDEInstance, 0L, 0L, DNS_UNREGISTER );
	::DdeUninitialize( m_dwDDEInstance );
}

// ****************************************************************************
//
//  Function Name:	GetDDEServerInstance
//
//  Description:	Returns DDE identifier for the PSD DDE server.
//
//  Returns:		DWORD - server DDE id.
//
//  Exceptions:		None
//
// ****************************************************************************
//
DWORD RStandaloneApplication::GetDDEServerInstance() const
{
	return m_dwDDEInstance;
}


// ****************************************************************************
//
//  Function Name:	AlreadyRunning
//
//  Description:		Tries to connect to an instance of the PSD DDE Server. If 	
//							the connect succeeds then we are already running. If the
//							connect suceeds, send a "SetForegroundWindow" command to
//							the running server.
//
//  Returns:			TRUE if another instance of PSD is running.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneApplication::AlreadyRunning( HSZ hServName )
{
	 // use Windows ANSI code page
	HSZ hTopicName = ::DdeCreateStringHandle( m_dwDDEInstance, _T("SetForegroundWindow"), CP_WINANSI );

	// could not create string, ASSUME that another instance is running.
	if ( hTopicName == 0L )
	{
		TpsAssertAlways( "Could not create DDE topic - assuming another instance is running... " );
		return TRUE;
	}

	// attempt to connect to the server
	HCONV hConv = ::DdeConnect( m_dwDDEInstance, hServName, hTopicName, NULL );

	if ( hConv != 0L )
	{
		// connect succeeded, send XTYP_EXECUTE command
		// send the application command line along with the command
		DWORD dwResult = 0;

		// send the XTYP_EXECUTE command
		HDDEDATA hData = ::DdeClientTransaction(
			(LPBYTE) AfxGetApp()->m_lpCmdLine,
			(DWORD) strlen( AfxGetApp()->m_lpCmdLine ),	// length of data
			hConv,													// handle to conversation
			0L,														// handle to item name string
			0L,														// clipboard data format
			XTYP_EXECUTE,											// transaction type
			(DWORD) 5000,											// time-out duration
			(LPDWORD) &dwResult 									// pointer to transaction result
			);		

		// the server should have processed the command.
		TpsAssert( dwResult == DDE_FACK, "There is a server running but it did not return DDE_FACK. Is it starting up??" );
		TRACE("dwResult = %d, DdeGetLastError = %d\n", dwResult, ::DdeGetLastError( m_dwDDEInstance ) );

		// clean-up the data
		DdeFreeDataHandle( hData );
	
		// yes, another instance of PSD is running.
		return TRUE;
	}

	// free topic name
	::DdeFreeStringHandle( m_dwDDEInstance, hTopicName );

	// no, another instance was not found.
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	::DDEParseCommandLine
//
//  Description:		Parses the command line gotten from DDE
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void DDEParseCommandLine( CString& szCommandLine, CCommandLineInfo& commandLineInfo )
	{
	// Build an array of arguments
	RArray<CString> argumentList;

	BOOLEAN fStartQuote = FALSE;
	CString currentArgument;

	// Loop through the command line string
	for( int i = 0; i < szCommandLine.GetLength( ); ++i )
		{
		// Check for a quote
		if( szCommandLine[ i ] == '\"' )
			{
			// Add the current argument
			if( !currentArgument.IsEmpty( ) )
				{
				argumentList.InsertAtEnd( currentArgument );
				currentArgument.Empty( );
				}

			// Toggle the start quote flag
			fStartQuote = !fStartQuote;
			}

		// Check for a space, unless we are in a quote
		else if( szCommandLine[ i ] == ' ' && !fStartQuote )
			{
			// Add the current argument
			if( !currentArgument.IsEmpty( ) )
				{
				argumentList.InsertAtEnd( currentArgument );
				currentArgument.Empty( );
				}
			}

		// Otherwise add the character to the current argument
		else
			currentArgument += szCommandLine[ i ];
		}
#ifdef	BLANKPAGE_FIX_WHEN_RUNNING
	//	If currentArgument is not empty, add to argumentList
	if ( !currentArgument.IsEmpty( ) )
		{
		argumentList.InsertAtEnd( currentArgument );
		currentArgument.Empty( );
		}
#endif	//	BLANKPAGE_FIX_WHEN_RUNNING

	// Parse the arguments. This was lifted straight from MFC
	for( i = 0; i < argumentList.Count( ); i++ )
		{
		BOOL bFlag = FALSE;
		BOOL bLast = ( (i + 1) == argumentList.Count( ) );

		const char* p = argumentList[i];

		if (p[0] == '-' || p[0] == '/')
			{
			// remove flag specifier
			bFlag = TRUE;
			++p;
			}

		commandLineInfo.ParseParam(p, bFlag, bLast);
		}
	}

// ****************************************************************************
//
//  Function Name:	DdeCallback
//
//  Description:		Registered with DDE to receive client DDE requests.
//						
//	Parameters
//    uType				transaction type
//    uFmt				clipboard data format
//    hconv				handle to the conversation
//    hsz1				handle to a string
//    hsz2				handle to a string
//    hdata				handle to a global memory object
//    dwData1			transaction-specific data
//    dwData2			transaction-specific data
//
//  Returns:			Depends on uType (see WIN32 documentation)
//
//  Exceptions:		None
//
// ****************************************************************************
//

// unreferenced parameters
#pragma warning( disable : 4100 )

HDDEDATA CALLBACK RStandaloneApplication::DdeCallback( UINT uType, UINT uFmt, HCONV hconv, 
													  HSZ hsz1, HSZ hsz2, HDDEDATA hdata, 
													  DWORD dwData1, DWORD dwData2 )
{
	
	// filter the incoming messages
	switch ( uType )
	{
		case XTYP_ADVSTART:				// Connect requests
		case XTYP_CONNECT:
			return (HDDEDATA) TRUE;
			break;

		case XTYP_ADVREQ:				// Data
		case XTYP_REQUEST:
		case XTYP_WILDCONNECT:
			return (HDDEDATA) NULL;
			break;

		case XTYP_ADVDATA:				// we should have valid data, 
		case XTYP_EXECUTE:				// fall through to interpret the XTYP_EXECUTE data.
		case XTYP_POKE:
			break;

		case XTYP_ADVSTOP:				// DDE notifications
		case XTYP_CONNECT_CONFIRM:
		case XTYP_DISCONNECT:
		case XTYP_ERROR:
		case XTYP_MONITOR:
		case XTYP_REGISTER:
		case XTYP_XACT_COMPLETE:
		case XTYP_UNREGISTER:
			return (HDDEDATA) TRUE;
			break;

		default:						// undocumented DDE commands
			ASSERT( FALSE );
			return (HDDEDATA) TRUE;
		break;
	}

	// this is a static function, so we cannot use "this"
	RStandaloneApplication *pApp = (RStandaloneApplication*) AfxGetApp();
	ASSERT_VALID( pApp );

	CString szFileName;
	DWORD dwResult = ParseDDECommand( hsz1, hsz2, hdata, szFileName );

	// is this a DDE command coming from the Windows shell (via our registry entry)
	if( HIWORD( dwResult ) == RStandaloneApplication::DDE_WindowsShellCommand )
	{
		// NOTE (DCS) - If the logic below is changed it should also be changed 
		// in HandleShellCommand

		// yes, this is from the registry, what type of command is it?
		switch( LOWORD( dwResult ) )
		{
		case RStandaloneApplication::DDE_Open:
			pApp->OpenFile( szFileName );
		break;

		case RStandaloneApplication::DDE_New:
			pApp->DoFileNew( );
		break;

		case RStandaloneApplication::DDE_Print:
			{
				RStandaloneDocument* pDocument = pApp->OpenFile( szFileName );

				if( pDocument )
					pDocument->Print( FALSE );
			}
		break;

		default:
			TpsAssertAlways( "Unknown System DDE Command" );
		}
	}
	else if( HIWORD( dwResult ) == RStandaloneApplication::DDE_InternalCommand )
	{
		// no, this is one of our internal DDE commands, sent from one instance
		// of the application to another.
		switch( LOWORD( dwResult ) )
		{
		case RStandaloneApplication::DDE_SecondInstanceQuery:
			{
				// bring the mainframe to the front.
				CWnd *pMainWnd = pApp->GetMainWnd();

				if ( pMainWnd != NULL )
				{
					ASSERT_VALID( pMainWnd );

					// restore if minimized.
					if ( pMainWnd->IsIconic() != FALSE )
						pMainWnd->ShowWindow( SW_SHOWNORMAL );

					pMainWnd->SetForegroundWindow();
				}

				// allow this instance of PSD to handle the command line which 
				// the first instance was launched with
				if ( szFileName.IsEmpty() == FALSE )
				{
					RCommandLineInfo commandLineInfo( pApp );
					::DDEParseCommandLine( szFileName, commandLineInfo );
					pApp->HandleShellCommand( commandLineInfo, TRUE );
				}
			}
			break;

		default:
			TpsAssertAlways( "Unknown Internal DDE Command" );
		}
	}
	else
		TpsAssertAlways( "Unknown DDE Command" );

	return (HDDEDATA)  DDE_FACK;
}

// unreferenced parameters
#pragma warning( default : 4100 )

// ****************************************************************************
//
//  Function Name:	ParseDDECommand( 
//							HSZ hszCommandType,		- whether from the Windows Shell or internal
//							HSZ hszSubType,			- type of command
//							HDDEDATA hData,			- command specific data
//							CString& szFileName )	- return value (the filename)
//
//  Description:		
//							Parses DDE commands from the Windows Shell and our
//							own internal DDE commands sent between instances
//							of the application.
//
//  Returns:			DWORD
//							HIWORD contains the System command identifier
//							LOWORD contains the command type identifier
//							szFileName contains command specific data
//							
//  Exceptions:		None
//
// ****************************************************************************
//
DWORD RStandaloneApplication::ParseDDECommand( HSZ hszCommandType, HSZ, HDDEDATA hData, CString& szFileName )
{
	WORD	wSystem = 0;
	WORD	wCommand = 0;

	try
	{
		RStandaloneApplication *pApp = (RStandaloneApplication*) AfxGetApp();
		ASSERT_VALID( pApp );
		DWORD dwInst = pApp->GetDDEServerInstance();
		TpsAssert( dwInst != 0, "How can we be here if the DDE server has not been registered??" );

		CString szCommand;
		CString szCommandLine;

		// clear the return value filename
		szFileName.Empty();
		
		// request length of command type string
		DWORD dwResult = ::DdeQueryString( dwInst, hszCommandType, NULL, 0, CP_WINANSI );

		if ( dwResult > 0 )
		{
			// grab a buffer from the CString
			LPTSTR lpCommand = szCommand.GetBuffer( dwResult );

			// copy hszCommandType into CString buffer
			dwResult = ::DdeQueryString( dwInst, hszCommandType, lpCommand, dwResult+1, CP_WINANSI );
			szCommand.ReleaseBuffer();
		}

		// is it a Windows System (thru registry entries) request or a PSD client request?
		if ( szCommand.CompareNoCase( _T("System") ) == 0 )
		{
			// yes, this is a System DDE command
			wSystem = (WORD) RStandaloneApplication::DDE_WindowsShellCommand;

			// how much data is there?
			const int nMaxLen = ::DdeGetData( hData, (LPBYTE) NULL,	0, 0 );

			// alloc. a CString and get a buffer
			CString szData;
			LPSTR lpData = szData.GetBuffer( nMaxLen );

			// copy data into CString
			DWORD dwRet = ::DdeGetData( hData, (LPBYTE) lpData,	nMaxLen, 0 );
			szData.ReleaseBuffer();

			if ( dwRet > 0 )
			{
				// extract what is between the parens and store in szFileName
				const int nLowPos = szData.Find( '(' );
				const int nHighPos = szData.Find( ')' );
				TpsAssert( nLowPos < nHighPos, "Malformed DDE commands in registry." );

				if ( nLowPos < nHighPos )
					szFileName = szData.Mid( nLowPos+1, nHighPos - nLowPos - 1 );

				// evaluate the type of command
				CString szCommandType;
				szCommandType = szData.Left( nLowPos );

				if ( szCommandType.CompareNoCase( _T("Open") ) == 0 )
					wCommand = (WORD) RStandaloneApplication::DDE_Open;
				else if( szCommandType.CompareNoCase( _T("New") ) == 0 )
					wCommand = (WORD) RStandaloneApplication::DDE_New;
				else if( szCommandType.CompareNoCase( _T("Print") ) == 0 )
					wCommand = (WORD) RStandaloneApplication::DDE_Print;
				else
					TpsAssertAlways( "Unknown DDE command!" );

			}
		}
		else if ( szCommand.CompareNoCase( _T("SetForegroundWindow") ) == 0 )
		{
			// no, this is an internal DDE command
			wSystem = (WORD) RStandaloneApplication::DDE_InternalCommand;
			wCommand = (WORD) RStandaloneApplication::DDE_SecondInstanceQuery;

			// the second string can (optionally) contain the command line
			// which was used to launch the second instance.
			// request length of string
			const int nMaxLen = ::DdeGetData( hData, (LPBYTE) NULL, 0, 0 );

			// alloc. a CString and get a buffer
			LPSTR lpData = szFileName.GetBuffer( nMaxLen );

			// copy data into CString
			DWORD dwRet = ::DdeGetData( hData, (LPBYTE) lpData,	nMaxLen, 0 );
			szFileName.ReleaseBuffer();
		}
		else
			TpsAssertAlways( "Unknown DDE command!" );
	}
	catch( ... )
	{
		::ReportException( kMemory );
	}

	return MAKELONG( wCommand, wSystem );
}


// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::InitInstance( )
//
//  Description:		Initializes the application instance. Called by MFC in
//							WinMain, just before CWinApp::Run( ).
//
//							We just delegate to our Initialize routine
//
//  Returns:			TRUE:	Success
//							FALSE:	Failure
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneApplication::InitInstance( )
	{
	try
		{
#ifdef	TPSDEBUG
		//	Disable GDI batching so we can see what we are doing.
		GdiSetBatchLimit( 1 );
#endif	//	TPSDEBUG

		// Install the Renaissance new handler.
		InstallNewHandler( );

		// only allow one instance to run.
		if ( InitialiseDDEServers() == FALSE)
			throw FALSE;

		// Handle the command line (must be done before splash screen)
		RCommandLineInfo commandLineInfo( this );
		ParseCommandLine( commandLineInfo );

		// Are we printing?
		m_fLaunchedForPrinting = commandLineInfo.m_nShellCommand == CCommandLineInfo::FilePrint;
		BOOLEAN	fEnableSplash	= !m_fLaunchedForPrinting;
		if ( fEnableSplash )
			fEnableSplash = (commandLineInfo.m_nShellCommand != CCommandLineInfo::FileNew) || !commandLineInfo.m_pCommandData;


		// Call our cross platform initialization method
		if( !Initialize( m_pApplicationGlobals ) )
			throw FALSE;

		// Handle the shell command (ie. New, Open, or Print)
		if( !HandleShellCommand( commandLineInfo ) )
			throw FALSE;

		PositionAndShowMainWindow( );
		}

	catch( ... )
		{
		return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::ExitInstance( )
//										  
//  Description:		Called by MFC to cleanup the application instance before
//							termination.
//
//							We just delegate to our Shutdown routine
//
//  Returns:			Exit code
//
//  Exceptions:		None
//
// ****************************************************************************
//
int RStandaloneApplication::ExitInstance( )
	{
	// We have run into problems with MFC not being able to flush the clipboard the
	// first time it tries because Filemaker Pro seems to be stealing it. Try again.
	COleDataSource::FlushClipboard( );

	// Call our cross platform shutdown method
	Shutdown( );

	// Destroy the application menu
	::DestroyMenu( m_hApplicationMenu );

	// Remove the Renaissance new handler
	RemoveNewHandler( );

	// free up DDE Server
	ShutdownDDEServer();

	// Call the base method
	return CWinApp::ExitInstance( );
	}


// ****************************************************************************
//
//  Function Name:	DoMessageBox( LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt )
//
//  Description:		MFC Overide
//							All calls to AfxMessageBox get routed through here. Ensure
//							that the dialog title is correct.
//
//  Returns:			see documentation
//
//  Exceptions:		None
//
// ****************************************************************************
//
int RStandaloneApplication::DoMessageBox( LPCTSTR lpszPrompt, UINT nType, UINT )
{
	try
	{
		CString szAppName;

		// REVIEW STA VC5 - Our conversion operator stopped working in VC5, and I dont know why. I have to call it explicitly.
		szAppName = GetResourceManager().GetResourceString( STRING_APPLICATION_NAME ).operator CString( );

		// Get the active window
		HWND hwnd = ::GetActiveWindow( );

		// if we don't have the active window use NULL and MB_TASKMODAL..
		if( hwnd == NULL )
			nType |= MB_TASKMODAL;

		return ::MessageBox( hwnd, lpszPrompt, szAppName, nType );
	}
	catch( ... )
	{
	}

	// do not display an error dialog here, as we may be trying to display
	// an out of memory error already...
	return IDCANCEL;	
}



// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::PreTranslateMessage(MSG* pMsg )
//
//  Description:		MFC Overide
//
//  Returns:			see documentation
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneApplication::PreTranslateMessage(MSG* pMsg )
{
	return CWinApp::PreTranslateMessage( pMsg );
}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OpenDocumentFile( )
//
//  Description:		The framework calls this member function to open the named
//							CDocument file for the application.
//
//							Overridden MFC function. Just delegate to our doc creation
//							code. This is necessary to make the MRU list work
//
//  Returns:			A pointer to the newly created document
//
//  Exceptions:		None 
//
// ****************************************************************************
//
CDocument* RStandaloneApplication::OpenDocumentFile( LPCTSTR lpszFileName )
	{
	return OpenFile( RMBCString( lpszFileName ) );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnIdle( long lCount )
//										  
//  Description:		Called by MFC when there are no events pending.
//
//  Returns:			TRUE:	we need more processing time.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneApplication::OnIdle( long lCount )
	{
   BOOL bMore = CWinApp::OnIdle(lCount);

	RIdleInfo* pIdleInfo = ::GetIdleInfo();

	//	If we need to warn the user of a low memory condition and have'nt reported a 
	//	memory exception recently then do so
	if ( pIdleInfo->m_fMemoryLowWarningNeeded 
		&& (kMemoryTimeoutMillSecs < (::RenaissanceGetTickCount() - pIdleInfo->m_yLastMemExceptionTime)) )
		{
		RAlert( ).AlertUser( STRING_ERROR_MEMORY );
		pIdleInfo->m_yLastMemExceptionTime = ::RenaissanceGetTickCount();
		pIdleInfo->m_fMemoryLowWarningNeeded = FALSE;
		}

	// Do the same for resources.
	if ( pIdleInfo->m_fResourceLowWarningNeeded 
		&& (kMemoryTimeoutMillSecs < (::RenaissanceGetTickCount() - pIdleInfo->m_yLastResourceExceptionTime)) )
		{
		RAlert( ).AlertUser( STRING_ERROR_RESOURCE );
		pIdleInfo->m_yLastResourceExceptionTime = ::RenaissanceGetTickCount();
		pIdleInfo->m_fResourceLowWarningNeeded = FALSE;
		}

	// Give the mainframe idle time
	RMainframe* pMainframe = static_cast<RMainframe*>( ::AfxGetMainWnd( ) );
	pMainframe->OnIdle( );

	return bMore;
	}

#endif	//	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnXPrinterChanged( const RMBCString& rPrinterChangeInfo )
//
//  Description:		Called when the given printer's charecteristics changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnXPrinterChanged( const RMBCString& /*rPrinterChangeInfo*/ )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnXPreferencesChanged( const RMBCString& rPreferencesChangedInfo )
//
//  Description:		Called when the preferences (for example win.ini) changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnXPreferencesChanged( const RMBCString& /*rPreferencesChangedInfo*/ )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::GetDocument( )
//
//  Description:		Determines if the document associated with the specified
//							file is open.
//
//  Returns:			A pointer to the document if it is open, NULL if it is not
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RStandaloneApplication::GetDocument( const RMBCString& filename ) const
	{
	// Iterate the document list looking for a document with this filename
	YDocumentIterator iterator = m_DocumentCollection.Start( );
	for( ; iterator != m_DocumentCollection.End( ); ++iterator )
		if( ( *iterator )->GetDocumentFilename( ).iCompare( filename ) == 0 )
			return *iterator;

	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::CreateNewDocument( )
//
//  Description:		Called to create a new document
//
//  Returns:			A pointer to the newly created document
//
//  Exceptions:		Memory 
//
// ****************************************************************************
//
RStandaloneDocument* RStandaloneApplication::CreateNewDocument( YDocumentType documentType, BOOLEAN fMakeVisible, const RMBCString& filename )
	{
	// Get the document template
	CDocTemplate* pDocTemplate = NULL;
	POSITION pos = GetFirstDocTemplatePosition( );

	YDocumentType i = documentType;
	while( i + 1 )
		{
		TpsAssert( pos, "Doc template not found." );
		pDocTemplate = GetNextDocTemplate( pos );
		--i;
		}

	TpsAssert( pDocTemplate, "NULL doc template." );

	// Get the filename to use. If it is NULL, we will generate the default title
	const char* pFilename = filename.IsEmpty( ) ? NULL : (const char*)filename;

	// Create the document
	RStandaloneDocument* pDocument = dynamic_cast<RStandaloneDocument*>( pDocTemplate->OpenDocumentFile( pFilename, fMakeVisible ) );

	// Add it to our document collection
	m_DocumentCollection.InsertAtEnd( pDocument );

	// Initialize the document
	pDocument->Initialize( this, documentType );

	// Initialize the views
	YViewIterator iterator = pDocument->GetViewCollectionStart( );
	for( ; iterator != pDocument->GetViewCollectionEnd( ); ++iterator )
		( *iterator )->Initialize( pDocument );

	return pDocument;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::CloseDocument( )
//
//  Description:		Called to close a document and remove it from out list
//
//  Returns:			Nothing
//
//  Exceptions:		None 
//
// ****************************************************************************
//
void RStandaloneApplication::CloseDocument( RStandaloneDocument* pDocument )
	{
	// Remove the document from the document list
	TpsAssert( m_DocumentCollection.Find( pDocument ) != m_DocumentCollection.End( ), "Document not found." );
	m_DocumentCollection.Remove( pDocument );

#ifdef _WINDOWS

	// Remember if the active view on the closed document was maximized
	WINDOWPLACEMENT windowPlacement;
	windowPlacement.length = sizeof( windowPlacement );
	if( pDocument->GetFirstViewPosition( ) )
		{
		pDocument->GetActiveView( )->GetCWnd( ).GetParentFrame( )->GetWindowPlacement( &windowPlacement );
		m_fLastClosedWindowWasMaximized = ( windowPlacement.showCmd == SW_SHOWMAXIMIZED );
		}

#endif
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::WasLastClosedWindowMaximized( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if the last window closed was maximized or if no
//							windows have ever been closed.
//
//  Exceptions:		None 
//
// ****************************************************************************
//
BOOLEAN RStandaloneApplication::WasLastClosedWindowMaximized( ) const
	{
	return m_fLastClosedWindowWasMaximized;
	}


// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnFileOpen( )
//
//  Description:		Called when open is chosen from the file menu
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnFileOpen( )
	{
	// Get the file formats supported for loading by this application
	RFileFormatCollection fileFormatCollection;
	GetLoadFileFormats( fileFormatCollection );

	// Display the file open dialog and get the name of the selected file
	RMBCString filename;
	DoFileOpenDialog( fileFormatCollection, filename );

	// Open the file
	OpenFile( filename );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OpenFile( )
//
//  Description:		Opens the specified file
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
RStandaloneDocument* RStandaloneApplication::OpenFile( const RMBCString& filename ) const
	{
	RStandaloneDocument* pDocument = NULL;

	try
		{
		if( !filename.IsEmpty( ) )
			{
			// If it is already open, active it
			pDocument = GetDocument( filename );
			if( pDocument )
				pDocument->Activate( );

			// Otherwise, open it
			else
				{
				// Get the file formats supported for loading by this application
				RFileFormatCollection fileFormatCollection;
				GetLoadFileFormats( fileFormatCollection );

				// Get the file format which matches this file
				RFileFormat* pFileFormat = fileFormatCollection.GetMatchingFileFormat( filename );

				// If we found a file format that matches the file, load the file
				if( pFileFormat )
					pDocument = pFileFormat->Load( filename );

				// Otherwise display an error saying it is an invalid file
				else
					{
					RAlert alert;
					RMBCString string = alert.FormatAlertString( STRING_ERROR_INVALID_FILE, filename );
					alert.AlertUser( string );
					}
				}
			}
		}

	catch( ... )
		{
		// Clean up
		delete pDocument;
		pDocument = NULL;

		// Tell the user we could not open their document
		RAlert alert;
		RMBCString string = alert.FormatAlertString( STRING_FILE_OPEN_FAILED_ERROR, filename );
		alert.AlertUser( string );
		}

	return pDocument;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::PrintFile( )
//
//  Description:		Print the specified file
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RStandaloneApplication::PrintFile( const RMBCString& filename, BOOLEAN fFromDDE ) const
	{
	try
		{
		// Create a disk storage
		RChunkStorage storage( filename, kRead );

		// Load the document type
		storage.Start( kDocumentTypeTag );
		YDocumentType documentType;
		storage >> documentType;
		storage.SelectParentChunk( );

		// Create a new uninitialized document of the correct type
		RStandaloneDocument* pDocument = CreateUninitializedDocument( documentType );

		// Read it
		pDocument->Read( storage, kLoading );

		// Make sure we can print
		if( pDocument->CanPrint( ) )
			{
			// Set the title
			pDocument->SetTitle( filename );

			// Determine if we are background printing or not
			RRenaissanceUserPreferences preferences;
			uLONG uValue;
			if( !preferences.GetLongValue( RRenaissanceUserPreferences::kBackgroundPrinting, uValue ) )
				uValue = 0;

			// If we are background printing and the request came from DDE, submit the print job
			if( fFromDDE && uValue )
				m_pPrintQueue->SubmitJob( pDocument );

			// Otherwise, just print immediatly
			else
				{
				pDocument->Print( NULL );
				pDocument->FreeDocumentContents( );
				delete pDocument;
				}
			}

		// We couldnt print, just delete and leave
		else
			{
			pDocument->FreeDocumentContents( );
			delete pDocument;
			}
		}
	catch( ... )
		{
		;
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::DoFileOpenDialog( )
//
//  Description:		Displays the file open dialog.
//
//  Returns:			The name of the chosen file. The string is empty if
//							the cancel button was chosen.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::DoFileOpenDialog( RFileFormatCollection& fileFormatCollection, RMBCString& filename ) const
	{
#ifdef _WINDOWS
	// Get the load file types
	RFileTypeCollection fileTypeCollection;
	fileFormatCollection.GetLoadFileTypes( fileTypeCollection );
	RMBCString filterString;
	fileTypeCollection.GetFileDialogFilterString( filterString );

	// Popup the open file dialog
	CFileDialog fileDialog( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT, filterString, NULL );
	if( fileDialog.DoModal( ) == IDOK )
		filename = fileDialog.GetPathName( );
	else
		filename.Empty( );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::StartRecordingScript( )
//
//  Description:		Start recording a script.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::StartRecordingScript( )
	{
	TpsAssert( !m_fPlayback && !GetRecordingScript(), "Start Script called during Record or Playback" );

#ifdef	_WINDOWS
	const char* kFilters = "Renaissance Script Files|*.scr|All Files|*.*||";
	CFileDialog	fileDialog( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, kFilters, NULL );
	if ( fileDialog.DoModal( ) == IDOK )
		{
		///	Open file and set m_pScript
		GetRecordingScript( )	= new RStreamScript( fileDialog.GetPathName( ), kReadWrite );
		}
#endif
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::StopRecordingScript( )
//
//  Description:		Stop recording a script.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::StopRecordingScript( )
	{
	TpsAssert( !m_fPlayback && GetRecordingScript( ), "Stop Script called during Record or Playback" );

	//	Close and delete the ScriptStream
	delete GetRecordingScript( );
	GetRecordingScript( )		= NULL;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::ChooseScript( )
//
//  Description:		Pick a script to play.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::ChooseScript( )
	{
	TpsAssert( !m_fPlayback && !GetRecordingScript( ), "Either Currently recording, or Currently in playback - Cannot choose a script" );

	if ( m_fPlayback || GetRecordingScript( ) )
		return;

#ifdef	_WINDOWS
	const char* kFilters = "Renaissance Script Files|*.scr|All Files|*.*||";
	CFileDialog	fileDialog( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, kFilters, NULL );
	if ( fileDialog.DoModal( ) == IDOK )
		{
		RStreamScript	script( fileDialog.GetPathName( ), kRead );
		PlaybackScript( script );
		}
#endif
	}


// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::PlaybackScript( )
//
//  Description:		Playback the given script.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::PlaybackScript( RScript& script )
	{
	RAction*		pAction;
	YActionId	actionId;

	TpsAssert( !m_fPlayback && !GetRecordingScript( ), "Either Currently recording, or Currently in playback - Cannot choose a script" );

	if ( m_fPlayback || GetRecordingScript( ) )
		return;

	m_fPlayback = TRUE;		//	I am starting to playback a script

	//	Get the first action and go until end of script is reached...
	//	Loop reading name & getting actions until an kEndOfFile is returned for an error.
	while ( !script.AtEnd( ))
		{
		script	>> actionId;	//	Retrieve the actionId from the script

		//	Get the current view, and create the action
		try
			{
			pAction = ::GetActiveView( )->CreateScriptAction( actionId, script );
			}
		catch( YException /* exception */ )
			{
			pAction = NULL;
			}

		if (pAction != NULL)
			::GetActiveView( )->GetRDocument( )->SendAction( pAction );
		else
			{
			//	Alert user that script failed.
			break;
			}
		}

	m_fPlayback = FALSE;	//	I am done the current playback
	}

// ****************************************************************************
//																												 
//  Function Name:	RStandaloneApplication::GetApplicationPath( )
//
//  Description:		Gets the full path that this application was launched from,
//							without the application name.
//
//  Returns:			The full path
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString RStandaloneApplication::GetApplicationPath( ) const
	{
#ifdef _WINDOWS

	// Get the module file name
	_TCHAR moduleFileName[ _MAX_PATH ];
	::GetModuleFileName( NULL, moduleFileName, _MAX_PATH );

	// Look for the first directory seperator
	const _TCHAR kDirectorySeperator = '\\';
	_TCHAR* p = strrchr( moduleFileName, kDirectorySeperator );
	TpsAssert( p, "No directory seperator found" );

	// Copy the module file name to our path string
	RMBCString path( moduleFileName, p - moduleFileName + 1 );

	return path;

#endif	// _WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::GetApplicationMenu( )
//
//  Description:		Accessor
//
//  Returns:			The app menu
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMenu& RStandaloneApplication::GetApplicationMenu( ) const
	{
	return *m_pApplicationMenu;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::SupportedFormatOnClipboard( )
//
//  Description:		Determines if a supported data format is available on the
//							clipboard
//
//  Returns:			TRUE if a supported format is available
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RStandaloneApplication::SupportedFormatOnClipboard( ) const
	{
	BOOLEAN fSupportedFormatFound = FALSE;

	// Create a clipboard transfer, and open it for pasting.
	try
		{
		RClipboardDataSource clipboardTransfer;

		// Enumerate the formats availble, looking for one which we support
		clipboardTransfer.BeginEnumFormats( );

		YDataFormat dataFormat;

		while( fSupportedFormatFound == FALSE && clipboardTransfer.GetNextFormat( dataFormat ) == TRUE )
			{										  
			// Can always handle component format
			if( dataFormat == kComponentFormat )
				fSupportedFormatFound = TRUE; 

			// See if there is a mapping between this data format and an available component
			else
				{
				YComponentType componentType;
				fSupportedFormatFound = ::GetComponentManager( ).MapDataFormatToComponentType( dataFormat, componentType );
				}
			}
		}

	catch( ... )
		{
		;
		}

	return fSupportedFormatFound;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::PrintDocument( )
//
//  Description:		Prints the specified document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::PrintDocument( RStandaloneDocument* pDocument )
	{
	// Create a memory storage
	RBufferStream stream;
	RStorage	parentStorage( &stream );
	RChunkStorage storage( &parentStorage );

	// Save the document to this storage
	pDocument->Write( storage, kPrintingWrite );	

	// Create a new document to print from
	RStandaloneDocument* pPrintDocument = pDocument->CreateCopy( );

	// Read the new document from the memory storage
	storage.SeekAbsolute( 0 );
	pPrintDocument->Read( storage, kPrintingRead );

	// Set the title
	pPrintDocument->SetTitle( pDocument->GetTitle( ) );

	// Determine if we are background printing or not
	RRenaissanceUserPreferences preferences;
	uLONG uValue;
	if( !preferences.GetLongValue( RRenaissanceUserPreferences::kBackgroundPrinting, uValue ) )
		uValue = 0;

	// If we are background printing, submit the print job
	if( uValue )
		m_pPrintQueue->SubmitJob( pPrintDocument );

	// Otherwise, just print immediatly
	else
		{
		pPrintDocument->Print( NULL );
		pPrintDocument->FreeDocumentContents( );
		delete pPrintDocument;
		}
	}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::SaveAllModified( )
//
//  Description:		Called by the framework to save all documents when the
//							application’s main frame window is to be closed
//
//  Returns:			Nonzero if safe to terminate the application;
//							0 if not safe to terminate the application.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneApplication::SaveAllModified( )
	{
	// If there are jobs remaining to be printed, ask the user if they want to quit
	if( !m_pPrintQueue->CanShutdown( ) )
		{
		if( RAlert( ).QueryUser( STRING_ALERT_PRINT_JOBS_REMAINING ) != kAlertYes )
			return FALSE;
		}

	// We got by the printing check, now call the base method
	BOOL fReturn = CWinApp::SaveAllModified( );

	// If we are really going to terminate, make sure the print queue deletes its documents
	// before they are deleted by MFC
	if( fReturn )
		m_pPrintQueue->DeleteAllJobs( );

	return fReturn;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::IsIdleMessage( )
//
//  Description:		The default MFC idle processing idles way too much; after
//							every mouse message. This disables idling for mouse and
//							timer messages
//
//  Returns:			Nonzero if OnIdle should be called after processing message
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneApplication::IsIdleMessage( MSG* pMsg )
	{
	// Call the base method
	if( !CWinApp::IsIdleMessage( pMsg ) )
		return FALSE;

	UINT message = pMsg->message;

	//	Will process every mouse up/down message
	// NOTE (DCS - 3/20/97) Not calling OnIdle after a WM_NCLBUTTDOWN message does not allow the
	// framework to hide undocked toolbars when the close (X) button is pressed. Also note that
	// WM_NCLBUTTONDOWN is included in the range WM_NCMOUSEMOVE to WM_NCMBUTTONDBLCLK.
	if ( (message == WM_LBUTTONUP) || (message == WM_LBUTTONDOWN) || (message == WM_NCLBUTTONDOWN))
		return TRUE;

	// Disable idling on other mouse messages
	if( ( message >= WM_MOUSEFIRST && message <= WM_MOUSELAST ) ||
		 ( message >= WM_NCMOUSEMOVE && message <= WM_NCMBUTTONDBLCLK ) )
			return FALSE;

	// Disable idling on timer messages
	if( message == WM_TIMER )
	  return FALSE;
	
	// We can idle on this message
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::HandleShellCommand( )
//
//  Description:		Our version of MFCs ProcessShellCommand
//
//  Returns:			FALSE if the application should terminate
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
BOOLEAN RStandaloneApplication::HandleShellCommand( RCommandLineInfo& commandLineInfo, BOOLEAN fFromDDE )
	{
	BOOLEAN fResult = TRUE;

	// NOTE (DCS) - If the logic below is changed it should also be changed 
	// in the DdeCallback.

	switch( commandLineInfo.m_nShellCommand )
		{
		// Create a new file
		case CCommandLineInfo::FileNew :
			DoFileNew( );
			break;

		// Open a file
		case CCommandLineInfo::FileOpen:
			OpenFile( commandLineInfo.m_strFileName );
			break;

		// Print a file
		case CCommandLineInfo::FilePrintTo:
		case CCommandLineInfo::FilePrint:
			PrintFile( commandLineInfo.m_strFileName, fFromDDE );
			//	False to not start up the application
			fResult = FALSE;
			break;
		}

	return fResult;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::ParseParam( )
//
//  Description:		Parse one of the commands from the command line
//
//  Returns:			FALSE if the command was not parsed
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
BOOLEAN	RStandaloneApplication::ParseParam( RCommandLineInfo& , const RMBCString& , BOOLEAN )
{
	return FALSE;	//	Standalone does no additional procesing
}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::CreateUninitializedDocument( )
//
//  Description:		Creates a new uninitialized document, that MFC doesnt
//							know about.
//
//  Returns:			A pointer to a new document
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RStandaloneApplication::CreateUninitializedDocument( YDocumentType ) const
	{
	UnimplementedCode( );
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::WasLaunchedForPrinting( )
//
//  Description:		Returns true if the application was launched to just
//							print a document.
//
//  Returns:			See above
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
BOOLEAN RStandaloneApplication::WasLaunchedForPrinting( ) const
	{
	return m_fLaunchedForPrinting;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::RegisterDocument( )
//
//  Description:		Registers a new document class
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RStandaloneApplication::RegisterDocument( UINT nIDResource,
															  CRuntimeClass* pDocClass,
															  CRuntimeClass* pFrameClass,
															  CRuntimeClass* pViewClass )
	{
	// Create and add a new doc template
	AddDocTemplate( new CMultiDocTemplate( nIDResource, pDocClass, pFrameClass, pViewClass ) );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::GetMainframe( ) const
//
//  Description:		Return our mainframe.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMainframe* RStandaloneApplication::GetMainframe( ) const
	{
#ifdef _WINDOWS
	TpsAssertIsObject( RMainframe, ::AfxGetMainWnd() );
	return static_cast< RMainframe* > ( ::AfxGetMainWnd() );
#endif
	}

#endif	// _WINDOWS

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::DoFileNew( )
//
//  Description:		Does whatever is necessary to create a new document.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::DoFileNew( )
	{
	// Default is just to create a new document
	CreateNewDocument( 0 );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnNew( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnNew( )
	{
	DoFileNew( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnQuit( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnQuit( )
	{
	WinCode( OnAppExit( ) );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnOpen( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnOpen( )
	{
	OnFileOpen( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnScriptRecord( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnScriptRecord( )
	{
	if (GetRecordingScript( ))
		StopRecordingScript( );
	else
		StartRecordingScript( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnUpdateScriptRecord( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnUpdateScriptRecord( RCommandUI& commandUI ) const
	{
	if( m_fPlayback )
		commandUI.Disable( );	//	must be in playback
	else
		commandUI.Enable( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnUpdateScriptPlayback( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnUpdateScriptPlayback( RCommandUI& commandUI ) const
	{
	if ( GetRecordingScript( ) )
		commandUI.Disable( );
	else
		commandUI.Enable( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnScriptPlayback( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnScriptPlayback( )
	{
	ChooseScript( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnFileFailDialog( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnFileFailDialog( )
{
	static REditFileFailure FileFailDlg;
	FileFailureStruct FileFailData;
	int FileSuccessCount = GetFileNumOperationsTillFailCount();

	if ( FileSuccessCount == -1 )
	{
		FileFailData.eFail = kNeverFail;
		FileFailData.uwNumAllocates = 0;
	}
	else
	{
		FileFailData.eFail = kFailAfter;
		FileFailData.uwNumAllocates = static_cast<uWORD>(FileSuccessCount);
	}
	FileFailDlg.SetData( &FileFailData );
	if ( FileFailDlg.DoModal() )
	{
		FileFailDlg.FillData( &FileFailData );
		(FileFailData.eFail == kFailAfter) ? (GetFileNumOperationsTillFailCount() = FileFailData.uwNumAllocates) : (GetFileNumOperationsTillFailCount() = -1);
		GetFileNumTimesToFailCount() = FileFailData.uwNumTimesToFail;
	}
}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnFileSucceed( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnFileSucceed( )
	{
	GetFileNumOperationsTillFailCount( ) = -1;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnFileFail( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnFileFail( YCommandID commandId )
	{
	int numOperationsTillFail = 25;

	switch( commandId )
		{
		case COMMAND_MENU_DEBUG_FILE_FAIL1 :
			numOperationsTillFail = 25;
			break;

		case COMMAND_MENU_DEBUG_FILE_FAIL2 :
			numOperationsTillFail = 50;
			break;

		case COMMAND_MENU_DEBUG_FILE_FAIL3 :
			numOperationsTillFail = 100;
			break;

		case COMMAND_MENU_DEBUG_FILE_FAIL4 :
			numOperationsTillFail = tintRandom( 75 ) + 25;
		}

	GetFileNumOperationsTillFailCount( ) = numOperationsTillFail;
	GetFileNumTimesToFailCount( ) = 1;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnMemoryFailDialog( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnMemoryFailDialog( )
{
	static REditMemFailure MemFailDlg;
	MemoryFailureStruct MemFailData;
	int MemSuccessCount	= GetMemSuccessCount();
	int MemFailureCount	= GetMemFailureCount();

	if ( MemSuccessCount == -1 )
	{
		MemFailData.eFail = kNeverFail;
		MemFailData.uwNumAllocates = 0;
		MemFailData.uwNumFailures	= 0;
	}
	else
	{
		MemFailData.eFail = kFailAfter;
		MemFailData.uwNumAllocates = static_cast<uWORD>(MemSuccessCount);
		MemFailData.uwNumFailures	= static_cast<uWORD>(MemFailureCount);
	}
	MemFailDlg.SetData( &MemFailData );
	if ( MemFailDlg.DoModal() )
	{
		MemFailDlg.FillData( &MemFailData );
		(MemFailData.eFail == kFailAfter) ? (GetMemSuccessCount() = MemFailData.uwNumAllocates) : (GetMemSuccessCount() = -1);
		(MemFailData.eFail == kFailAfter) ? (GetMemFailureCount() = MemFailData.uwNumFailures) : (GetMemFailureCount() = -1);
	}
}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnMemorySucceed( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnMemorySucceed( )
	{
	GetMemSuccessCount( ) = -1;
	GetMemFailureCount( ) = -1;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnMemoryFail( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnMemoryFail( YCommandID commandId )
	{
	int memSuccessCount = 10; 
	int memFailureCount = 10;

	switch( commandId )
		{
		case COMMAND_MENU_DEBUG_MEM_FAIL1 :
			memSuccessCount = memFailureCount = 10;
			break;

		case COMMAND_MENU_DEBUG_MEM_FAIL2 :
			memSuccessCount = memFailureCount = 25;
			break;

		case COMMAND_MENU_DEBUG_MEM_FAIL3 :
			memSuccessCount = memFailureCount = 100;
			break;

		case COMMAND_MENU_DEBUG_MEM_FAIL4 :
			memSuccessCount = tintRandom( 75 ) + 25;
			memFailureCount = tintRandom( 75 ) + 25;
			break;
		}

	GetMemSuccessCount( ) = memSuccessCount;
	GetMemFailureCount( ) = memFailureCount;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::OnMemoryPurge( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::OnMemoryPurge( )
	{
	try
		{
		//	For now this is most likely more than we can allocate on any
		//	 of our machines.
		uBYTE*	pubData	= new uBYTE[ 0x7FFFFFFF ];
		delete pubData;
		}
	catch( ... )
		{
		;
		}
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::Validate( ) const
	{
	RApplication::Validate( );
	TpsAssertIsObject( RStandaloneApplication, this );
	}

#endif		//	TPSDEBUG


#ifdef	_WINDOWS
// ****************************************************************************
//
//  Function Name:	RCommandLineInfo::ParseParam( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	RMBCString	param( pszParam );
	if ( !m_pApplication || !m_pApplication->ParseParam( *this, param, (BOOLEAN)bFlag ) )
		CCommandLineInfo::ParseParam( pszParam, bFlag, bLast );
}

#endif //	_WINDOWS