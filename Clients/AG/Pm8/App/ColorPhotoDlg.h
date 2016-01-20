//****************************************************************************
//
// File Name:   ColorPhotoDlg.h
//
// Project:     Pretzel user interface
//
// Author:      Lance Wilson
//
// Description: Definition of the CColorPhotoDlg dialog object
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1999 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ColorPhotoDlg.h                                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:04p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _COLORPHOTODLG_H_
#define _COLORPHOTODLG_H_

#include "Resource.h"

class CPColorPhotoDlg : public CDialog
{
// Construction
public:
	
								CPColorPhotoDlg( CWnd* pParent = NULL ) ;
	virtual					~CPColorPhotoDlg() ;


   virtual RColor			SelectedColor() const;
	virtual void			SetColor( RColor crColor );

	BOOL						IsColorPhoto( const RColor crColor ) const;
//	void						AddSelectedToMRU();

// Dialog Data
	//{{AFX_DATA(RWinColorDlg)
	enum { IDD = DIALOG_COLOR_DIALOG_PHOTOS };
	int m_nTint;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RWinColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RWinColorDlgEx)
	virtual BOOL OnInitDialog();
	afx_msg void OnPhotoImport();
	afx_msg void OnPhotoEdit();
	afx_msg void OnSelChangeTint();
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	//}}AFX_MSG

	afx_msg LRESULT OnColorChange( WPARAM nID, LPARAM lParam );
	
	DECLARE_MESSAGE_MAP()

private:

	RColor					m_crColor;
	CString					m_strErrorText;
};

#endif // _COLORPHOTODLG_H_

