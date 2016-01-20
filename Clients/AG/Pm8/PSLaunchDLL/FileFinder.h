/* @doc
   @module FileFinder.h.h - definition of the CBFileFinder class |

   The <c CBFileFinder> class is used to find all files matching
   a given pathname specification.
  
   <cp>Copyright 1995 Brøderbund Software, Inc., all rights reserved
*/

#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <afxtempl.h>

// @class The object used to find files
//
class CBFileFinder
{
public:
	// @cmember constructor
    CBFileFinder();
	// @cmember destructor
    ~CBFileFinder();

	// @cmember returns an array of pathnames matching the specified wildcard
	BOOL GetPathnames (CString strWildcard, CArray<CString, CString> *pPathnames);

    // @cmember returns the application's home drive/directory
    const CString &AppDirectory();

private:
	// @cmember array of complete (matched) path names
	CArray<CString, CString>	m_Pathnames;

    // @cmember application EXE drive/directory
    static CString m_strAppDirectory;
};

#endif
  
