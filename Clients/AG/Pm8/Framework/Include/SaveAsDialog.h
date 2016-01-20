//****************************************************************************
//
// File Name:		SaveAsDialog.h
//
// Project:			Renaissance Text Component
//
// Author:			Mike Heydlauf
//
// Description:	defines class RSaveAsDialog.
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/SaveAsDialog.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _SAVEASDIALOG_H_
#define _SAVEASDIALOG_H_

#include "FileFormat.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		SaveAsDataStruct
//
//  Description:		Save data
//
// ****************************************************************************
//
class FrameworkLinkage RSaveAsData
	{
	public :
		// Operations
		virtual void ChangeToDefaultProjectDirectory( );
		virtual BOOLEAN ShouldChangeToDefaultProjectDirectory( );

		// Data
		RMBCString							sPathName;
		RFileFormatCollection			m_FileFormatCollection;
		RFileFormat*						m_pSelectedFileFormat;
	};

// ****************************************************************************
//
//  Class Name:		SaveAsHTMLData
//
//  Description:		Save data for HTML projects
//
// ****************************************************************************
//
class FrameworkLinkage RSaveAsHTMLData : public RSaveAsData
	{
	public :
		// Operations
		virtual void ChangeToDefaultProjectDirectory( );
		virtual BOOLEAN ShouldChangeToDefaultProjectDirectory( );
	};

// ****************************************************************************
//
//  Class Name:		RSaveAsDialog
//
//  Description:		Save dialog
//
// ****************************************************************************
//
class FrameworkLinkage RSaveAsDialog
	{
	// Construction
	public:
												RSaveAsDialog( CWnd* pParent = NULL );

	// Operations
	public :
		int									DoModal( RSaveAsData* pSaveAsData );
		static BOOLEAN						m_fLoadProjectPath; //Flag to load default project path the first time only.
		static BOOLEAN						m_fLoadHTMLProjectPath; //Flag to load default html project path the first time only.

	// Implementation
	protected:

	// Members
	private :
		CWnd*									m_pParent;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_SAVEASDIALOG_H_
