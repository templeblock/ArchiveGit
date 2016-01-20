/* @doc
   @module FileFinder.h.h - implementation of the CBFileFinder class |

   The <c CBFileFinder> class is used to find all files matching
   a given pathname specification.
  
   <cp>Copyright 1995 Brøderbund Software, Inc., all rights reserved
*/

#include "stdafx.h"
#include <io.h>
#include "FileFinder.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CString CBFileFinder::m_strAppDirectory("");


// @mfunc <c CBFileFinder> default constructor.
//
CBFileFinder::CBFileFinder()
{
}


// @mfunc <c CBFileFinder> destructor
//
CBFileFinder::~CBFileFinder()
{
}

// @mfunc Fills an array of strings with complete pathnames matching
// the specified wildcard.
//
// @rdesc TRUE on success
//
BOOL CBFileFinder::GetPathnames(
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
// @mfunc <c CBFileFinder> function to learn the application's "home" directory,
//        based on the Windows API ::GetModuleFilename call.
//
// @rdesc const reference to a CString containing the drive and directory, with a
//        trailing backslash.
//
const CString &CBFileFinder::AppDirectory()
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
