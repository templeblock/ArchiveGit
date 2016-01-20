// ****************************************************************************
//
//  File Name:			ComponentDataRenderer.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of RComponentDataRenderer
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
//  $Logfile:: /PM8/Framework/Include/ComponentDataRenderer.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTDATARENDERER_H_
#define		_COMPONENTDATARENDERER_H_

#ifndef		_DATATRANSFER_H_
#include		"DataTransfer.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
// Class Name:			RComponentDataRenderer
//
// Description:		Data renderer which renders various formats from component
//							format.
//
// ****************************************************************************
//
class FrameworkLinkage RComponentDataRenderer : public RDataRenderer
	{
	// Operations
	public :
		virtual BOOLEAN					RenderData( RDataTransferSource* pDataSource, RDataTransferTarget* pDataTarget, YDataFormat dataFormat );
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _COMPONENTDATARENDERER_H_
