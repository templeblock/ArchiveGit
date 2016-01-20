// ****************************************************************************
//
//  File Name:			Cursors.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RCursor Class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/Cursor.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CURSORS_H_
#define		_CURSORS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RCursor
//
//	 Description:	This class allows the managing of cursors
//
// ****************************************************************************
//
class FrameworkLinkage RCursor : public RObject
	{
	//	Constructor, Destructor
	public :
													RCursor( );
													~RCursor( );

	//	Methods
	public :
		void										ShowCursor( );
		void										HideCursor( );

		void										StartSpinCursor( BOOLEAN fStartNow = TRUE );
		void										SpinCursor( );
		void										StopSpinCursor( );		

		void										ClearCursor( );
		void										SetCursor( YResourceId cursorId );
#ifdef	_WINDOWS
		//	This call is to be able to load the default system cursors which have this type of syntax
		void										SetCursor( LPCTSTR lpctstrCursorId );
#endif	//	_WINDOWS
#ifdef	MAC
		//	This call is to be able to load the default system cursors which have this type of syntax
		void										SetCursor( const Cursor* pCursor );
#endif	//	MAC

	//	Implmentation
	private :
		void										DoStartSpin( );

	//	Members
	private :
		static YCounter						m_StartSpinCount;
		static uLONG							m_StartSpinTime;
		static BOOLEAN							m_fSpinStarted;
		static YResourceId					m_CursorId;

#ifdef	TPSDEBUG
	//	Debugging support
	public :
		void										Validate( ) const;
#endif
	} ;


// ****************************************************************************
//
//  Function Name:	RCursor::RCursor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCursor::RCursor( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCursor::~RCursor( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCursor::~RCursor( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCursor::SpinCursor( )
//
//  Description:		Determine if I should start the cursor spinning
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RCursor::SpinCursor( )
{
	if ( (m_StartSpinCount > 0) && (RenaissanceGetTickCount() >= m_StartSpinTime) )
		DoStartSpin( );
}

// ****************************************************************************
//
//  Class Name:	RWaitCursor
//
//	 Description:	Wrapper around StartSpinCursor and StopSpinCursor
//
// ****************************************************************************
//
class FrameworkLinkage RWaitCursor
	{
	// Construction & destruction
	public :
												RWaitCursor( BOOLEAN fImmediate = TRUE );
												~RWaitCursor( );
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CURSORS_H_

