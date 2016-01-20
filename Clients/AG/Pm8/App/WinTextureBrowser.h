//****************************************************************************
//
// File Name:   WinTextureBrowser.h
//
// Project:     Framework user interface
//
// Author:      Lance Wilson
//
// Description: Definition of the RWinTextureBrowser dialog object
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
//  $Logfile:: /PM8/App/WinTextureBrowser.h                                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:14p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _WINTEXTUREBROWSER_H_
#define _WINTEXTUREBROWSER_H_

#include "resource.h"

#include "CollectionSearcher.h"
#include "TextureSearchCollection.h"
#include "ComponentDocument.h"
#include "WinGridCtrl.h"
#include "ImageColor.h"
#include "ColorPalette.h"

class RTextureGridCtrl : public RGridCtrlBase
{
public:
	
								RTextureGridCtrl() ;
	virtual					~RTextureGridCtrl() ;

	void						SetTint( YTint tint );
	void						SetData( RSearchResultArray* pResultArray ) ;
	YTint						SelectColor( const RColor& crColor );
	RColor					SelectedColor() const;

protected:

	void						EmptyCache();
	RImageColor*			GetCachedItem( int nIndex );

	virtual void			DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void			MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

		// Generated message map functions
	//{{AFX_MSG(RTextureGridCtrl)
	afx_msg void   OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* ) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	typedef	RArray<RImageColor *>				RCachedItemArray ;
	typedef	RArray<RImageColor *>::YIterator	RCachedItemIterator ; 

	RCachedItemArray		m_arrComponentCache;
	RSearchResultArray*	m_pResultArray ;	// The result array returned from the searcher

	YTint						m_nTint;

};

class RWinTextureBrowser : public CDialog
{
// Construction
public:
	
								RWinTextureBrowser( CWnd* pParent = NULL ) ;
	virtual					~RWinTextureBrowser() ;


   virtual RColor			SelectedColor() const;
	virtual void			SetColor( RColor crColor ) ;

	void						AddSelectedToMRU();

// Dialog Data
	//{{AFX_DATA(RWinColorDlg)
	enum { IDD = DIALOG_COLOR_DIALOG_TEXTURES };
	CComboBox			 m_cbTintList ;
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
	afx_msg void OnColorSelChange();
	afx_msg void OnSelchangeTextureTint() ;
	//}}AFX_MSG

	afx_msg LRESULT OnColorChange( WPARAM nID, LPARAM lParam );
	
	DECLARE_MESSAGE_MAP()

private:

	RWinColorPaletteWell		m_scCurrent;
	RWinColorPaletteWell		m_scMRUList[5];
	RWinColorPaletteWell		m_scTransparent;

	static RColor				mruColors[5];

	RTextureGridCtrl		m_gcTextureList;
	RColor					m_crColor;

	RCollectionSearcher< RTextureSearchCollection >* m_pCollectionSearcher ; 
};

#endif // _WINTEXTUREBROWSER_H_
