// ****************************************************************************
//
//  File Name:			MdiChildFrame.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMdiChildFrame class
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
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/MdiChildFrame.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MDICHILDFRAME_H_
#define		_MDICHILDFRAME_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RMdiChildFrame
//
//  Description:	Frame class for Renaissance view
//
// ****************************************************************************
//
class FrameworkLinkage RMdiChildFrame : public CMDIChildWnd
	{
	// Implementation
	protected :
		afx_msg void						OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd );
		virtual void						ActivateFrame( int nCmdShow = -1 );

	DECLARE_DYNCREATE( RMdiChildFrame )
	DECLARE_MESSAGE_MAP( )
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _MDICHILDFRAME_H_
