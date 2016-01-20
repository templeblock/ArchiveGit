// ****************************************************************************
//
//  File Name:			StreamScript.h
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Declaration of the RStreamScript class
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
//  $Logfile:: /PM8/Framework/Include/StreamScript.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STREAMSCRIPT_H_
#define		_STREAMSCRIPT_H_

#ifndef		_SCRIPT_H_
#include		"Script.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RFileStream;

// ****************************************************************************
//
// Class Name:			RStreamScript
//
// Description:		Class to script access functions 
//
// ****************************************************************************
//
class FrameworkLinkage RStreamScript : public RScript
	{
	// Construction, destruction & Initialization
	public :
												RStreamScript( );
												RStreamScript( const RMBCString& rFilename, const EAccessMode eOpenMode );
												RStreamScript( RFileStream& pStream );
		virtual								~RStreamScript( );

		// Operations
	public :
		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer );
		virtual void						Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer );
		virtual void						Close( );
		virtual BOOLEAN					AtEnd( );
		virtual RStream&					GetStream( );
		virtual void						Open( EAccessMode eOpenMode );

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	
		// Member data
	private :
		RFileStream*						m_pStream;
		BOOLEAN								m_fDeleteOnClose;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif