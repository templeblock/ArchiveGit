/*
// $Workfile: AUTOEXEC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/AUTOEXEC.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:28:24   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:32:16   FRED
// Initial revision.
// 
//    Rev 1.0   15 Nov 1995 14:41:12   FRED
// Initial revision.
*/

class CAutoexecBatFile;

#ifndef __AUTOEXEC_H__
#define __AUTOEXEC_H__

#include "textline.h"

class CAutoexecBatFile : public CTextLineFile
{
public:
	CAutoexecBatFile();
	virtual ~CAutoexecBatFile();
	
public:
	BOOL Locate(int nDrive = -1);
};

#endif					// #ifndef __AUTOEXEC_H__
