// ****************************************************************************
//
//  File Name:			StandaloneApplication.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RStandaloneApplication class
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
//  $Logfile:: /PM8/Framework/Include/StandaloneApplication.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STANDALONEAPPLICATION_H_
#define		_STANDALONEAPPLICATION_H_

#include		"Application.h"

#ifdef		_WINDOWS
	#define		FrameworkApplication	CWinApp
#else
	#define		FrameworkApplication	LApplication
#endif

#ifdef		_WINDOWS
	// Windows DDE Support.
	#include "DDEML.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forward References and Typedefs
class RComponentApplication;
class RStandaloneDocument;
class RScript;
class RComponentDocument;
class RDocument;
class RComponentAttributes;
class RFileFormat;
class RFileFormatCollection;
class RMenu;
class RPrintQueue;
class RMainframe;
class RCommandLineInfo;

typedef RList<RStandaloneDocument*>					YDocumentCollection;
typedef YDocumentCollection::YIterator				YDocumentIterator;

// ****************************************************************************
//
//  Class Name:	RStandaloneApplication
//
//  Description:	The base class for standalone Renaissance applications.
//
//						The application class is responsible for creating and
//						managing documents and components.
//
// ****************************************************************************
//
class FrameworkLinkage RStandaloneApplication : public FrameworkApplication, public RApplication
	{
	// Construction, destruction & initialization
	public :
												RStandaloneApplication( YMenuIndex componentSubMenuIndex, YResourceId uwDocumentResourceID );
		virtual								~RStandaloneApplication( );
		virtual BOOLEAN					Initialize( RApplicationGlobals* pGlobals );
		virtual void						Shutdown( );

	// Operations
	public :
		virtual RAction*					CreateScriptAction( const YActionId& actionId, RScript& script );
		RStandaloneDocument*				CreateNewDocument( YDocumentType documentType, BOOLEAN fMakeVisible = TRUE, const RMBCString& filename = RMBCString( ) );
		virtual RStandaloneDocument*	CreateUninitializedDocument( YDocumentType documentType ) const;
		void									CloseDocument( RStandaloneDocument* pDocument );
		RStandaloneDocument*				GetDocument( const RMBCString& filename ) const;
		RMBCString							GetApplicationPath( ) const;
		RMenu&								GetApplicationMenu( ) const;
		RMainframe*							GetMainframe( ) const;
		virtual void						OnXPrinterChanged( const RMBCString& rPrinterChangeInfo );
		virtual void						OnXPreferencesChanged( const RMBCString& rPreferencesChangedInfo );
		virtual void						GetLoadFileFormats( RFileFormatCollection& fileFormatCollection ) const = 0;
		BOOLEAN								SupportedFormatOnClipboard( ) const;
		BOOLEAN								WasLastClosedWindowMaximized( ) const;
		BOOLEAN								WasLaunchedForPrinting( ) const;
		virtual void						CalibratePrinters(  );

	//	Public Scripting API's
	public :
		void									StartRecordingScript( );
		void									StopRecordingScript( );
		void									ChooseScript( );
		void									PlaybackScript( RScript& pScript );
		BOOLEAN								SendAction( RAction* pAction );
		void									PrintDocument( RStandaloneDocument* pDocument );

	// Implementation
	protected :
		virtual void						DoFileOpenDialog( RFileFormatCollection& fileFormatCollection, RMBCString& filename ) const;
		virtual void						DoFileNew( );
		virtual void						PositionAndShowMainWindow( );
		virtual void						PositionAndHideMainWindow( );

	// Implmentation
	private :
		void									InitializeBitmapToolbox( );
		virtual void						OnFileOpen( );
		RStandaloneDocument*				OpenFile( const RMBCString& filename ) const;
		void									PrintFile( const RMBCString& filename, BOOLEAN fFromDDE ) const;
		void									LoadMainMenu( );

	// Command handlers
	private :
		void									OnNew( );
		void									OnQuit( );
		void									OnOpen( );
		void									OnScriptRecord( );
		void									OnUpdateScriptRecord( RCommandUI& commandUI ) const;
		void									OnScriptPlayback( );
		void									OnUpdateScriptPlayback( RCommandUI& commandUI ) const;
		void									OnFileFailDialog( );
		void									OnFileSucceed( );
		void									OnFileFail( YCommandID commandId );
		void									OnMemoryFailDialog( );
		void									OnMemorySucceed( );
		void									OnMemoryFail( YCommandID commandId );
		void									OnMemoryPurge( );

	// Member data
	protected :
		YResourceId							m_uwDocumentResourceID;
		YDocumentCollection				m_DocumentCollection;

	// Member data
	private :
		RMenu*								m_pApplicationMenu;
		YMenuIndex							m_ComponentSubMenuIndex;
		RApplicationGlobals*				m_pApplicationGlobals;
		BOOLEAN								m_fLastClosedWindowWasMaximized;
		RPrintQueue*						m_pPrintQueue;
		BOOLEAN								m_fPlayback;
		BOOLEAN								m_fLaunchedForPrinting;

	// Command map
	public :
		virtual RCommandMapBase*		GetRCommandMap( ) const;

	private :
		static RCommandMap<RStandaloneApplication, RApplication> m_CommandMap;
		friend class RCommandMap<RStandaloneApplication, RApplication>;

#ifdef	_WINDOWS
	private:
		// DDE Support
		DWORD									m_dwDDEInstance;
		HMENU									m_hApplicationMenu;
		enum									{ DDE_InternalCommand = 1, DDE_WindowsShellCommand = 2 };
		enum									{ DDE_SecondInstanceQuery = 1, DDE_Open = 2, DDE_New = 3, DDE_Print = 4 };

	// Windows specific functions
	public :
		virtual BOOL						InitInstance( );
		int									ExitInstance( );
		virtual BOOL						PreTranslateMessage( MSG *pMsg );
		virtual CDocument*				OpenDocumentFile( LPCTSTR lpszFileName );
		BOOL									OnIdle( long lCount );
		virtual BOOL						IsIdleMessage( MSG* pMsg );
		virtual BOOL						SaveAllModified( );
		virtual BOOLEAN					ParseParam( RCommandLineInfo& commandInfo, const RMBCString& param, BOOLEAN fFlag );
		virtual BOOLEAN					HandleShellCommand( RCommandLineInfo& commandLineInfo, BOOLEAN fFromDDE = FALSE );
		virtual int							DoMessageBox( LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt );

		// DDE Support
		DWORD									GetDDEServerInstance( ) const;
		BOOL									AlreadyRunning( HSZ hServName );
		BOOL									InitialiseDDEServers();
		void									ShutdownDDEServer();

		static HDDEDATA CALLBACK		DdeCallback( UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, DWORD dwData1, DWORD dwData2 );
		static DWORD						ParseDDECommand( HSZ hszCommandType, HSZ hszSubType, HDDEDATA hData, CString& szFileName );

	// Windows implementation
	protected :
		void									RegisterDocument( UINT nIDResource,
																		CRuntimeClass* pDocClass,
																		CRuntimeClass* pFrameClass,
																		CRuntimeClass* pViewClass );

		BOOL									RegisterDDEServices();
		BOOL									FirstInstance();

	private:
		LPSTR									m_lpWindowClassName;

#endif	// _WINDOWS

#ifdef	MAC
	//	Document creation processing
	public :
		virtual	void						StartUp( );
		virtual	Boolean					ObeyCommand( CommandT inCommand, void *ioParam );
		virtual	void						FindCommandStatus( CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, Char16& outMark, Str255 outName );
#endif	//	MAC

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const = 0;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::GetRCommandMap( )
//
//  Description:		Gets a pointer to this classes command map.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RStandaloneApplication::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

#ifdef _WINDOWS
// ****************************************************************************
// 					Helper Classes
// ****************************************************************************
class FrameworkLinkage RCommandLineInfo : public CCommandLineInfo
{
	public :
		RCommandLineInfo( RStandaloneApplication* pApp ) 
				: m_pApplication( pApp ), m_pCommandData(NULL) { ; }

	public :
		//plain char* version on UNICODE for source-code backwards compatibility
		virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	public :
		void*							m_pCommandData;	//	Holder for application specific command line information

	private :
		RStandaloneApplication* m_pApplication;
} ;

#endif	// _WINDOWS

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _STANDALONEAPPLICATION_H_
