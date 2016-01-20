// ****************************************************************************
//
//  File Name:			CommandTarget.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Base class for command targets
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
//  $Logfile:: /PM8/Framework/Include/CommandTarget.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMMANDTARGET_H_
#define		_COMMANDTARGET_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RCommandMapBase;

// ****************************************************************************
//
//  Class Name:	RCommandTarget
//
//  Description:	Base class for command targets
//
// ****************************************************************************
//
class RCommandTarget : public RObject
	{
	// Operations
	public :
		virtual RCommandMapBase*		GetRCommandMap( ) const;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RCommandTarget::GetRCommandMap( )
//
//  Description:		Default implementation of GetRCommandMap. Just returns NULL.
//
//  Returns:			Pointer to this classes command map.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RCommandTarget::GetRCommandMap( ) const
	{
	return NULL;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _COMMANDTARGET_H_
