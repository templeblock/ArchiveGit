/*
// $Workfile: AUTOEXEC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/AUTOEXEC.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:24:50   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:49:26   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:26:22   FRED
// Initial revision.
// 
//    Rev 1.1   20 Dec 1995 09:56:30   JAY
// 32-bit fixes
// 
//    Rev 1.0   15 Nov 1995 14:41:12   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "util.h"
#include "autoexec.h"

#ifndef WIN32
extern "C" void far pascal DOS3Call(void);
#endif

////////////////////////////////////////////////////////////////
// CAutoexecBatFile

CAutoexecBatFile::CAutoexecBatFile()
{
}

CAutoexecBatFile::~CAutoexecBatFile()
{
}

BOOL CAutoexecBatFile::Locate(int nDrive /*=-1*/)
{
#ifdef _MAC
   // As if you didn't know, the Mac has no autoexec.bat file!
   return FALSE;
#else

	BOOL fResult = FALSE;
	
	char szName[_MAX_PATH];
	
	// Get drive to use.
	
#ifndef WIN32
	if (nDrive == -1)
	{
		// Get the boot drive using int 21/3305
		// This is only available on DOS 4.0 or better.
		
		if (HIBYTE(HIWORD(GetVersion())) > 4)
		{
			_asm
			{
				push	ax
				push	dx
				
				mov	ax,03305h
	
				call	DOS3Call
				
				xor	dh,dh
				dec	dx				; A <-- 0, ...
				mov	nDrive,dx
				
				pop	dx
				pop	ax
			}
		}
	}
#endif

	// If we still don't have a drive, search for a fixed disk with an
	// AUTOEXEC.BAT file in the root.
	
	if (nDrive == -1)
	{
		for (int nTestDrive = 0; nTestDrive < 26; nTestDrive++)
		{
#ifdef WIN32
			static char cbDrive[] = "x:\\";
			cbDrive[0] = nTestDrive+'A'+1;
			if (GetDriveType(cbDrive) == DRIVE_FIXED)
#else
			if (GetDriveType(nTestDrive) == DRIVE_FIXED)
#endif
			{
				wsprintf(szName, "%c:\\AUTOEXEC.BAT", nTestDrive+'A');
				if (Util::FileExists(szName))
				{
					// Found an AUTOEXEC.BAT in the root directory, use this drive.
					nDrive = nTestDrive;
					break;
				}
			}
		}
	}
	
	if (nDrive != -1)
	{
		// Use the name AUTOEXEC.BAT on the drive we found.
		wsprintf(szName, "%c:\\AUTOEXEC.BAT", nDrive+'A');
		TRACE("Autoexec.bat file = '%s\n", szName);
		if (Util::FileExists(szName))
		{
			fResult = Name(szName);
		}
	}
	
	return fResult;

#endif // else _MAC
}
