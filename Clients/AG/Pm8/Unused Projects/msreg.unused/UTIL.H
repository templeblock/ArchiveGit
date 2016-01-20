/*
// $Workfile: UTIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/UTIL.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:28p Jayn
// 
//    Rev 1.1   14 Dec 1995 11:07:20   FRED
// Changes for VC++ 4.0
// 
//    Rev 1.0   22 Nov 1995 08:51:50   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:42   FRED
//  
// 
//    Rev 1.0   25 Oct 1995 17:47:06   FRED
// Initial revision.
*/ 

class Util;

#ifndef __UTIL_H__
#define __UTIL_H__

#ifndef WIN32
#include <dos.h>
#endif

#define DISK_SPACE_NOT_AVAILABLE		((DWORD)-1)
#define CLUSTER_SIZE_NOT_AVAILABLE	((DWORD)-1)

class Util
{
public:
	static void CenterWindowInWindow(CWnd *pcwndWindow, CWnd *pcwndInWindow = NULL);
	static void FormatDouble(double dNumber, int nPrecision, CString& csOutput);
	static DWORD GetAvailableDiskSpace(const CString &csDirectory);
	static DWORD GetAvailableDiskSpace(char cDriveLetter);
	static DWORD GetClusterSize(const CString &csDirectory);
	static DWORD GetClusterSize(char cDriveLetter);
	static BOOL MakeDirectory(const CString &csDirectory);
	static BOOL DirectoryExists(const CString& csDirectory);
	static BOOL DirectoryIsWritable(const CString& csDirectory);
	static BOOL ExpandDirectory(CString &csDirectory, BOOL fAppendBackslash = TRUE);
	static BOOL DirectoryDrive(const CString csDirectory, CString &csDrive);
	static BOOL FileExists(const CString& csFile);
	
/*
// LoadString()
//  throw( CMemoryException, CResourceException);
//
// Loads the string resource identified by 'nID' into CString 'csString'.
// Returns 'csString'.
*/
	static CString& LoadString(UINT nID, CString& csString);

/*
// SplitPath()
//  throw( CMemoryException )
//
// Split a full file name into its two components (path and file name).
*/

	static void SplitPath(const CString &csFullPath, CString *pcsPath, CString *pcsFilename);
	
/*
// AppendBackslashToPath()
//  throw( CMemoryException )
//
// Make sure a path ends with a backslash.
*/

	static CString& AppendBackslashToPath(CString &csPath);

/*
// RemoveBackslashFromPath()
//  throw( CMemoryException )
//
// Make sure a path does not end with a backslash.
*/

	static CString& RemoveBackslashFromPath(CString &csPath, BOOL fKeepRootDirectoryBackslash = TRUE);

/*
// ConstructPath()
//  throw( CMemoryException )
//
// Construct a full file name from its two components (path and file name).
*/

	static CString& ConstructPath(CString &csFullPath, const CString &csPath, const CString &csFileName);
	
/*
// FormatString(), FormatStringV()
//		throw( CMemoryException, CResourceException )
//
// Primitive printf() formatting utility. Can take resource ID's in place
// of string pointers.
//
// Currently supported formats:
//
//		%d - signed integer (sizeof(int) bytes on stack)
//		%s - FAR string (sizeof(LPSTR) bytes on stack)
//				if high word is zero, then low word is resource ID
*/

	static const CString& FormatString(CString &csOutput, LPCSTR lpszFormat, ...);
	static const CString& FormatStringV(CString &csOutput, LPCSTR lpszFormat, LPCSTR lpArgs);
	
/*
// MessageBox()
//		throw( CMemoryException, CResourceException )
//
// Message box function that takes printf() style arguments.
*/

	static int MessageBox(UINT uType, UINT uHelpID, LPCSTR lpszFormat, ...);
	
/*
// SplitPath()
//  throw( CMemoryException )
//
// Split a full file name into four components (drive, directory, file, extension).
//
// Comments:
//
//		The path is not expanded before parsing.
//		If the path does not contain a file name, then it must end with a backslash.
*/

	static void SplitPath(const CString &csPath, CString *pcsDrive, CString *pcsDirectory, CString *pcsFile, CString *pcsExtension);
	
/*
// ConstructPath()
//  throw( CMemoryException )
//
// Construct a full file name from four components (drive, directory, file, and extension)
//
// Assumptions:
//
//		csDrive does not end with a backslash
//		csFile does not start with a backslash
//		csFile does not end with a dot
*/

	static CString& ConstructPath(CString &csPath, const CString &csDrive, const CString &csDirectory, const CString &csFile, const CString &csExtension);

/*
// GetDriveType()
//
// Get the type for a drive.
*/

	static UINT GetTypeOfDrive(char cDrive);

/*
// HMemMove
//
// Move huge memory, allowing overlapping moves.
*/

#ifndef WIN32
	static void HMemMove(
	   char __huge* hpSource,        // Source buffer pointer
	   char __huge* hpDest,          // Destination buffer pointer
	   unsigned long lCount);        // Count of bytes to move
#endif

/*
// Draw3dButton()
//
// Draw Windows 3.X type button.
*/

	static BOOL Draw3dButton(CDC* pDC, const CRect& crButton, COLORREF clBorder, COLORREF clHighlight, COLORREF clShadow, COLORREF clFace, int nDepth = 2, BOOL fMask = FALSE);
	
/*
// Draw3dBorder
//
// Draw a 3D border with optional inner border.
*/

	static BOOL Draw3dBorder(
		CDC* pDC,
		const CRect& crBorder,
		COLORREF clHighlight,
		COLORREF clShadow,
		COLORREF clBorderHighlight,
		COLORREF clBorderShadow,
		COLORREF clBackground,
		int nDepth = 2,
		BOOL fDrawInsideBorder = FALSE,
		BOOL fDrawBackground = FALSE,
		BOOL fMask = FALSE);
		
/*
// FitAspect
//
// Fit an aspect in a target extent.
*/

	static void FitAspect(int* pnAspectX, int* pnAspectY, int nTargetX, int nTargetY);
	
/*
// Validate a string representation of a number.
*/

	static BOOL ValidSignedNumber(LPCSTR pString, long lMinimum = -0x7fffffff, long lMaximum = 0x7fffffff);
	static BOOL ValidUnsignedNumber(LPCSTR pString, unsigned long lMinimum = 0, unsigned long lMaximum = 0xffffffff);
	
/*
// Validate a string representation of a floating point number.
*/

	static BOOL ValidDouble(LPCSTR pString, double dMinimum = -1e308, double dMaximum = 1e308);
	
/*
// Trim white space (spaces and tabs) off a string.
*/

	static BOOL Trim(CString& csString, BOOL fTrimFront = TRUE, BOOL fTrimBack = TRUE);
};

/*
// A semaphore class to enable clean-up upon routine exit.
*/

class CBoolSemaphore
{
private:
	BOOL& m_rBool;
	BOOL m_fLocked;
public:
	CBoolSemaphore(BOOL &rBool);
	~CBoolSemaphore(void);
	BOOL Lock(void);
	void Unlock(void);
};

/*
// A directory walking class.
*/

class CFileIterator
{
public:
	CFileIterator();
	~CFileIterator();
	BOOL FindFirst(LPCSTR pName, DWORD dwDOSAttributes);
	BOOL FindNext(void);
	void Close(void);

	LPCSTR Name(void);
	DWORD Attributes(void);
protected:
#ifdef WIN32
	HANDLE m_hSearch;
	WIN32_FIND_DATA m_ffd;
	DWORD m_dwExclude;
#else
	struct find_t m_buffer;
#endif
};

#endif
