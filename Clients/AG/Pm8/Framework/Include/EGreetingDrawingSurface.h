// ****************************************************************************
//
//  File Name:			EGreetingDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas & R. Hood
//
//  Description:		Declaration of the REGreetingDrawingSurface class
//
//  Portability:		Win32 or Mac dependent
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
//  $Logfile:: /PM8/Framework/Include/EGreetingDrawingSurface.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_EGREETINGDRAWINGSURFACE_H_
#define		_EGREETINGDRAWINGSURFACE_H_

#ifndef		_PRINTERDRAWINGSURFACE_H_
#include		"PrinterDrawingSurface.h"
#endif

#ifndef		_BITMAPIMAGE_H_
#include		"BitmapImage.h"
#endif

#ifndef		_OFFSCREENDRAWINGSURFACE_H_
#include		"OffscreenDrawingSurface.h"
#endif

#ifndef		_MERGEDATA_H_
#include		"MergeData.h"
#endif

// ****************************************************************************
//
//  Class Name:	REGreetingDrawingSurface
//
//  Description:	
//
// ****************************************************************************
class FrameworkLinkage REGreetingDrawingSurface : public RPrinterDrawingSurface
{
// Construction, destruction & Initialization
public :
											REGreetingDrawingSurface();
	virtual								~REGreetingDrawingSurface();
	virtual BOOLEAN					Initialize(const RIntSize& rBitmapSize);

// Functions to set the state of the device
public :
	virtual BOOLEAN					BeginPage();
	virtual BOOLEAN					EndPage();

	void									SetEmailSubject(const RMBCString& rEmailSubject) {m_rEmailSubject = rEmailSubject;}
	RMBCString							GetEmailSubject() {return m_rEmailSubject;}
	void									SetEmailMessage(const RMBCString& rEmailMessage) {m_rEmailMessage = rEmailMessage;}
	RMBCString							GetEmailMessage() {return m_rEmailMessage;}
	void									SetMergeData(RMergeData* pMergeData) {TpsAssert(dynamic_cast<RMergeData*>(pMergeData), "Bad pMergeData in REGreetingDrawingSurface::SetMergeData()."); m_pMergeData = pMergeData;}

// Member data
private:
	// The email subject
	RMBCString							m_rEmailSubject;
	
	// The email heading
	RMBCString							m_rEmailMessage;
	
	// The list of addresses we will be sending the EGreetings to
	// and a list iterator
	RMergeData*							m_pMergeData;
	RMergeIterator						m_rMergeIterator;	
	
	// The bitmap image we will send and the offscreen drawing surface
	// we use to render to the bitmap
	RBitmapImage						m_rBitmap;
	ROffscreenDrawingSurface		m_rODS;

	
	// REVIEW Additional MoNet info

#ifdef TPSDEBUG
// Debugging stuff
public :
	virtual void						Validate() const {}
#endif // TPSDEBUG
};

#endif // _EGREETINGDRAWINGSURFACE_H_
