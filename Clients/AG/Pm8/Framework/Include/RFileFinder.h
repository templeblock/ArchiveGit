//****************************************************************************
//
// File Name:		RFileFinder.h
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
//  $Logfile: /PM8/Framework/Include/RFileFinder.h $
//   $Author: Gbeddow $
//     $Date: 3/03/99 6:16p $
// $Revision: 1 $
//
//****************************************************************************

#ifndef RFILEFINDER_H
#define RFILEFINDER_H

#include <afxtempl.h>

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// The object used to find files
//
class FrameworkLinkage RFileFinder
{
public:
	// constructor
    RFileFinder();

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

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// RFILEFINDER_H
