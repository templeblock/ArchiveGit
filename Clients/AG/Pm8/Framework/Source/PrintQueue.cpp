// ****************************************************************************
//
//  File Name:			PrintQueue.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of RPrintQueue
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
//  $Logfile:: /PM8/Framework/Source/PrintQueue.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "PrintQueue.h"
#include "StandaloneDocument.h"

// ****************************************************************************
//
//  Function Name:	RPrintQueue::RPrintQueue
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrintQueue::RPrintQueue( int nPrintThreads ) : RQueue<RStandaloneDocument>( )
	{
	StartThreads( nPrintThreads );
	}

// ****************************************************************************
//
//  Function Name:	RPrintQueue::DeleteJob
//
//  Description:		Deletes the specified job
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintQueue::DeleteJob( RStandaloneDocument* pJob )
	{
	pJob->FreeDocumentContents( );
	delete pJob;
	}

// ****************************************************************************
//
//  Function Name:	RPrintQueue::ProcessJob
//
//  Description:		Processes the specified job
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintQueue::ProcessJob( RStandaloneDocument* pJob )
	{
	pJob->Print( &m_CancelPrintingSignal );
	}

// ****************************************************************************
//
//  Function Name:	RPrintQueue::ShouldContinue
//
//  Description:		Determines if this queue should continue processing jobs
//
//  Returns:			TRUE if the queue should continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrintQueue::ShouldContinue( )
	{
	return m_CancelPrintingSignal.IsPrintingCanceled( ) == FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPrintQueue::Cancel
//
//  Description:		Cancels processing of jobs in this queue
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintQueue::Cancel( )
	{
	m_CancelPrintingSignal.CancelPrinting( );
	}

