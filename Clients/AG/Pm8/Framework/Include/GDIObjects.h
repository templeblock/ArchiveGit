// ****************************************************************************
//
//  File Name:			GDIObjects.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the various GDI Objects
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
//  $Logfile:: /PM8/Framework/Include/GDIObjects.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GDIOBJECTS_H_
#define		_GDIOBJECTS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RGDIBrush
//
//	 Description:	This class allows the caching of Windows Brushes with the
//						RCache class
//
// ****************************************************************************
//
class FrameworkLinkage RGDIBrush
	{
	//	Constructor, Destructor
	public :
													RGDIBrush( COLORREF color );
													RGDIBrush( HBRUSH	hbrush );
													~RGDIBrush( );

	//	Methods
	public :
		BOOLEAN									operator==( const RGDIBrush& rhs );
		BOOLEAN									operator!=( const RGDIBrush& rhs );

		HBRUSH									GetBrush( );

	//	Members
	private :
		HBRUSH									m_hBrush;
		BOOLEAN									m_fCleanUp;
	} ;

// ****************************************************************************
//
//  Class Name:	RGDIPen
//
//	 Description:	This class allows the caching of Windows Pens with the
//						RCache class
//
// ****************************************************************************
//
	// ****************************************************************************
	//
	//  Class Name:	RGDIPenInfo
	//
	//	 Description:	This class allows the caching of Windows Pens with the
	//						RCache class.  It is a helper class for RGDIPen
	//
	// ****************************************************************************
	//
	class	FrameworkLinkage RGDIPenInfo
		{
			//	Constructor
			public :	
														RGDIPenInfo( int, int, COLORREF );
														~RGDIPenInfo( );

			//	Operators
			public :
				BOOLEAN								operator==( const RGDIPenInfo& rhs );
				BOOLEAN								operator!=( const RGDIPenInfo& rhs );

			//	Data members
			public :
				int									m_nStyle;
				int									m_nWidth;
				COLORREF								m_Color;
		};

class FrameworkLinkage RGDIPen
	{
	//	Constructor, Destructor
	public :
													RGDIPen( const RGDIPenInfo& pen );
													RGDIPen( HPEN	hpen );
													~RGDIPen( );

	//	Methods
	public :
		BOOLEAN									operator==( const RGDIPen& rhs );
		BOOLEAN									operator!=( const RGDIPen& rhs );

		HPEN										GetPen( );

	//	Members
	private :
		HPEN										m_hPen;
		BOOLEAN									m_fCleanUp;
	} ;

// ****************************************************************************
//
//  Class Name:	RGDIFont
//
//	 Description:	This class allows the caching of Windows Fonts with the
//						RCache class
//
// ****************************************************************************
//
class FrameworkLinkage RGDIFont
	{
	//	Constructor, Destructor
	public :
													RGDIFont( const YFontInfo& fontInfo );
													RGDIFont( HFONT hFont );
													~RGDIFont( );

	//	Methods
	public :
		BOOLEAN									operator==( const RGDIFont& rhs );
		BOOLEAN									operator!=( const RGDIFont& rhs );

		HFONT										GetFont( );

	//	Members
	private :
		HFONT										m_hFont;
		BOOLEAN									m_fCleanUp;
	} ;

// ****************************************************************************
//
//  Class Name:	RGDICaret
//
//	 Description:	This class allows the caching of Windows Carets with the
//						RCache class
//
// ****************************************************************************
//
	// ****************************************************************************
	//
	//  Class Name:	RCaretInfo
	//
	//	 Description:	This class holdes the information for creating a caret
	//						It is a helper class for RGDICaret
	//
	// ****************************************************************************
	//
	class	FrameworkLinkage RGDICaretInfo
		{
			//	Constructor
			public :
													RGDICaretInfo( const RIntPoint& topRight, const RIntPoint& bottomLeft );

			//	Operators
			public :
				BOOLEAN							operator==( const RGDICaretInfo& rhs );
				BOOLEAN							operator!=( const RGDICaretInfo& rhs );

			//	Private Data
			public:
				RIntPoint						m_TopRight;
				RIntPoint						m_BottomLeft;
		};

class FrameworkLinkage RGDICaret
	{
	//	Constructor, Destructor
	public :
													RGDICaret( const RGDICaretInfo& info );
													~RGDICaret( );

	//	Methods
	public :
		BOOLEAN									operator==( const RGDICaret& rhs );
		BOOLEAN									operator!=( const RGDICaret& rhs );

		void										SelectCaret( HWND hwnd );

	//	Members
	private :
		HBITMAP									m_hbmpCaret;
		RIntSize									m_CaretSize;
		RGDICaretInfo							m_CaretInfo;
	} ;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_GDIOBJECTS_H_

