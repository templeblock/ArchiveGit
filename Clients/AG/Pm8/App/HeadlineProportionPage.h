//****************************************************************************
//
// File Name:  HeadlineProportionPage.h
//
// Project:    Renaissance headline user interface
//
// Author:     Lance Wilson
//
// Description: Definition of RHeadlineProportionPage class 
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
//  $Logfile:: /PM8/App/HeadlineProportionPage.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
#ifndef _HEADLINEPROPORTIONPAGE_H_
#define _HEADLINEPROPORTIONPAGE_H_

#ifndef _IMAGEGRIDCTRL_H_
	#include "ImageGridCtrl.h"
#endif

class RHeadlineInterface ;

#pragma pack( push, 1 )
typedef struct tagProportionPageData
{
	uWORD		uwScale1 ;				//Headline Scale 1
	uWORD		uwScale2 ;				//Headline Scale 2
	uWORD		uwScale3 ;				//Headline Scale 3
} ProportionPageData ;
#pragma pack( pop )

#define UM_HEADLINE_PROPORTION_CHANGE	(WM_USER + 106)

class RHeadlineProportionPage : public CDialog
{
	DECLARE_DYNCREATE(RHeadlineProportionPage)

// Construction
public:

						RHeadlineProportionPage();
	virtual			~RHeadlineProportionPage();

	void				ApplyData( RHeadlineInterface* pInterface );
	void				FillData( RHeadlineInterface* pInterface );

// Dialog Data
	//{{AFX_DATA(RHeadlineProportionPage)
	enum { IDD = IDD_PROPORTION_PAGE };
	RImageGridCtrl	m_gcImageList ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RHeadlineProportionPage)
	protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL				LoadResData( HMODULE hModule, LPTSTR lpszName, ProportionPageData& data ) ;

	static BOOL CALLBACK SelectResItem (
		HANDLE hModule,	// resource-module handle 
		LPCTSTR lpszType,	// pointer to resource type 
		LPTSTR lpszName,	// pointer to resource name 
		LONG lParam) ;		// application-defined parameter  

	// Generated message map functions
	//{{AFX_MSG(RHeadlineProportionPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeImageList( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RBitmapImageList	m_ilImageList ;

	uWORD		m_uwScale1 ;				//Headline Scale 1
	uWORD		m_uwScale2 ;				//Headline Scale 2
	uWORD		m_uwScale3 ;				//Headline Scale 3

	UINT		m_uiResID ;
};

#endif // _HEADLINEPROPORTIONPAGE_H_
