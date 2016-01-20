// ****************************************************************************
//
//  File Name:		TabbedEditArtEffect.h
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Declaration of the RTabbedEditArtEffect class
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					500 Redwood Blvd
//					Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

#ifndef _TABBEDEDITARTEFFECT_H_
#define _TABBEDEDITARTEFFECT_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "GraphicCompResource.h"
#include "PhotoWorkshopResource.h"
//#include "RenaissanceResource.h"
#include "ImagePreviewCtrl.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "BitmapImage.h"
#include "ImageLibrary.h"
#include "OffscreenDrawingSurface.h"
#include "EffectsCtrls.h"

class RTabbedEditImage;
class RImageInterface;

// ****************************************************************************
//
//  Class Name:		RTabbedEditArtEffect
//
//  Description:	Image Edit Artistic Effects dialog
//
// ****************************************************************************
class RTabbedEditArtEffect : public CDialog
{
// Construction
public:
	RTabbedEditArtEffect(CWnd* pParent = NULL);   // standard constructor
	~RTabbedEditArtEffect();

	enum { IDD = IDD_EDIT_ARTEFFECTS_TAB };
	YArtisticEffect				m_nEffect;
	int							m_nIntensityIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
//	virtual void OnOK();
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	// Generated message map functions
	//{{AFX_MSG(RTabbedEditArtEffect)
	// NOTE: the ClassWizard will add member functions here
	afx_msg void OnShowWindow(  BOOL bShow, UINT nStatus );
	afx_msg void OnUndoEffect();
	afx_msg void OnNoEffect();
	afx_msg void OnEffectSelChange();
	afx_msg LONG OnApplyEditsToIEDBitmap( WPARAM, LPARAM );
	afx_msg LONG OnResetUpdate( WPARAM, LPARAM );
	afx_msg LONG OnOKFromIED( WPARAM, LPARAM );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LONG OnUndoFromIED( WPARAM, LPARAM ) { OnUndoEffect(); return 1L; }

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RTabbedEditArtEffect)
	protected:
	RPresetGridCtrl			m_effectsGridCtrl;	// the preset grid
	//}}AFX_VIRTUAL

private:	
	void			PreviewEffectsImage();
	void			UpdatePreview();

	RBitmapImage	*ApplyEffect( RBitmapImage *pBitmap );
	void			ApplyGearEffect( RBitmapImage& rBitmap );
	RBitmapImage	*ApplyLutColorEffect( RBitmapImage& rBitmap );

// Data members
private:
	enum  EPathTypes			{kStrong, kMedium, kLight};
	enum  ELocalConstant		{kLastGearEffect = 7, kSepiaEffect = 8};
	
	RComponentDocument*		m_pPreviewComponent;
	RImagePreviewCtrl 		m_rImagePreviewCtrl;
	YArtisticEffect			m_aIntensity[10][3];

	CBitmapButton				m_cButtonUndo;
};


#endif 