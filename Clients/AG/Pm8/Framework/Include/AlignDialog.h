//****************************************************************************
//
// File Name:		AlignDialog.h
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of RAlignDialog.   
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
//  $Logfile:: /PM8/Framework/Include/AlignDialog.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// RAlignDialog dialog

#ifndef _ALIGNDIALOG_H_
#define _ALIGNDIALOG_H_

#ifndef _FRAMEWORKRESOURCEIDS_H_
#include "FrameworkResourceIDs.h"
#endif

#ifndef _ALIGNSELECTIONACTION_H_
#include "AlignSelectionAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		RAlignDialog
//
//  Description:		Dialog which allows the user to choose the alignment of
//							objects within the project
//
// ****************************************************************************
//
class FrameworkLinkage RAlignDialog : public CDialog
{
// Construction
public:

	enum	EStaticDimensions	{
				kBorder = 12
			};

	RAlignDialog( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(RAlignDialog)
	enum { IDD = DIALOG_ALIGN };

	CStatic	m_staticDisplay;
	int		m_iAlignHorz;
	int		m_iAlignVert;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RAlignDialog)
	protected:
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	RAlignSelectionAction::EAlignHorizontal	m_eAlignHorz;
	RAlignSelectionAction::EAlignVertical		m_eAlignVert;

	CRect					m_rcDisplay;
	CPoint				m_ptStaticCenter;
	CSize					m_sizeStatic;

	CSize					m_sizeCircle;
	CSize					m_sizeRectangle;

	CBrush				m_brushRedBlue;
	CBrush				m_brushGreenBlue;

	CPen					m_penBlack;

	virtual	BOOL		OnInitDialog();

				void		InitializeBrushesAndPens();
				void		InitializeRadioControls();
				void		InitializeStaticControlData();
				void		UpdateDisplay();

				void		DeriveCircleRect( CRect& rcCircle );
				void		DeriveRectangleRect( CRect& rcRectangle );

	// Generated message map functions
	//{{AFX_MSG(RAlignDialog)
	afx_msg void OnPaint();
	//}}AFX_MSG

	afx_msg	void	OnAlignHorizontal( UINT nID );
	afx_msg	void	OnAlignVertical( UINT nID );

	DECLARE_MESSAGE_MAP()

public:
	RAlignSelectionAction::EAlignHorizontal	GetHorizontalAlignment();
	RAlignSelectionAction::EAlignVertical		GetVerticalAlignment();
};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _ALIGNDIALOG_H_
