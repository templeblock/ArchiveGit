// ****************************************************************************
//
//  File Name:			Cursors.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RCursor class
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
//  $Logfile:: /PM8/Framework/Source/Cursor.cpp                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Cursor.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"StandaloneApplication.h"
#include "ApplicationGlobals.h"
#include "ResourceManager.h"

const uLONG	kWaitCursorDelayTime	= 1000;	//	1 milli-seconds

YCounter		RCursor::m_StartSpinCount	= 0;
uLONG			RCursor::m_StartSpinTime	= 0;
BOOLEAN		RCursor::m_fSpinStarted		= FALSE;
YResourceId	RCursor::m_CursorId			= (YResourceId)-1;

// ****************************************************************************
//
//  Function Name:	RCursor::ShowCursor( )
//
//  Description:		Call the system ShowCursor routine
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::ShowCursor( )
	{
	MacWinDos( ::ShowCursor(), ::ShowCursor( TRUE ), xxx );
	}

// ****************************************************************************
//
//  Function Name:	RCursor::HideCursor( )
//
//  Description:		If the cursor count is > 0, show the cursor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::HideCursor( )
	{
	MacWinDos( ::HideCursor(), ::ShowCursor( FALSE ), xxx );
	}

// ****************************************************************************
//
//  Function Name:	RCursor::StartSpinCursor( )
//
//  Description:		Start spinning the cursor if the count is > 0
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::StartSpinCursor( BOOLEAN fStartNow )
	{
	TpsAssert( m_StartSpinCount >= 0, "The SpinCursor count is negative" );

	uLONG	ulTime	= RenaissanceGetTickCount( );
	//	If starting now, set start time to now....
	if ( fStartNow )
		m_StartSpinTime	= ulTime;
	//	Else, if not started, set start time to now + delay
	else if ( m_StartSpinCount == 0 )
		m_StartSpinTime	= ulTime + kWaitCursorDelayTime;

	++m_StartSpinCount;
	SpinCursor( );
	}

// ****************************************************************************
//
//  Function Name:	RCursor::SpinCursor( )
//
//  Description:		Continue spinning the cursor if the count is > 0
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::DoStartSpin( )
	{
	TpsAssert( m_StartSpinCount > 0, "The SpinCursor was never started" );
	TpsAssert( m_StartSpinTime <= RenaissanceGetTickCount()," The Cursor is starting to spin to soon" );
	if ( !m_fSpinStarted )
		AfxGetApp( )->BeginWaitCursor( );
	m_fSpinStarted = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RCursor::StopSpinCursor( )
//
//  Description:		Stop spinning the cursor if the count is > 0
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::StopSpinCursor( )
	{
	TpsAssert( m_StartSpinCount >= 1, "The SpinCursor was never started" );
	if ( ((--m_StartSpinCount) == 0) && m_fSpinStarted )
		{
		AfxGetApp( )->EndWaitCursor( );
		m_fSpinStarted = FALSE;
		}
	}

// ****************************************************************************
//
//  Function Name:	RCursor::ClearCursor( )
//
//  Description:		Set the cursor to have an invalid ID so it will always be
//							reset when a SetCursor is called.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::ClearCursor( )
	{
	m_CursorId = (YResourceId)-1;		//	Noone uses this value, for now...
	}

// ****************************************************************************
//
//  Function Name:	RCursor::SetCursor( )
//
//  Description:		Set the cursor to have the given ID
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::SetCursor( YResourceId cursorId )
	{
	//	Assume that if the cursor is the same, it is already being used
	if (cursorId != m_CursorId)
		{
		YResourceCursor	resource = GetResourceManager().GetResourceCursor( cursorId );
		if (resource != NULL)
			MacWinDos( ::SetCursor( resource ), ::SetCursor( resource ), xxx );
		m_CursorId = cursorId;
		}
	}


#ifdef	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RCursor::SetCursor( )
//
//  Description:		Set the cursor to have the given ID
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::SetCursor( LPCTSTR lpctstrCursorId )
	{
	TpsAssert( HIWORD(lpctstrCursorId) == 0, "Only the default cursors are allowed to be named" );
	YResourceId  cursorId = LOWORD( reinterpret_cast<DWORD>( lpctstrCursorId ) );
	SetCursor( cursorId );
	}

#endif	//	_WINDOWS

#ifdef	MAC

// ****************************************************************************
//
//  Function Name:	RCursor::SetCursor( )
//
//  Description:		Set the cursor to have the value
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RCursor::SetCursor( const Cursor* pCursor )
	{
	::SetCursor( pCursor );
	}

#endif	//	MAC

// ****************************************************************************
//
//  Function Name:	RWaitCursor::RWaitCursor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RWaitCursor::RWaitCursor( BOOLEAN fImmediate )
	{
	RCursor( ).StartSpinCursor( fImmediate );
	}

// ****************************************************************************
//
//  Function Name:	RWaitCursor::~RWaitCursor( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RWaitCursor::~RWaitCursor( )
	{
	RCursor( ).StopSpinCursor( );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCursor::Validate( )
//
//  Description:		Validate the object.
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCursor::Validate( ) const
	{
	RObject::Validate( );
	TpsAssert( m_StartSpinCount >= 0, "Validate called when Spin cursor counter is negative" );
	}

#endif	//	TPSDEBUG