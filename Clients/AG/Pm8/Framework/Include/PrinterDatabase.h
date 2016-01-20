// ****************************************************************************
//
//  File Name:			RPrinterDatabase.h
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
//  $Logfile:: /PM8/Framework/Include/PrinterDatabase.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PRINTERDATABASE_H_
#define		_PRINTERDATABASE_H_

#include	"PrinterCompensation.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forward declarations.
class RFileStream;

const uBYTE kVersionInitialPrinterDB = 1;
const uBYTE kVersionDriverPrintableAreaPrinterDB = kVersionInitialPrinterDB + 1;		//	This version of printer dbs contains driver printable area info.
const uBYTE kVersionCurrentPrinterDB( kVersionDriverPrintableAreaPrinterDB );
const RMBCString kDbInitialFileHeader("Printer Database");
const RMBCString kDbDriverVersionFileHeader("Printer Database with driver version info");

// ****************************************************************************
//
//  Class Name:	RPrinterDatabase
//
//  Description:	Encapsulates info for previously connected printers.
//
// ****************************************************************************
//
class FrameworkLinkage RPrinterDatabase
	{
	// Construction, destruction & Initialization
	public :
														RPrinterDatabase( );
		virtual										~RPrinterDatabase( );

		// public operations
	public :
		static RPrinterDatabase*				Startup( const RMBCString& rDatabaseFilename );
		RPrinterCompensationCollection&		GetPrinterCompensationCollection( );
		RFileStream*&								GetDatabaseFile( );
		void											UpdatePrinter( RPrinter* pPrinter, const RIntRect& rNewPrintableArea );
		BOOLEAN										UpdatePrinter( RPrinter* pPrinter );
		void											Update( );
		void											Shutdown( );
		RPrinterCompensation*					FindPrinter( RPrinter* pPrinter );
		uBYTE											GetVersion( ) const;
		void											SetVersion( const uBYTE version );
	
	// accessors
	public :

	// Member data
	private :
		RPrinterCompensationCollection		m_ActualPrinterInfo;	//	saved info about printers
		BOOLEAN										m_fChanged;
		uBYTE											m_yVersion;
		RFileStream*								m_pFile;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_PRINTERDATABASE_H_
