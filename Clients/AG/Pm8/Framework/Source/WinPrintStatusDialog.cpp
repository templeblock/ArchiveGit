//****************************************************************************
//
// File Name:		WinPrintStatusDialog.cpp
//
// Project:			Renaissance Component
//
// Author:			Mike Heydlauf
//
// Description:	Methods for RPrintStatusDialog.  
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
//  $Logfile:: /PM8/Framework/Source/WinPrintStatusDialog.cpp                $
//   $Author:: Gbeddow                                                       $
//     $Date:: 3/03/99 6:18p                                                 $
// $Revision:: 1                                                             $
//
//****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"PrintStatusDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIDs.h"
#include "DrawingSurface.h"
#include "PrinterDrawingSurface.h"

#ifndef	_WINDOWS
	#error	This file must be compilied only on Windows
#endif	//	_WINDOWS

/////////////////////////////////////////////////////////////////////////////
// RPrintStatusDialog dialog

//
//Static Member Initialization
BOOLEAN RPrintStatusDialog::m_fCancelPrint = FALSE;

//*****************************************************************************************************
// Function Name:	RPrintStatusDialog::RPrintStatusDialog
//
// Description:	Constructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RPrintStatusDialog::RPrintStatusDialog( RDrawingSurface* pDrawingSurface, RCancelPrintingSignal* pCancelPrintingSignal ) :
	m_pDrawingSurface( pDrawingSurface ),
	m_pCancelPrintingSignal( pCancelPrintingSignal )
{
	m_sPrinter = "";
	m_sDocName = "";
	m_nCurrentPage = 1;
	m_nTotalPages = 1;
}

//*****************************************************************************************************
// Function Name:	RPrintStatusDialog::~RPrintStatusDialog
//
// Description:	Destructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RPrintStatusDialog::~RPrintStatusDialog()
{
	//
	// Just in case the dialog is getting destructed without the flag already turned on..
	// We need to make sure the event loop terminates and relinquishes control back to the App.
	m_fCancelPrint = TRUE; 
}

//*****************************************************************************************************
// Function Name:	RPrintStatusDialog::DoDataExchange
//
// Description:	Data exchange
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPrintStatusDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RPrintStatusDialog)
	DDX_Text( pDX, CONTROL_EDIT_STATUS_PRINTER, m_sPrinter );
	DDX_Text( pDX, CONTROL_EDIT_STATUS_PRINTING, m_sDocName );
	DDX_Text( pDX, CONTROL_EDIT_STATUS_CURRENTPAGE, m_nCurrentPage );
	DDX_Text( pDX, CONTROL_EDIT_STATUS_TOTALPAGE, m_nTotalPages );
	DDX_Control( pDX, CONTROL_PROGRESS_STATUS_INDICATOR, m_ctrlProgessBar );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RPrintStatusDialog, CDialog)
	//{{AFX_MSG_MAP(RPrintStatusDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RPrintStatusDialog message handlers

//*****************************************************************************************************
// Function Name:	RPrintStatusDialog::OnInitDialog
//
// Description:	Initialize dialog controls.
//
// Returns:			TRUE
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOL RPrintStatusDialog::OnInitDialog() 
{
	CenterWindow( );

	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//*****************************************************************************************************
// Function Name:	RPrintStatusDialog::OnCancel
//
// Description:	Set Cancel flag for print loop. Close Dialog
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPrintStatusDialog::OnCancel()
{
	CDialog::OnCancel();
	m_fCancelPrint = TRUE;
	m_pCancelPrintingSignal->CancelPrinting( );
}

