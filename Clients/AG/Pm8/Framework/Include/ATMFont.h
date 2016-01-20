// ****************************************************************************
//
//  File Name:			ATMFont.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RATMFont class
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
//  $Logfile:: /PM8/Framework/Include/ATMFont.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ATMFONT_H_
#define		_ATMFONT_H_

#ifdef	MAC
	#ifdef	powerc
		#ifndef	_H_ATMInterface
			#include "ATMInterface.h"
		#endif
	#else
		#define ATM_NOERR		(0)	/* Normal return */
		#define ATM_NO_VALID_FONT	(-1) 
		#define ATM_MEMORY	(-4)	/* Ran out of memory */
		typedef long ATMFixed; 
		typedef struct 
		{
  			ATMFixed x, y;
		} ATMFixedPoint;
		typedef struct
		{
			ATMFixed a, b, c, d, tx, ty;
		} ATMFixedMatrix;
	#endif	// power mac
#else
	#ifndef	_H_ATM
		#include "Atm.h"
	#endif
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RATMFont
//
//  Description:	The Renaissance ATM Font information class. All information
//						relavent to a font will be retrieved from this class.
//
// ****************************************************************************
//
class	FrameworkLinkage RATMFont : public RFont
{
	//	Construction, Destruction & Initialization
	public :
												RATMFont( const YFontInfo& info, BOOLEAN fInitialize );
		virtual 								~RATMFont( );

	//	Operations
	public :
		virtual RPath*						GetGlyphPath( const RCharacter& character );

#ifdef _WINDOWS
	// static wrapper functions to make ATM functions available from outside framework
	public :
		static BOOL ATMFontAvailable(
			LPSTR    lpFacename,
			int      nWeight,
			BYTE     cItalic,
			BYTE     cUnderline,
			BYTE     cStrikeOut,
			int *    lpFromOutline )
		{
			return ::ATMFontAvailable( lpFacename, nWeight, cItalic, cUnderline, cStrikeOut, lpFromOutline );
		}
		static WORD ATMGetVersion()
		{
			return ::ATMGetVersion();
		}
#endif // _WINDOWS

	// Implementation
	private :
		BOOLEAN								ExtractCharacterOutline( int character, uLONG cookie ) const;

	// Member Data
	private :
		HTMEM									m_hPath;
#ifdef MAC
		UniversalProcPtr					m_MoveToUPP;
		UniversalProcPtr					m_LineToUPP;
		UniversalProcPtr					m_CurveToUPP;
		UniversalProcPtr					m_ClosePathUPP;
#endif

	//	Private structures for ATMFont class use
	public :
		// type defs 
		typedef struct tagYATMSegmentInfoRecord
		{
			short							opCode; 
			short							padding; 
			ATMFixedPoint				pt1;
			ATMFixedPoint				pt2;
			ATMFixedPoint				pt3;
		}	YATMSegmentInfoRecord;

	// local array and iterator defs
	typedef	RArray<YATMSegmentInfoRecord>				YATMSegmentInfoArray;
	typedef	YATMSegmentInfoArray::YIterator			YATMSegmentInfoArrayIterator;


#ifdef	TPSDEBUG
	//	Debugging Support
	public :
		virtual	void						Validate( ) const;
#endif
} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_ATMFONT_H_
