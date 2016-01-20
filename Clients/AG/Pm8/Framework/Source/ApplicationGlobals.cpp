// ****************************************************************************
//
//  File Name:			ApplicationGlobals.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Implementation of the Global Data class
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
//  $Logfile:: /PM8/Framework/Source/ApplicationGlobals.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ApplicationGlobals.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "StandaloneApplication.h"
#include "FrameworkResourceIDs.h"
#include "AutoDrawingSurface.h"
#include	"Application.h"
#include	"DataObject.h"
#include	"ResourceManager.h"
#include	"Menu.h"
#include	"Caret.h"
#include	"Stream.h"
#include	"IdleProcessing.h"
#include "CorrectSpell.h"
#include "IntelliFinder.h"
#include "DialogUtilities.h"
#include "Configuration.h"
#include "ComponentManager.h"
#include "DefaultDrawingSurface.h"
#include "ScratchBitmapImage.h"
#include "ColorDialogInterfaceImp.h"

#ifdef	MAC
	#include "ATMInterface.h"
#endif
#ifdef	_WINDOWS
	#include	"Atm.h"
#endif

const	int		kBrushCacheSize				= 32;
const	int		kPenCacheSize					= 32;
const	int		kFontCacheSize					= 16;

static	RIdleInfo _GlobalIdleInfo;

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::RApplicationGlobals( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RApplicationGlobals::RApplicationGlobals( RStandaloneApplication* pAppBase )
	: m_pApplication( pAppBase ),
	  m_pAutoDrawingSurface( NULL ),
	  m_pActiveView( NULL ),
	  m_pOpenStreamList( NULL ),
	  m_fUseOffscreens( TRUE ),
	  m_fDebugMode( FALSE ),
	  m_fTrueTypeAvailable( FALSE ),
	  m_fATMAvailable( FALSE ),
	  m_pCorrectSpell( NULL ),
	  m_pIntelliFinder( NULL ),
	  m_nMaxGradientDPI( YIntDimension(kDefaultMaxGradientDPI) ),
	  m_pctGradientPrintPrecision( YPercentage(kDefaultGradientPrintPrecision/100.0) ),
	  m_pctGradientDisplayPrecision( YPercentage(kDefaultGradientDisplayPrecision/100.0) ),
#ifdef	_WINDOWS
	  m_BrushCache(kBrushCacheSize),
	  m_PenCache(kPenCacheSize),
	  m_FontCache(kFontCacheSize),
	  m_pBitmapPool( NULL ),
	  m_rFontListString(""),
	  m_ulNumberOfFonts( 0 ),
	  m_pPrintDialog( NULL ),
	  m_fInDragDrop( FALSE ),
	  m_pScript( NULL ),
#ifdef	USE_SMARTHEAP
	  m_mpSmartheapHandlePool( NULL ),
	  m_slMemSuccessCount( -1 ),
	  m_slMemFailureCount( -1 ),
#endif	//	USE_SMARTHEAP
#endif	//	_WINDOWS
	  m_pIdleInfo( &_GlobalIdleInfo ),
	  m_fUseBitmapTrackingFeedback( TRUE )
	{
	try
		{
		m_pAutoDrawingSurface = new RAutoDrawingSurface;
		m_pAppFontList = new RFontList;
		m_pCaret = new RCaret;
		m_pCursorManager = new RCursor;
		m_pResourceManager = new RResourceManager;
		m_pCorrectSpell = new RCorrectSpell;
		m_pIntelliFinder = new RIntelliFinder;
		m_pComponentManager = new RComponentManager;

		CreateColorDialog = RColorDialogInterfaceImp::CreateInstance;

		StartupGlobalBuffer( );

		//	make an empty open streams list
		m_pOpenStreamList = new ROpenStreamList;

#ifdef	_WINDOWS
		m_fTrueTypeAvailable = TRUE;
		m_fATMAvailable		= static_cast<BOOLEAN>( ATMProperlyLoaded( ) );
#endif //_WINDOWS
	BuildFontList();
		//	Initialize Handle Memory Manager
		if (!tmemStartup( 0 ))
			throw kMemory;

		//
		// load application-wide preferences
		uLONG	uwTmp;
		RGradientConfiguration rConfig;
		if( rConfig.GetLongValue(RGradientConfiguration::kMaxDpi, uwTmp)
				&& uwTmp>=kMinMaxGradientDPI
				&& uwTmp<=kMaxMaxGradientDPI )
			m_nMaxGradientDPI	= YIntDimension(uwTmp);
		if( rConfig.GetLongValue(RGradientConfiguration::kPrintPrecision, uwTmp)
				&& uwTmp>=kMinGradientPrintPrecision
				&& uwTmp<=kMaxGradientPrintPrecision )
			m_pctGradientPrintPrecision = YPercentage(uwTmp/100.0);
		if( rConfig.GetLongValue(RGradientConfiguration::kDisplayPrecision, uwTmp)
				&& uwTmp>=kMinGradientDisplayPrecision
				&& uwTmp<=kMaxGradientDisplayPrecision )
			m_pctGradientDisplayPrecision	= YPercentage(uwTmp/100.0);
		}
	catch( YException /* exception */ )
		{
		ShutdownGlobalBuffer( );

		//	No need to call tmemShutdown since it was the last thing executed
		// and if it failed, it does not need cleanup
		//	REVIEW BDR 5/2/96 - no top level handler for this throw - need alert HERE!
		throw;
		}

	// Check the preferences to see if we should use bitmap tracking feedback
	static const RMBCString kUseBitmapTrackingFeedbackKey = "UseBitmapTrackingFeedback";

	uLONG uValue;
	RPowerUserConfiguration config;
	if( config.GetLongValue( kUseBitmapTrackingFeedbackKey, uValue) )
		m_fUseBitmapTrackingFeedback = uValue;

	//	Record our language version from the preferences
	RRenaissanceLanguagePreferences printShopLanguagePreferences;
	
	//	If no value is recorded in the registry then assume english language.
	RMBCString rValue;
	if( !printShopLanguagePreferences.GetStringValue( RRenaissanceLanguagePreferences::kPaperType, rValue ) )
		m_eLanguageVersion = kEnglishLanguage;
	else
		{
		//	The language value is stored as a string because that's all the current installer can put in
		//	the registry.
		uValue = atoi( rValue );
		m_eLanguageVersion = static_cast<ELanguageVersions>(uValue);
		}

#ifdef	TPSDEBUG
	m_fDebugMode = TRUE;	//	If debugging, we are in debug mode....
#else
	uLONG ulValue;
	RMainframeConfiguration rConfig;		
	if (rConfig.GetLongValue( RMainframeConfiguration::kDebugMenu, ulValue ) )
		m_fDebugMode = (ulValue > 0) ? TRUE : FALSE;
#endif
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::BuildFontList( )
//
//  Description:		Builds and sorts a list of available fonts.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplicationGlobals::BuildFontList()
{
#ifdef	_WINDOWS
	//
	// Begin -Build font list-
	CDC	dc;
	HDC	hdc	= ::GetDC( HWND_DESKTOP );
	dc.Attach( hdc );

	//Define CArray of CStrings to hold font names...
	StringArray	arFonts; 
	//
	//Load arFonts with device's font names..
	GetFontNames( &dc, &arFonts);
	SortFontNames( &arFonts );
	//if the font list string is not empty then empty it.  we may be rebuilding 
	//the list.
	if( !m_rFontListString.IsEmpty() )
		m_rFontListString = "";

	for( int nIdx = 0; nIdx < arFonts.GetSize(); nIdx++ )
	{
		//if( nIdx != 0 ) m_rFontListString += kListFieldSeperator;
		if( !ExactStringIsInList( m_rFontListString, RMBCString(arFonts[nIdx]) ) )
		{
			if( nIdx != 0 ) m_rFontListString += kListFieldSeperator;
			m_rFontListString += RMBCString(arFonts[nIdx]);
		}
	}
	//
	// End -Build font list-
   m_ulNumberOfFonts = arFonts.GetSize();
	
	dc.Detach( );
	::ReleaseDC( HWND_DESKTOP, hdc );

#endif	//	_WINDOWS
#ifdef MAC																	// REVEIW MikeH do this for windows
	{
	long	result;
	// check for availability of TrueType
	if ( Gestalt( gestaltFontMgrAttr, &result ) == noErr )
		m_fTrueTypeAvailable = ( (result & 0x01) != 0 );
	// check for availability of ATM
	if ( initVersionATM( ATMProcs5Version ) != 0 )
		m_fATMAvailable = TRUE;
	}
#endif	//	MAC


}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::~RApplicationGlobals( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RApplicationGlobals::~RApplicationGlobals( )
	{
	TpsAssert( m_DataObjectList.Count() == 0, "Application Globals being deleted with objects still in DataObjectList" );

	m_pCorrectSpell->Terminate();
	m_pIntelliFinder->Terminate();
	delete m_pAutoDrawingSurface;
	delete m_pAppFontList;
	delete m_pCaret;
	delete m_pCursorManager;
	delete m_pResourceManager;
	delete m_pCorrectSpell;
	delete m_pIntelliFinder;
	delete m_pComponentManager;

#ifdef _WINDOWS
	DeleteDefaultPrinterDC( );
#endif	//	_WINDOWS

	ShutdownGlobalBuffer( );
	ATMFinish( );								//	Done using ATM
	tmemShutdown( );							//	free the memory pool
	}

#ifdef _WINDOWS

extern "C" BOOLEAN	AreApplicationGlobalsAvailable( )
	{
	return NULL != RApplication::m_gsApplicationGlobals;
	}

extern "C" PVOID	GetGlobalBitmapPool( )
	{
	return (RApplication::GetApplicationGlobals())->GetGlobalBitmapPool( );
	}

extern "C" PVOID	SetGlobalBitmapPool( PVOID pv )
	{
	(RApplication::GetApplicationGlobals())->SetGlobalBitmapPool( pv );
	return pv;
	}

#ifdef	USE_SMARTHEAP
extern "C" MEM_POOL	GetGlobalSmartHeapHandlePool( )
	{
	return (RApplication::GetApplicationGlobals())->GetGlobalSmartHeapHandlePool( );
	}

extern "C" MEM_POOL	SetGlobalSmartHeapHandlePool( MEM_POOL mpPool )
	{
	(RApplication::GetApplicationGlobals())->SetGlobalSmartHeapHandlePool( mpPool );
	return mpPool;
	}
#endif	//	USE_SMARTHEAP


// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetDefaultPrinterDC( )
//
//  Description:		Create the default printer information
//
//  Returns:			The Printer DC (or NULL if there is none)
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
HDC RApplicationGlobals::GetDefaultPrinterDC( )
	{
	if ( !m_pPrintDialog )
		{
		try
			{
			m_pPrintDialog = new CPrintDialog( FALSE );
			if ( m_pPrintDialog->GetDefaults( ) )
				m_pPrintDialog->CreatePrinterDC( );
			}
		catch( ... )
			{
			;
			}
		}

	return (m_pPrintDialog)? m_pPrintDialog->GetPrinterDC() : NULL;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::DeleteDefaultPrinterDC( )
//
//  Description:		Clean up the Default printer DC information
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplicationGlobals::DeleteDefaultPrinterDC( )
	{
	if ( m_pPrintDialog )
		{
		HDC	hdc	= m_pPrintDialog->GetPrinterDC( );
		if ( hdc )
			::DeleteDC( hdc );
		if( m_pPrintDialog->m_pd.hDevNames )
			::GlobalFree( m_pPrintDialog->m_pd.hDevNames );
		if( m_pPrintDialog->m_pd.hDevMode )
			::GlobalFree( m_pPrintDialog->m_pd.hDevMode );
		delete m_pPrintDialog;
		m_pPrintDialog = NULL;
		}
	}

#endif //	_WINDOWS
