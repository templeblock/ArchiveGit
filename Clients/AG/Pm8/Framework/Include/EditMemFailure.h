//****************************************************************************
//
// File Name:			EditMemFailure.H
//
// Project:				Renaissance application framework
//
// Author:				Mike Heydlauf
//
// Description:		Defines REditMemFailure class used as a programmers interface
//							to the Memory Failure Dialog
//
// Portability:		Windows Specific
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, Ma 02158
//							(617)332-0202
//
//  Client:				Broderbund Software, Inc.         
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/EditMemFailure.h                       $
//   $Author:: Gbeddow                                                       $
//     $Date:: 3/03/99 6:15p                                                 $
// $Revision:: 1                                                             $
//
//****************************************************************************

#ifndef _EDITMEMFAILURE_H_
#define _EDITMEMFAILURE_H_

#ifndef	_MEMORYFAILDLG_H_
#include "MemoryFailDlg.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RMemoryFailDlg;

class REditMemFailure
{
// Construction
public:
	REditMemFailure::REditMemFailure();
	virtual	~REditMemFailure();

	void SetData ( MemoryFailureStruct* pMFData );
	void FillData( MemoryFailureStruct* pMFData );
	BOOLEAN	DoModal();



// Attributes
public:
private:
	MemoryFailureStruct   m_MemFailData;
	RMemoryFailDlg        m_MemFailDlg;


};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_EDITMEMFAILURE_H_
