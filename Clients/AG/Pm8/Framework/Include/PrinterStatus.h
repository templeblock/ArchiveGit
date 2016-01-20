//****************************************************************************
//
// File Name:		RPrintStatus.h
//
// Project:			Renaissance Component
//
// Author:			Mike Heydlauf
//
// Description:	Definition of RPrintStatusDialog.  Template for each project's  
//						print dialog.
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/PrinterStatus.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _PRINTSTATUS_H_
#define _PRINTSTATUS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// Forward declarations
class RDrawingSurface;
class RCancelPrintingSignal;

// ****************************************************************************
//
//  Class Name:	RPrinterStatus
//
//  Description:	encapsulate printer status.
//
// ****************************************************************************
//
class RPrintStatusDialog;
class FrameworkLinkage RPrinterStatus
	{
	public:
		RPrinterStatus( );
		virtual ~RPrinterStatus( );

		BOOLEAN	SetProgress( YCounter yCurrentPage, YCounter yTotalPages, YCounter yCurrentElementIndex, YCounter yTotalElements );
		BOOLEAN	Reset( );
		void		StartStatus( RDrawingSurface* pDrawingSurface, const RMBCString& rPrinter, const RMBCString& rDocName
			, RCancelPrintingSignal* pCancelPrintingSignal, int nNumElements, int nNumPages );
		void		EndStatus(  );
		void		PrintElement( );
		void		PrintPage( );

		RPrintStatusDialog*		m_pStatusDialog;					//	current printer status dialog
		YCounter						m_yCurrentElementIndex;			//	index of current element being printed
		YCounter						m_yCurrentTotalElements;		//	number of elements in current side of project being printed
		BOOLEAN						m_fWaitCursorOn;
		int							m_nNumElements;
		int							m_nCurrentElement;
		int							m_nNumPages;
		int							m_nCurrentPage;

	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_PRINTSTATUS_H_