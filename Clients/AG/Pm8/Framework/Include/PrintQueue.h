// ****************************************************************************
//
//  File Name:			PrintQueue.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of RPrintQueue
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
//  $Logfile:: /PM8/Framework/Include/PrintQueue.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_PRINTQUEUE_H_
#define	_PRINTQUEUE_H_

#include "Queue.h"
#include "PrinterDrawingSurface.h"
#include "StandaloneDocument.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RPrintQueue
//
//  Description:	Class to encapsulate a multithreaded print queue
//
// ****************************************************************************
//
class RPrintQueue : public RQueue<RStandaloneDocument>
	{
	// Construction
	public :
													RPrintQueue( int nPrintThreads = 1 );

	// Overridden implementation
	protected :
		virtual void							DeleteJob( RStandaloneDocument* job );
		virtual void							ProcessJob( RStandaloneDocument* job );
		virtual BOOLEAN						ShouldContinue( );
		virtual void							Cancel( );

	// Members
	private :
		RCancelPrintingSignal				m_CancelPrintingSignal;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _PRINTQUEUE_H_
