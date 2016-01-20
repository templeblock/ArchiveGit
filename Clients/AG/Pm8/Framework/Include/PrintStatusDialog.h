//****************************************************************************
//
// File Name:		PrintStatusDialog.h
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
//  $Logfile:: /PM8/Framework/Include/PrintStatusDialog.h             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _PRINTSTATUSDIALOG_H_
#define _PRINTSTATUSDIALOG_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// Forward declarations
class RDrawingSurface;
class RCancelPrintingSignal;

class FrameworkLinkage RPrintStatusDialog : public CDialog
{
// Construction
public:
	RPrintStatusDialog( RDrawingSurface* pDrawingSurface, RCancelPrintingSignal* pCancelPrintingSignal );   
	~RPrintStatusDialog();

	static BOOLEAN m_fCancelPrint;
//	static BOOLEAN IsCanceled();
	inline void SetPrintStatusData( const RMBCString& rPrinter, const RMBCString& rDocName );
	void SetProgress( int nCurrentPage, int nTotalPages, int nElementsDone, int nTotalElements );

protected:
// Dialog Data
	//{{AFX_DATA(RPrintStatusDialog)
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RPrintStatusDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	CString						m_sPrinter;
	CString						m_sDocName;
	int							m_nTotalPages;
	int							m_nCurrentPage;
	CProgressCtrl				m_ctrlProgessBar;
	RDrawingSurface*			m_pDrawingSurface;
	RCancelPrintingSignal*	m_pCancelPrintingSignal;

	// Generated message map functions
	//{{AFX_MSG(RPrintStatusDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//*****************************************************************************************************
// Function Name:	RPrintStatusDialog::SetPrintStatusData
//
// Description:	Set data to be displayed in status dialog
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
inline void RPrintStatusDialog::SetPrintStatusData( const RMBCString& rPrinter, const RMBCString& rDocName )
{
	// REVIEW STA VC5 - Our conversion operator stopped working in VC5, and I dont know why. I have to call it explicitly.
	m_sPrinter = rPrinter.operator CString( );
	m_sDocName = rDocName.operator CString( );
}

//*****************************************************************************************************
// Function Name:	RPrintStatusDialog::SetProgress
//
// Description:	Set print job's progress
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
inline void RPrintStatusDialog::SetProgress( int nCurrentPage, int nTotalPages, int nElementsDone, int nTotalElements )
{
	m_nCurrentPage = nCurrentPage;
	m_nTotalPages = nTotalPages;
	m_ctrlProgessBar.SetRange( 0, nTotalElements );
	m_ctrlProgessBar.SetStep( 1 );
	if ( nTotalElements > 0 )
	{
		m_ctrlProgessBar.SetPos( nElementsDone );
	}
	UpdateData( FALSE );
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_PRINTSTATUSDIALOG_H_