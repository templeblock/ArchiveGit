/*	$Header: /PM8/App/InitCapDialog.h 1     3/03/99 6:06p Gbeddow $
//
//	Definition of the RInitCapDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of an initial capital compound graphic.
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
// $Log: /PM8/App/InitCapDialog.h $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 1     2/08/99 3:10p Rgrenfel
// Implementation of the initial capitals grouped object dialog.
*/

#ifndef _INITCAPDIALOG_H_
#define _INITCAPDIALOG_H_

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

class RColorSettingsInterface;
class RInitCapDialog : public RCommonGroupedObjectDialog
{
// Construction
public:
	RInitCapDialog( RStandaloneDocument* pParentDocument,
						 RComponentDocument* pEditedComponent,
						 int nResourceId = RInitCapDialog::IDD );

	virtual ~RInitCapDialog();

// Dialog Data
	//{{AFX_DATA(RGenericGroupedObjectDialog)
	enum { IDD = IDD_INITCAP };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGenericGroupedObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	enum ECannedInitialEffects { kPlain = 0, kBlendDown, kBlendRight, kBlendLeft, kRadialBlend, kOutline, kGhost, kNeon, kBlur, kDropShadowRight, kDropShadowLeft, kSoftShadowRight, kSoftShadowLeft, kStackRight, kStackLeft, kStackBottom, kPerspective, kNumberOfCannedInitialEffects };

// Implementation
protected:
	virtual void															UpdatePreview( BOOLEAN fUpdateHeadline = TRUE, BOOLEAN fUpdateGraphic = TRUE );
	virtual void															CreateGraphicsList( RMBCString& rList );
	virtual void															CreateEffectsList( RMBCString& rList );
	virtual const YComponentType&										GetComponentType( ) const;
	virtual void															ApplyHeadlineEffect();

	virtual void															WriteUIContextData( RUIContextData& contextData ) const;
	virtual RUIContextData*												ReadUIContextData();

	RCollectionSearcher<RInitCapGraphicSearchCollection>*		m_pGraphicCollectionSearcher;

	// Static table of canned headline effects data
	//static const RGOHeadlineData										m_rCannedInitialEffects[ kNumberOfCannedInitialEffects ];

	RSolidColor																m_rSolidColorEffect;
	BOOLEAN																	m_fIsGradient;

	// Generated message map functions
	//{{AFX_MSG(RGenericGroupedObjectDialog)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSelchangeListboxEffects();
	virtual afx_msg void OnButtonGOEffectColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _INITCAPDIALOG_H_