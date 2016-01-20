// ****************************************************************************
//
//  File Name:			PsdClipboardViewerDialog.h
//
//  Project:			Renaissance Graphic Imager
//
//  Author:				G. Brown
//
//  Description:		Declaration of the CPsdClipboardViewerDialog class
//
//  Portability:		MFC dependent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Include/PsdClipboardViewerDialog.h            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PSDCLIPBOARDVIEWERDIALOG_H_
#define		_PSDCLIPBOARDVIEWERDIALOG_H_

#ifndef		_PSD3GRAPHIC_H_
#include		"Psd3Graphic.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CPsdClipboardViewerDialog dialog

class CPsdClipboardViewerDialog : public CDialog
{
// Construction
public:
							CPsdClipboardViewerDialog(CWnd* pParent = NULL);   // standard constructor
							~CPsdClipboardViewerDialog();

// Overrides
protected:
	virtual BOOL		OnInitDialog();
	virtual void		DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	RPsd3Graphic*		GetGraphicFromClipboard();
	
	// Message handler functions
	afx_msg void		OnPaint();
	DECLARE_MESSAGE_MAP()

// Dialog Data	
public:
	CStatic				m_cGraphicPreview;
	CButton				m_cRadioPsd3Graphic;
	CButton				m_cRadioPsd5Graphic;

private:	
	RPsd3Graphic*		m_pPsd3Graphic;
};

#endif // _PSDCLIPBOARDVIEWERDIALOG_H_