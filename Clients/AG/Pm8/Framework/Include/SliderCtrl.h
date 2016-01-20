//****************************************************************************
//
// File Name:		SliderCtrl.h
//
// Project:			Renaissance Framework Component
//
// Author:			Daniel Selman
//
// Description:	Definition of RSliderCtrl - overide of CSliderCtrl
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
//  $Logfile:: /PM8/Framework/Include/SliderCtrl.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _SLIDERCTRL_H_
#define _SLIDERCTRL_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

/////////////////////////////////////////////////////////////////////////////
// RSliderCtrl window

class RSliderCtrl : public CSliderCtrl
{
// Construction
public:
	RSliderCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RSliderCtrl)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~RSliderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(RSliderCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_SLIDERCTRL_H_