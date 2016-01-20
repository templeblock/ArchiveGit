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
//  $Logfile:: /PM8/Framework/Include/PrintStatus.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _PRINTSTATUS_H_
#define _PRINTSTATUS_H_

//
// Forward declarations
class RDrawingSurface;

// ****************************************************************************
//
//  Class Name:	RPrinterStatus
//
//  Description:	encapsulate printer status.
//
// ****************************************************************************
//
class RPrintStatusDialog;
class RPrinterStatus
	{
	public:
		RPrinterStatus( );
		virtual ~RPrinterStatus( );

		BOOLEAN	SetProgress( YCounter yCurrentPage, YCounter yTotalPages, YCounter yCurrentElement, YCounter yTotalElements );
		void		StartStatus( RDrawingSurface* pDrawingSurface, const RMBCString& rPrinter, const RMBCString& rDocName );
		void		EndStatus(  );

		RPrintStatusDialog*		m_pStatusDialog;					//	current printer status dialog
		YCounter						m_yCurrentElementIndex;			//	index of current element being printed
		YCounter						m_yCurrentTotalElements;		//	number of elements in current side of project being printed
		BOOLEAN						m_fWaitCursorOn;

	};

#endif	//	_PRINTSTATUS_H_