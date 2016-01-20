//****************************************************************************
//
// File Name:  HeadlineFontDlg.h
//
// Project:    Renaissance headline user interface
//
// Author:     Lance Wilson
//
// Description: Defines RHeadlineFont class
//
// Portability: Windows Specific
//
// Developed by:  Broderbund Software, Inc.
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineFontDlg.h                                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINEFONT_H_
#define _HEADLINEFONT_H_

#ifndef _IMAGEBTN_H_
	#include "ImageBtn.h"
#endif

#ifndef _HEADLINEINTERFACE_H_
	#include "HeadlineInterface.h"
#endif

#include "FontComboBox.h"

class RHeadlineInterface ;

#define UM_HEADLINE_FONT_CHANGE	(WM_USER + 100)

class RHeadlineFont : public CDialog
{
// Construction
public:

						RHeadlineFont(CWnd* pParent = NULL);   // standard constructor

	BOOL				Create( CWnd* pParentWnd = NULL ) ;
	void				ApplyData( RHeadlineInterface* pInterface ) ;
	void				FillData( RHeadlineInterface* pInterface );

// Dialog Data
	//{{AFX_DATA(RHeadlineFont)
	enum { IDD = IDD_HEADLINE_FONT };
	RImageBtn		m_rbJustifyLeft ;
	RImageBtn		m_rbJustifyCenter ;
	RImageBtn		m_rbJustifyRight ;
	RImageBtn		m_rbJustifyFull ;
	RImageBtn		m_cbBold ;
	RImageBtn		m_cbItalic ;
	CPFontComboBox m_gbFontList;
	CString			m_strFontName ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RHeadlineFont)
	protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void				InitFontList() ;
	void				UpdateControls() ;

// Generated message map functions
	//{{AFX_MSG(RHeadlineFont)
	afx_msg void	OnSelChangeFontList();
	afx_msg void	OnJustifyChange();
	afx_msg void	OnBold();
	afx_msg void	OnItalic();
	afx_msg void	OnFontChange( );
	afx_msg void	OnDestroy( );
	virtual BOOL	OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	RWindowsImageList	m_ImageList ;
	int					m_nJustification;

	EDistortEffects	m_eDistortEffects ;
	ETextEscapement   m_eEscapement ;
};

#endif // _HEADLINEFONT_H_
