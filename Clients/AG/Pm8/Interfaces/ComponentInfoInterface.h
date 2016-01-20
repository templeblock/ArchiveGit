// ****************************************************************************
//
//  File Name:			ComponentInfoInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				L. Wilson
//
//  Description:		Declaration of the IComponentInfo interface
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.
//							500 Redwood Blvd
//							Novato, CA  94948
//							(415) 382-4400
//
//  Copyright (C) 1997 Broderbudn Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/Interfaces/ComponentInfoInterface.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTINFOINTERFACE_H_
#define		_COMPONENTINFOINTERFACE_H_

#include		"ComponentAttributes.h"

class RDocument ;
class RStandaloneDocument ;

class IComponentInfo	
{
public:

													IComponentInfo() {}
		virtual									~IComponentInfo() {}

		virtual RRealSize						GetMaxObjectSize( const YComponentType& componentType ) = 0;
		virtual RRealSize						GetMinObjectSize( const YComponentType& componentType, BOOLEAN fMaintainAspect = FALSE ) = 0;
		virtual RRealSize						GetDefaultObjectSize( const YComponentType& componentType ) = 0;
		virtual RComponentAttributes		GetDefaultObjectAttributes( const YComponentType& componentType ) const = 0 ;

		virtual RStandaloneDocument*		GetTopLevelDocument( ) const = 0;
} ;

#endif // _COMPONENTINFOINTERFACE_H_
