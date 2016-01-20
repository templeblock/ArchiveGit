// ****************************************************************************
//
//  File Name:			MouseTracker.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMouseTracker class
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
//  $Logfile:: /PM8/Framework/Include/MouseTracker.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MOUSETRACKER_H_
#define		_MOUSETRACKER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;
class RDrawingSurface;
class RWindowView;

// ****************************************************************************
//
//  Class Name:	RMouseTracker
//
//  Description:	This class is used to encapsulate a tracking operation.
//						Derive specific operations off of this base class.
//
// ****************************************************************************
//
class FrameworkLinkage RMouseTracker : public RObject
	{
	// Construction & Destruction & Initialization
	public :
												RMouseTracker( );
												RMouseTracker( RView* pView );
		virtual								~RMouseTracker( );
		virtual void						Initialize( RView* pView );

	// Operations
	public :
		virtual void						BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys ) = 0;
		virtual void						ContinueTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys ) = 0;
		virtual void						EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys ) = 0;
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const = 0;
		virtual void						CancelTracking( );
		BOOLEAN								IsTracking( );

	// Implementation
	protected :
		void									Render( ) const;

	// Member data
	protected :
		BOOLEAN								m_fTracking;
		RView*								m_pView;

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
//  Function Name:	RMouseTracker::ContinueTracking( )
//
//  Description:		Called when tracking ends; ie. when the mouse button goes
//							up.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RMouseTracker::ContinueTracking( const RRealPoint&, YModifierKey )
	{
	TpsAssert( m_fTracking == TRUE, "Not tracking" );
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::IsTracking( )
//
//  Description:		Returns whether this tracker is currently being used for a
//							tracking operation
//
//  Returns:			TRUE	: Tracking
//							FALSE	: Not Tracking
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RMouseTracker::IsTracking( )
	{
	return m_fTracking;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _MOUSETRACKER_H_
