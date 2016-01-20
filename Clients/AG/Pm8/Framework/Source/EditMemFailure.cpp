//****************************************************************************
//
// File Name:  EditMemFailure.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Handles REditMemFailure methods used as a programmers interface
//				to the Memory Failure Dialog
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
// Client:		   Broderbund
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
// $Header:: /PM8/Framework $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "EditMemFailure.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

REditMemFailure::REditMemFailure()
{
	NULL;
}

REditMemFailure::~REditMemFailure()
{
	NULL;
}
//****************************************************************************
//
// Function Name: DoModal
//
// Description:   Programmer front end for drawing the Memory Failure dialog
//
// Returns:       TRUE if user pressed OK
//
// Exceptions:    None 
//
//****************************************************************************
BOOLEAN REditMemFailure::DoModal()
{
	int OkorCancel = IDOK;

	m_MemFailDlg.SetData( &m_MemFailData );
	OkorCancel = m_MemFailDlg.DoModal();
	if ( OkorCancel == IDOK )
	{
		m_MemFailDlg.FillData( &m_MemFailData );

	}

	return( static_cast<BOOLEAN>(OkorCancel == IDOK) );
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills a MemoryFailureStruct with the Memory Failure Dlg's data
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void REditMemFailure::FillData( MemoryFailureStruct* pMFData )
{
	*pMFData = m_MemFailData;
}

//****************************************************************************
//
// Function Name: SetData
//
// Description:   Sets the Memory Failure Dlg's data with the MemoryFailureStruct  
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void REditMemFailure::SetData( MemoryFailureStruct* pMFData )
{
	m_MemFailData = *pMFData;
}