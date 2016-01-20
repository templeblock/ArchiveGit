// ****************************************************************************
//
//  File Name:			ClipboardDataTransfer.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the clipboard data transfer classes
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
//  $Logfile:: /PM8/Framework/Include/ClipboardDataTransfer.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CLIPBOARDDATATRANSFER_H_
#define		_CLIPBOARDDATATRANSFER_H_

#ifdef _WINDOWS
	#include "MfcDataTransfer.h"
	#define SourceBaseClass RMFCDataTransferSource
	#define TargetBaseClass RMFCDataTransferTarget
#else

#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif


// ****************************************************************************
//
// Class Name:			RClipboardDataSource
//
// Description:		Encapsulates the clipboard as a data transfer source
//
// ****************************************************************************
//
class FrameworkLinkage RClipboardDataSource : public SourceBaseClass
	{
	// Construction & Destruction
	public :
												RClipboardDataSource( );
												~RClipboardDataSource( );
	};

// ****************************************************************************
//
// Class Name:			RClipboardDataTarget
//
// Description:		Encapsulates the clipboard as a data transfer target
//
// ****************************************************************************
//
class FrameworkLinkage RClipboardDataTarget : public TargetBaseClass
	{
	// Construction & Destruction
	public :
												RClipboardDataTarget( RDataRenderer* pDataRenderer );
												~RClipboardDataTarget( );
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _CLIPBOARDDATATRANSFER_H_
