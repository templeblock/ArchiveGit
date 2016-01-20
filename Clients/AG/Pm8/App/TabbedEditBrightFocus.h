// ****************************************************************************
//
//  File Name:		TabbedEditBrightFocus.h
//
//  Project:		Renaissance Application Component
//
//  Authors:			Bob Gotsch & John Fleischhauer
//
//  Description:	Declaration of the RTabbedEditBrightFocus class
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

#ifndef _TABBEDEDITBRIGHTFOCUS_H_
#define _TABBEDEDITBRIGHTFOCUS_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "GraphicCompResource.h"
#include "PhotoWorkshopResource.h"
#include "ImagePreviewCtrl.h"
#include "UndoableAction.h"

class RTabbedEditImage;
class RTabbedEditBrightFocus;
class REditUndoManager;

// ****************************************************************************
//
//  Class Name:		RBrightFocusAction
//
//  Description:	Undoable action for Adjus Color dialog
//
// ****************************************************************************
//
class RBrightFocusAction : public RUndoableAction
{
	public :
		enum EActionType
		{
			kNone,
			kBrightness,
			kContrast,
			kSharpness,
			kCoarse
		};	

	public :
								RBrightFocusAction( RTabbedEditBrightFocus* pOwner, EActionType eAction, YFloatType yBrightness, YFloatType yContrast, YSharpness ySharpness);

		virtual void			Undo( );
		virtual BOOLEAN			WriteScript( RScript& script ) const;

		//	Identifier
		static YActionId		m_ActionId;

	protected :
		RTabbedEditBrightFocus*	m_pOwnerDlg;

	private:
		EActionType	m_eAction;

		YFloatType	m_yBrightness;
		YFloatType	m_yContrast;
		YSharpness	m_ySharpness;

#ifdef	TPSDEBUG
	//	Debugging code
	public :
		virtual	void			Validate( ) const;
#endif
};


// ****************************************************************************
//
//  Class Name:		RTabbedEditBrightFocus
//
//  Description:	Image Edit Brightness / Focus Color dialog
//
// ****************************************************************************
class RTabbedEditBrightFocus : public CDialog
{
// Construction
public:
	RTabbedEditBrightFocus(CWnd* pParent = NULL);   // standard constructor
	~RTabbedEditBrightFocus();

	enum { IDD = IDD_EDIT_BRIGHTNESS_TAB };

	void	ProcessUndo( RBrightFocusAction::EActionType eAction, YFloatType yBrightness, YFloatType yContrast, YSharpness ySharpness);

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RTabbedAdjustCol)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
//	virtual void OnOK();
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(RTabbedAdjustCol)
	afx_msg void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
	afx_msg void	OnUndo();
	afx_msg LONG 	OnApplyEditsToIEDBitmap( WPARAM, LPARAM );
	afx_msg LONG 	OnResetUpdate( WPARAM, LPARAM );
	afx_msg LONG	OnOKFromIED( WPARAM, LPARAM );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	afx_msg LONG OnUndoFromIED( WPARAM, LPARAM ) { OnUndo(); return 1L; }

private:	
	void UpdatePreviews();
	void	OnFineRadio();
	void	OnCoarseRadio();
	BOOL	UpdateLUTs(YFloatType brightnessFraction, YFloatType contrastFraction);

	BOOL	ApplyLUTsToPreviewControls();
	BOOL	ApplySharpen(RComponentDocument *pComponent, YSharpness sharpness);
	BOOL	ApplyLUTsToComponent( RComponentDocument *pComponent, RToneLUTs *pLUTs );
	void	ApplyLUTsToBitmap( RBitmapImage *pSrcBitmap, RBitmapImage *pDestBitmap, RToneLUTs *pLUTs );

	void	UpdateUndoButton();

// Data members
	RComponentDocument*		m_pBeforeComponent;
	RComponentDocument*		m_pAfterComponent;

	CSliderCtrl				m_cBrightnessSlider;
	CSliderCtrl				m_cContrastSlider;
	CSliderCtrl				m_cSharpnessSlider;
	
	RImagePreviewCtrl		m_rImageBeforeCtrl;
	RImagePreviewCtrl		m_rImageAfterCtrl;

	RToneLUTs				m_rLUTs;

	YSharpness				m_sharpness;

	REditUndoManager*		m_pUndoManager;
	CBitmapButton			m_cButtonUndo;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBEDEDITBRIGHTFOCUS_H__FA8F0632_970F_11D1_B515_00A02416AB46__INCLUDED_)
