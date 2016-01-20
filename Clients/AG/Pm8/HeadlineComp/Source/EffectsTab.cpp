//****************************************************************************
//
// File Name:		EffectsTab.cpp
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Manages CEffectsTab class which is used for the Effects 
//						Property Page
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
//  $Logfile:: /PM8/HeadlineComp/Source/EffectsTab.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME


#ifndef	WIN
	#error	This file must be compilied only on Windows
#endif	//	WIN


#include "EditHeadlineDlg.h"

#include "HeadlinesCanned.h"
#include "Color.h"
//#include "EffectsTab.h"

/////////////////////////////////////////////////////////////////////////////
// CEffectsTab dialog

IMPLEMENT_DYNCREATE(CEffectsTab, CPropertyPage)

CEffectsTab::CEffectsTab() : CPropertyPage(CEffectsTab::IDD)
{
	//{{AFX_DATA_INIT(CEffectsTab)
	m_flShadowDepth = 0.0f;
	m_flProjectionDepth = 0.0f;
	m_slProjectionVanishPtX	= 0;
	m_slProjectionVanishPtX	= 0;
	m_slShadowVanishPtX		= 0;
	m_slShadowVanishPtX		= 0;
	m_fProjectionBlend = FALSE;
	m_fShadowBlend = FALSE;
	//}}AFX_DATA_INIT
	m_eSelProjection	= EProjectionEffects(0);
	m_eSelShadow		= EShadowEffects(0);
}

CEffectsTab::CEffectsTab( struct EffectsData* pEffectsData  ): CPropertyPage(CEffectsTab::IDD)
{
	SetData( pEffectsData );
}

void CEffectsTab::SetData( struct EffectsData* pEffectsData  )
{
	m_flShadowDepth			= pEffectsData->flShadowDepth;
	m_flProjectionDepth		= pEffectsData->flProjectionDepth;
	m_nNumStages				= pEffectsData->nNumStages;

	m_slProjectionVanishPtX	= pEffectsData->nProjectionVanishPtX;
	m_slProjectionVanishPtY	= pEffectsData->nProjectionVanishPtY;
	m_slShadowVanishPtX		= pEffectsData->nShadowVanishPtX;
	m_slShadowVanishPtY		= pEffectsData->nShadowVanishPtY;

	m_fProjectionBlend		= pEffectsData->fProjectionBlend;
	m_fShadowBlend				= pEffectsData->fShadowBlend;
	m_eSelProjection			= pEffectsData->eProjectionEffect;
	m_eSelShadow				= pEffectsData->eShadowEffect;

	m_colorProjection			= pEffectsData->projectionFillDesc.GetSolidColor(YPercentage(0.0));
	m_colorProjectionBlend	= pEffectsData->projectionFillDesc.GetSolidColor(YPercentage(1.0));
	m_colorShadow				= pEffectsData->shadowFillDesc.GetSolidColor(YPercentage(0.0));
	m_colorShadowBlend		= pEffectsData->shadowFillDesc.GetSolidColor(YPercentage(1.0));

}

CEffectsTab::~CEffectsTab()
{
	NULL;
}


void CEffectsTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectsTab)
	DDX_Control(pDX, CONTROL_COMBO_FX_SHADOW_BLEND_COLOR, m_cmbShadowBlendColor);
	DDX_Control(pDX, CONTROL_COMBO_EFFECTS_BLEND_COLOR, m_cmbProjectionBlendColor);
	DDX_Control(pDX, CONTROL_COMBO_FX_SHADOW_COLOR, m_cmbShadowColor);
	DDX_Control(pDX, CONTROL_COMBO_EFFECTS_COLOR, m_cmbProjectionColor);
	DDX_Control(pDX, CONTROL_EDIT_EFFECTS_STAGES, m_ctrlNumStages);
	DDX_Text(pDX, CONTROL_EDIT_EFFECTS_STAGES, m_nNumStages);
	if (m_fIsStack)
	{
		DDV_MinMaxInt(pDX, m_nNumStages, kNumStagesMin, kNumStagesMax );
	}
	DDX_Text(pDX, CONTROL_EDIT_EFFECTS_STACK_DEPTH, m_flProjectionDepth);
	DDV_MinMaxFloat(pDX, m_flProjectionDepth, kProjectionDepthMin, kProjectionDepthMax);
	DDX_Text(pDX, CONTROL_EDIT_EFFECTS_SHADOW_DEPTH, m_flShadowDepth);
	DDV_MinMaxFloat(pDX, m_flShadowDepth, kShadowDepthMin, kShadowDepthMax);
	DDX_Text(pDX, CONTROL_EDIT_EFFECTS_VANISHING_POINTX, m_slProjectionVanishPtX);
	DDV_MinMaxLong(pDX, m_slProjectionVanishPtX, kProjectionVanishPtXMin, kProjectionVanishPtXMax);
	DDX_Text(pDX, CONTROL_EDIT_EFFECTS_VANISHING_POINTY, m_slProjectionVanishPtY);
	DDV_MinMaxLong(pDX, m_slProjectionVanishPtY, kProjectionVanishPtYMin, kProjectionVanishPtYMax);
	DDX_Text(pDX, CONTROL_EDIT_FX_SHADOW_VANISHING_POINTX, m_slShadowVanishPtX);
	DDV_MinMaxLong(pDX, m_slShadowVanishPtX, kShadowVanishPtXMin, kShadowVanishPtXMax);
	DDX_Text(pDX, CONTROL_EDIT_FX_SHADOW_VANISHING_POINTY, m_slShadowVanishPtY);
	DDV_MinMaxLong(pDX, m_slShadowVanishPtY, kShadowVanishPtYMin, kShadowVanishPtYMax);
	DDX_Check(pDX, CONTROL_CHECKBOX_EFFECTS_BLEND, m_fProjectionBlend);
	DDX_Check(pDX, CONTROL_CHECKBOX_EFFECTS_SHADOW_BLEND, m_fShadowBlend);
	//}}AFX_DATA_MAP


	int	nConvertSel = int(m_eSelProjection);
	DDX_Radio( pDX, CONTROL_RADIO_BTN_EFFECTS_NO_EFFECTS, nConvertSel );
	m_eSelProjection = EProjectionEffects(nConvertSel);

	nConvertSel = int(m_eSelShadow);
	DDX_Radio( pDX, CONTROL_RADIO_BTN_EFFECTS_NO_SHADOW, nConvertSel );
	m_eSelShadow = EShadowEffects(nConvertSel);

}


BEGIN_MESSAGE_MAP(CEffectsTab, CPropertyPage)
	//{{AFX_MSG_MAP(CEffectsTab)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_ILLUMINATED_EXTRUSION, OnRadioBtnEffectsIlluminatedExtrusion)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_NO_EFFECTS, OnRadioBtnEffectsNoEffects)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_PSUEDO_PERSPECTIVE_EXTRUSION, OnRadioBtnEffectsPsuedoPerspectiveExtrusion)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_PSUEDO_PERSPECTIVE_STACK, OnRadioBtnEffectsPsuedoPerspectiveStack)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_SIMPLE_STACK, OnRadioBtnEffectsSimpleStack)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_SOLID_EXTRUSION, OnRadioBtnEffectsSolidExtrusion)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_SPIN_STACK, OnRadioBtnEffectsSpinStack)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_NO_SHADOW, OnRadioBtnEffectsNoShadow)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_DROP_SHADOW, OnRadioBtnEffectsDropShadow)
	ON_BN_CLICKED(CONTROL_RADIO_BTN_EFFECTS_SOFT_DROP_SHADOW, OnRadioBtnEffectsSoftDropShadow)
	ON_BN_CLICKED(CONTROL_CHECKBOX_EFFECTS_BLEND, OnCheckboxProjectionBlend)
	ON_BN_CLICKED(CONTROL_CHECKBOX_EFFECTS_SHADOW_BLEND, OnCheckboxShadowBlend)
	ON_CBN_SELCHANGE( CONTROL_COMBO_FX_SHADOW_BLEND_COLOR, OnCmbShadowBlendColorSelChange )
	ON_CBN_SELCHANGE( CONTROL_COMBO_EFFECTS_BLEND_COLOR, OnCmbProjectionBlendColorSelChange )
	ON_CBN_SELCHANGE( CONTROL_COMBO_FX_SHADOW_COLOR, OnCmbShadowColorSelChange )
	ON_CBN_SELCHANGE( CONTROL_COMBO_EFFECTS_COLOR, OnCmbProjectionColorSelChange )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectsTab message handlers
BOOL CEffectsTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	//
	//Check to make sure all the data passed in is valid..
	//REVIEW -Mike Houle (Mike Heydlauf) -how could I use AssertValid here?
	ASSERT( m_nNumStages <= 100 && (m_nNumStages >= 1 || m_eSelProjection == kNoProjection ));
	ASSERT( m_flShadowDepth <= 1.0f && m_flShadowDepth >= 0.0f);
	ASSERT( m_flProjectionDepth <= 1.0f && m_flProjectionDepth >= 0.0f);
	
	if ( !m_fProjectionBlend )
	{
		m_cmbProjectionBlendColor.ShowWindow( SW_HIDE );
	} 
	if ( !m_fShadowBlend )
	{
		m_cmbShadowBlendColor.ShowWindow( SW_HIDE );
	}

	m_cmbShadowBlendColor.SetCurSel(GetEColorFromRColor(&m_colorShadowBlend) );
	m_cmbProjectionBlendColor.SetCurSel(GetEColorFromRColor(&m_colorProjectionBlend) );
	m_cmbShadowColor.SetCurSel( GetEColorFromRColor(&m_colorShadow) );
	m_cmbProjectionColor.SetCurSel(GetEColorFromRColor(&m_colorProjection) );

	switch( m_eSelProjection )
	{
	case kNoProjection:
		{
			m_ctrlNumStages.ShowWindow( SW_HIDE );
			m_fIsStack = FALSE;
		}
		break;
	case kSimpleStack:
		{
			m_ctrlNumStages.ShowWindow( SW_SHOW );
			m_fIsStack = TRUE;
		}
		break;
	case kSpinStack:
		{
			m_ctrlNumStages.ShowWindow( SW_SHOW );
			m_fIsStack = TRUE;
		}
		break;
	case kPseudoPerspectiveStack:
		{
			m_ctrlNumStages.ShowWindow( SW_SHOW );
			m_fIsStack = TRUE;
		}
		break;
	case kSolidExtrusion:
		{
			m_ctrlNumStages.ShowWindow( SW_HIDE );
			m_fIsStack = FALSE;
		}
		break;
	case kPseudoPerspectiveExtrusion:
		{
			m_ctrlNumStages.ShowWindow( SW_HIDE );
			m_fIsStack = FALSE;
		}
		break;
	case kIlluminateExtrusion:
		{
			m_ctrlNumStages.ShowWindow( SW_HIDE );
			m_fIsStack = FALSE;
		}
		break;
	default:
		m_ctrlNumStages.ShowWindow( SW_HIDE );
		ASSERT(FALSE);
		break;

	}
	
	//
	//Need to init combo boxes using getecolorfromrcolor().

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

EColors CEffectsTab::GetEColorFromRColor( RSolidColor* pColor )
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



void CEffectsTab::OnRadioBtnEffectsNoEffects() 
{
	//
	//If a non-stack projection was selected, reset the number of stages(for a stack) to 1
	//and disable the number of stacks edit field.
	m_fIsStack = FALSE;
	UpdateData( TRUE );
	m_nNumStages = 1;
	m_ctrlNumStages.ShowWindow( SW_HIDE );
	UpdateData( FALSE );
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsIlluminatedExtrusion() 
{
	//
	//If a non-stack projection was selected, reset the number of stages(for a stack) to 1
	//and disable the number of stacks edit field.
	m_fIsStack = FALSE;
	UpdateData( TRUE );
	m_nNumStages = 1;
	m_ctrlNumStages.ShowWindow( SW_HIDE );
	UpdateData( FALSE );
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsSolidExtrusion() 
{
	//
	//If a non-stack projection was selected, reset the number of stages(for a stack) to 1
	//and disable the number of stacks edit field.
	m_fIsStack = FALSE;
	UpdateData( TRUE );
	m_nNumStages = 1;
	m_ctrlNumStages.ShowWindow( SW_HIDE );
	UpdateData( FALSE );
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsPsuedoPerspectiveExtrusion() 
{
	//
	//If a non-stack projection was selected, reset the number of stages(for a stack) to 1
	//and disable the number of stacks edit field.
	m_fIsStack = FALSE;
	UpdateData( TRUE );
	m_nNumStages = 1;
	m_ctrlNumStages.ShowWindow( SW_HIDE );
	UpdateData( FALSE );
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsPsuedoPerspectiveStack() 
{
	//
	//Call update data to catch an invalid number of stages selection
	UpdateData( TRUE );
	if ( !m_fIsStack )
	{
		m_nNumStages = kNumStagesDefault;	
		UpdateData( FALSE );
	}
	m_fIsStack = TRUE;
	m_ctrlNumStages.ShowWindow( SW_SHOW );
	
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsSimpleStack() 
{
	//
	//Call update data to catch an invalid number of stages selection
	UpdateData( TRUE );
	if ( !m_fIsStack )
	{
		m_nNumStages = kNumStagesDefault;	
		UpdateData( FALSE );
	}
	m_fIsStack = TRUE;
	m_ctrlNumStages.ShowWindow( SW_SHOW);
	
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsSpinStack() 
{
	//
	//Call update data to catch an invalid number of stages selection
	UpdateData( TRUE );
	if ( !m_fIsStack )
	{
		m_nNumStages = kNumStagesDefault;	
		UpdateData( FALSE );
	}
	m_fIsStack = TRUE;
	m_ctrlNumStages.ShowWindow( SW_SHOW );
	
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsNoShadow() 
{
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsDropShadow() 
{
	SetModified( TRUE );
}

void CEffectsTab::OnRadioBtnEffectsSoftDropShadow() 
{
	SetModified( TRUE );
}



void CEffectsTab::OnCheckboxProjectionBlend() 
{
	m_fProjectionBlend = !m_fProjectionBlend;
	if ( m_fProjectionBlend )
	{
		m_cmbProjectionBlendColor.ShowWindow( SW_SHOW );
	}
	else 
	{
		m_cmbProjectionBlendColor.ShowWindow( SW_HIDE );
	}
	
	SetModified( TRUE );
}
void CEffectsTab::OnCheckboxShadowBlend() 
{
	m_fShadowBlend = !m_fShadowBlend;
	if ( m_fShadowBlend )
	{
		m_cmbShadowBlendColor.ShowWindow( SW_SHOW );
	}
	else 
	{
		m_cmbShadowBlendColor.ShowWindow( SW_HIDE );
	}
	
	SetModified( TRUE );
}

void CEffectsTab::FillData( EffectsDataStruct* pEffectsData)
{
	pEffectsData->flShadowDepth			= m_flShadowDepth;		
	pEffectsData->flProjectionDepth    	= m_flProjectionDepth;		
	pEffectsData->nNumStages				= m_nNumStages;

	pEffectsData->nProjectionVanishPtX	= m_slProjectionVanishPtX;
	pEffectsData->nProjectionVanishPtY	= m_slProjectionVanishPtY;
	pEffectsData->nShadowVanishPtX   	= m_slShadowVanishPtX; 
	pEffectsData->nShadowVanishPtY	  	= m_slShadowVanishPtY;


	pEffectsData->fProjectionBlend		= static_cast<BOOLEAN>( m_fProjectionBlend );
	pEffectsData->fShadowBlend    		= static_cast<BOOLEAN>( m_fShadowBlend );		
	pEffectsData->eProjectionEffect		= m_eSelProjection;		
	pEffectsData->eShadowEffect			= m_eSelShadow;
	if( m_fProjectionBlend )
		pEffectsData->projectionFillDesc	= RHeadlineColor( m_colorProjection, m_colorProjectionBlend, kEast );
	else
		pEffectsData->projectionFillDesc	= RHeadlineColor( m_colorProjection );
	if( m_fShadowBlend )
		pEffectsData->shadowFillDesc	= RHeadlineColor( m_colorShadow, m_colorShadowBlend, kEast );
	else
		pEffectsData->shadowFillDesc	= RHeadlineColor( m_colorShadow );

}

void CEffectsTab::OnCmbShadowBlendColorSelChange()
{
	m_colorShadowBlend = RSolidColor(EColors(m_cmbShadowBlendColor.GetCurSel()));

	SetModified( TRUE );
}

void CEffectsTab::OnCmbProjectionBlendColorSelChange()
{
	m_colorProjectionBlend = RSolidColor(EColors(m_cmbProjectionBlendColor.GetCurSel()));

	SetModified( TRUE );
}

void CEffectsTab::OnCmbShadowColorSelChange()
{
	m_colorShadow = RSolidColor(EColors(m_cmbShadowColor.GetCurSel()));

	SetModified( TRUE );
}

void CEffectsTab::OnCmbProjectionColorSelChange()
{
	m_colorProjection = RSolidColor(EColors(m_cmbProjectionColor.GetCurSel()));
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
void CEffectsTab::SetPointerToOwner( REditHeadlineDlg* pParentPropSheet )
{
	ASSERT( pParentPropSheet );
	m_pParentPropSheet = pParentPropSheet;

}

BOOL CEffectsTab::OnApply()
{

	m_pParentPropSheet->UpdateHeadlinePreview();
	return CPropertyPage::OnApply();
}

BOOL CEffectsTab::OnKillActive() 
{
	UpdateData( TRUE );
	m_pParentPropSheet->UpdateTabData( REditHeadlineDlg::kEffectsTab );
	return CPropertyPage::OnKillActive();
}
