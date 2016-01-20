//****************************************************************************
//
// File Name: OutlineTab.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Manages COutlineTab class which is used for the Outline 
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
//  $Logfile:: /PM8/HeadlineComp/Source/OutlineTab.cpp                        $
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
// COutlineTab dialog

COutlineTab::COutlineTab()
	: CPropertyPage(COutlineTab::IDD)
{
	//{{AFX_DATA_INIT(COutlineTab)
	//}}AFX_DATA_INIT

	m_eOutlineEffect = EOutlineEffects(0);
	m_colorStroke = RSolidColor(kBlack);
	m_colorFill = RSolidColor(kBlack);
	m_colorShadow = RSolidColor(kBlack);
	m_colorHilite = RSolidColor(kBlack);
}

COutlineTab::COutlineTab( OutlineDataStruct* pOutlineData ) : CPropertyPage(COutlineTab::IDD)
{
	SetData( pOutlineData );
}

void COutlineTab::SetData( OutlineDataStruct* pOutlineData )
{
	m_eOutlineEffect = pOutlineData->eOutlineEffect ; 
	m_eLineWeight	 = ELineWeight((int)pOutlineData->eLineWeight); 
	m_colorStroke	 = pOutlineData->colorStroke ;
	m_colorFill		 = pOutlineData->outlineFillDesc.GetSolidColor();
	m_colorShadow	 = pOutlineData->colorShadow ;
	m_colorHilite	 = pOutlineData->colorHilite ;

}

void COutlineTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutlineTab)
	DDX_Control(pDX, IDC_CMB_STROKE_COLOR, m_cmbStrokeColor);
	DDX_Control(pDX, IDC_CMB_SHADOW_COLOR, m_cmbShadowColor);
	DDX_Control(pDX, IDC_CMB_HILITE_COLOR, m_cmbHiliteColor);
	DDX_Control(pDX, IDC_CMB_FILL_COLOR, m_cmbFillColor);
	//}}AFX_DATA_MAP
	//
	//Attach member variable to Radio Button...
	int	nConvertOutlineType = int(m_eOutlineEffect);
	DDX_Radio( pDX, IDC_RD_STROKE, nConvertOutlineType );
	m_eOutlineEffect = EOutlineEffects(nConvertOutlineType);

	int	nConvertLineWeight = int(m_eLineWeight);
	DDX_Radio( pDX, IDC_RD_HAIRLINE, nConvertLineWeight );
	m_eLineWeight = ELineWeight(nConvertLineWeight);
}

BEGIN_MESSAGE_MAP(COutlineTab, CPropertyPage)
	//{{AFX_MSG_MAP(COutlineTab)
	ON_BN_CLICKED(IDC_RD_STROKE, OnRdStroke)
	ON_BN_CLICKED(IDC_RD_FILL, OnRdFill)
	ON_BN_CLICKED(IDC_RD_STROKE_FILL, OnRdStrokeAndFill )
	ON_BN_CLICKED(IDC_RD_HEAVY, OnRdHeavy )
	ON_BN_CLICKED(IDC_RD_DOUBLE, OnRdDouble )
	ON_BN_CLICKED(IDC_RD_BLURRED, OnRdBlurred )
	ON_BN_CLICKED(IDC_RD_TRIPLE, OnRdTriple )
	ON_BN_CLICKED(IDC_RD_HAIRLINEDBL, OnRdHairlineDouble )
	ON_BN_CLICKED(IDC_RD_DBLHAIRLINE, OnRdDoubleHairline )
	//ON_BN_CLICKED(IDC_RD_BEVELED, OnRdBeveled )
	ON_CBN_SELCHANGE( IDC_CMB_STROKE_COLOR, OnCmbStrokeColorSelChange )
	ON_CBN_SELCHANGE( IDC_CMB_SHADOW_COLOR, OnCmbShadowColorSelChange )
	ON_CBN_SELCHANGE( IDC_CMB_HILITE_COLOR, OnCmbHiliteColorSelChange )
	ON_CBN_SELCHANGE( IDC_CMB_FILL_COLOR, OnCmbFillColorSelChange )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutlineTab message handlers

BOOL COutlineTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetupColorComboBoxes();
	//
	//TODO: How do I make it pick the current selection?
	m_cmbStrokeColor.SetCurSel((int)GetEColorFromRColor(&m_colorStroke));
	m_cmbFillColor.SetCurSel((int)GetEColorFromRColor(&m_colorFill));
	m_cmbHiliteColor.SetCurSel((int)GetEColorFromRColor(&m_colorHilite));
	m_cmbShadowColor.SetCurSel((int)GetEColorFromRColor(&m_colorShadow));
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

EColors COutlineTab::GetEColorFromRColor( RSolidColor* pColor )
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
void COutlineTab::SetupColorComboBoxes()
{
	m_cmbStrokeColor.ShowWindow( SW_HIDE );
	m_cmbFillColor.ShowWindow( SW_HIDE );
	m_cmbHiliteColor.ShowWindow( SW_HIDE );
	m_cmbShadowColor.ShowWindow( SW_HIDE );

	switch( m_eOutlineEffect )
	{
	case kSimpleStroke:
		m_cmbStrokeColor.ShowWindow( SW_SHOW );
		break;

	case kSimpleFill:
		m_cmbFillColor.ShowWindow( SW_SHOW );
		break;

	case kSimpleStrokeFill:
		m_cmbStrokeColor.ShowWindow( SW_SHOW );
		m_cmbFillColor.ShowWindow( SW_SHOW );
		break;

	case kHeavyOutline:
		m_cmbFillColor.ShowWindow( SW_SHOW );
		m_cmbShadowColor.ShowWindow( SW_SHOW );
		break;


	case kDoubleOutline:
		m_cmbFillColor.ShowWindow( SW_SHOW );
		m_cmbShadowColor.ShowWindow( SW_SHOW );
		m_cmbStrokeColor.ShowWindow( SW_SHOW );
		m_cmbHiliteColor.ShowWindow( SW_HIDE );
		break;

	case kBlurredOutline:
		m_cmbFillColor.ShowWindow( SW_SHOW );
		break;

	case kTripleOutline:
		m_cmbStrokeColor.ShowWindow( SW_SHOW );
		m_cmbFillColor.ShowWindow( SW_SHOW );
		m_cmbShadowColor.ShowWindow( SW_SHOW );
		m_cmbHiliteColor.ShowWindow( SW_SHOW );
		break;

	case kHairlineDoubleOutline:
		m_cmbStrokeColor.ShowWindow( SW_SHOW );
		m_cmbFillColor.ShowWindow( SW_SHOW );
		m_cmbShadowColor.ShowWindow( SW_SHOW );
		break;

	case kDoubleHairlineOutline:
		m_cmbStrokeColor.ShowWindow( SW_SHOW );
		m_cmbFillColor.ShowWindow( SW_SHOW );
		m_cmbShadowColor.ShowWindow( SW_SHOW );
		break;

	case kEmbossedOutline:
		break;

	case kDebossedOutline:
		break;

	case kSoftEmbossedOutline:
		break;

	case kSoftDebossedOutline:
		break;

	case kGlintOutline:
		break;

	case kRoundedOutline:
		break;

	case kSimpleFillStroke:
		break;

	default:
		ASSERT( FALSE );
		break;
	}

}

////////////////////////////////////////////////////////////////////////////////////
//Function: UncheckRadioIfNecessary
//
//Description: Unchecks disabled radio btn if its checked
//
//Paramaters: None
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void COutlineTab::UncheckRadioIfNecessary()
{
	CButton* pEmbossed				= (CButton*)GetDlgItem( IDC_RD_EMBOSSED );
	CButton* pDebossed				= (CButton*)GetDlgItem( IDC_RD_DEBOSSED );
	CButton* pSoftEmbossed			= (CButton*)GetDlgItem( IDC_RD_SOFTEMBOSSED );
	CButton* pSoftDebossed			= (CButton*)GetDlgItem( IDC_RD_SOFTDEBOSSED );
	CButton* pRounded					= (CButton*)GetDlgItem( IDC_RD_ROUNDED );
	CButton* pGlint					= (CButton*)GetDlgItem( IDC_RD_GLINT );
	CButton* pSimpleFillStroke		= (CButton*)GetDlgItem( IDC_RD_SIMPLEFILLSTROKE );


	switch( m_eOutlineEffect )
	{
	case kSimpleStroke:
	case kSimpleFill:
	case kSimpleStrokeFill:
	case kHeavyOutline:
	case kDoubleOutline:
	case kBlurredOutline:
	case kTripleOutline:
	case kHairlineDoubleOutline:
	case kDoubleHairlineOutline:
		{
		 pEmbossed->SetCheck(0);
		 pDebossed->SetCheck(0);
		 pSoftEmbossed->SetCheck(0);
		 pSoftDebossed->SetCheck(0);
		 pRounded->SetCheck(0);
		 pGlint->SetCheck(0);
		 pSimpleFillStroke->SetCheck(0);
		}
		break;


	case kEmbossedOutline:
		break;

	case kDebossedOutline:
		break;

	case kSoftEmbossedOutline:
		break;

	case kSoftDebossedOutline:
		break;

	case kGlintOutline:
		break;

	case kRoundedOutline:
		break;

	case kSimpleFillStroke:
		break;

	default:
		ASSERT( FALSE );
		break;
	}

}


////////////////////////////////////////////////////////////////////////////////////
//Function: FillData
//
//Description:Loads data from the property page into a OutlineTabDataStruct
//
//Paramaters: pOutlineData -OutlineTabDataStruct to accept Outline Page data.
//
//Returns:    VOID
//
////////////////////////////////////////////////////////////////////////////////////
void COutlineTab::FillData( OutlineDataStruct* pOutlineData )
{

	pOutlineData->eOutlineEffect	= m_eOutlineEffect;
	//
	//Adjust radio to map to enum (ie. radio btn don't have hairline weight)
	pOutlineData->eLineWeight		= ELineWeight((int)m_eLineWeight );
	pOutlineData->outlineFillDesc	= m_colorFill;
	pOutlineData->colorStroke     = m_colorStroke;
	pOutlineData->colorShadow     = m_colorShadow;
	pOutlineData->colorHilite		= m_colorHilite;
}

void COutlineTab::OnRdStroke() 
{
	m_eOutlineEffect = kSimpleStroke;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}
void COutlineTab::OnRdFill() 
{
	m_eOutlineEffect = kSimpleFill;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}
void COutlineTab::OnRdStrokeAndFill() 
{
	m_eOutlineEffect = kSimpleStrokeFill;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}
void COutlineTab::OnRdHeavy() 
{
	m_eOutlineEffect = kHeavyOutline;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}
void COutlineTab::OnRdDouble() 
{
	m_eOutlineEffect = kDoubleOutline;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
} 
void COutlineTab::OnRdTriple() 
{
	m_eOutlineEffect = kTripleOutline;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}
void COutlineTab::OnRdHairlineDouble() 
{
	m_eOutlineEffect = kHairlineDoubleOutline;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}
void COutlineTab::OnRdDoubleHairline() 
{
	m_eOutlineEffect = kDoubleHairlineOutline;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}


void COutlineTab::OnRdBlurred() 
{
	m_eOutlineEffect = kBlurredOutline;
	SetupColorComboBoxes();
	UncheckRadioIfNecessary();
	SetModified( TRUE );
}
/*void COutlineTab::OnRdBeveled() 
{
	m_eOutlineEffect = kEmbossedOutline;
	SetupColorComboBoxes();
	SetModified( TRUE );
} */


void COutlineTab::OnCmbStrokeColorSelChange()
{
//	EColors eNewColor = EColors(m_cmbStrokeColor.GetCurSel()); 
//	m_colorStroke.Set( eNewColor );
	m_colorStroke = RSolidColor( EColors(m_cmbStrokeColor.GetCurSel()) );
	SetModified( TRUE );
}

void COutlineTab::OnCmbHiliteColorSelChange()
{
//	EColors eNewColor = EColors(m_cmbHiliteColor.GetCurSel()); 
//	m_colorStroke.Set( eNewColor );
	m_colorHilite = RSolidColor( EColors(m_cmbHiliteColor.GetCurSel()) );
	SetModified( TRUE );
}

void COutlineTab::OnCmbShadowColorSelChange()
{
//	EColors eNewColor = EColors(m_cmbShadowColor.GetCurSel()); 
//	m_colorStroke.Set( eNewColor );
	m_colorShadow = RSolidColor( EColors(m_cmbShadowColor.GetCurSel()) );
	SetModified( TRUE );
}

void COutlineTab::OnCmbFillColorSelChange()
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
void COutlineTab::SetPointerToOwner( REditHeadlineDlg* pParentPropSheet )
{
	ASSERT( pParentPropSheet );
	m_pParentPropSheet = pParentPropSheet;

}

BOOL COutlineTab::OnApply()
{

	m_pParentPropSheet->UpdateHeadlinePreview();
	return CPropertyPage::OnApply();
}

BOOL COutlineTab::OnKillActive() 
{
	UpdateData( TRUE );
//	return CPropertyPage::OnKillActive();
	m_pParentPropSheet->UpdateTabData( REditHeadlineDlg::kOutlineTab );
	return CPropertyPage::OnKillActive();
}

