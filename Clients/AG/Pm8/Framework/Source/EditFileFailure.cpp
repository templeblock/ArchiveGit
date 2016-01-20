//****************************************************************************
//
// File Name:  EditMemFailure.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Handles REditFileFailure methods used as a programmers interface
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

#include "EditFileFailure.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

REditFileFailure::REditFileFailure()
{
	NULL;
}

REditFileFailure::~REditFileFailure()
{
	NULL;
}
//****************************************************************************
//
// Function Name: DoModal
//
// Description:   Programmer front end for drawing the File Failure dialog
//
// Returns:       TRUE if user pressed OK
//
// Exceptions:    None 
//
//****************************************************************************
BOOLEAN REditFileFailure::DoModal()
{
	int OkorCancel = IDOK;

	m_FileFailDlg.SetData( &m_FileFailData );
	OkorCancel = m_FileFailDlg.DoModal();
	if ( OkorCancel == IDOK )
	{
		m_FileFailDlg.FillData( &m_FileFailData );

	}

	return( static_cast<BOOLEAN>(OkorCancel == IDOK) );
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills a FileFailureStruct with the File Failure Dlg's data
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void REditFileFailure::FillData( FileFailureStruct* pMFData )
{
	*pMFData = m_FileFailData;
}

//****************************************************************************
//
// Function Name: SetData
//
// Description:   Sets the File Failure Dlg's data with the FileFailureStruct  
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void REditFileFailure::SetData( FileFailureStruct* pMFData )
{
	m_FileFailData = *pMFData;
}