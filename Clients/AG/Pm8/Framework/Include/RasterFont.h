// ****************************************************************************
//
//  File Name:			RasterFont.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RRasterFont class
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
//  $Logfile:: /PM8/Framework/Include/RasterFont.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_RASTERFONT_H_
#define		_RASTERFONT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RRasterFont
//
//  Description:	The Renaissance RasterFont information class. All information
//						relavent to a font will be retrieved from this class.
//
// ****************************************************************************
//
class	FrameworkLinkage RRasterFont : public RFont
	{
	//	Construction, Destruction & Initialization
	public :
													RRasterFont( const YFontInfo& info, BOOLEAN fInitialize );

	//	Operations
	public :
		virtual RPath*							GetGlyphPath( const RCharacter& character );

#ifdef	TPSDEBUG
	//	Debugging Support
	public :
		virtual	void							Validate( ) const;
#endif
	} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_RASTERFONT_H_
