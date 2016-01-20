// ****************************************************************************
//
//  File Name:			GraphicApplication.h
//
//  Project:			Graphic Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGraphicApplication class
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
//  $Logfile:: /PM8/GraphicComp/Include/GraphicApplication.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GRAPHICAPPLICATION_H_
#define		_GRAPHICAPPLICATION_H_

#include "ComponentApplication.h"

// Constants used in data transfer
const YDataFormat kPsd3SquareGraphicFormat =		::RegisterClipboardFormat( "Psd3 Square Graphic Format" );
const YDataFormat kPsd5SquareGraphicFormat =		::RegisterClipboardFormat( "Psd5 Square Graphic Format" );
const YDataFormat kPsd3ColumnGraphicFormat =		::RegisterClipboardFormat( "Psd3 Column Graphic Format" );
const YDataFormat kPsd5ColumnGraphicFormat =		::RegisterClipboardFormat( "Psd5 Column Graphic Format" );
const YDataFormat kPsd3RowGraphicFormat =			::RegisterClipboardFormat( "Psd3 Row Graphic Format" );
const YDataFormat kPsd5RowGraphicFormat =			::RegisterClipboardFormat( "Psd5 Row Graphic Format" );

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RGraphicApplication
//
//  Description:	The Graphic Application.
//
// ****************************************************************************
//
class RGraphicApplication : public RComponentApplication
	{
	// Component overrides
	public :
		virtual void						GetSupportedComponentTypes( RComponentTypeCollection& componentTypeCollection ) const;
		virtual BOOLEAN					CanPasteDataFormat( YDataFormat dataFormat, YComponentType& componentType ) const;
		virtual RComponentDocument*	CreateNewDocument( const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading );

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif
	};

#endif		// _GRAPHICAPPLICATION_H_
