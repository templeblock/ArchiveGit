/*
// $Workfile: UTIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:36p $
*/

/*
// Revision History:
//
// $Log: /PM8/UNINST/UTIL.H $
// 
// 1     3/03/99 6:36p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:40:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:30:46   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 12:38:20   Fred
// Initial revision.
// 
//    Rev 1.1   26 Dec 1995 10:39:24   FRED
// Removed anachronistic "const".
// 
// 
//    Rev 1.0   16 Oct 1995 16:41:38   JAY
// Initial revision.
// 
//    Rev 2.11   13 Sep 1995 11:33:34   JAY
// New 32-bit stuff
// 
//    Rev 2.10   30 Aug 1995 11:00:54   FRED
// Added Trim(). Fixed FormatDouble(negative).
// 
//    Rev 2.9   29 Aug 1995 12:47:08   FRED
// 
//    Rev 2.8   29 Aug 1995 10:53:12   FRED
// Added ValidDouble()
// 
//    Rev 2.7   24 Aug 1995 17:57:56   FRED
// Number string validation functions
// 
//    Rev 2.6   22 Aug 1995 07:57:52   JAY
// Added CBoolSemaphore.
// 
//    Rev 2.5   14 Aug 1995 10:59:30   FRED
// Added FitAspect
// 
//    Rev 2.4   01 Jul 1995 09:24:38   FRED
// Draw3dBorder can now draw optional background.
// 
// 
//    Rev 2.3   30 Jun 1995 10:14:24   FRED
// Added Draw3dBorder()
// 
//    Rev 2.2   05 Jun 1995 16:50:42   JAY
// Added Draw3dButton.
// 
//    Rev 2.1   09 May 1995 08:03:54   JAY
// Added HMemMove.
// 
//    Rev 2.0   07 Feb 1995 15:49:06   JAY
// Initial revision.
// 
//    Rev 1.0   30 Nov 1994 16:40:08   JAY
// Initial revision.
// 
//    Rev 1.9   04 Aug 1994 08:30:56   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   23 Jun 1994 11:46:06   FRED
// RemoveBackslashFromPath() now preserves the
// trailing backslash on a root directory by default.
// 
// 
//    Rev 1.7   21 Jun 1994 14:38:08   FRED
// Removed non-functioning MessageBox() variations.
// Added SplitPath() and ConstructPath() that do drive, dir, file, and ext.
// 
// 
//    Rev 1.6   17 Jun 1994 15:08:02   FRED
// Added some variations on MessageBox().
// 
// 
//    Rev 1.5   17 Jun 1994 14:57:50   FRED
// Added FormatString(), FormatStringV(), and MessageBox().
// 
// 
//    Rev 1.4   16 Jun 1994 18:14:14   FRED
// Added AppendBackslashToPath(), RemoveBackslashFromPath(),
// and ConstructPath().
// 
// 
//    Rev 1.3   16 Jun 1994 15:10:56   FRED
// Changed SplitPath() to take pointers to CStrings.
// 
// 
//    Rev 1.2   16 Jun 1994 14:38:04   JAY
// Added SplitPath().
// 
//    Rev 1.1   16 Jun 1994 09:32:20   JAY
// Added LoadString().
// 
//    Rev 1.0   15 Jun 1994 17:13:06   FRED
// Initial revision.
*/ 

#ifndef _UTIL_H_
#define _UTIL_H_

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
