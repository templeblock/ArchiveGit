/*
// $Workfile: CSIGNAL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CSIGNAL.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:50   Fred
// Initial revision.
// 
//    Rev 1.0   26 Mar 1997 15:18:08   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "csignal.h"

CSignal SignalHandler;

CSignal::CSignal()
{
	SetSignal(SIGFPE, ProcessFPE);
}

CSignal::~CSignal()
{
}

void (_cdecl * _cdecl CSignal::SetSignal(int nSignal, void (_cdecl *pProcessor)(int nSignal)))(int nSignal)
{
	return signal(nSignal, pProcessor);
};

void CSignal::ProcessFPE(int nSignal)
{
	od("Floating Point exception!\n");
}
