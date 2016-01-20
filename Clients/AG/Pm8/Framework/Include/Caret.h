// ****************************************************************************
//
//  File Name:			Caret.h
//
//  Project:			Renaissance Framework Application
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RCaret class
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
//  $Logfile:: /PM8/Framework/Include/Caret.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_CARET_H_
#define	_CARET_H_

#ifndef	_CACHE_H_
#include	"Cache.h"
#endif

#ifdef	_WINDOWS
	#ifndef		_GDIOBJECTS_H_
	#include		"GDIObjects.h"
	#endif

	typedef	RCache<RGDICaret, RGDICaretInfo>		RGDICaretCache;
#endif	//	_WINDOWS

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;

// ****************************************************************************
//
//  Class Name:	RCaret
//
//  Description:	The class for handling the display and position of
//						a text insertion point caret.
//
// ****************************************************************************
//

class FrameworkLinkage RCaret
	{
	//	Construction, Destruction
	public :
													RCaret( );
													~RCaret( );

	//	Operations
	public :
		void										SetCaretOwner( RView* pView );
		RView*									GetCaretOwner( ) const;

		void										ShowCaret( );
		void										HideCaret( );
		void										SetCaretSize( RIntSize size, const R2dTransform& transform );
		void										SetCaretPosition( RRealPoint position );

	//	Implementation
	private :
		void										CreateCaret( );

	//	Memeber Data
	private :
		YCounter									m_VisibleCount;
		RIntSize									m_Size;
		YAngle									m_Rotation;
		RRealSize								m_Scale;
		RIntPoint								m_Position;
		RView*									m_pView;
		BOOLEAN									m_fCaretDirty;

#ifdef	_WINDOWS
		RGDICaretCache							m_CaretCache;
#endif	//	_WINDOWS
	} ;


// ****************************************************************************
//
//  Class Name:	RAutoCaret
//
//  Description:	Class for automatically hiding and showing the caret on 
//						creation and destruction
//
// ****************************************************************************
//
class	FrameworkLinkage RAutoCaret
	{
	//	Construction & Destruction
	public :
													RAutoCaret( );
													~RAutoCaret( );
	} ;

#ifdef	_WINDOWS
// ****************************************************************************
//
//  Function Name:	RGDICaretInfo::RGDICaretInfo( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGDICaretInfo::RGDICaretInfo( const RIntPoint& topRight, const RIntPoint& bottomLeft )
	: m_TopRight( topRight ),
	  m_BottomLeft( bottomLeft )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RGDICaretInfo::operator==( )
//
//  Description:		Equality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RGDICaretInfo::operator==( const RGDICaretInfo& rhs )
	{
	return (BOOLEAN)((rhs.m_TopRight == m_TopRight) && (rhs.m_BottomLeft == m_BottomLeft));
	}

// ****************************************************************************
//
//  Function Name:	RGDICaretInfo::operator!=( )
//
//  Description:		Inequality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RGDICaretInfo::operator!=( const RGDICaretInfo& rhs )
	{
	return (BOOLEAN)((rhs.m_TopRight != m_TopRight) || (rhs.m_BottomLeft != m_BottomLeft));
	}

// ****************************************************************************
//
//  Function Name:	RGDICaret::RGDICaret( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGDICaret::RGDICaret( const RGDICaretInfo& info )
	: m_hbmpCaret( NULL ),
	  m_CaretInfo( info )
	{
	RIntSize	delta( m_CaretInfo.m_BottomLeft.m_x - m_CaretInfo.m_TopRight.m_x,
						 m_CaretInfo.m_BottomLeft.m_y - m_CaretInfo.m_TopRight.m_y );

	m_CaretSize = delta;
	if (delta.m_dx < 0)
		delta.m_dx	= -delta.m_dx;
	if (delta.m_dy < 0)
		delta.m_dy	= -delta.m_dy;

	if ((delta.m_dx != 0) && (delta.m_dy != 0))
		{
		HDC		hdc		= NULL;
		HDC		hdcMem	= NULL;
		HBITMAP	hbm		= NULL;
		HBITMAP	hbmOld	= NULL;
		if ( ((hdc = ::GetDC( HWND_DESKTOP )) != NULL) &&
			  ((hdcMem = ::CreateCompatibleDC( hdc )) != NULL) && 
			  ((hbm = ::CreateBitmap(delta.m_dx, delta.m_dy, 1, 1, NULL)) != NULL) &&
			  ((hbmOld = (HBITMAP)::SelectObject( hdcMem, hbm )) != NULL)  )
			{
			::PatBlt( hdcMem, 0, 0, delta.m_dx, delta.m_dy, BLACKNESS);
			::SelectObject( hdcMem, ::GetStockObject( WHITE_PEN ));
			if ( ((m_CaretSize.m_dx < 0) && (m_CaretSize.m_dy < 0)) ||
				  ((m_CaretSize.m_dx > 0) && (m_CaretSize.m_dy > 0))  )
				{
				::MoveToEx( hdcMem, 0, 0, NULL );
				::LineTo( hdcMem, delta.m_dx, delta.m_dy-1 );
				}
			else
				{
				::MoveToEx( hdcMem, 0, delta.m_dy-1, NULL );
				::LineTo( hdcMem, delta.m_dx, 0 );
				}

			m_hbmpCaret = hbm;
			hbm			= NULL;	//	Don't want to delete it...
			}

		if (hbmOld)	::SelectObject( hdcMem, hbmOld );
		if (hbm)		::DeleteObject( hbm );
		if (hdcMem)	::DeleteDC( hdcMem );
		if (hdc)		::ReleaseDC( HWND_DESKTOP, hdc );
		}
	}

// ****************************************************************************
//
//  Function Name:	RGDICaret::~RGDICaret( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGDICaret::~RGDICaret( )
	{
	if (m_hbmpCaret != NULL)
		::DeleteObject( m_hbmpCaret );
	}

// ****************************************************************************
//
//  Function Name:	RGDICaret::SelectCaret( )
//
//  Description:		Select the caret as the current caret
//
//  Returns:			m_hFont
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RGDICaret::SelectCaret( HWND hwnd )
	{
	if ( m_hbmpCaret )
		::CreateCaret( hwnd, m_hbmpCaret, m_CaretSize.m_dx, m_CaretSize.m_dy );
	else
		::CreateCaret( hwnd, NULL, m_CaretSize.m_dx, m_CaretSize.m_dy );
	}

// ****************************************************************************
//
//  Function Name:	RGDICaret::operator==( )
//
//  Description:		Equality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RGDICaret::operator==( const RGDICaret& rhs )
	{
	return (BOOLEAN)(m_CaretInfo == rhs.m_CaretInfo);
	}

// ****************************************************************************
//
//  Function Name:	RGDIFont::operator!=( )
//
//  Description:		Inequality operator.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RGDICaret::operator!=( const RGDICaret& rhs )
	{
	return (BOOLEAN)(m_CaretInfo != rhs.m_CaretInfo);
	}
#endif	//	_WINDOWS


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CARET_H_

