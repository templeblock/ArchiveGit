// ****************************************************************************
//
//  File Name:			PrinterCompensation.h
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RPrinter class
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
//  $Logfile:: /PM8/Framework/Include/PrinterCompensation.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PRINTERCOMPENSATION_H_
#define		_PRINTERCOMPENSATION_H_

#include "Printer.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RPrinterCompensation
//
//  Description:	Encapsulate info for a printer whose printable area differs
//						from the area reported by its driver.
//
// ****************************************************************************
//
class FrameworkLinkage RPrinterCompensation : public RPrinter
	{	
	//	Operations :
	public :
								RPrinterCompensation( RArchive* pArchive, const uBYTE version );
								RPrinterCompensation( RPrinter& pPrinter, const RIntRect& rNewPrintableArea );
		virtual BOOLEAN	Update( );
		BOOLEAN				Update( const RIntRect &rActualPrintableArea );
		RIntRect&			GetActualPrintableArea(  );
	};

//	Typedefs.
class FrameworkLinkage RPrinterCompensationCollection: public RArray<RPrinterCompensation> {};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_PRINTERCOMPENSATION_H_
