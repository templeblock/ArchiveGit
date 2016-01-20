//****************************************************************************
//
// File Name: BehindEffectsTab.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Manages CBehindEffectsTab class which is used for the Outline 
//				Property Page
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Source/BehindEffectsTab.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "HeadlineIncludes.h"
ASSERTNAME

#ifndef	WIN
	#error	This file can only be compilied on Windows
#endif

#include "EditHeadlineDlg.h"
#include "Color.h"

/////////////////////////////////////////////////////////////////////////////
// CBehindEffectsTab dialog

CBehindEffectsTab::CBehindEffectsTab()
	: CPropertyPage(CBehindEffectsTab::IDD)
{
	//{{AFX_DATA_INIT(CBehindEffectsTab)
	//}}AFX_DATA_INIT

	m_eBehindEffect = EFramedEffects(0);
	m_colorFill = RSolidColor(kBlack);
	m_colorShadow = RSolidColor(kBlack);
	m_colorHilite = RSolidColor(kBlack);
}

CBehindEffectsTab::CBehindEffectsTab( struct BehindEffectsData* pBehindEffectsData ) : CPropertyPage(CBehindEffectsTab::IDD)
{
	SetData( pBehindEffectsData );
}

void CBehindEffectsTab::SetData(  struct BehindEffectsData* pBehindEffectsData )
{
	m_eBehindEffect = pBehindEffectsData->eBehindEffect  ; 
	if ( pBehindEffectsData->behindFillDesc.IsTransparent() )
		m_colorFill		 = RSolidColor( kBlack );
	else
		m_colorFill		 = pBehindEffectsData->behindFillDesc.GetSolidColor();
	m_colorShadow	 = pBehindEffectsData->colorShadow ;
	m_colorHilite	 = pBehindEffectsData->colorHilite ;
}

void CBehindEffectsTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBehindEffectsTab)
	DDX_Control(pDX, IDC_CMB_SHADOW_COLOR, m_cmbShadowColor);
	DDX_Control(pDX, IDC_CMB_HILITE_COLOR, m_cmbHiliteColor);
	DDX_Control(pDX, IDC_CMB_FILL_COLOR, m_cmbFillColor);
	//}}AFX_DATA_MAP

	int	nBehindEffect = int(m_eBehindEffect);
	DDX_Radio( pDX, IDC_RD_NOFRAME, nBehindEffect  );
	m_eBehindEffect = EFramedEffects(nBehindEffect );
}

BEGIN_MESSAGE_MAP(CBehindEffectsTab, CPropertyPage)
	//{{AFX_MSG_MAP(CBehindEffectsTab)
	ON_BN_CLICKED(IDC_RD_NOFRAME, OnRdNoFrame)
	ON_BN_CLICKED(IDC_RD_SILHOUETTE, OnRdSilhouette)
	ON_BN_CLICKED(IDC_RD_EMBOSS, OnRdEmboss)
	ON_BN_CLICKED(IDC_RD_DEBOSS, OnRdDeboss)
	ON_CBN_SELCHANGE( IDC_CMB_SHADOW_COLOR, OnCmbShadowColorSelChange )
	ON_CBN_SELCHANGE( IDC_CMB_HILITE_COLOR, OnCmbHiliteColorSelChange )
	ON_CBN_SELCHANGE( IDC_CMB_FILL_COLOR, OnCmbFillColorSelChange )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBehindEffectsTab message handlers

BOOL CBehindEffectsTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetupColorComboBoxes();
	//
	//TODO: How do I make it pick the current selection?
	m_cmbFillColor.SetCurSel((int)GetEColorFromRColor(&m_colorFill));
	m_cmbHiliteColor.SetCurSel((int)GetEColorFromRColor(&m_colorHilite));
	m_cmbShadowColor.SetCurSel((int)GetEColorFromRColor(&m_colorShadow));
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

EColors CBehindEffectsTab::GetEColorFromRColor( RSolidColor* pColor )
{
	// REVIEW Mike Houle (Mike Heydlauf) - This is cheezy
	static RSolidColor colorBlack			= kBlack;
	static RSolidColor colorWhite			= kWhite;
	static RSolidColor colorYellow		= kYellow;
	static RSolidColor colorMagenta		= kMagenta;
	static RSolidColor colorRed			= kRed;
	static RSolidColor colorCyan			= kCyan;
	static RSolidColor colorGreen			= kGreen;
	static RSolidColor colorBlue			= kBlue;
	static RSolidColor colorLightGray	= kLightGray;
	static RSolidColor colorGray			= kGray;
	static RSolidColor colorDarkGray		= kDarkGray;

	if (*pColor == colorBlack )
		return kBlack;
	if (*pColor == colorWhite )
		return kWhite;
	if (*pColor == colorYellow )
		return kYellow;
	if (*pColor == colorMagenta )
		return kMagenta;
	if (*pColor == colorRed )
		return kRed;
	if (*pColor == colorCyan )
		return kCyan;
	if (*pColor == colorGreen )
		return kGreen;
	if (*pColor == colorBlue )
		return kBlue;
	if (*pColor == colorLightGray )
		return kLightGray;
	if (*pColor == colorGray )
		return kGray;
	if (*pColor == colorDarkGray )
		return kDarkGray;

	return EColors(0);
}

////////////////////////////////////////////////////////////////////////////////////
//Function: SetupColorComboBoxes
//
//Description: Displays only the color combo boxes valid for the current outline choice
//
//Paramaters: None
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CBehindEffectsTab::SetupColorComboBoxes()
{
	m_cmbFillColor.ShowWindow( SW_HIDE );
	m_cmbHiliteColor.ShowWindow( SW_HIDE );
	m_cmbShadowColor.ShowWindow( SW_HIDE );
	switch( m_eBehindEffect )
	{
	case kFrameNoFrame:
		{
		m_cmbFillColor.ShowWindow( SW_HIDE );
		m_cmbHiliteColor.ShowWindow( SW_HIDE );
		m_cmbShadowColor.ShowWindow( SW_HIDE );
		}
		break;

	case kSilhouette:
		{
		m_cmbFillColor.ShowWindow( SW_HIDE );
		m_cmbHiliteColor.ShowWindow( SW_HIDE );
		m_cmbShadowColor.ShowWindow( SW_HIDE );
		}
		break;

	case kEmboss:
		{
		m_cmbFillColor.ShowWindow( SW_SHOW );
		m_cmbHiliteColor.ShowWindow( SW_SHOW );
		m_cmbShadowColor.ShowWindow( SW_SHOW );
		}
		break;

	case kDeboss:
		{
		m_cmbFillColor.ShowWindow( SW_SHOW );
		m_cmbHiliteColor.ShowWindow( SW_SHOW );
		m_cmbShadowColor.ShowWindow( SW_SHOW );
		}
		break;

	default:
		ASSERT( FALSE );
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////
//Function: FillData
//
//Description:Loads data from the property page into a BehindEffectsTabDataStruct
//
//Paramaters: pOutlineData -BehindEffectsTabDataStruct to accept Outline Page data.
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CBehindEffectsTab::FillData( struct BehindEffectsData* pBehindEffectsData )
{

	pBehindEffectsData->eBehindEffect	= m_eBehindEffect;
	pBehindEffectsData->colorShadow     = m_colorShadow;
	pBehindEffectsData->colorHilite		= m_colorHilite;
	pBehindEffectsData->behindFillDesc	= m_colorFill;
}

void CBehindEffectsTab::OnRdNoFrame() 
{
	m_eBehindEffect = kFrameNoFrame;
	SetupColorComboBoxes();
	SetModified( TRUE );
}
void CBehindEffectsTab::OnRdSilhouette() 
{
	m_eBehindEffect = kSilhouette;
	SetupColorComboBoxes();
	SetModified( TRUE );
}
void CBehindEffectsTab::OnRdEmboss() 
{
	m_eBehindEffect = kEmboss;
	SetupColorComboBoxes();
	SetModified( TRUE );
}
void CBehindEffectsTab::OnRdDeboss() 
{
	m_eBehindEffect = kDeboss;
	SetupColorComboBoxes();
	SetModified( TRUE );
}

/*void CBehindEffectsTab::OnCmbStrokeColorSelChange()
{
//	EColors eNewColor = EColors(m_cmbStrokeColor.GetCurSel()); 
//	m_colorStroke.Set( eNewColor );
	m_colorStroke.Set( EColors(m_cmbStrokeColor.GetCurSel()) );
	SetModified( TRUE );
} */

void CBehindEffectsTab::OnCmbHiliteColorSelChange()
{
//	EColors eNewColor = EColors(m_cmbHiliteColor.GetCurSel()); 
//	m_colorStroke.Set( eNewColor );
	m_colorHilite = RSolidColor( EColors(m_cmbHiliteColor.GetCurSel()) );
	SetModified( TRUE );
}

void CBehindEffectsTab::OnCmbShadowColorSelChange()
{
//	EColors eNewColor = EColors(m_cmbShadowColor.GetCurSel()); 
//	m_colorStroke.Set( eNewColor );
	m_colorShadow = RSolidColor( EColors(m_cmbShadowColor.GetCurSel()) );
	SetModified( TRUE );
}

void CBehindEffectsTab::OnCmbFillColorSelChange()
{
//	EColors eNewColor = EColors(m_cmbFillColor.GetCurSel()); 
//	m_colorStroke.Set( eNewColor );
	m_colorFill = RSolidColor( EColors(m_cmbFillColor.GetCurSel()) );
	SetModified( TRUE );
}
////////////////////////////////////////////////////////////////////////////////////
//Function: SetPointerToOwner
//
//Description: Stores a pointer to the REditHeadlineDlg class that called it so I can
//					call methods in REditHeadlineDlg
//
//Paramaters: pParentPropSheet - pointer to REditHeadlineDlg that instantiated this 
//											instance.
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void CBehindEffectsTab::SetPointerToOwner( REditHeadlineDlg* pParentPropSheet )
{
	ASSERT( pParentPropSheet );
	m_pParentPropSheet = pParentPropSheet;
}

BOOL CBehindEffectsTab::OnApply()
{

	m_pParentPropSheet->UpdateHeadlinePreview();
	return CPropertyPage::OnApply();
}

BOOL CBehindEffectsTab::OnKillActive() 
{
	m_pParentPropSheet->UpdateTabData( REditHeadlineDlg::kBehindEffectsTab );
	return CPropertyPage::OnKillActive();
}
