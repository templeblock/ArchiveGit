/*	$Header: /PM8/App/LogoMakerDialog.h 1     3/03/99 6:07p Gbeddow $
//
//	Definition of the RLogoMakerDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a logo compound graphic.
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
// $Log: /PM8/App/LogoMakerDialog.h $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 1     2/08/99 3:10p Rgrenfel
// Implementation of the logo grouped object dialog.
*/

#ifndef _LOGOMAKERDIALOG_H_
#define _LOGOMAKERDIALOG_H_

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


class RLogoMakerDialog : public RCommonGroupedObjectDialog
{
// Construction
public:
	RLogoMakerDialog( RStandaloneDocument* pParentDocument,
							RComponentDocument* pEditedComponent,
							int nResourceId = RLogoMakerDialog::IDD );

	~RLogoMakerDialog();

// Dialog Data
	//{{AFX_DATA(RGenericGroupedObjectDialog)
	enum { IDD = IDD_LOGO };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGenericGroupedObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void															CreateGraphicsList( RMBCString& rList );
	virtual void															CreateEffectsList( RMBCString& rList );
	virtual const YComponentType&										GetComponentType( ) const;
	virtual void															ApplyHeadlineEffect();
	virtual void															UpdateGraphicDocument( RComponentDocument*& pTempGraphicDoc , RComponentDocument*& pGraphicDoc );
	virtual void															SetDefaultGraphicBoundingRect( );

	virtual void															WriteUIContextData( RUIContextData& contextData ) const;
	virtual RUIContextData*												ReadUIContextData();
	virtual RRealRect														GetDefaultHeadlineRect();

	RCollectionSearcher<RLogoGraphicSearchCollection>*			m_pGraphicCollectionSearcher;

	// Static table of canned headline effects data
	static const RGOHeadlineData										m_rCannedHeadlineEffects[];
// Generated message map functions
	//{{AFX_MSG(RGenericGroupedObjectDialog)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSelchangeListboxEffects();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _LOGOMAKERDIALOG_H_