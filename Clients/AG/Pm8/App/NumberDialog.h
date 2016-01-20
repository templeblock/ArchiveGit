/*	$Header: /PM8/App/NumberDialog.h 1     3/03/99 6:08p Gbeddow $
//
//	Definition of the RNumberDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a number compound graphic.
//	
// Author:     Mike Heydlauf
//
// Portability: Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
// $Log: /PM8/App/NumberDialog.h $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 1     2/08/99 3:10p Rgrenfel
// Implementation of the smart number grouped object dialog.
*/

#ifndef _NUMBERDIALOG_H_
#define _NUMBERDIALOG_H_

#include "Resource.h"

#ifndef _COMMONGROUPEDOBJECTDIALOG_H_
	#include "InitCapDialog.h"
#endif

#ifndef _CUSTOMGRAPHICSEARCH_H_
	#include "CustomGraphicSearch.h"
#endif

#ifndef _COLLECTIONSEARCHER_H_
	#include "CollectionSearcher.h"
#endif

// ****************************************************************************
//
//	The Number dialog is a Init Cap dialog.  The only additional funcionality
// the number dialog has is the smart number checkbox.  All the effects that
// can be applied to a init cap can be applied to a number, therefore I just
// derived RNumberDialog from InitCapDialog rather than deriving from 
// RCommonGroupedObjectDialog and redefining all the common functionality.
//
// ****************************************************************************
class RNumberDialog : public RInitCapDialog
{
// Construction
public:
	RNumberDialog( RStandaloneDocument* pParentDocument,
						RComponentDocument* pEditedComponent,
						int nResourceId = RNumberDialog::IDD );

	virtual ~RNumberDialog();

// Dialog Data
	//{{AFX_DATA(RGenericGroupedObjectDialog)
	enum { IDD = IDD_NUMBER };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGenericGroupedObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual	void															CreateGraphicsList( RMBCString& rList );
	virtual const YComponentType&										GetComponentType( ) const;
	virtual	void															DisplayNumber();
	virtual	RUIContextData*											ReadUIContextData( ) ;
	virtual	void															WriteUIContextData( RUIContextData& contextData ) const;
	RMBCString																GetSmartNumberSuffix( const RMBCString& rNumber );
	void																		StripSmartNumber( RMBCString& rNumber );

	RCollectionSearcher<RNumberGraphicSearchCollection>*		m_pGraphicCollectionSearcher;

	BOOLEAN																	m_fSmartNumber;

	// Generated message map functions
	//{{AFX_MSG(RGenericGroupedObjectDialog)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnEditChangeHeadlineText();
	virtual afx_msg void OnButtonNumberSmartNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _NUMBERDIALOG_H_