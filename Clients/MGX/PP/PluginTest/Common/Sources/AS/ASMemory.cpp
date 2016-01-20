/*
 * Name:
 *	ASMemory.cpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ASMemory.cpp is used to manage memory allocations.
 *  This file declares the new and delete operators for memory allocation within 
 *  a plug-in.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.4 6/28/94	DL	First version.
 *		Created by Dave Lazarony.
 */

#include "ASTypes.h"
#include "SPBasic.h"

#include <stdlib.h>

//#include "Types.h"

extern "C" SPBasicSuite *sSPBasic;

void * operator new(size_t size)
{
	void *addr = nil;
	
	if (sSPBasic)
	{
		 ASErr error = sSPBasic->AllocateBlock(size, &addr);
		 if (error)
		 	addr = nil; 	
	}
//	else
//		DebugStr("\p new() called without sSPBlocks");
	
	return addr;
}

void operator delete(void * p)
{
	if (sSPBasic)
		 ASErr error = sSPBasic->FreeBlock(p);
//	else
//		DebugStr("\p delete() called without sSPBlocks");
}
