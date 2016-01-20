// ****************************************************************************
//
//  File Name:			MacPrinterDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPrinterDrawingSurface class
//
//  Portability:		Macintosh independent
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
//  $Logfile:: /PM8/Framework/Source/MacPrinterDrawingSurface.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************


#include "FrameworkIncludes.h"

ASSERTNAME

#include "PrinterDrawingSurface.h"


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
RPrinterDrawingSurface::RPrinterDrawingSurface( const THPrint hPrintInfo ) : m_hPrintInfo( hPrintInfo ),
																							 		  m_pPrintPort( NULL ),
																									  m_fResolutionChanged( FALSE )
{
	m_fIsPrinting = TRUE;
	TpsAssert( hPrintInfo, "NULL print info handle" );
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::~RPrinterDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterDrawingSurface::~RPrinterDrawingSurface( )
{
TPrStatus			statusInfo;

	TpsAssert( m_pPrintPort, "NULL print port." );
	TpsAssert( m_hPrintInfo, "NULL print info handle" );

	// close print port
	::PrCloseDoc( m_pPrintPort );

	// spool if necessary  
	if ( ::PrError() == noErr )
		if ( (**m_hPrintInfo).prJob.bJDocLoop == bSpoolLoop )
			::PrPicFile( m_hPrintInfo, nil, nil, nil, &statusInfo );

 	// reset print record dpi
	if ( m_fResolutionChanged )
		DefaultDPI( );
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::Initialize( )
//
//  Description:		Initialization
//
//  Returns:			Boolean
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::Initialize( const GrafPtr pPort, 
														  const RIntRect& boundingRect, 
														  const BOOLEAN fUseMaxResolution ) 
{
	TpsAssert( pPort, "NULL graf ptr" );
	TpsAssert( m_hPrintInfo, "NULL print info handle" );

	// call the base class  
	if ( !RGpDrawingSurface::Initialize( pPort ) )
		return FALSE;

	// set printer to maximum descrete resolution  
	if ( fUseMaxResolution )
		if ( !SetDPI( GetMaxDPI( TRUE ) ) )
			return FALSE;

	// open print port  
	m_pPrintPort = ::PrOpenDoc( m_hPrintInfo, nil, nil );
	if ( m_pPrintPort == NULL || ::PrError() != noErr )
		return FALSE;

	// clip output to printable page 
	::ClipRect( &((**m_hPrintInfo).prInfo.rPage) );				// per old Tech Note #72

	m_rcBounds = boundingRect;
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::BeginPage( )
//
//  Description:		Open print page. 
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::BeginPage( )
{
	TpsAssert( m_pPrintPort, "NULL print port." );
	::PrOpenPage( m_pPrintPort, NULL );
	return( ::PrError() == noErr );
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::EndPage( )
//
//  Description:		Close print page.
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::EndPage( )
{
	TpsAssert( m_pPrintPort, "NULL print port." );
	::PrClosePage( m_pPrintPort );
	return( PrError() == noErr );
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::BeginDoc( )
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
	return TRUE;
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
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::GetDPI( )
//
//  Description:		Gets the current DPI of the device
//
//  Returns:			A size object containing the device DPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RPrinterDrawingSurface::GetDPI( ) const
{
RIntSize 	dpi( 72, 72 );												// mac dpi always defaults to 72

	if ( m_hPrintInfo != NULL )										// check for drawing surface initalization
	{
		dpi.m_dx = (**m_hPrintInfo).prInfo.iHRes;
		dpi.m_dy = (**m_hPrintInfo).prInfo.iVRes;
	}

	return dpi;
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::GetMaxDPI( )
//
//  Description:		Gets the maximum possible DPI of the device
//
//  Returns:			A size object containing the device DPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RPrinterDrawingSurface::GetMaxDPI( const BOOLEAN fDescrete ) const
{
RIntSize 	dpi( 72, 72 );												// mac dpi always defaults to 72
TGetRslBlk	pb;

	// query the drive for printer resolutions
	memset( &pb, 0, sizeof( TGetRslBlk ) );						// always zero param blocks
 	pb.iOpCode = getRslDataOp;
 	pb.iRgType = 1;
	::PrGeneral( (char *)&pb );
	if ( ::PrError() != noErr || pb.iError != noErr )			// check for manager or driver error
		return dpi;

	if ( !fDescrete && 													// descrete resolution request
			pb.xRslRg.iMax == pb.yRslRg.iMax &&						// only interested in square resolutions
			pb.xRslRg.iMax != 0 && pb.yRslRg.iMax != 0 )			// range not available
	{
		dpi.m_dx = pb.xRslRg.iMax;										// maximum of range
		dpi.m_dy = pb.yRslRg.iMax;
		return dpi;
	}

	// loop thru resolutions to find largest
	TRslRec *pRslRec = pb.rgRslRec;
	for ( int i = 0; i < pb.iRslRecCnt; i++ )
	{
		if (pRslRec->iXRsl == pRslRec->iYRsl)						// only interested in square resolutions
			if (pRslRec->iXRsl > dpi.m_dx)
			{
				dpi.m_dx = pRslRec->iXRsl;
				dpi.m_dy = pRslRec->iYRsl;
			}
		pRslRec++;
	}

	return dpi;
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::GetMinDPI( )
//
//  Description:		Gets the minimum possible DPI of the device
//
//  Returns:			A size object containing the device DPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RPrinterDrawingSurface::GetMinDPI( const BOOLEAN fDescrete ) const
{
RIntSize 	dpi( 72, 72 );												// mac dpi always defaults to 72
TGetRslBlk	pb;

	// query the drive for printer resolutions
	memset( &pb, 0, sizeof( TGetRslBlk ) );						// always zero param blocks
 	pb.iOpCode = getRslDataOp;
 	pb.iRgType = 1;
	::PrGeneral( (char *)&pb );
	if ( ::PrError() != noErr || pb.iError != noErr )			// check for manager or driver error
		return dpi;

	if ( !fDescrete && 													// descrete resolution request
			pb.xRslRg.iMin == pb.yRslRg.iMin &&						// only interested in square resolutions
			pb.xRslRg.iMin != 0 && pb.yRslRg.iMin != 0 )			// range not available
	{
		dpi.m_dx = pb.xRslRg.iMin; 									// minimum of range
		dpi.m_dy = pb.yRslRg.iMin;
		return dpi;
	}

	// loop thru resolutions to find smallest
	TRslRec *pRslRec = pb.rgRslRec;
	for ( int i = 0; i < pb.iRslRecCnt; i++ )
	{
		if (pRslRec->iXRsl == pRslRec->iYRsl)						// only interested in square resolutions
			if (pRslRec->iXRsl < dpi.m_dx)
			{
				dpi.m_dx = pRslRec->iXRsl;
				dpi.m_dy = pRslRec->iYRsl;
			}
		pRslRec++;
	}

	return dpi;
}


// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SetDPI( )
//
//  Description:		Sets the DPI of the device
//
//  Returns:			Boolean indicating success
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDrawingSurface::SetDPI( const RIntSize& dpi ) 
{
TSetRslBlk	pb;

	TpsAssert( m_hPrintInfo != NULL, "NULL print info handle" );
	if (dpi.m_dx != dpi.m_dy)										// can only handle square resolutions
		return FALSE;

	memset( &pb, 0, sizeof( TSetRslBlk ) );					// always zero param blocks
 	pb.iOpCode = setRslOp;
 	pb.hPrint = m_hPrintInfo;
 	pb.iXRsl = dpi.m_dx;
 	pb.iYRsl = dpi.m_dy;
	::PrGeneral( (char *)&pb );
	if ( ::PrError() != noErr || pb.iError != noErr )		// check for manager or driver error
		return FALSE;
	
	m_fResolutionChanged = TRUE;
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::DefaultDPI( )
//
//  Description:		Resets the DPI of the device to the system default
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::DefaultDPI( ) 
{
	if ( SetDPI( RIntSize( 0, 0 ) ) )
		m_fResolutionChanged = FALSE;
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
	if ( m_hPrintInfo == NULL )
		return FALSE;
	// printer species belongs to the PostScript LaserWriter driver ancestry
	return( (((**m_hPrintInfo).prStl.wDev & 0x0000FF00L) >> 8) == 0x03 );
}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::IsPrinting( )
//
//  Description:		Called to determine if this surface is printing
//
//  Returns:			TRUE if the surface is being used for printing
//
//  Exceptions:		None
//
// ****************************************************************************
//
//BOOLEAN RPrinterDrawingSurface::IsPrinting( ) const
//{
//	TpsAssert( ( m_pPrintPort != NULL ), "NULL output port." );
//	TpsAssert( ( m_hPrintInfo != NULL ), "NULL print info handle." );
//
//	return TRUE;
//}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::Validate( ) const
{
	RGpDrawingSurface::Validate( );
	TpsAssertIsObject( RPrinterDrawingSurface, this );
	TpsAssert( m_pGrafPort, "NULL port." );
	TpsAssert( m_pPrintPort, "NULL print port." );
	TpsAssert( m_hPrintInfo, "NULL print info handle." );
}

#endif	// TPSDEBUG
