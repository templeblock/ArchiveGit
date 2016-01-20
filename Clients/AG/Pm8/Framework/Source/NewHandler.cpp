// ****************************************************************************
//
//  File Name:			NewHandler.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Implementation of the new handling functionality
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
//  $Logfile:: /PM8/Framework/Source/NewHandler.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ExceptionHandling.h"
#include "ApplicationGlobals.h"
#include "NewHandler.h"
#include "DataObject.h"
#include "New.h"


#ifdef	_WINDOWS

#ifndef	USE_SMARTHEAP

static	new_handler		_originalNewHandler;

#else		//	USE_SMARTHEAP

static	pnh				_originalNewHandler;

#ifdef	_WINDOWS
static	MEM_ERROR_FN	_originalErrorHandler;
#endif	//	_WINDOWS

// ****************************************************************************
//
//  Function Name:	SmartHeapErrorHandler( );
//
//  Description:		The New handler.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
MEM_BOOL MEM_ENTRY2	SmartHeapErrorHandler( MEM_ERROR_INFO* errorInfo );
MEM_BOOL MEM_ENTRY2	SmartHeapErrorHandler( MEM_ERROR_INFO* errorInfo )
{
	MEM_BOOL	ret_value	= 0;		//	Signals error
	if (errorInfo->errorCode == MEM_OUT_OF_MEMORY)
	{
		//	Call normal NewHandler
		RenaissanceNewHandler( 1 );
		ret_value	= 1;	//	signals a retry of the allocation
	}
	else
		ret_value	= _originalErrorHandler( errorInfo );

	return ret_value;
}
#endif	//	USE_SMARTHEAP
#endif

// ****************************************************************************
//
//  Function Name:	RenaissanceNewHandler( );
//
//  Description:		The New handler.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
	int __cdecl RenaissanceNewHandler( size_t )
	//void RenaissanceNewHandler( )
	{
		int	objectsFreed = 0;

		//	Here is where we try to free up some memory.
		RDataObjectList&			dataObjectList	= GetDataObjectList();
		RDataObjectListIterator	iteratorEnd		= dataObjectList.End();
		RDataObjectListIterator	iterator			= dataObjectList.Start();

		for ( ; iterator != iteratorEnd; ++iterator )
		{
			//	Free up some memory
			if ( !(*iterator)->IsLocked() && (*iterator)->IsInMemory() )
			{
				(*iterator)->Deallocate( );
				//
				//	Count the object if it was actually freed.
				if ( (*iterator)->IsInMemory() )
					++objectsFreed;
			}
		}

		//	If not enough memory could be freed up, just throw
		//	and OutOfMemory error
		if ( objectsFreed == 0 )
			throw kMemory;
		return 1;
	}

// ****************************************************************************
//
//  Function Name:	InstallNewHandler( );
//
//  Description:		Install the products new handler.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	InstallNewHandler( )
{

#ifdef	_WINDOWS
	//_originalNewHandler = _set_new_handler( RenaissanceNewHandler );
	AfxSetNewHandler( RenaissanceNewHandler );
#ifdef	USE_SMARTHEAP
	//	SmartHeap does not use the NewHandler.....
	_originalErrorHandler	= MemSetErrorHandler( SmartHeapErrorHandler );
#endif	//	USE_SMARTHEAP

#else	//	_WINDOWS
	_originalNewHandler = set_new_handler( RenaissanceNewHandler );
#endif	//	_WINDOWS
}

// ****************************************************************************
//
//  Function Name:	RemoveNewHandler( );
//
//  Description:		Removes the products new handler.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RemoveNewHandler( )
{
	set_new_handler( _originalNewHandler );

#ifdef	_WINDOWS
#ifdef	USE_SMARTHEAP
	//	SmartHeap does not use the NewHandler.....
	MemSetErrorHandler( _originalErrorHandler );
#endif	//	USE_SMARTHEAP
#endif	//	_WINDOWS
}



