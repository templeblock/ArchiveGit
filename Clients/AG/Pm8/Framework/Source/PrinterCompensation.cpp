// ****************************************************************************
//
//  File Name:			PrinterCompensation.cpp
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPrinter class
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
//  $Logfile:: /PM8/Framework/Source/PrinterCompensation.cpp         $Author:: Eric                                                           $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"PrinterCompensation.h"
#include	"PrinterDatabase.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage



// ****************************************************************************
//
// Function Name:		RPrinterCompensation::RPrinterCompensation( const RPrinter& pPrinter, const RIntRect& rNewPrintableArea )
//
// Description:		ctor.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RPrinterCompensation::RPrinterCompensation( RPrinter& rPrinter, const RIntRect& rNewPrintableArea )
: RPrinter( kTemplatePrinter )
	{
	//	Copy in given printer.
	RPrinter* pThisPrinter = static_cast<RPrinter*>(this);
	*pThisPrinter = rPrinter;

	//	Save given printable area in our printable area collection.
	GetActualPrintableArea() = rNewPrintableArea;
	}

// ****************************************************************************
//
// Function Name:		RPrinterCompensation::RPrinterCompensation( RArchive& archive )
//
// Description:		ctor. Read our printer info.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RPrinterCompensation::RPrinterCompensation( RArchive* pArchive, const uBYTE version )
: RPrinter( kTemplatePrinter )
	{
	Read( pArchive );
	
	//	Get the additional information not read by the base method because
	//	of versioning conflicts.
	if ( kVersionDriverPrintableAreaPrinterDB <= version )
		*pArchive >> m_PrintableAreaFromDriver;	
	}


// ****************************************************************************
//
//  Function Name:	RPrinterCompensation::GetActualPrintableArea(  )
//
//  Description:		Return the printable area for the given paper size and orientation.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect& RPrinterCompensation::GetActualPrintableArea(  )
	{
	return CalcPrintableAreaRect( m_PaperSize, m_Orientation, !kUpdatePaperAreas );
	}

// ****************************************************************************
//
// Function Name:		RPrinterCompensation::Update( )
//
// Description:		Do nothing. PrinterCompensations cannot be updated in this fashion.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPrinterCompensation::Update( )
	{
	TpsAssertAlways( "Cannot update a printer specification from a driver." );

	return FALSE;
	}

// ****************************************************************************
//
// Function Name:		RPrinterCompensation::Update( const RIntRect &rActualPrintableArea )
//
// Description:		Replace actual printable area with given area.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPrinterCompensation::Update( const RIntRect &rActualPrintableArea )
	{
	GetActualPrintableArea() = rActualPrintableArea;

	return TRUE;
	}
