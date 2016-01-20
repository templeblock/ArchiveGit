//****************************************************************************
//
// File Name:  ExportGraphicDlg.h
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description: Defines RExportGraphicDlg class
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software, Inc.
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ExportGraphicDlg.h                                    $
//   $Author:: Jfleischhauer                                                  $
//     $Date:: 3/08/99 2:43p                                                  $
// $Revision:: 2                                                              $
//
//****************************************************************************

#ifndef _EXPORTGRAPHICDLG_H_
#define _EXPORTGRAPHICDLG_H_

//#include "RenaissanceResource.h"
class RComponentView ;
class RBitmapImage;

void ExportGraphicComponent( RComponentView* pComponentView, RMBCString *pInitialDir = NULL ) ;
void ExportGraphicImage( RBitmapImage* pImage, RMBCString *pInitialDir = NULL ) ;

/////////////////////////////////////////////////////////////////////////////
// RExportGraphicDlg dialog

class RExportGraphicDlg : public CFileDialog
{
	DECLARE_DYNAMIC(RExportGraphicDlg)

public:

							RExportGraphicDlg( RComponentView* pComponentView, CString& strFilter ) ;
							RExportGraphicDlg( RBitmapImage* pImage, CString& strFilter ) ;
							~RExportGraphicDlg() ;

	const RIntSize&	GetSize() { return m_szExportSize ; }

protected:
	//{{AFX_MSG(RExportGraphicDlg)
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnMaintainAspectChange() ;
	afx_msg void OnKillFocusExportWidth() ;
	afx_msg void OnKillFocusExportHeight() ;
	afx_msg void OnHelp() ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	RIntSize        m_szExportSize ;

	YScaleFactor	 m_flScaleFactor ;
	BOOLEAN			 m_fMaintainAspect ;
	BOOLEAN			 m_fExportingImage ;

};

#endif // _EXPORTGRAPHICDLG_H_
