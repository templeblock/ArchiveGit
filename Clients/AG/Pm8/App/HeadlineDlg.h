//****************************************************************************
//
// File Name:  HeadlineDlg.h
//
// Project:    Renaissance headline user interface
//
// Author:     Lance Wilson
//
// Description: Definition of RHeadlineDlg class and RHeadlinePreview class
//
// Portability: Windows Specific
//
// Developed by:   Broderbund Software
//				   500 Redwood Blvd.
//				   Novato, CA 94948
//			       (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineDlg.h                                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
#ifndef _HEADLINEDLG_H_
#define _HEADLINEDLG_H_

#include "HeadlineCompResource.h"
#include "HeadlineFacePage.h"
#include "HeadlineShapePage.h"
#include "HeadlinePositionPage.h"
#include "HeadlineOutlinePage.h"
#include "HeadlineShadowPage.h"
#include "HeadlineDepthPage.h"
#include "HeadlineProportionPage.h"
#include "HeadlineFontDlg.h"
#include "ControlContainer.h"
#include "LogoSplash.h"

class RComponentView ;
class RComponentDocument ;
class RHeadlineInterface ;

#define UM_HEADLINE_DATACHANGE	(WM_USER + 100)

const WORD kImageWidth  = 50 ;
const WORD kImageHeight = 46 ;
const WORD kNumRows     =  2 ;
const WORD kNumCols     =  7 ;
const int  kDefNumLines =  3 ;

/////////////////////////////////////////////////////////////////////////////
// RHeadlineEdit window

class RHeadlineEdit : public CEdit
{
public:
							RHeadlineEdit() : m_nLineLimit( kDefNumLines ) {}
	virtual				~RHeadlineEdit() {}

	void					SetLineLimit( int nNumLines = kDefNumLines ) ;

protected:	// Generated message map functions

	//{{AFX_MSG(RHeadlineEdit)
	afx_msg void    OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) ;
	afx_msg LRESULT OnPaste( WPARAM wParam, LPARAM lParam ) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	int					m_nLineLimit ;
} ;

/////////////////////////////////////////////////////////////////////////////
// RHeadlineReadyMades window

class RHeadlineReadyMades : public RGridCtrl<UINT>
{
public:
							RHeadlineReadyMades() ;
	virtual				~RHeadlineReadyMades() ;

	BOOLEAN				Initialize( BOOLEAN fHorz, BOOLEAN fVert ) ;
	void					LoadReadyMade( int nSel, RHeadlineInterface*	pInterface, RComponentView* pView );

	void					SelectReadyMade( CString& strID );
	CString				GetID( int nIndex );

	int					SetTopIndexToFirstVertical() 
							{ return SetTopIndex( m_nFirstVertical ); }

protected:	// Generated message map functions

	virtual void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) ;
	virtual void	MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) ;

	//{{AFX_MSG(RHeadlineReadyMades)
	//}}AFX_MSG

private:

	int			m_nFirstVertical;

} ;

/////////////////////////////////////////////////////////////////////////////
// RHeadlineContextData


/////////////////////////////////////////////////////////////////////////////
// RHeadlineDlg dialog

class RHeadlineDlg : public CDialog
{
// Construction
public:

						RHeadlineDlg( RComponentDocument* pEditedComponent ) ;
	virtual			~RHeadlineDlg();

// Interface
public:

	RComponentDocument* CreateNewComponent( RDocument* pParentDocument ) ;

// Dialog Data
	//{{AFX_DATA(RHeadlineDlg)
	enum { IDD = IDD_HEADLINE_DLG };
//	RHeadlinePreview	m_ctlPreview;
	CString				m_strHeadlineText;
	RHeadlineEdit		m_ecHeadlineText ;
//	int		m_nDistortMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RHeadlineDlg)
	protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void				ApplyData() ;
	void				InitControlData () ;
	void				InitializeDocument() ;
	void				InitializeUIFromComponent( RComponentDocument* pEditedComponent );
	void				ShowPage (int nPage) ;
	void				UpdatePreview() ;

	// Generated message map functions
	//{{AFX_MSG(RHeadlineDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeTabs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusHeadlineText();
	afx_msg LRESULT OnFontChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnFaceChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnShapeChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnPositionChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnOutlineChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnShadowChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnDepthChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnProportionChange (WPARAM wParam, LPARAM lParam) ;
	afx_msg LRESULT OnMenuChar( UINT nChar, UINT nFlags, CMenu* pMenu );
	afx_msg void OnChangeHeadlineText();
	afx_msg void OnReadyMade() ;
	afx_msg void OnCustomize() ;
	afx_msg void OnSelChangeImageList( );
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy() ;
	afx_msg void OnHelp() ;

#ifdef	TPSDEBUG
	afx_msg void	OnSaveAs() ;
	void				ResaveReadyMades() ;
#endif
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	enum { kReadyMade, kCustomize } ;

	struct RHeadlineContextData
	{
						RHeadlineContextData() : m_wSchema(0) {}
		
		sWORD			m_wMode;				// ReadyMade(0) or, Customize(1)
		sWORD			m_wIndex;			// Tab index when in customize
		RMBCString	m_strReadyMade;	// ReadyMade name

		virtual void Write( RArchive& archive ) const;
		virtual void Read( RArchive& archive );

	private:

		WORD			m_wSchema;
	};

	CTabCtrl				m_ctlTabCtrl ;
	CStatic				m_ctlStaticText ;
	RHeadlineFont		m_ctlFontDlg ;

	RHeadlineFacePage			m_pageFace ;
	RHeadlineShapePage		m_pageShape ;
	RHeadlindOutlinePage		m_pageOutline ;
	RHeadlineShadowPage		m_pageShadow ;
	RHeadlineDepthPage		m_pageDepth ;
	RHeadlinePositionPage	m_pagePosition ;
	RHeadlineProportionPage	m_pageProportion ;

	CDialog*						m_pCurrentPage ;

	RHeadlineReadyMades		m_gcImageList ;

	RHeadlineContextData		m_rContextData;	// State information

	int					m_nMode ;
	UINT					m_uiTimer ;
	BOOLEAN				m_fMultiLine ;
	BOOLEAN				m_fVertText ;

	RControlView*			m_pControlView ;
	RControlDocument*		m_pControlDocument ;
	RComponentDocument*	m_pComponentDocument ;
	RComponentView*		m_pComponentView ;
	RHeadlineInterface*	m_pInterface ;
	RHeadlineHolder*     m_pInitialData ;
	RComponentDocument*	m_pEditedComponent;

};


// Subclass the control view so we can eat button down messages
class RHeadlinePreviewView : public RControlView
	{
	public:
							RHeadlinePreviewView( CDialog* pDialog, int nControlId, RControlDocument* pDocument );
		virtual void	OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual			YDropEffect	OnXDragEnter( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point );
	};

class RHeadlinePreviewDocument : public RControlDocument
	{
	public:
		virtual RControlView* CreateView( CDialog* pDialog, int nControlId );
	};

#endif // _HEADLINEDLG_H_
