//****************************************************************************
//
// File Name:  LogoSplash.h
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description: Definition of a splash control for displaying the
//              print shop logo.
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
//  $Logfile:: /PM8/Framework/Include/LogoSplash.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _LOGOSPLASH_H_
#define _LOGOSPLASH_H_

/////////////////////////////////////////////////////////////////////////////
// RLogoSplash window

class FrameworkLinkage RLogoSplash : public CStatic
{
// Construction
public:
	RLogoSplash();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RLogoSplash)
	virtual void PreSubclassWindow() ;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~RLogoSplash();

	// Generated message map functions
protected:
	//{{AFX_MSG(RLogoSplash)
	afx_msg void OnNcPaint() ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	HBITMAP m_hBitmap ;

};

#endif

/////////////////////////////////////////////////////////////////////////////
