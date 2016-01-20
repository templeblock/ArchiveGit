// ****************************************************************************
//
//  File Name:			EditComponentAttributesDialog.h
//
//  Project:			Renaissance
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the REditComponentAttributesDialog class
//
//  Portability:		MFC dependent
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
//  $Logfile:: /PM8/Framework/Include/EditComponentAttribDialog.h             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_EDITCOMPONENTATTRIBUTESDIALOG_H_
#define		_EDITCOMPONENTATTRIBUTESDIALOG_H_

#ifndef _WINDOWS
	#error Windows only
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentAttributes;

// ****************************************************************************
//
// Class Name:			REditComponentAttributesDialog
//
// Description:		Debug dialog used to edit component attributes
//
// ****************************************************************************
//
class FrameworkLinkage REditComponentAttributesDialog : public CDialog
	{
	// Construction, destruction & initialization
	public :
													REditComponentAttributesDialog( const RComponentAttributes& componentAttributes );

	// Operations
	public :
		void										DoDataExchange( CDataExchange* pDX );
		void										FillInComponentAttributes( RComponentAttributes& componentAttributes ) const;

	// Member data
	public :
		BOOL										m_fSelectable;
		BOOL										m_fMovable;
		BOOL										m_fResizable;
		BOOL										m_fRotatable;
		YZLayer									m_ZLayer;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _EDITCOMPONENTATTRIBUTESDIALOG_H_
