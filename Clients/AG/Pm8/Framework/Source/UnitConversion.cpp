// *****************************************************************************
//
// File name:			UnitConversion.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of unit conversion functions
//
// Portability:		Platform-independent
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
// Client:				Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
//  $Logfile:: /PM8/Framework/Source/UnitConversion.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

// Forward looking definition of GetSystemDPI function
YIntDimension GetSystemDPI( void );

// Instantiation of kSystemDPI
extern FrameworkLinkage YIntDimension kSystemDPI = GetSystemDPI();

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "FrameworkResourceIds.h"

const YIntDimension kDefaultSystemDPI = 1440;

// ****************************************************************************
//
//  Function Name:		GetSystemDPI()
//
//  Description:		Determines the value to use for system DPI
//
//  Returns:			system DPI
//
//  Exceptions:			None
//
// ****************************************************************************
FrameworkLinkage YIntDimension GetSystemDPI( void )
{
	static YIntDimension nSystemDPI = 0;

	if (!nSystemDPI)
	{
		HMODULE hInst = GetModuleHandle( NULL );
		TpsAssert( hInst, "Invalid module handle!" );

		nSystemDPI = kDefaultSystemDPI;
		HRSRC hRsrc = ::FindResource( hInst, "DPI", "SYSTEM" );

		if (hRsrc)
		{
			HGLOBAL hResData = ::LoadResource( hInst, hRsrc );

			if (hResData)
			{
				nSystemDPI = *((WORD *) hResData);
				::FreeResource( hResData );
			}
		}
	}

	return nSystemDPI;
}

// ****************************************************************************
//
//  Function Name:		InchesToMillimeters()
//
//  Description:			Converts inches to millimeters
//
//  Returns:				Millimeters
//
//  Exceptions:			None
//
// ****************************************************************************
YRealDimension InchesToMillimeters(YRealDimension nInches)
{
	return nInches * kMillimetersPerInch;
}

// ****************************************************************************
//
//  Function Name:		MillimetersToInches()
//
//  Description:			Converts millimeters to inches
//
//  Returns:				Inches
//
//  Exceptions:			None
//
// ****************************************************************************
YRealDimension MillimetersToInches(YRealDimension nMillimeters)
{		
	return nMillimeters / kMillimetersPerInch;
}

// ****************************************************************************
//
//  Function Name:		LocalUnitsToInches( )
//
//  Description:			Converts a measurement in the users local system of 
//								measurement to inches
//
//  Returns:				Inches
//
//  Exceptions:			None
//
// ****************************************************************************
//
FrameworkLinkage YRealDimension LocalUnitsToInches( YRealDimension localUnits )
	{
	// Get the converion factor string from the resource manager
	RMBCString conversionString = ::GetResourceManager( ).GetResourceString( STRING_LOCAL_UNIT_TO_INCHES_CONVERSION );

	// Convert this string to a floating point conversion factor
	YFloatType conversionFactor = ::atof( conversionString );

	// Convert the measurement and return it
	return localUnits / conversionFactor;
	}

// ****************************************************************************
//
//  Function Name:		InchesToLocalUnits( )
//
//  Description:			Converts a measurement in inches to the unit of 
//								measurement to users local system
//
//  Returns:				Local Units
//
//  Exceptions:			None
//
// ****************************************************************************
//
FrameworkLinkage YRealDimension InchesToLocalUnits( YRealDimension inches )
	{
	// Get the converion factor string from the resource manager
	RMBCString conversionString = ::GetResourceManager( ).GetResourceString( STRING_LOCAL_UNIT_TO_INCHES_CONVERSION );

	// Convert this string to a floating point conversion factor
	YFloatType conversionFactor = ::atof( conversionString );

	// Convert the measurement and return it
	return inches * conversionFactor;
	}
