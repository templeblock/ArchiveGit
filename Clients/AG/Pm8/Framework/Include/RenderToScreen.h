//****************************************************************************
//
// File Name:		RenderToScreen.cpp
//
// Project:			Renaissance application framework
//
// Author:			Mike Houle
//
// Description:	Debugging Code
//
// Portability:	Windows Specific
//
// Developed by:  Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/RenderToScreen.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef	_RENDERTOSCREEN_H_
#define _RENDERTOSCREEN_H_

#include "OffscreenDrawingSurface.h"
#include "BitmapImage.h"

#ifndef _WINDOWS
	#error	Only compile under windows...
#endif

inline void RenderToScreen( RDrawingSurface& ds, const RIntRect& rcSource )
{
#ifdef	TPSDEBUG
	RIntRect	rcDest( 0, 0, rcSource.Width(), rcSource.Height() );
	HDC	hdc = ::GetDC( HWND_DESKTOP );
	RDcDrawingSurface	screenDS;
	screenDS.Initialize( hdc, hdc );
	screenDS.BlitDrawingSurface(ds, rcSource, rcDest, kBlitSourceCopy);
	screenDS.DetachDCs( );
	VERIFY( ::ReleaseDC( HWND_DESKTOP, hdc ) );
#else
ds;	//	Remove warnings of unused parameters
rcSource;
#endif
}

inline void RenderToScreen( RBitmapImage& rBitmap )
{
#ifdef	TPSDEBUG
	HDC	hdc = ::GetDC( HWND_DESKTOP );
	RDcDrawingSurface	screenDS;
	screenDS.Initialize( hdc, hdc );

	RIntSize rSize( rBitmap.GetWidthInPixels( ), rBitmap.GetHeightInPixels( ) );
	rBitmap.Render( screenDS, rSize );

	screenDS.DetachDCs( );
	VERIFY( ::ReleaseDC( HWND_DESKTOP, hdc ) );

#else
rBitmap;	//	Remove warnings of unused parameters
#endif
}

#endif	// _RENDERTOSCREEN_H_

