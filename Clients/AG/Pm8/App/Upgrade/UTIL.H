/*
// $Workfile: UTIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/UTIL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:02   Fred
// Initial revision.
// 
//    Rev 1.2   09 Jul 1997 09:55:36   Fred
// Moved strings to resource file
// 
//    Rev 1.1   14 Oct 1996 13:34:34   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:41:08   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:36   JAY
// Initial revision.
// 
//    Rev 1.6   29 Nov 1995 10:24:16   JAY
// Added Trim().
// 
//    Rev 1.5   19 Jan 1995 14:20:38   JAY
// New free pictures!
// 
//    Rev 1.4   13 Jan 1995 13:47:26   FRED
//  
// 
//    Rev 1.3   10 Jan 1995 16:16:36   FRED
//  
// 
//    Rev 1.2   22 Dec 1994 10:25:04   JAY
// Moved MAKE_DOUBLE to UTIL.H
// 
//    Rev 1.1   02 Dec 1994 14:51:28   FRED
//  
// 
//    Rev 1.0   03 Nov 1994 12:57:02   FRED
//  
// 
//    Rev 1.0   25 Oct 1994 16:21:52   JAY
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

#define	MAKE_DOUBLE(l) (((double)(((l)+5000L)/10000L))/100.0)

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
// SafeLoadString()
//  does not throw exceptions, an empty string is returned on error.
//
// Loads the string resource identified by 'nID' into CString 'csString'.
*/
	static BOOL SafeLoadString(UINT nID, CString& csString);

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
// FormatWindowText()
//		throw( CMemoryException, CResourceException )
//
// Retrieves the text of a window and uses this text as the format string
// for a call to FormatString() with the rest of the arguments.
*/

	static const void FormatWindowText(CWnd* pWnd, ...);
	
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
// Draw3dButton()
//
// Draw Windows 3.X type button.
*/

	static BOOL Draw3dButton(CDC* pDC, const CRect& crButton, COLORREF clBorder, COLORREF clHighlight, COLORREF clShadow, COLORREF clFace, int nDepth = 2, BOOL fMask = FALSE);

/*
// PriceString()
//
// Create a string corresponding to a price (in cents).
*/

	static void PriceString(CString& csPrice, long lPrice, BOOL fDollarSign = TRUE, LPCSTR pszBlankString = NULL);

/*
// Trim white space (spaces and tabs) off a string.
*/

	static BOOL Trim(CString& csString, BOOL fTrimFront = TRUE, BOOL fTrimBack = TRUE);

/*
// Fill a rectangle with a given color.
*/

	static void FillRectangleWithColor(CDC& dc, const CRect& crRect, COLORREF clColor);

/*
// Draw a client border.
*/

	static void DrawClientBorder(CDC& dc, CRect& crRect);
};

#endif
