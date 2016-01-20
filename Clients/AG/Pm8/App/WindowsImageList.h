//****************************************************************************
//
// File Name:  WindowsImageList.h
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description: Definition of the RWindowsImageList class
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
//  $Logfile:: /PM8/App/WindowsImageList.h                                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:14p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _WINDOWSIMAGELIST_H_
#define _WINDOWSIMAGELIST_H_

class RWindowsImageList
{
public:

						enum 
						{ 
							kNormal = 0, 
							kTransparent = 1,
							kDisabled    = 2,
							kFocused     = 4
						} ;

						RWindowsImageList() ;
	virtual			~RWindowsImageList() ;

	CSize				GetBitmapDimensions() const ;
	WORD				GetImageCount() const ;

	BOOL				LoadBitmap( UINT nID, COLORREF crMask = CLR_DEFAULT ) ;
	BOOL				LoadBitmap( LPCTSTR lpszBitmapID, COLORREF crMask = CLR_DEFAULT ) ;
//	BOOL				LoadMappedBitmap (UINT nID, COLORREF crTransColor, LPCOLORMAP lpColorMap = NULL, int iNumMaps = 0) ;

	BOOL				LoadImageList( UINT nBitmapID, int cx, COLORREF crMask = CLR_NONE ) ;
	BOOL				LoadImageList( LPCTSTR lpszBitmapID, int cx, COLORREF crMask = CLR_NONE ) ;

	BOOL				LoadImageList( UINT nBitmapID, int cx, int cy, COLORREF crMask = CLR_NONE ) ;
	BOOL				LoadImageList( LPCTSTR lpszBitmapID, int cx, int cy, COLORREF crMask = CLR_NONE ) ;

	BOOL				Draw( HDC hdc, CPoint pt, int nIndex = 0, UINT uiFlags = kNormal ) ;
	BOOL				DrawDisabled( HDC hdc, CPoint pt, int nIndex = 0 ) ;

	void				DeleteObject() ;
	WORD				SetImageRow( WORD nNewRow ) ;

protected:

	void				CreateMask( int nWidth, int nHeight, COLORREF crMask ) ;

private:

	HBITMAP			m_hbmImageList ;
	HBITMAP			m_hbmImageMask ;
	HBITMAP			m_hbmImageDisabled ;

	HBITMAP			m_hbmOldImageList ;
	HBITMAP			m_hbmOldImageMask ;
	HBITMAP			m_hbmOldImageDisabled ;

	HDC				m_hdcImageList ;
	HDC				m_hdcImageMask ;
	HDC				m_hdcImageDisabled ;

	CSize				m_szImage ;
	COLORREF			m_crMask ;

	WORD				m_cxImageCount ;

	WORD				m_nImageRow ;
} ;

#include "BitmapImage.h"
#include "FrameworkBaseTypes.h"

class RBitmapImageList : public RBitmapImage
{
// Construction
public:

	enum EDrawStyle
	{
		kNormal,
		kTransparent,
		kDisabled,
		kBlend50, 
		kBlend25 
	} ;

						RBitmapImageList() ;
	virtual			~RBitmapImageList() ;

// Operations
public:

	BOOLEAN			Create( int cx, int cy, uLONG ulBitDepth, BOOLEAN bMask, uWORD nInitial, uWORD nGrow );
	BOOLEAN			Create( YResourceId yBitmapID, int cx, uWORD nGrow, YPlatformColor crMask ) ;
	BOOLEAN			Create( YResourceId yBitmapID, int cx, int cy, uWORD nGrow, YPlatformColor crMask ) ;

	int				Add( RBitmapImage* pbmImage, RBitmapImage* pbmMask ) ;
  	int				Add( RBitmapImage* pbmImage, YPlatformColor crMask ) ;

	BOOLEAN			Render( RDrawingSurface& ds, int nImage, RIntRect rcDestRect, uLONG ulStyle );
	BOOLEAN			Render( RDrawingSurface& ds, int nImage, int nRow, RIntRect rcDestRect, uLONG ulStyle );

	RIntSize			GetImageSize() ;
	uLONG				GetImageCount() ;
	
//	BOOLEAN			DeleteImageList() ;

//	BOOLEAN			Remove( int nImage ) ;
//	BOOLEAN			Replace( int nImage, RBitmapImage* pbmImage, RBitmapImage* pbmMask ) ;


protected:


private:

	RBitmapImage	m_bmMask ;
	RIntSize			m_szImage ;


	BOOLEAN			m_fMask ;
	YPlatformColor m_crMask ;

	uWORD				m_uwGrow ;
	uLONG				m_ulImageCount ;
} ;

#endif _WINDOWSIMAGELIST_H_