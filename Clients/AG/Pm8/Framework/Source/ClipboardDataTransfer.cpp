// ****************************************************************************
//
//  File Name:			ClipboardDataTransfer.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the clipboard data transfer classes
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
//  $Logfile:: /PM8/Framework/Source/ClipboardDataTransfer.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ClipboardDataTransfer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RClipboardDataSource::RClipboardDataSource( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RClipboardDataSource::RClipboardDataSource( ) : RMFCDataTransferSource( new COleDataObject )
	{
	// Attach to the clipboard
	m_pOleDataObject->AttachClipboard( );
	}

// ****************************************************************************
//
//  Function Name:	RClipboardDataSource::~RClipboardDataSource( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RClipboardDataSource::~RClipboardDataSource( )
	{
	m_pOleDataObject->Release( );
	delete m_pOleDataObject;
	}

// ****************************************************************************
//
//  Function Name:	RClipboardDataTarget::RClipboardDataTarget( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RClipboardDataTarget::RClipboardDataTarget( RDataRenderer* pDataRenderer )
	: RMFCDataTransferTarget( pDataRenderer )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RClipboardDataTarget::~RClipboardDataTarget( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RClipboardDataTarget::~RClipboardDataTarget( )
	{
	// Copy the data to the clipboard. Dont delete. MFC/OLE Takes ownership
	m_pOleDataSource->SetClipboard( );
	}

#endif	// _WINDOWS
