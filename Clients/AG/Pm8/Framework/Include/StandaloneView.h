// ****************************************************************************
//
//  File Name:			StandaloneView.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RStandaloneView class
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
//  $Logfile:: /PM8/Framework/Include/StandaloneView.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STANDALONEVIEW_H_
#define		_STANDALONEVIEW_H_

#include	"WindowView.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RStandaloneView;
class RStandaloneDocument;

// ****************************************************************************
//
//  Class Name:		RZoomLevelMapEntry
//
//  Description:		Class that maps a command id to a zoom level
//
// ****************************************************************************
//
class FrameworkLinkage RZoomLevelMapEntry
	{
	public :
												RZoomLevelMapEntry( YCommandID commandId, YZoomLevel zoomLevel );

		YCommandID							m_CommandId;
		YZoomLevel							m_ZoomLevel;
	};

typedef RArray<RZoomLevelMapEntry> YZoomLevelMap;
typedef YZoomLevelMap::YIterator YZoomLevelIterator;

#ifdef _WINDOWS
	#include "MessageDispatch.h"
	#define	FrameworkView	RMessageDispatch<CView>
#else
	#define	FrameworkView	LCommandoView
#endif

// ****************************************************************************
//
//  Class Name:		RStandaloneView
//
//  Description:		Base class for standalone Renaissance views.
//
//							This class is responsible for translating Windows specific
//							messages into Renaissance xEvents.
//
// ****************************************************************************
//
class FrameworkLinkage RStandaloneView : public FrameworkView, public RWindowView
	{
	// Construction, destruction, & initialization
	public :
												RStandaloneView( );
		virtual								~RStandaloneView( );
		virtual void						Initialize( RDocument* pDocument );

	// Operations
	public :
		virtual void						OnXResize( const RRealSize& size );
		virtual void						RecalculateFitToWindow( ) = 0;
		virtual void						RecalculateFitObjectsToWindow( ) = 0;
		virtual void						SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
		virtual void						InitialUpdate( );

	// Accessors
	public :
		virtual RRealSize					GetDPI( ) const;
		RStandaloneDocument*				GetStandaloneDocument( ) const;
		YZoomLevel							GetZoomLevel( ) const;

	// Implementation
	protected :
		virtual void						SetZoomLevel( YZoomLevel zoomLevel, BOOLEAN fUpdateScrollBars = TRUE );

	// Command handlers
	protected :
		void									OnFitToWindow( );

	// Command handlers
	private :
		void									OnZoom( YCommandID commandId );
		void									OnUpdateZoom( RCommandUI& commandUI ) const;
		void									OnUpdateFitToWindow( RCommandUI& commandUI ) const;
		void									OnFitObjectsToWindow( );
		void									OnUpdateFitObjectsToWindow( RCommandUI& commandUI ) const;
		void									OnZoomIn( );
		void									OnUpdateZoomIn( RCommandUI& commandUI ) const;
		void									OnZoomOut( );
		void									OnUpdateZoomOut( RCommandUI& commandUI ) const;

	// Member data
	protected :
		YZoomLevelMap						m_ZoomLevelMap;
		BOOLEAN								m_fFitToWindow;
		YZoomLevel							m_ZoomLevel;

	// Command map
	public :
		virtual RCommandMapBase*		GetRCommandMap( ) const;

	private :
		static RCommandMap<RStandaloneView, RView> m_CommandMap;
		friend class RCommandMap<RStandaloneView, RView>;

	// Windows specific stuff
#ifdef	_WINDOWS
	
	// Generated message map functions
	protected:
		DECLARE_DYNAMIC(RStandaloneView)

	// Overrides
	protected :
		virtual void						OnDraw( CDC* pDC );
		virtual BOOL						OnPreparePrinting( CPrintInfo* pInfo );
	
	// Conversions
	public :
		virtual CWnd&						GetCWnd( ) const;

	friend class RStandaloneViewMessageDispatch;

#endif	// _WINDOWS

	//	Macintosh specific stuff
#ifdef	MAC
	public:

	//	Activate & Deactivate
	public :
		virtual void						ActivateSelf( );
		virtual void						DeactivateSelf( );

	public:
		virtual void						DrawSelf();	

	//	Mouse processing and such.
	public :
		virtual void						ClickSelf(const SMouseDownEvent &inMouseDown);

	//	Menu and command processing
	public :
		virtual	Boolean					ObeyCommand( CommandT inCommand, void *ioParam);
		virtual	void						FindCommandStatus( CommandT	inCommand,
																		 Boolean		&outEnabled,
																		 Boolean		&outUsesMark,
																		 Char16		&outMark,
																		 Str255		outName );

	//	Conversions
	public :
		virtual	LCommandoView&			GetCommandoView( ) const;

#endif	//	Mac

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RStandaloneView::GetRCommandMap( )
//
//  Description:		Gets a pointer to this classes command map.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RStandaloneView::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

// ****************************************************************************
//
//  Function Name:	RZoomLevelMapEntry::RZoomLevelMapEntry( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RZoomLevelMapEntry::RZoomLevelMapEntry( YCommandID commandId, YZoomLevel zoomLevel )
	: m_CommandId( commandId ),
	  m_ZoomLevel( zoomLevel )
	{
	;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _STANDALONEVIEW_H_
