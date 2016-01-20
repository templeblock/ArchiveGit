// ****************************************************************************
//
//  File Name:		TabbedEditFixFlaw.h
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Declaration of the RTabbedEditFixFlaw class
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

#ifndef _TABBEDEDITFIXFLAW_H_
#define _TABBEDEDITFIXFLAW_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "GraphicCompResource.h"
#include "PhotoWorkshopResource.h"
//#include "RenaissanceResource.h"
#include "ScrollableControl.h"
#include "Resource.h"

class RTabbedEditImage;
class RFlyout;


enum EFixFlawCursorType
{
	kCursorNone			= -1,
	kCursorRoundSmall	= 0,
	kCursorRoundMedium,
	kCursorRoundLarge,
	kCursorTypesMax
};	

// keep this array coordinated with the above enum
static UINT uCursorIDs[ kCursorTypesMax ] =
{
	IDC_FIXFLAW_CIRCLE10,
	IDC_FIXFLAW_CIRCLE20,
	IDC_FIXFLAW_CIRCLE30
};	

// ****************************************************************************
//
//  Class Name:		REditFixFlawView
//
//  Description:	Preview control for fix flaw UI
//
// ****************************************************************************
class REditFixFlawView : public RScrollControlView
{
	public :
		REditFixFlawView( CDialog* pDialog, int nControlId, RControlDocument* pDocument );

		virtual void	OnXMouseMessage( EMouseMessages eMessage, RRealPoint devicePoint, YModifierKey modifierKeys );

		void				SetEditCursor( EFixFlawCursorType rType );

	private:
		struct RFixFlawCursorInfo
		{
			HCURSOR	m_hCursor;
			RIntSize	m_rSize;
		};

		EFixFlawCursorType 	m_nCurrentCursorType;

		RFixFlawCursorInfo  m_rCursors[ kCursorTypesMax ];
};


// ****************************************************************************
//
//  Class Name:		REditFixFlawDocument
//
//  Description:	Document for fix flaw preview control
//
// ****************************************************************************
class REditFixFlawDocument : public RScrollControlDocument
{
	// Operations
	public :
		virtual RControlView*	CreateView( CDialog* pDialog, int nControlId );

		void							FreeAllActions();
		void							Undo( );
};


// ****************************************************************************
//
//  Class Name:		RTabbedEditFixFlaw
//
//  Description:	Image Edit Fix Flaw dialog
//
// ****************************************************************************
class RTabbedEditFixFlaw : public CDialog
{ 
// Construction
public:
	RTabbedEditFixFlaw(CWnd* pParent = NULL);   // standard constructor
	~RTabbedEditFixFlaw();

	enum { IDD = IDD_EDIT_FIXFLAW_TAB };

	void ApplyTool( const RRealPoint& rMousePoint, RIntSize rToolSize );
	BOOLEAN ToolInImageArea( const RRealPoint& rMousePoint, RIntSize rToolSize );

protected:
	enum TOOL_TYPE
	{
		TOOL_NONE,
		TOOL_RED_EYE,
		TOOL_PET_EYE,
		TOOL_DUST,
		TOOL_SHINE
	};

	virtual BOOL OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(RTabbedEditFixFlaw)
	afx_msg void OnShowWindow(  BOOL bShow, UINT nStatus );
	afx_msg void OnDestroy( );
	afx_msg void OnRedEyeSm();
	afx_msg void OnRedEyeMed();
	afx_msg void OnRedEyeLg();
	afx_msg void OnPetEyeSm();
	afx_msg void OnPetEyeMed();
	afx_msg void OnPetEyeLg();
	afx_msg void OnDustCirSm();
	afx_msg void OnDustCirMed();
	afx_msg void OnDustCirLg();
	afx_msg void OnDustOvalSm();
	afx_msg void OnDustOvalMed();
	afx_msg void OnDustOvalLg();
	afx_msg void OnShineSm();
	afx_msg void OnShineMed();
	afx_msg void OnShineLg();
	afx_msg void OnRedEye();
	afx_msg void OnPetEye();
	afx_msg void OnDust();
	afx_msg void OnShine();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnUndo();
	afx_msg LONG OnApplyEditsToIEDBitmap( WPARAM, LPARAM );
	afx_msg LONG OnCleanupForReset( WPARAM, LPARAM );
	afx_msg LONG OnResetUpdate( WPARAM, LPARAM );
	afx_msg LONG OnOKFromIED( WPARAM, LPARAM );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage( MSG* pMsg );

	afx_msg LONG OnUndoFromIED( WPARAM, LPARAM ) { OnUndo(); return 1L; }
	afx_msg LONG OnZoomInFromIED( WPARAM, LPARAM ) { OnZoomIn(); return 1L; }
	afx_msg LONG OnZoomOutFromIED( WPARAM, LPARAM ) { OnZoomOut(); return 1L; }

private:	
	void	UpdatePreview();
	void	InitializeDocument();
	void	UpdateUndoButton();

	// Data members
	RTabbedEditImage*			m_pParentDlg;

	RComponentDocument*		m_pComponentDocument;

	REditFixFlawDocument*	m_pControlDocument;
	REditFixFlawView*			m_pControlView;

	TOOL_TYPE					m_nCurrentTool;
	TOOL_TYPE					m_nTempTool;
	UINT							m_nToolSel;

	RFlyout*						m_pFlyOut;

	CBitmapButton				m_cButtonZoomIn;
	CBitmapButton				m_cButtonZoomOut;
	CBitmapButton				m_cButtonUndo;

	YComponentBoundingRect	m_rBoundingRect;
	RRealRect					m_rCropArea;
	RRealSize					m_rImageSize;
};

#endif	// _TABBEDEDITFIXFLAW_H_
