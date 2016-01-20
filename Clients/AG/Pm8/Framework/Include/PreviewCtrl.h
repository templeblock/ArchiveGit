//****************************************************************************
//
// File Name:		RPreviewCtrl.h
//
// Project:			Renaissance Component
//
// Author:			Mike Heydlauf
//
// Description:	Definition of RPreviewCtrl. 
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/PreviewCtrl.h                  $
//   $Author:: Gbeddow                                                       $
//     $Date:: 3/03/99 6:16p                                                 $
// $Revision:: 1                                                             $
//
//****************************************************************************
#ifndef _PREVIEWCTRL_H_
#define _PREVIEWCTRL_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RBitmapImage;

class FrameworkLinkage RPreviewCtrl : public CButton
{
// Construction
public:
	RPreviewCtrl();
	virtual ~RPreviewCtrl();
	void SetPreviewBitmap( RBitmapImage* prPreviewBmp, BOOL fMirrorImage = FALSE );
	void GetPreviewImageRect( RECT* PrevRect );
	void DisableDrawing( );
	void EnableDrawing( );

	// Generated message map functions
protected:
	CRect		 m_cPreviewRect;
	CRect		 m_cImageRect;
	BOOL		 m_fMirrorImage;
	BOOL		 m_fDrawingDisabled;
	RBitmapImage* m_prPreviewBmp;
	RIntSize	 m_rOffsetSize;

	virtual void DrawCtrl();
	virtual void Render( RDrawingSurface& drawingSurface );
	//{{AFX_MSG(RPushlikeCheckBox)
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_PREVIEWCTRL_H_
