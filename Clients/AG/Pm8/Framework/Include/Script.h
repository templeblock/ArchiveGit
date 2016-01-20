// ****************************************************************************
//
//  File Name:			Script.h
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Declaration of the RScript class
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
//  $Logfile:: /PM8/Framework/Include/Script.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SCRIPT_H_
#define		_SCRIPT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RStream;

// ****************************************************************************
//
// Class Name:			RScript
//
// Description:		Class to script access functions 
//
// ****************************************************************************
//
class FrameworkLinkage RScript : public RArchive
	{
	// Construction, destruction & Initialization
	public :
												RScript( );
		virtual								~RScript( );

		// Operations
	public :
		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer ) = 0;
		virtual void						Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer ) = 0;
		virtual BOOLEAN					AtEnd( ) = 0;
		virtual void						Close( ) = 0;
		virtual RStream&					GetStream( ) = 0;
		virtual void						Open( EAccessMode eOpenMode ) = 0;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	
		// Member data
	private :
		
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif
