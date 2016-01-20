//****************************************************************************
//
// File Name:  HeadlineOutlinePage.h
//
// Project:    Renaissance User Interface
//
// Author:     Lance Wilson
//
// Description: Definition of RHeadlindOutlinePage class for use in the
//					 headline dialog.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//					  500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineOutlinePage.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINEOUTLINEPAGE_H_
#define _HEADLINEOUTLINEPAGE_H_

#ifndef _IMAGEGRIDCTRL_H_
	#include "ImageGridCtrl.h"
#endif

#ifndef _WINCOLORDLG_H_
	#include "WinColorDlg.h"
#endif

class RHeadlineInterface ;

#define UM_HEADLINE_OUTLINE_CHANGE	(WM_USER + 104)

#pragma pack( push, 1 )
typedef struct tagHeadlineOutlineData
{
	sWORD  nOutlineEffect ;
	sWORD  nLineWeight ;

} HeadlineOutlineData ;
#pragma pack( pop )

class RHeadlindOutlinePage : public CDialog
{
	DECLARE_DYNCREATE(RHeadlindOutlinePage)

// Construction
public:

	enum EApplyHints { kNoHint, kFaceChanged, kShapeChanged, kDepthChanged } ;

						RHeadlindOutlinePage();
						~RHeadlindOutlinePage();

	void				ApplyData( RHeadlineInterface* pInterface, RComponentView* pComponentView, UINT uiHints = kNoHint );
	void				FillData( RHeadlineInterface* pInterface, RComponentView* pComponentView );

	static void		ComputeEmbossDebossTints( RHeadlineInterface* pInterface );

// Dialog Data
	//{{AFX_DATA(RHeadlindOutlinePage)
	enum { IDD = IDD_OUTLINE_PAGE };
	RImageGridCtrl		m_gcImageList ;
	RWinColorBtn		m_btnColor ;
	int					m_nGlowChecked;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RHeadlindOutlinePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL				LoadResData( HMODULE hModule, LPTSTR lpszName, HeadlineOutlineData& data ) ;

	static 
	BOOL CALLBACK	SelectResItem ( HANDLE hModule, LPCTSTR lpszType,
							LPTSTR lpszName, LONG lParam) ;		

	// Generated message map functions
	//{{AFX_MSG(RHeadlindOutlinePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnGlow() ;
	afx_msg void OnColor() ;
	afx_msg void OnSelChangeImageList( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RBitmapImageList	m_ilImageList ;

	HeadlineOutlineData	m_data ;
	RSolidColor				m_crStroke ;
	RSolidColor				m_crHairline ;
	
	RColor					m_crFaceColor ;		// For use in restoring face and 
	RColor					m_crBehindColor ;		// behind colors after being forced 
	BOOLEAN					m_fRestoreData  ;    // for emboss/deboss
} ;

#endif // _HEADLINEOUTLINEPAGE_H_