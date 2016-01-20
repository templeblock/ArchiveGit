// ****************************************************************************
//
//  File Name:			ComponentApplication.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RComponentApplication class
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
//  $Logfile:: /PM8/Framework/Include/ComponentApplication.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTAPPLICATION_H_
#define		_COMPONENTAPPLICATION_H_

#include		"Application.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentDocument;
class RComponentAttributes;

class RComponentTypeCollection : public RArray<YComponentType> { };

// ****************************************************************************
//
//  Class Name:	RComponentApplication
//
//  Description:	This class is a non-template base class for RComponentApp.
//						Its gives a place to put most of the RComponentApplication
//						functionality without putting it in the .h file.
//
// ****************************************************************************
//
class FrameworkLinkage RComponentApplication : public RApplication
	{
	// Operations
	public :
		virtual RAction*						CreateScriptAction( const YActionId& actionId, RScript& script );
		virtual void							GetSupportedComponentTypes( RComponentTypeCollection& componentTypeCollection ) const = 0;
		virtual BOOLEAN						AppendToComponentMenu( ) const;
		virtual BOOLEAN						CanPasteDataFormat( YDataFormat dataFormat, YComponentType& componentType ) const;
		virtual RComponentDocument*		CreateNewDocument( const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading ) = 0;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void							Validate( ) const = 0;
#endif
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _COMPONENTAPPLICATION_H_
