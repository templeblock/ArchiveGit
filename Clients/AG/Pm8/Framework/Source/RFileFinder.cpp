//****************************************************************************
//
// File Name:		RFileFinder.cpp
//
// Project:			Renaissance framework
//
// Author:			Richard E. Grenfell
//
// Description:	Provides a file search class.
//
// Portability:	Windows Specific
//
// Developed by:	Broderbund Software Inc.
//
//  Copyright (C) 1998 Brøderbund Software, Inc., All Rights Reserved.
//
//  $Logfile: /PM8/Framework/Source/RFileFinder.cpp $
//   $Author: Gbeddow $
//     $Date: 3/03/99 6:17p $
// $Revision: 1 $
//
//****************************************************************************

#include "FrameworkIncludes.h"
#include <io.h>
#include "RFileFinder.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CString RFileFinder::m_strAppDirectory("");


// default constructor.
//
RFileFinder::RFileFinder()
{
}


// Fills an array of strings with complete pathnames matching
// the specified wildcard.
//
// Returns: TRUE on success
//
BOOL RFileFinder::GetPathnames(
	CString strWildcard,					// @parm wildcarded pathname
	CArray<CString, CString> *pPathnames)	// @parm pointer to array to hold pathname strings
{
	BOOL	result = FALSE;

    // do it the old-fashioned way
    struct _finddata_t sData;
    long lSearchHandle;

    lSearchHandle = ::_findfirst(strWildcard, &sData);

    if (lSearchHandle != -1)    // if first name was found
    {
		result = TRUE;

        do  // loop as long as we have file names
        {
            // put name in list
			CString strPathname = sData.name;
			pPathnames->Add (strPathname);

            // find next name
        } while (::_findnext(lSearchHandle, &sData) == 0)
            ;

        ::_findclose(lSearchHandle);
    }

	return result;
}

//////////
// RFileFinder function to learn the application's "home" directory,
//        based on the Windows API ::GetModuleFilename call.
//
// Returns: const reference to a CString containing the drive and directory, with
//				a trailing backslash.
//
const CString &RFileFinder::AppDirectory()
{
    if (m_strAppDirectory.IsEmpty())
    {
        // Get the current application directory
        char szAppPath [_MAX_PATH];
        char szDrive   [_MAX_DRIVE];
        char szDir     [_MAX_DIR];

        ::GetModuleFileName( NULL, szAppPath, _MAX_PATH);
        ::_splitpath( szAppPath, szDrive, szDir, NULL, NULL );

        m_strAppDirectory = CString(szDrive);
        m_strAppDirectory += CString(szDir);
    }

    return m_strAppDirectory;
}
