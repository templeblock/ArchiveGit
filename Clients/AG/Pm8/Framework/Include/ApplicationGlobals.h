// ****************************************************************************
//
//  File Name:			ApplicationGlobals.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the Application wide Globals
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
//  $Logfile:: /PM8/Framework/Include/ApplicationGlobals.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_APPLICATIONGLOBALS_H_
#define		_APPLICATIONGLOBALS_H_

#ifndef		_APPLICATION_H_
#include		"Application.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RAlert;
class RCursor;
class RResourceManager;
class RMenu;
class RCaret;
class RFont;
class RStandaloneApplication;
class RDataObject;
class RView;
class ROpenStreamList;
class RFontList;
class RAutoDrawingSurface;
class RIdleInfo;
class RCorrectSpell;
class RIntelliFinder;
class RComponentManager;

#include "versioninfo.h"

#ifdef	_WINDOWS
	#include "GdiObjects.h"

	sLONG&	GetFileNumOperationsTillFailCount( );
	uLONG&	GetFileNumTimesToFailCount( );
#endif	//	_WINDOWS

RView*&	GetActiveView( );
ROpenStreamList*& GetOpenStreamList( );

typedef  BOOLEAN ( ColorDialogCreator )( YInterfaceId, void ** );
typedef	ColorDialogCreator*	ColorDialogCreatorProc;

typedef	RList<RDataObject*>					RDataObjectList;
typedef	RDataObjectList::YIterator			RDataObjectListIterator;

const		RMBCString kListFieldSeperator	= "|";

#ifdef	_WINDOWS
	typedef	RCache<RGDIBrush, COLORREF>		RGDIBrushCache;
	typedef	RCache<RGDIPen, RGDIPenInfo>		RGDIPenCache;
	typedef	RCache<RGDIFont, YFontInfo>		RGDIFontCache;
#endif	//	_WINDOWS

// ****************************************************************************
//
//  Class Name:	RApplicationGlobals
//
//  Description:	The Holder of ALL Globals that will be used in Renaissanc.
//
// ****************************************************************************
//
class FrameworkLinkage RApplicationGlobals
	{
	//	Initialization & Destruction
	public :
												RApplicationGlobals( RStandaloneApplication* pAppBase );
												~RApplicationGlobals( );
	
	//	Accessors to the Globals
	public :		
		RAutoDrawingSurface&				GetAutoDrawingSurface( );
		RFontList&							GetAppFontList( );
		RStandaloneApplication*			GetApplication( );
		RCaret&								GetCaret( );
		RCursor&								GetCursorManager( );
		RDataObjectList&					GetDataObjectList( );
		RResourceManager&					GetResourceManager( );
		RCorrectSpell&						GetCorrectSpell( );
		RIntelliFinder&					GetIntelliFinder( );
		RView*&								GetActiveView( );
		BOOLEAN&								GetUseOffscreens( );
		BOOLEAN&								GetDebugMode( );
		RComponentManager&				GetComponentManager( );
		BOOLEAN&								GetInDragDrop( );
		RScript*&							GetRecordingScript( );
		void								BuildFontList();

		BOOLEAN								IsTrueTypeAvailable( ) const;
		BOOLEAN								IsATMAvailable( ) const;

		YIntDimension						GetMaxGradientDPI() const;
		YPercentage							GetGradientDisplayPrecision() const;
		YPercentage							GetGradientPrintPrecision() const;
		ELanguageVersions					GetLanguageVersion( ) const;

		const RVersionInfo&					GetCurrentDocumentVersion() const;						
		void								SetCurrentDocumentVersion(RVersionInfo &info );		

		BOOLEAN								UseBitmapTrackingFeedback( ) const;

#ifdef	_WINDOWS	
		RGDIBrushCache&					GetBrushCache( );
		RGDIPenCache&						GetPenCache( );
		RGDIFontCache&						GetFontCache( );

		sLONG&								GetFileNumOperationsTillFailCount( );
		uLONG&								GetFileNumTimesToFailCount( );

		BOOLEAN								AreApplicationGlobalsAvailable();
		PVOID									GetGlobalBitmapPool();
		PVOID									SetGlobalBitmapPool(PVOID pvoid);

		sLONG&								GetMemSuccessCount( );
		sLONG&								GetMemFailureCount( );

		const RMBCString&					GetFontList( );
		uLONG									GetNumberOfFonts( );

		HDC									GetDefaultPrinterDC( );
		void									DeleteDefaultPrinterDC( );

	#ifdef	USE_SMARTHEAP
			//	SmartHeap managment
		MEM_POOL								GetGlobalSmartHeapHandlePool( );
		MEM_POOL								SetGlobalSmartHeapHandlePool( MEM_POOL mpPool );
	#endif	//	USE_SMARTHEAP
#endif	//	_WINDOWS

	//	Global buffer management
	public :
		ROpenStreamList*&					GetOpenStreamList( );
		RIdleInfo*							GetIdleInfo( );

	// Overrideable creator procs
	public:
		ColorDialogCreatorProc			CreateColorDialog;		

	//	Globals to be used by Everyone
	private :
		RStandaloneApplication*			m_pApplication;
		RAutoDrawingSurface*				m_pAutoDrawingSurface;
		RFontList*							m_pAppFontList;
		RCaret*								m_pCaret;
		RCursor*								m_pCursorManager;
		RDataObjectList					m_DataObjectList;
		RResourceManager*					m_pResourceManager;
		RCorrectSpell*						m_pCorrectSpell;
		RIntelliFinder*					m_pIntelliFinder;
		RComponentManager*				m_pComponentManager;
		BOOLEAN								m_fUseOffscreens;
		BOOLEAN								m_fDebugMode;

		BOOLEAN								m_fTrueTypeAvailable;
		BOOLEAN								m_fATMAvailable;
		BOOLEAN								m_fUseBitmapTrackingFeedback;

#ifdef	_WINDOWS
		RGDIBrushCache						m_BrushCache;
		RGDIPenCache						m_PenCache;
		RGDIFontCache						m_FontCache;

		sLONG									m_slMemSuccessCount;
		sLONG									m_slMemFailureCount;
		sLONG									m_slFileNumOperationsTillFailCount;
		uLONG									m_ulFileNumTimesToFailCount;

		PVOID									m_pBitmapPool;
		RMBCString							m_rFontListString;
		uLONG									m_ulNumberOfFonts;

		CPrintDialog*						m_pPrintDialog;

	#ifdef	USE_SMARTHEAP
			MEM_POOL								m_mpSmartheapHandlePool;
	#endif	//	USE_SMARTHEAP

#endif	//	_WINDOWS

		RView*								m_pActiveView;
		ROpenStreamList*					m_pOpenStreamList;
		RIdleInfo*							m_pIdleInfo;
		YIntDimension						m_nMaxGradientDPI;
		YPercentage							m_pctGradientDisplayPrecision;
		YPercentage							m_pctGradientPrintPrecision;
		ELanguageVersions					m_eLanguageVersion;

		BOOLEAN								m_fInDragDrop;
		RScript*								m_pScript;

		RVersionInfo						m_DocumentVersion;	
} ;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetAutoDrawingSurface( )
//
//  Description:		Return the Alert object
//
//  Returns:			m_AlertManager
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RAutoDrawingSurface& RApplicationGlobals::GetAutoDrawingSurface( )
	{
	return *m_pAutoDrawingSurface;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetAppFontList( )
//
//  Description:		Return the Application Font List
//
//  Returns:			m_AlertManager
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RFontList& RApplicationGlobals::GetAppFontList( )
	{
	return *m_pAppFontList;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetApplication( )
//
//  Description:		Return the Application
//
//  Returns:			*m_pApplication
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RStandaloneApplication* RApplicationGlobals::GetApplication( )
	{
	return m_pApplication;
	}


// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetCaret( )
//
//  Description:		Return the Caret object
//
//  Returns:			m_Caret
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCaret& RApplicationGlobals::GetCaret( )
	{
	return *m_pCaret;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetCursorManager( )
//
//  Description:		Return the Cursor object
//
//  Returns:			m_CursorManager
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCursor& RApplicationGlobals::GetCursorManager( )
	{
	return *m_pCursorManager;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetDataObjectList( )
//
//  Description:		Return the Data Object List
//
//  Returns:			m_DataObjectList
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RDataObjectList& RApplicationGlobals::GetDataObjectList( )
	{
	return m_DataObjectList;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetResourceManager( )
//
//  Description:		Return the Resource Manager
//
//  Returns:			m_ResourceManager
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RResourceManager& RApplicationGlobals::GetResourceManager( )
	{
	return *m_pResourceManager;
	}


// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetCorrectSpell( )
//
//  Description:		Return the Inso Correct Spell checker.
//
//  Returns:			m_pCorrectSpell
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCorrectSpell& RApplicationGlobals::GetCorrectSpell( )
	{
	return *m_pCorrectSpell;
	}


// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetIntelliFinder( )
//
//  Description:		Return the Inso IntelliFinder for thesaurus operations.
//
//  Returns:			m_pIntelliFinder
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RIntelliFinder& RApplicationGlobals::GetIntelliFinder( )
	{
	return *m_pIntelliFinder;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetComponentManager( )
//
//  Description:		Return the component manager
//
//  Returns:			m_pComponentManager
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RComponentManager& RApplicationGlobals::GetComponentManager( )
	{
	return *m_pComponentManager;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetUseOffscreens( )
//
//  Description:		Return the state of the m_fUseOffscreens variable.
//
//  Returns:			m_fUseOffscreens
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN& RApplicationGlobals::GetUseOffscreens( )
	{
	return m_fUseOffscreens;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetDebugMode( )
//
//  Description:		Return the state of the m_fDebugMode variable.
//
//  Returns:			m_fUseOffscreens
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN& RApplicationGlobals::GetDebugMode( )
	{
		return m_fDebugMode;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::UseBitmapTrackingFeedback( )
//
//  Description:		Return the state of the m_fUseBitmapTrackingFeedback variable.
//
//  Returns:			m_fUseBitmapTrackingFeedback
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RApplicationGlobals::UseBitmapTrackingFeedback( ) const
	{
	return m_fUseBitmapTrackingFeedback;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::IsTrueTypeAvailable( )
//
//  Description:		Return TRUE if TrueType is available on this system
//
//  Returns:			m_fTrueTypeAvailable
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RApplicationGlobals::IsTrueTypeAvailable( ) const
	{
	return m_fTrueTypeAvailable;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::IsATMAvailable( )
//
//  Description:		Return TRUE if ATM is available on this system
//
//  Returns:			m_fATMAvailable
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RApplicationGlobals::IsATMAvailable( ) const
	{
	return m_fATMAvailable;
	}

#ifdef	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetBrushCache( )
//
//  Description:		Return the BrushCache object
//
//  Returns:			m_BrushCache
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGDIBrushCache& RApplicationGlobals::GetBrushCache( )
	{
	return m_BrushCache;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetPenCache( )
//
//  Description:		Return the PenCache object
//
//  Returns:			m_PenCache
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGDIPenCache& RApplicationGlobals::GetPenCache( )
	{
	return m_PenCache;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetFontCache( )
//
//  Description:		Return the FontCache object
//
//  Returns:			m_FontCache
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGDIFontCache& RApplicationGlobals::GetFontCache( )
	{
	return m_FontCache;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetGlobalBitmapPool( )
//
//  Description:		Bitmap library shared static globals structure
//
//  Returns:			void pointer to bitmap static globals - bitmap lib casts
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline PVOID RApplicationGlobals::GetGlobalBitmapPool( )
	{
	TpsAssert( m_pBitmapPool != NULL, "Bitmap pool not initialized" );
	return m_pBitmapPool;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::SetGlobalBitmapPool( )
//
//  Description:		Set bitmap library static global pool pointer
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline PVOID RApplicationGlobals::SetGlobalBitmapPool( PVOID pvoid )
	{
		m_pBitmapPool = pvoid;
		return m_pBitmapPool;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetFileNumOperationsTillFailCount( )
//
//  Description:		Return the number of file operations to perform before a forced
//								error should occur
//
//  Returns:			m_slFileNumOperationsTillFailCount
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline sLONG& RApplicationGlobals::GetFileNumOperationsTillFailCount( )
	{
	return m_slFileNumOperationsTillFailCount;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetFileNumTimesToFailCount( )
//
//  Description:		Return the number of times a force file operation failure 
//								should occur
//
//  Returns:			m_ulFileNumTimesToFailCount
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline uLONG& RApplicationGlobals::GetFileNumTimesToFailCount( )
	{
	return m_ulFileNumTimesToFailCount;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetMemSuccessCount( )
//
//  Description:		Return the Memory Sucess count
//
//  Returns:			m_slMemSuccessCount
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline sLONG& RApplicationGlobals::GetMemSuccessCount( )
	{
	return m_slMemSuccessCount;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetMemFailureCount( )
//
//  Description:		Return the Memory Failure count
//
//  Returns:			m_slMemSuccessCount
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline sLONG& RApplicationGlobals::GetMemFailureCount( )
	{
	return m_slMemFailureCount;
	}

#ifdef	USE_SMARTHEAP
// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetGlobalSmartHeapHandlePool( )
//
//  Description:		SmartHeap MEM_POOL for application handle memory
//
//  Returns:			m_mpSmartheapHandlePool
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline MEM_POOL RApplicationGlobals::GetGlobalSmartHeapHandlePool( )
	{
	TpsAssert( m_mpSmartheapHandlePool != NULL, "SmartHeap pool not allocated" );
	return m_mpSmartheapHandlePool;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::SetGlobalSmartHeapHandlePool( )
//
//  Description:		Set SmartHeap MEM_POOL for application handle memory
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline MEM_POOL RApplicationGlobals::SetGlobalSmartHeapHandlePool( MEM_POOL mpPool )
	{
		m_mpSmartheapHandlePool = mpPool;
		return mpPool;
	}
#endif	//	USE_SMARTHEAP

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetFontList( )
//
//  Description:		Return the font list
//
//  Returns:			m_rFontListString
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline const RMBCString& RApplicationGlobals::GetFontList( )
	{
	return m_rFontListString;
	}
// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetNumberOfFonts( )
//
//  Description:		Return the number of fonts in the font list
//
//  Returns:			m_ulNumberOfFonts
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline uLONG RApplicationGlobals::GetNumberOfFonts( )
	{
	return m_ulNumberOfFonts;
	}


#endif	//	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetActiveView( )
//
//  Description:		Return the active view
//
//  Returns:			m_pActiveView
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RView*& RApplicationGlobals::GetActiveView( )
	{
	return m_pActiveView;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetOpenStreamList( )
//
//  Description:		Return the open streams list
//
//  Returns:			m_pOpenStreamList
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline ROpenStreamList*& RApplicationGlobals::GetOpenStreamList( )
	{
	return m_pOpenStreamList;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetOpenStreamList( )
//
//  Description:		Return the open streams list
//
//  Returns:			m_pOpenStreamList
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RIdleInfo* RApplicationGlobals::GetIdleInfo( )
	{
	return m_pIdleInfo;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetMaxGradientDPI( )
//
//  Description:		Return the max gradient DPI
//
//  Returns:			m_nMaxGradientDPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YIntDimension RApplicationGlobals::GetMaxGradientDPI( ) const
	{
	return m_nMaxGradientDPI;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetGradientDisplayPrecision( )
//
//  Description:		Return the display precision for gradients
//
//  Returns:			m_pctGradientDisplayPrecision (0.5 -> 1.0)
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YPercentage RApplicationGlobals::GetGradientDisplayPrecision( ) const
	{
	return m_pctGradientDisplayPrecision;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetGradientPrintPrecision( )
//
//  Description:		Return the print precision for gradients
//
//  Returns:			m_pctGradientPrintPrecision (0.5 -> 1.0)
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YPercentage RApplicationGlobals::GetGradientPrintPrecision( ) const
	{
	return m_pctGradientPrintPrecision;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetLanguageVersion( )
//
//  Description:		Return the language version as specified in the registry.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline ELanguageVersions RApplicationGlobals::GetLanguageVersion( ) const
	{
	return m_eLanguageVersion;
	}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetLanguageVersion( )
//
//  Description:		Return the DragDrop state of the application.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN&	RApplicationGlobals::GetInDragDrop( )
{
	return m_fInDragDrop;
}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetRecordingScript( )
//
//  Description:		Return the Recording Script
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RScript*& RApplicationGlobals::GetRecordingScript( )
{
	return m_pScript;
}


// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetCurrentDocumentVersion( )
//
//  Description:	Returns the version of the current document being read
//					used for determining what version of the application was used
//					to create a file when reading it in.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline const RVersionInfo& RApplicationGlobals::GetCurrentDocumentVersion( ) const
{
	return m_DocumentVersion;
}

// ****************************************************************************
//
//  Function Name:	RApplicationGlobals::GetCurrentDocumentVersion( )
//
//  Description:	Returns the version of the current document being read
//					used for determining what version of the application was used
//					to create a file when reading it in.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void	RApplicationGlobals::SetCurrentDocumentVersion(RVersionInfo &info ) 
{
	m_DocumentVersion = info;
}


// ****************************************************************************
// 					Inlines for accessing 'Global' information
// ****************************************************************************

inline RStandaloneApplication& GetApplication( )
	{
	return( *( RApplication::GetApplicationGlobals( ) )->GetApplication( ) );
	}

inline RAutoDrawingSurface&	GetAutoDrawingSurface( )
	{
	return (RApplication::GetApplicationGlobals())->GetAutoDrawingSurface( );
	}

inline RFontList&	GetAppFontList( )
	{
	return (RApplication::GetApplicationGlobals())->GetAppFontList( );
	}

inline RCaret& GetCaret( )
	{
	return (RApplication::GetApplicationGlobals())->GetCaret( );
	}

inline RCursor& GetCursorManager( )
	{
	return (RApplication::GetApplicationGlobals())->GetCursorManager( );
	}

inline RDataObjectList&	GetDataObjectList( )
	{
	return (RApplication::GetApplicationGlobals())->GetDataObjectList( );
	}

inline RResourceManager& GetResourceManager( )
	{
	return (RApplication::GetApplicationGlobals())->GetResourceManager();
	}

inline RCorrectSpell& GetCorrectSpell( )
	{
	return (RApplication::GetApplicationGlobals())->GetCorrectSpell();
	}

inline RIntelliFinder& GetIntelliFinder( )
	{
	return (RApplication::GetApplicationGlobals())->GetIntelliFinder();
	}

inline RComponentManager& GetComponentManager( )
	{
	return (RApplication::GetApplicationGlobals())->GetComponentManager();
	}

inline BOOLEAN& GetUseOffscreens( )
	{
	return (RApplication::GetApplicationGlobals())->GetUseOffscreens();
	}

inline BOOLEAN& GetDebugMode( )
	{
	return (RApplication::GetApplicationGlobals())->GetDebugMode();
	}

inline RScript*& GetRecordingScript()
	{
	return (RApplication::GetApplicationGlobals( ))->GetRecordingScript( );
	}

inline BOOLEAN& GetInDragDrop()
	{
	return (RApplication::GetApplicationGlobals( ))->GetInDragDrop( );
	}

inline BOOLEAN UseBitmapTrackingFeedback( )
	{
	return (RApplication::GetApplicationGlobals())->UseBitmapTrackingFeedback();
	}

inline BOOLEAN	IsTrueTypeAvailable( )
	{
	return (RApplication::GetApplicationGlobals())->IsTrueTypeAvailable();
	}

inline BOOLEAN	IsATMAvailable( )
	{
	return (RApplication::GetApplicationGlobals())->IsATMAvailable();
	}

#ifdef	_WINDOWS

inline RGDIBrushCache& GetBrushCache( )
	{
	return (RApplication::GetApplicationGlobals())->GetBrushCache( );
	}

inline RGDIPenCache& GetPenCache( )
	{
	return (RApplication::GetApplicationGlobals())->GetPenCache( );
	}

inline RGDIFontCache& GetFontCache( )
	{
	return (RApplication::GetApplicationGlobals())->GetFontCache( );
	}

inline sLONG& GetMemSuccessCount( )
	{
	return (RApplication::GetApplicationGlobals())->GetMemSuccessCount( );
	}

inline sLONG& GetMemFailureCount( )
	{
	return (RApplication::GetApplicationGlobals())->GetMemFailureCount( );
	}

inline sLONG& GetFileNumOperationsTillFailCount( )
	{
	return (RApplication::GetApplicationGlobals())->GetFileNumOperationsTillFailCount( );
	}

inline uLONG& GetFileNumTimesToFailCount( )
	{
	return (RApplication::GetApplicationGlobals())->GetFileNumTimesToFailCount( );
	}

inline HDC GetDefaultPrinterDC( )
	{
	return (RApplication::GetApplicationGlobals())->GetDefaultPrinterDC( );
	}

inline void DeleteDefaultPrinterDC( )
	{
	(RApplication::GetApplicationGlobals())->DeleteDefaultPrinterDC( );
	}

#endif	//	_WINDOWS

inline RView*& GetActiveView( )
	{
	return (RApplication::GetApplicationGlobals())->GetActiveView( );
	}

inline ROpenStreamList*& GetOpenStreamList( )
	{
	return (RApplication::GetApplicationGlobals())->GetOpenStreamList( );
	}

inline RIdleInfo* GetIdleInfo( )
	{
	return (RApplication::GetApplicationGlobals())->GetIdleInfo( );
	}

inline YIntDimension GetMaxGradientDPI()
	{
	return (RApplication::GetApplicationGlobals())->GetMaxGradientDPI();
	}

inline YPercentage GetGradientDisplayPrecision()
	{
	return (RApplication::GetApplicationGlobals())->GetGradientDisplayPrecision();
	}

inline YPercentage GetGradientPrintPrecision()
	{
	return (RApplication::GetApplicationGlobals())->GetGradientPrintPrecision();
	}

inline ELanguageVersions GetLanguageVersion()
	{
	return (RApplication::GetApplicationGlobals())->GetLanguageVersion();
	}

template <class S>
S ChooseEnglishOrEuropean( S& EnglishChoice, S& EuropeanChoice )
	{
	if ( kEnglishLanguage == GetLanguageVersion( ) )
		return EnglishChoice;
	else
		return EuropeanChoice;
	}

//
// Constants, defaults, and valid ranges for preferences...
//

//
//
// Default maximum gradient DPI
const uWORD	kDefaultMaxGradientDPI				= 72;
const uWORD	kMinMaxGradientDPI					= 40;
const uWORD	kMaxMaxGradientDPI					= 2400;
//
// Default gradient display precision
const uWORD	kDefaultGradientDisplayPrecision	= 98;
const uWORD	kMinGradientDisplayPrecision		= 70;
const uWORD	kMaxGradientDisplayPrecision		= 100;
//
// Default gradient print precision
const uWORD	kDefaultGradientPrintPrecision	= 100;
const uWORD	kMinGradientPrintPrecision			= 70;
const uWORD	kMaxGradientPrintPrecision			= 100;


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_APPLICATIONGLOBALS_H_
