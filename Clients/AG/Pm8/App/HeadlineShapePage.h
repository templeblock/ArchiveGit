//****************************************************************************
//
// File Name:  HeadlineShapePage.h
//
// Project:    Renaissance User Interface
//
// Author:     Lance Wilson
//
// Description: Definition of RHeadlineShapePage class for use in the
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
//  $Logfile:: /PM8/App/HeadlineShapePage.h                                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINESHAPEPAGE_H_
#define _HEADLINESHAPEPAGE_H_

#include "ImageBtn.h"
#include "ImageGridCtrl.h"
#include "WinColorDlg.h"
#include "HeadlineInterface.h"

class RHeadlineInterface ;

#define UM_HEADLINE_SHAPE_CHANGE	(WM_USER + 102)

class RHeadlineShapePage : public CDialog
{
	DECLARE_DYNCREATE(RHeadlineShapePage)

// Construction
public:
						RHeadlineShapePage();
						~RHeadlineShapePage();

	void				ApplyData( RHeadlineInterface* pInterface ) ;
	void				FillData( RHeadlineInterface* pInterface ) ;
	void				SetComponentType( const YComponentType& componentType ) ;
	void				SetNumTextLines( int nLines ) { m_nTextLineCount = nLines ; }

	static void		SetBehindColor( RHeadlineInterface* pInterface, const RColor& crColor  ) ;

// Dialog Data
	//{{AFX_DATA(RHeadlineShapePage)
	enum { IDD = IDD_SHAPE_PAGE };
//	RImageBtn		m_rbDistort ;
//	RImageBtn		m_rbNonDistort ;
	RImageBtn		m_rbHorizontal ;
	RImageBtn		m_rbVertical ;
	RWinColorBtn	m_btnColor ;
	RImageGridCtrl m_gcImageList ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RHeadlineShapePage)
	protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOLEAN				UpdateControls() ;
	BOOLEAN				UpdateImageList() ;

	// Generated message map functions
	//{{AFX_MSG(RHeadlineShapePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor() ;
	afx_msg void OnSelChangeImageList( );
	afx_msg void OnDistortChange() ;
	afx_msg void OnOrientationChange() ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RWindowsImageList	m_DistortList ;
	RWindowsImageList	m_OrientationList ;
	RBitmapImageList	m_ilShapeList ;

	EDistortEffects	m_eDistortEffect ;
	ETextEscapement	m_eEscapement ;
	BOOL					m_fDistort ;
	int					m_nSelShapeIdx ;
	UINT					m_uiImageList ;

							// State data
	int					m_nTextLineCount ;		
	YComponentType		m_yComponentType ;
};

inline void	RHeadlineShapePage::SetComponentType( const YComponentType& componentType )
{
	m_yComponentType = componentType ;
}

#endif // _HEADLINESHAPEPAGE_H_
