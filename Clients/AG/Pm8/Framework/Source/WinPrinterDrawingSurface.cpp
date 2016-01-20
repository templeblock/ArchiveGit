// ****************************************************************************
//
//  File Name:			WinPrinterDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				Eric VanHelene
//
//  Description:		Definition of the RPrinterDrawingSurface class
//
//  Portability:		Windows dependent
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
//  $Logfile:: /PM8/Framework/Source/WinPrinterDrawingSurface.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************


#include "FrameworkIncludes.h"

ASSERTNAME

#include "PrinterDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef _WINDOWS
#error this is a windows only file
#endif

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::RPrinterDrawingSurface( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterDrawingSurface::RPrinterDrawingSurface( RCancelPrintingSignal* pCancelPrintingSignal ) 
	: RDcDrawingSurface( FALSE, TRUE ),
	  m_fPageStarted( FALSE ),
	  m_fDocStarted( FALSE ),
	  m_pCancelPrintingSignal( pCancelPrintingSignal )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::~RPrinterDrawingSurface( )
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterDrawingSurface::~RPrinterDrawingSurface( ) 
	{
	//	Clean finish printing.
	if( m_fPageStarted )
		EndPage();

	if ( m_fDocStarted )
		EndDoc();
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::RPrinterDrawingSurface::Initialize( HDC hOutputDC, HDC hAttributeDC )
//
//  Description:		Prepare the drawing surface for use.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::Initialize( HDC hOutputDC, HDC hAttributeDC )
	{
	//	Remember to use the printer palette.
	m_fUseScreenPalette = FALSE;

	//	Proceed with our initialization.
	BOOLEAN fSuccess = RDcDrawingSurface::Initialize( hOutputDC, hAttributeDC );

	return fSuccess;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::Initialize( const RIntRect& boundingRect )
//
//  Description:		Install the drawing surface.
//
//  Returns:			TRUE: Success
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::Initialize( const RIntRect& /*boundingRect*/  )
	{
	UnimplementedCode();
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::fIsPostScript( )
//
//  Description:		Deturmines if the device is a postscript device
//
//  Returns:			Boolean
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::fIsPostScript( ) const
{
	int	escCmd	= POSTSCRIPT_DATA;	//	This is only an int because Windows says it must
	return Escape(m_hOutputDC, QUERYESCSUPPORT, sizeof(int), (LPSTR)&escCmd, NULL);
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::GetDPI( ) const
//
//  Description:		Gets the DPI of the device
//
//  Returns:			A size object containing the device DPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RPrinterDrawingSurface::GetDPI( ) const
	{
	return RDcDrawingSurface::GetDPI( );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::BeginDoc( const RDocInfo* pDocInfo )
//
//  Description:		Start a document on the printer.
//
//  Returns:			TRUE: call succeeded.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::BeginDoc( )
	{
	CheckPrintingCanceled( );

	TpsAssert( m_hOutputDC, "No output dc." );
	TpsAssert( !m_fDocStarted, "Starting a new doc without ending the last one." );
	TpsAssert( !m_fPageStarted, "Starting a new doc without ending the last page." );

	if ( m_fPageStarted )
		EndPage();

	//	Dont give the driver a chance to screw up our dc.
	//	Remove our objects and put them back in after the gdi call.
	HDC hOutputDC = m_hOutputDC;
	HDC hAttributeDC = m_hAttributeDC;
	DetachDCs();
	
	m_fDocStarted = static_cast<BOOLEAN>( 0 < ::StartDoc( hOutputDC, &(m_DocInfo.m_DocInfo) ) );

	//	Restore our objects.
	Initialize( hOutputDC, hAttributeDC );

	return m_fDocStarted;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::EndDoc( )
//
//  Description:		Ends a document on the printer.
//
//  Returns:			TRUE: call succeeded.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::EndDoc( )
	{
	TpsAssert( m_hOutputDC, "No output dc." );

	if ( m_fPageStarted )
		EndPage();

	//	::EndDoc succeeds with a return value > 0 so the doc is no longer started if the return value <= 0
	if( m_fDocStarted )
		{
		//	Dont give the driver a chance to screw up our dc.
		//	Remove our objects and put them back in after the gdi call.
		HDC hOutputDC = m_hOutputDC;
		HDC hAttributeDC = m_hAttributeDC;
		DetachDCs();

		m_fDocStarted = static_cast<BOOLEAN>( 0 >= ::EndDoc( hOutputDC ) );
		
		//	Restore our objects.
		Initialize( hOutputDC, hAttributeDC );
		}

	return static_cast<BOOLEAN>( !m_fDocStarted );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::BeginPage( )
//
//  Description:		Starts a page on the printer.
//
//  Returns:			TRUE: call succeeded.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::BeginPage( )
	{
	CheckPrintingCanceled( );

	TpsAssert( m_hOutputDC, "No output dc." );
	TpsAssert( m_fDocStarted, "Starting a new page without starting a doc." );
	TpsAssert( !m_fPageStarted, "Starting a new page without ending the last one." );

	if( !m_fPageStarted )
		{
		//	Dont give the driver a chance to screw up our dc.
		//	Remove our objects and put them back in after the gdi call.
		HDC hOutputDC = m_hOutputDC;
		HDC hAttributeDC = m_hAttributeDC;
		DetachDCs();

		m_fPageStarted = static_cast<BOOLEAN>( 0 < ::StartPage( hOutputDC ) );

		//	Restore our objects.
		Initialize( hOutputDC, hAttributeDC );
		}
	
	//	Try to make pages print whether we draw something on them or not.
	//	Some drivers skip blank pages. Try to defeat this behaviour by drawing
	//	something harmless on every page.
	//	Disable coloring book mode while doing this.
	BOOLEAN fColoringBookModeOn = GetColoringBookMode();
	SetColoringBookMode( FALSE );
	RIntRect rClipRect( GetClipRect() );
	RColor rOldPenColor( GetPenColor() );
	
	//	Use a slightly off-white color.
	SetPenColor( RSolidColor( (YColorComponent)0, (YColorComponent)0, (YColorComponent)1 ) );
	MoveTo( RIntPoint( 0,0 ) );
	LineTo( RIntPoint( 0,1 ) );
	
	SetPenColor( rOldPenColor );

	SetColoringBookMode( fColoringBookModeOn );

	return m_fPageStarted;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::EndPage( )
//
//  Description:		Ends a page on the printer.
//
//  Returns:			TRUE: call succeeded.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::EndPage( )
	{
	TpsAssert( m_hOutputDC, "No output dc." );

	//	::EndPage succeeds with a return value > 0 so the page is no longer started if the return value <= 0
	if( m_fPageStarted )
		{
		//	Dont give the driver a chance to screw up our dc.
		//	Remove our objects and put them back in after the gdi call.
		HDC hOutputDC = m_hOutputDC;
		HDC hAttributeDC = m_hAttributeDC;
		DetachDCs();

		m_fPageStarted = static_cast<BOOLEAN>( 0 >= ::EndPage( hOutputDC ) );

		//	Restore our objects.
		Initialize( hOutputDC, hAttributeDC );
		}

	return static_cast<BOOLEAN>( !m_fPageStarted );
	}

