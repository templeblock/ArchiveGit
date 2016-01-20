// ****************************************************************************
//
//  File Name:			OffscreenDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the ROffscreenDrawingSurface class
//
//  Portability:		Platform independent
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
//  $Logfile:: /PM8/Framework/Include/DefaultDrawingSurface.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _DEFAULTDRAWINGSURFACE_H_
#define _DEFAULTDRAWINGSURFACE_H_

#ifdef _WINDOWS
	#include "DcDrawingSurface.h"
	#define BaseDrawingSurface RDcDrawingSurface
#endif // _WINDOWS
#ifdef MAC
	#include "GpDrawingSurface.h"	
	#define BaseDrawingSurface RGpDrawingSurface
#endif // MAC

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class FrameworkLinkage RDefaultDrawingSurface : public BaseDrawingSurface
{
// Construction, destruction & initialization
public:	
											RDefaultDrawingSurface(BOOLEAN fIsPrinting = FALSE);
	virtual 								~RDefaultDrawingSurface();

#ifdef _WINDOWS
	virtual BOOLEAN					Initialize(HDC, HDC) {return FALSE;}
#endif //_WINDOWS

private:
#ifdef _WINDOWS
	HDC									m_hScreenDC;
#endif //_WINDOWS
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _DEFAULTDRAWINGSURFACE_H_

