// ****************************************************************************
//
//  File Name:			ScrollBar.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RScrollBar class
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
//  $Logfile:: /PM8/Framework/Include/ScrollBar.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_SCROLLBAR_H_
#define	_SCROLLBAR_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#ifdef		_WINDOWS
	typedef int YScrollBarType;
	const YScrollBarType kHorizontal = SB_HORZ;
	const YScrollBarType kVertical = SB_VERT;
	const YScrollBarType kControl = SB_CTL;
#endif		// _WINDOWS

class RWindowView;

// ****************************************************************************
//
//  Class Name:	RScrollBar
//
//  Description:	Class to encapsulate a scroll bar
//
// ****************************************************************************
//
class FrameworkLinkage RScrollBar : public RObject
	{
	// Construction
	public :
												RScrollBar( YScrollBarType bar, RWindowView* pView );
												RScrollBar( CScrollBar* pScrollBar );

	// Operations
	public :
		void									SetRange( YIntDimension minRange, YIntDimension maxRange, YIntDimension pageSize, BOOLEAN fUpdateOnlyIfExtending );
		YIntDimension						SetPosition( YIntDimension position );
		YIntDimension						Scroll( YIntDimension scrollAmount );
		YIntDimension						ScrollPageUp( );
		YIntDimension						ScrollPageDown( );
	
	// Accessors
	public :
		YIntDimension						GetPosition( ) const;
		void									GetValues( YIntDimension& minRange, YIntDimension& maxRange, YIntDimension& position ) const;
		BOOLEAN								CanScrollUp( ) const;
		BOOLEAN								CanScrollDown( ) const;

	// Member data
	protected :
		HWND									m_Handle;

	// Member data
	private :
		YIntDimension						m_ScrollGranularity;
		YIntDimension						m_MaxScrollRange;
		YIntDimension						m_MinScrollRange;
		YIntDimension						m_PageSize;
		YScrollBarType						m_Bar;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_SCROLLBAR_H_
