// ****************************************************************************
//
//  File Name:			FontComboBox.h
//
//  Project:			Renaissance application
//
//  Author:				D. Selman
//
//  Description:		Definition of the CPFontComboBox class
//
//  Portability:		Windows
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/FontComboBox.h                                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************
#ifndef CPFontComboBox_H
#define CPFontComboBox_H

class CPFontComboBox : public CComboBox
{
protected:

public:
						CPFontComboBox( BOOLEAN fDeleteBitmap = TRUE );
	virtual			~CPFontComboBox();
	
	virtual BOOL	Initialise();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBFontComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct){;}
	//}}AFX_VIRTUAL

	static void		CreateBmp( CBitmap* pBmp, CDC* pDC, LOGFONT* pLF );
	static BOOL		CreateFontDIB( CDC* pDC );
	static void		AddFontToDIB( int nFontToAdd, CDC* pDC, LOGFONT* pLF );

	void				UpdateFontList();

	void				SetLinkedControl( CComboBox* pWnd ) 
						{ m_pLinkedControl = pWnd; }

	private:
		//@member memory dc used for BitBlt of the bitmap
	CDC				m_memDC;
		//@member logfont used to create bitmaps 
	LOGFONT			m_logFont;
		//@member font bitmap init thread
	CWinThread*		m_pBmpInitThread;

	BOOLEAN			m_fDeleteBitmap;

	static CComboBox*	m_pLinkedControl;

};

#endif //CPFontComboBox_H