//****************************************************************************
//
// File Name:  HeadlinePositionPage.h
//
// Project:    Renaissance User Interface
//
// Author:     Lance Wilson
//
// Description: Definition of RHeadlinePositionPage class for use in the
//				headline dialog.
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
//  $Logfile:: /PM8/App/HeadlinePositionPage.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINEPOSITIONPAGE_H_
#define _HEADLINEPOSITIONPAGE_H_

#ifndef _IMAGEGRIDCTRL_H_
	#include "ImageGridCtrl.h"
#endif

class RHeadlineInterface ;

#define UM_HEADLINE_POSITION_CHANGE	(WM_USER + 103)

typedef struct tagPositionPageData
{
	sWORD  nSelComp ;
	sWORD  nDistortEffect ;
	sWORD  nShearAngle ;
	sWORD  nRotation ;
	sWORD  nAltPercent ;
	sWORD  nKerning ;

} PositionPageData ;

class RHeadlinePositionPage : public CDialog
{
	DECLARE_DYNCREATE(RHeadlinePositionPage)

// Construction
public:
						RHeadlinePositionPage();
						~RHeadlinePositionPage();

	void				ApplyData( RHeadlineInterface* pInterface ) ;
	void				FillData( RHeadlineInterface* pInterface ) ; 

// Dialog Data
	//{{AFX_DATA(RHeadlinePositionPage)
	enum { IDD = IDD_POSITION_PAGE };
	RImageGridCtrl m_gcImageList ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RHeadlinePositionPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL				LoadResData( HMODULE hModule, LPTSTR lpszName, 
							PositionPageData& data ) ;
	static 
	BOOL CALLBACK	SelectResItem ( HANDLE hModule, LPCTSTR lpszType,
							LPTSTR lpszName, LONG lParam) ;		

	// Generated message map functions
	//{{AFX_MSG(RHeadlinePositionPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeImageList( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	PositionPageData	m_data ;
	RBitmapImageList	m_ilImageList ;

};

#endif // _HEADLINEPOSITIONPAGE_H_
