//****************************************************************************
//
// File Name:		EffectsTab.h
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Definition of CEffectsTab class 
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
//  $Logfile:: /PM8/HeadlineComp/Include/EffectsTab.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CEffectsTab dialog

#ifndef _EFFECTSTAB_H_
#define _EFFECTSTAB_H_

//#ifndef _RESOURCE_H_
//#include "Resource.h"
//#endif

#define kNumStagesMin			1
#define kNumStagesDefault		6
#define kNumStagesMax			100

#define kProjectionDepthMin	0.0f
#define kProjectionDepthMax	1.0f

#define kShadowDepthMin			0.0f
#define kShadowDepthMax			1.0f

#define kProjectionVanishPtXMin -1000000
#define kProjectionVanishPtXMax 1000000

#define kProjectionVanishPtYMin -1000000
#define kProjectionVanishPtYMax 1000000

#define kShadowVanishPtXMin -1000000
#define kShadowVanishPtXMax 1000000

#define kShadowVanishPtYMin -1000000
#define kShadowVanishPtYMax 1000000

class REditHeadlineDlg;
class CEffectsTab : public CPropertyPage	
{
	DECLARE_DYNCREATE(CEffectsTab)
// Construction
public:
	CEffectsTab();   // standard constructor
	CEffectsTab( struct EffectsData* pHlData  );
	~CEffectsTab();  // standard destructor
	void FillData( struct EffectsData* pEffectsData);
	void SetPointerToOwner( REditHeadlineDlg* pParentPropSheet );
	virtual BOOL OnKillActive();
	void SetData( struct EffectsData* pEffectsData  );
// Dialog Data
	//{{AFX_DATA(CEffectsTab)
	enum { IDD = IDD_PPG_EFFECTS };
	CComboBox	m_cmbShadowBlendColor;
	CComboBox	m_cmbProjectionBlendColor;
	CComboBox	m_cmbShadowColor;
	CComboBox	m_cmbProjectionColor;
	float	m_flShadowDepth;
	float	m_flProjectionDepth;
	CEdit m_ctrlNumStages;
	int	m_nNumStages;
	long	m_slProjectionVanishPtX;
	long	m_slProjectionVanishPtY;
	long	m_slShadowVanishPtX;
	long	m_slShadowVanishPtY;
	BOOL	m_fProjectionBlend;
	BOOL	m_fShadowBlend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectsTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	EProjectionEffects					m_eSelProjection;
	EShadowEffects							m_eSelShadow;

	EColors GetEColorFromRColor( RSolidColor* pColor );

	RSolidColor								m_colorProjection;
	RSolidColor								m_colorProjectionBlend;
	RSolidColor								m_colorShadow;
	RSolidColor								m_colorShadowBlend;

	BOOLEAN									m_fIsStack;

	REditHeadlineDlg* m_pParentPropSheet;

	// Generated message map functions
	//{{AFX_MSG(CEffectsTab)
	afx_msg void OnRadioBtnEffectsCastShadow();
	afx_msg void OnRadioBtnEffectsDropShadow();
	afx_msg void OnRadioBtnEffectsIlluminatedExtrusion();
	afx_msg void OnRadioBtnEffectsNoEffects();
	afx_msg void OnRadioBtnEffectsNoShadow();
	afx_msg void OnRadioBtnEffectsPsuedoPerspectiveExtrusion();
	afx_msg void OnRadioBtnEffectsPsuedoPerspectiveStack();
	afx_msg void OnRadioBtnEffectsSimpleStack();
	afx_msg void OnRadioBtnEffectsSoftCastShadow();
	afx_msg void OnRadioBtnEffectsSoftDropShadow();
	afx_msg void OnRadioBtnEffectsSolidExtrusion();
	afx_msg void OnRadioBtnEffectsSpinStack();
	afx_msg void OnCheckboxProjectionBlend();
	afx_msg void OnCheckboxShadowBlend();
	afx_msg void OnCmbShadowBlendColorSelChange();
	afx_msg void OnCmbProjectionBlendColorSelChange();
	afx_msg void OnCmbShadowColorSelChange();
	afx_msg void OnCmbProjectionColorSelChange();
	virtual BOOL OnApply( );
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //_EFFECTSTAB_H_