//****************************************************************************
//
// File Name:  HeadlineFacePage.h
//
// Project:    Renaissance User Interface
//
// Author:     Lance Wilson
//
// Description: Definition of RHeadlineFacePage class for use in the
//					 headline dialog.
//
// Portability: Windows Specific
//
// Developed by:  Broderbund Software
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineFacePage.h                                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINEFACEPAGE_H_
#define _HEADLINEFACEPAGE_H_

#ifndef _IMAGEGRIDCTRL_H_
	#include "ImageGridCtrl.h"
#endif

#ifndef _WINCOLORDLG_H_
	#include "WinColorDlg.h"
#endif

#include "HeadlineInterface.h"

#define UM_HEADLINE_FACE_CHANGE	(WM_USER + 101)

#pragma pack( push, 1 )

struct FacePageData
{	
	FacePageData() : nGradientType(0), nGradientAngle(0) {}

	uWORD nGradientType ;
	sWORD nGradientAngle ;

} ;

#pragma pack( pop )

class RHeadlineFacePage : public CDialog
{
	DECLARE_DYNCREATE(RHeadlineFacePage)

// Construction
public:
						RHeadlineFacePage();
						~RHeadlineFacePage();

	void				ApplyData( RHeadlineInterface* pInterface );
	void				FillData( RHeadlineInterface* pInterface );

// Dialog Data
	//{{AFX_DATA(RHeadlineFacePage)
	enum { IDD = IDD_FACE_PAGE };
	RImageGridCtrl		m_gcImageList ;
	RWinColorBtn		m_btnColor ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RHeadlineFacePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL				LoadResData( HMODULE hModule, LPTSTR lpszName, FacePageData& data ) ;

	static BOOL CALLBACK SelectResItem (
		HANDLE hModule,	// resource-module handle 
		LPCTSTR lpszType,	// pointer to resource type 
		LPTSTR lpszName,	// pointer to resource name 
		LONG lParam) ;		// application-defined parameter  

	// Generated message map functions
	//{{AFX_MSG(RHeadlineFacePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor() ;
	afx_msg void OnSelChangeImageList( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RBitmapImageList	m_ilImageList ;
	RColor				m_crData ;

	EOutlineEffects	m_eOutlineEffect ;
};

#endif // _HEADLINEFACEPAGE_H_
