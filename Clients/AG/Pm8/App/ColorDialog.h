// ****************************************************************************
//
//  File Name:	  ColorDialog.h
//
//  Project:	  Renaissance Application Framework
//
//  Author:		  Lance Wilson
//
//  Description: Declaration of the RColorDialog and the 
//               RColorDialogImp classes
//
//  Portability: Platform independent
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ColorDialog.h                                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:04p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

const int kNumPages = 2;

#include "resource.h"

#include "ColorDialogInterfaceImp.h"
#include "WinTextureBrowser.h"
#include "ColorPhotoDlg.h"
#include "WinColorDlg.h"

class IColorDialogImp : public RColorDialogInterfaceImp
{
public:

								IColorDialogImp();

		virtual int			DoModal( CWnd* pParent, UINT uwFlags );
		static BOOLEAN		CreateInstance( YInterfaceId, void ** );
};

class RColorDialog : public CDialog
{

// Construction
public:
	
								RColorDialog( CWnd* pParent = NULL, UINT uwFlags = kShowTransparent ) ;


	void						SetColor( const RColor& crColor ) ;
	RColor					SelectedColor() const;

// Dialog Data
	//{{AFX_DATA(RWinColorDlg)
	enum { IDD = DIALOG_COLOR_TABBED_DIALOG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RColorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	void					ShowPage( int nPage );

	// Generated message map functions
	//{{AFX_MSG(RColorDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy( );
	afx_msg void OnColorMore( );
	afx_msg void OnSelChangeTabs( NMHDR*, LRESULT* pResult ) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CTabCtrl						m_ctlTabCtrl;
	CDialog*						m_pCurrentPage;

	RWinColorPaletteDlgEx	m_dlgColorPalette;
	RWinTextureBrowser		m_dlgColorTexture;
	CPColorPhotoDlg			m_dlgColorPhoto;

	int							m_nStartPage;
	uWORD							m_uwFlags;
} ;




