// ****************************************************************************
//
//  File Name:			FilePrintAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Declaration of the RFilePrintAction class
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
//  $Logfile:: /PM8/Framework/Include/FilePrintAction.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FILEPRINTACTION_H_
#define		_FILEPRINTACTION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RFilePrintAction
//
//  Description:	Action to open a document
//
// ****************************************************************************
//
class FrameworkLinkage RFilePrintAction : public RAction
	{
	// Construction, destruction & Intialization
	public :
												RFilePrintAction( RScript& script );
												RFilePrintAction( const RMBCString& rInitialFilename, const RMBCString& rFinalFilename );
		virtual								~RFilePrintAction( );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;
		const RMBCString&					GetInitialFileName( );
		const RMBCString&					GetFinalFileName( );

	// Member data
	private :
		RMBCString							m_rInitialFilename;
		RMBCString							m_rFinalFilename;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _FILEPRINTACTION_H_
