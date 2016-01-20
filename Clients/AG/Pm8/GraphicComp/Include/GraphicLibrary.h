// ****************************************************************************
//
//  File Name:			GraphicLibrary.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//							G. Brown
//
//  Description:		Declaration of the RGraphicLibrary class
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
//  $Logfile:: /PM8/GraphicComp/Include/GraphicLibrary.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _GRAPHICLIBRARY_H_
#define _GRAPHICLIBRARY_H_

class RStream;
class RGraphic;

enum ELibraryVersion
{
	PSD30,
	PSD50
};

// ****************************************************************************
//
// Class Name:			RGraphicLibrary
//
// Description:		Abstract base class for PSD Graphic Libraries.
//
// ****************************************************************************
//
class RGraphicLibrary : public RObject
{
// Construction, destruction & Initialization
public :
	static ELibraryVersion GetLibraryVersion();

// Operations
public :
	virtual BOOLEAN OpenLibrary( RStream& stream ) = 0;
	virtual RGraphic* GetGraphic( RStream& stream, const char* pszGraphicName, BOOLEAN fKeepRawData = FALSE ) const = 0;	
	virtual uBYTE* GetRawData( RStream& stream, const char* pszGraphicName, uLONG& ulRawDataLength ) const = 0;	
	virtual char** GetGraphicNames( uWORD& numGraphics ) const = 0;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const = 0;
#endif	// TPSDEBUG
	};

#endif		// _GRAPHICLIBRARY_H_
