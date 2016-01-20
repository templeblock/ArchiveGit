/*
// $Workfile: CSIGNAL.H $
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
// $Log: /PM8/App/CSIGNAL.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:18   Fred
// Initial revision.
// 
//    Rev 1.0   26 Mar 1997 15:18:08   Fred
// Initial revision.
*/

class CSignal;

#ifndef __HEADER_H__
#define __HEADER_H__

#include <signal.h>

class CSignal
{
public:
	CSignal();
	virtual ~CSignal();
	
public:
	void (_cdecl * _cdecl SetSignal(int nSignal, void (_cdecl *pProcessor)(int nSignal)))(int nSignal);
	static void ProcessFPE(int nSignal);
};

#endif
