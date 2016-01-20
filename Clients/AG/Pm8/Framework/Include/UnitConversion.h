// ****************************************************************************
//
//  File Name:			UnitConversion.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of unit conversion functions
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
//  $Logfile:: /PM8/Framework/Include/UnitConversion.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

const YFloatType kMillimetersPerInch = 25.4;

FrameworkLinkage YRealDimension InchesToMillimeters(YRealDimension yInches);
FrameworkLinkage YRealDimension MillimetersToInches(YRealDimension yMillimeters);
FrameworkLinkage YRealDimension LocalUnitsToInches( YRealDimension localUnits );
FrameworkLinkage YRealDimension InchesToLocalUnits( YRealDimension inches );

// System DPI - A number large enough for Text metric resolution.
extern FrameworkLinkage YIntDimension kSystemDPI;

//	The printer point is the constant size for use by typography TextMetrics.
//	Please do not confuse it with the number of pixels in a printer inch.
const	YIntDimension	kPrinterPoint	= 72;

// KDM 2/18/98 Moved from WinBitMapImage.cpp to here
// Assumed DPI for imported images 
const uLONG kDefaultDIBXDpi = 200;
const uLONG kDefaultDIBYDpi = 200;

// ****************************************************************************
//
//  Function Name:	GetScreenDPI
//
//  Description:		Gets the DPI of the screen. This function should only be
//							called in those cases where neither a view nor a drawing
//							surface is available.
//
//  Returns:			void
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
inline RRealSize GetScreenDPI( )
	{
#ifdef _WINDOWS
	HDC hdc = ::CreateCompatibleDC( NULL );
	RRealSize temp( ::GetDeviceCaps( hdc, LOGPIXELSX ), ::GetDeviceCaps( hdc, LOGPIXELSY ) );
	::DeleteDC( hdc );
	return temp;
#elif defined(MAC)
	const int kMacDPI = 72;
	return RRealSize( kMacDPI, kMacDPI );
#endif 
	}

// ****************************************************************************
//
//  Function Name:	LogicalUnitsToDeviceUnits( )
//
//  Description:		Template function which converts a structure in logical
//							units to one in device units. Template variable T is the
//							structure to be converted, and must have an implementation
//							of Scale( ). Template variable S is a class used to do the
//							conversion, and must have an implementation of GetDPI( ).
//
//							T may be: RPoint, RSize, RRect, RVectorRect
//							S may be: RDrawingSurface, RView
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class S> void LogicalUnitsToDeviceUnits( T& t, const S& s )
	{
	// Get the dpi
	RRealSize dpi = s.GetDPI( );

	// Create the scaling factor
	RRealSize scalingFactor( dpi.m_dx / kSystemDPI, dpi.m_dy / kSystemDPI );

	// Scale
	t.Scale( scalingFactor );
	}

// ****************************************************************************
//
//  Function Name:	DeviceUnitsToLogicalUnits( )
//
//  Description:		Template function which converts a structure in device
//							units to one in logical units. Template variable T is the
//							structure to be converted, and must have an implementation
//							of Scale( ). Template variable S is a class used to do the
//							conversion, and must have an implementation of GetDPI( ).
//
//							T may be: RPoint, RSize, RRect, RVectorRect
//							S may be: RDrawingSurface, RView
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T, class S> void DeviceUnitsToLogicalUnits( T& t, const S& s )
	{
	// Get the dpi
	RRealSize dpi = s.GetDPI( );

	// Create the scaling factor
	RRealSize scalingFactor( kSystemDPI / dpi.m_dx, kSystemDPI / dpi.m_dy );

	// Scale
	t.Scale( scalingFactor );
	}

// ****************************************************************************
//
//  Function Name:	LogicalUnitsToScreenUnits( )
//
//  Description:		Template function which converts a structure in logical
//							units to one in screen units. Template variable T is the
//							structure to be converted, and must have an implementation
//							of Scale( ).
//
//							T may be: RPoint, RSize, RRect, RVectorRect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void LogicalUnitsToScreenUnits( T& t )
	{
	// Get the dpi
	RRealSize dpi = ::GetScreenDPI( );

	// Create the scaling factor
	RRealSize scalingFactor( dpi.m_dx / kSystemDPI, dpi.m_dy / kSystemDPI );

	// Scale
	t.Scale( scalingFactor );
	}

// ****************************************************************************
//
//  Function Name:	ScreenUnitsToLogicalUnits( )
//
//  Description:		Template function which converts a structure in screen
//							units to one in logical units. Template variable T is the
//							structure to be converted, and must have an implementation
//							of Scale( ).
//
//							T may be: RPoint, RSize, RRect, RVectorRect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void ScreenUnitsToLogicalUnits( T& t )
	{
	// Get the dpi
	RRealSize dpi = ::GetScreenDPI( );

	// Create the scaling factor
	RRealSize scalingFactor( kSystemDPI / dpi.m_dx, kSystemDPI / dpi.m_dy );

	// Scale
	t.Scale( scalingFactor );
	}

// ****************************************************************************
//
//  Function Name:	PrinterPointToLogicalUnit( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YIntDimension PrinterPointToLogicalUnit( YIntDimension n )
	{
	return (n * kSystemDPI) / kPrinterPoint;
	}

// ****************************************************************************
//
//  Function Name:	LogicalUnitToPrinterPoint( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YIntDimension LogicalUnitToPrinterPoint( YIntDimension n )
	{
	return (n * kPrinterPoint) / kSystemDPI;
	}

// ****************************************************************************
//
//  Function Name:	InchesToLogicalUnits( )
//
//  Description:		Converts the given distance in inches into a distance in
//							Renaissance logical units
//
//  Returns:			distance in logical units
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline YIntDimension InchesToLogicalUnits( T inches )
	{
	return (YIntDimension)( inches * kSystemDPI );
	}

// ****************************************************************************
//
//  Function Name:	InchesToLogicalUnits( )
//
//  Description:		Converts the given distance in inches into a distance in
//							Renaissance logical units
//
//  Returns:			distance in logical units
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YIntDimension InchesToLogicalUnits( YFloatType inches )
	{
	return ::Round( inches * kSystemDPI );
	}

// ****************************************************************************
//
//  Function Name:	LogicalUnitsToInches( )
//
//  Description:		Converts the given distance in logical units into a
//							distance  in inches
//
//  Returns:			distance in inches
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFloatType LogicalUnitsToInches( YIntDimension logicalUnits )
	{
	return (YFloatType)logicalUnits / kSystemDPI;
	}

// ****************************************************************************
//
//  Function Name:	InchesToPixels( )
//
//  Description:		Converts inches to pixels
//
//  Returns:			Length in pixels
//
//  Exceptions:		None
//
// ****************************************************************************
inline uLONG InchesToPixels(YRealDimension yInches, uLONG uDpi)
{
	return (uLONG)(yInches * (YRealDimension)uDpi);
}

// ****************************************************************************
//
//  Function Name:	PixelsToInches()
//
//  Description:		Converts pixels to inches
//
//  Returns:			Length in inches
//
//  Exceptions:		None
//
// ****************************************************************************
inline YRealDimension PixelsToInches(uLONG uPixels, uLONG uDpi)
{
	return (YRealDimension)uPixels / (YRealDimension)uDpi;
}

// ****************************************************************************
//
//  Function Name:	PelsPerMeterToDpi()
//
//  Description:		Converts PelsPerMeter to Dpi
//
//  Returns:			Dpi
//
//  Exceptions:		None
//
// ****************************************************************************
inline uLONG PelsPerMeterToDpi(uLONG uPelsPerMeter)
{
	const YRealDimension kMillimetersPerMeter = 1000.0;
	return (uLONG)((YRealDimension)uPelsPerMeter * (1.0 / ::MillimetersToInches(kMillimetersPerMeter)));
}

// ****************************************************************************
//
//  Function Name:	DpiToPelsPerMeter()
//
//  Description:		Converts Dpi to PelsPerMeter
//
//  Returns:			PelsPerMeter
//
//  Exceptions:		None
//
// ****************************************************************************
inline uLONG DpiToPelsPerMeter(uLONG uDpi)
{
	const YRealDimension kMillimetersPerMeter = 1000.0;
	return (uLONG)((YRealDimension)uDpi * ::MillimetersToInches(kMillimetersPerMeter));
}
