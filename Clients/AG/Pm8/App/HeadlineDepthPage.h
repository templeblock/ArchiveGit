//****************************************************************************
//
// File Name:   HeadlineDepthPage.h
//
// Project:     Renaissance User Interface
//
// Author:      Lance Wilson
//
// Description: Definition of RHeadlineDepthPage class for use in the
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
//  $Logfile:: /PM8/App/HeadlineDepthPage.h                                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINEDEPTHPAGE_H_
#define _HEADLINEDEPTHPAGE_H_

#ifndef _IMAGEGRIDCTRL_H_
	#include "ImageGridCtrl.h"
#endif

#ifndef _WINCOLORDLG_H_
	#include "WinColorDlg.h"
#endif

class RHeadlineInterface ;

#define UM_HEADLINE_DEPTH_CHANGE	(WM_USER + 105)

#pragma pack( push, 1 )

typedef struct tagDepthPageData
{
	sWORD		nDataType ;

	union 
	{ 
		struct
		{
			sWORD	nSelProjection ;
			sWORD	nSelShadow ;
			sWORD	nNumStages ;
			float	flDepth ;
			sWORD	nVanishPointX ;
			sWORD	nVanishPointY ;
			sWORD	fBlend ;
		};

		struct 
		{ 
			uWORD	nOpacity ;
			uWORD	nEdgeSoftness ;
			uWORD	nOffset ;
		};  

		sWORD	nBehindEffect ;
	} ; 

} DepthPageData ;

#pragma pack( pop )

class RHeadlineDepthPage : public CDialog
{
	DECLARE_DYNCREATE(RHeadlineDepthPage)

// Construction
public:

						RHeadlineDepthPage();
						~RHeadlineDepthPage();

	void				ApplyData( RHeadlineInterface* pInterface, RComponentView* pComponentView );
	void				FillData( RHeadlineInterface* pInterface, RComponentView* pComponentView );

// Dialog Data
	//{{AFX_DATA(RHeadlineDepthPage)
	enum { IDD = IDD_DEPTH_PAGE };
	RImageGridCtrl	m_gcImageList ;
	RWinColorBtn	m_btnColor ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RHeadlineDepthPage)
	protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL				LoadResData( HMODULE hModule, LPTSTR lpszName, DepthPageData& data ) ;

	static BOOL CALLBACK SelectResItem (
		HANDLE hModule,	// resource-module handle 
		LPCTSTR lpszType,	// pointer to resource type 
		LPTSTR lpszName,	// pointer to resource name 
		LONG lParam) ;		// application-defined parameter  

	// Generated message map functions
	//{{AFX_MSG(RHeadlineDepthPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor() ;
	afx_msg void OnSelChangeImageList( );
	afx_msg LRESULT OnAngleChange( WPARAM wParam, LPARAM lParam ) ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RWinAngleCtrl		m_wndBlendAngle ;
	RBitmapImageList	m_ilImageList ;

	DepthPageData		m_data ;
	YAngle				m_angle ;

};

#endif // _HEADLINEDEPTHPAGE_H_
