//****************************************************************************
//
// File Name:		EditFileFailure.H
//
// Project:			Renaissance application framework
//
// Author:			Eric VanHelene
//
// Description:	Defines REditFileFailure class used as a programmers interface
//						to the File Failure Dialog
//
// Portability:	Windows Specific
//
// Developed by:  Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:				Broderbund Software, Inc.
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/EditFileFailure.h                      $
//   $Author:: Gbeddow                                                       $
//     $Date:: 3/03/99 6:15p                                                 $
// $Revision:: 1                                                             $
//
//****************************************************************************

#ifndef _EDITFILEFAILURE_H_
#define _EDITFILEFAILURE_H_

#ifndef _FILEFAILDLG_H_
#include "FileFailDlg.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RFileFailDlg;

class REditFileFailure
{
// Construction
public:
	REditFileFailure::REditFileFailure();
	virtual	~REditFileFailure();

	void SetData ( FileFailureStruct* pMFData );
	void FillData( FileFailureStruct* pMFData );
	BOOLEAN	DoModal();



// Attributes
public:
private:
	FileFailureStruct   m_FileFailData;
	RFileFailDlg        m_FileFailDlg;


};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_EDITFILEFAILURE_H_
