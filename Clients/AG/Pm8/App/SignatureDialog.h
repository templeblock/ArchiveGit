/*	$Header: /PM8/App/SignatureDialog.h 1     3/03/99 6:11p Gbeddow $
//
//	Definition of the RSignatureDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a signature compound graphic.
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
// $Log: /PM8/App/SignatureDialog.h $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     2/10/99 5:08p Rgrenfel
// Created a new DisplayGraphic call that takes the new and old component
// documents instead of performing a collection read.
// 
// 1     2/08/99 3:11p Rgrenfel
// Implementation of the signature grouped object dialog.
*/

#ifndef _SIGNATUREDIALOG_H_
#define _SIGNATUREDIALOG_H_

#include "Resource.h"

#ifndef _COMMONGROUPEDOBJECTDIALOG_H_
	#include "CommonGroupedObjectDialog.h"
#endif

#ifndef _CUSTOMGRAPHICSEARCH_H_
	#include "CustomGraphicSearch.h"
#endif

#ifndef _COLLECTIONSEARCHER_H_
	#include "CollectionSearcher.h"
#endif


class RSignatureDialog : public RCommonGroupedObjectDialog
{
// Construction
public:
	RSignatureDialog( RStandaloneDocument* pParentDocument,
							RComponentDocument* pEditedComponent,
							int nResourceId = RSignatureDialog::IDD );
	~RSignatureDialog();

// Dialog Data
	//{{AFX_DATA(RGenericGroupedObjectDialog)
	enum { IDD = IDD_SIGNATURE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGenericGroupedObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void							CreateGraphicsList( RMBCString& rList );
	virtual void							CreateEffectsList( RMBCString& rList );
	virtual void							DisplayGraphic( const SearchResult& rSelGraphic, RComponentDocument*& pGraphicDoc );
	virtual void							DisplayGraphic( RComponentDocument *pNewGraphicDoc, RComponentDocument*& pGraphicDoc );
	virtual const YComponentType&		GetComponentType() const;
	virtual RRealRect						GetDefaultHeadlineRect();
	virtual void							SetupSeperator();
	virtual void							UpdatePreview( BOOLEAN fUpdateHeadline = TRUE, BOOLEAN fUpdateGraphic = TRUE );
	virtual RUIContextData*				ReadUIContextData( ) ;
	virtual void							WriteUIContextData( RUIContextData& contextData ) const;

	RHeadlineInterface*					m_pSeperatorHeadlineInterface;
	RComponentView*						m_pSeperatorHeadlineView;
	RGOHeadlineData						m_rSeperatorHeadlineData;

	RCollectionSearcher<RSignatureGraphicSearchCollection>*	m_pGraphicCollectionSearcher;

// Generated message map functions
	//{{AFX_MSG(RGenericGroupedObjectDialog)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSelchangeListboxGraphics();
	virtual afx_msg void OnSelchangeListboxEffects();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _SIGNATUREDIALOG_H_