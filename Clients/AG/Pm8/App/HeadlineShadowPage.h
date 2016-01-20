//****************************************************************************
//
// File Name:   HeadlineShadowPage.h
//
// Project:     Renaissance User Interface
//
// Author:      Lance Wilson
//
// Description: Definition of RHeadlineShadowPage class for use in the
//				    headline dialog.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineShadowPage.h                                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINESHADOWPAGE_H_
#define _HEADLINESHADOWPAGE_H_

#ifndef _IMAGEGRIDCTRL_H_
	#include "ImageGridCtrl.h"
#endif

#ifndef _WINCOLORDLG_H_
	#include "WinColorDlg.h"
#endif

class RHeadlineInterface ;

#define UM_HEADLINE_SHADOW_CHANGE	(WM_USER + 107)

#pragma pack( push, 1 )

typedef struct tagShadowPageData
{
	sWORD	nShadowOn ;

	sWORD	nOpacity ;
	sWORD	nEdgeSoftness ;
	sWORD	nOffset ;

} ShadowPageData ;

#pragma pack( pop )

class RHeadlineShadowPage : public CDialog
{
	DECLARE_DYNCREATE(RHeadlineShadowPage)

// Construction
public:

						RHeadlineShadowPage();
						~RHeadlineShadowPage();

	void				ApplyData( RHeadlineInterface* pInterface, RComponentView* pComponentView );
	void				FillData( RHeadlineInterface* pInterface, RComponentView* pComponentView );

// Dialog Data
	//{{AFX_DATA(RHeadlineShadowPage)
	enum { IDD = IDD_DEPTH_PAGE };
	RImageGridCtrl	m_gcImageList ;
	RWinColorBtn	m_btnColor ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RHeadlineShadowPage)
	protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL				LoadResData( HMODULE hModule, LPTSTR lpszName, ShadowPageData& data ) ;

	static BOOL CALLBACK SelectResItem (
		HANDLE hModule,	// resource-module handle 
		LPCTSTR lpszType,	// pointer to resource type 
		LPTSTR lpszName,	// pointer to resource name 
		LONG lParam) ;		// application-defined parameter  

	// Generated message map functions
	//{{AFX_MSG(RHeadlineShadowPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor() ;
	afx_msg void OnSelChangeImageList( );
	afx_msg LRESULT OnAngleChange( WPARAM wParam, LPARAM lParam ) ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RWinAngleCtrl		m_wndBlendAngle ;
	RBitmapImageList	m_ilImageList ;

	ShadowPageData		m_data ;
	YAngle				m_angle ;

};

#endif // _HEADLINESHADOWPAGE_H_
