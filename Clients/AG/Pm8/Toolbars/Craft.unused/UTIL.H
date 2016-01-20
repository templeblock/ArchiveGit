/*
// $Workfile: UTIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:35p $
*/

/*
// Revision History:
//
// $Log: /PM8/Toolbars/Printmaster/UTIL.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 31    2/25/99 5:23p Johno
// Moved AppendNamedApplicationRegistryPath() from UTIL.CPP to PMW.CPP,
// as some shared versions of UTIL.CPP could not find needed H file
// 
// 30    2/25/99 3:36p Johno
// Added AppendNamedApplicationRegistryPath() for finding component EXEs
// in the registry.
// 
// 29    2/15/99 3:18p Gbeddow
// performance improvements related to inserting categories in the Art
// Gallery tree control
// 
// 28    2/12/99 7:05p Psasse
// include <afxcmn.h> for compilation
// 
// 27    2/12/99 5:42p Gbeddow
// replace category drop-down control with tree control in the art gallery
// and web art gallery dialogs
// 
// 26    10/22/98 3:59p Jayn
// Added DrawClientBorder
// 
// 25    10/21/98 1:40p Johno
// Added RemoveExtraMenuSeparators()
// 
// 24    10/20/98 5:33p Jayn
// Improving the online order form.
// 
// 23    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 22    10/02/98 7:10p Jayn
// More content manager changes
// 
// 21    8/26/98 4:56p Dennis
// Added File Attribute exclusion flags parameter to FileExists.
// 
// 20    8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 19    7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 18    6/25/98 7:34p Psasse
// added "FillListFromDelimitedString
// 
// 17    6/11/98 9:35a Dennis
// Added instance handle parameter to LoadResourceBitmap()
// 
// 16    5/19/98 2:31p Hforman
// add IsStrInArray()
// 
// 15    2/26/98 4:06p Dennis
// Added ScalePBOXWithRounding
// 
// 14    2/19/98 5:06p Johno
// Added CComboFileNames
// 
// 13    2/10/98 1:28p Dennis
// Added GetRegistryString()
// 
// 12    1/27/98 4:45p Dennis
// Added IdentifierToString MACRO
// 
// 11    1/21/98 4:49p Fredf
// Added SetFileTime()
// 
// 10    1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 9     1/20/98 12:48p Fredf
// 
// 8     1/20/98 10:09a Dennis
// Added YieldToWindows
// 
// 7     1/20/98 8:55a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
// 
// 6     1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 5     12/08/97 9:54a Johno
// Got rid of pointer based memory allocation inline functions
// use 'new' instead
// 
// 4     12/03/97 2:43p Johno
// Added pointer based memory allocation inline functions
// 
// 3     11/14/97 11:59a Johno
// Added `RoundTo` inline functions
// 
// 2     11/12/97 3:38p Johno
// Added ROUNDTO32BITS so you have to do a big build!
// Ha ha!
// 
//    Rev 1.0   14 Aug 1997 15:26:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:24   Fred
// Initial revision.
// 
//    Rev 1.24   30 Jul 1997 14:01:52   Jay
// GetNowTime function in Util
// 
//    Rev 1.23   18 Jul 1997 13:48:30   Jay
// Windows and System directory functions
// 
//    Rev 1.22   11 Jul 1997 11:44:22   johno
// Added DEBUG_ERRORBOX macro / function
// 
//    Rev 1.21   09 Jul 1997 10:54:38   Fred
// Does not strip extra spaces out of category names
// 
//    Rev 1.20   09 Jun 1997 09:12:00   Fred
//  
// 
//    Rev 1.19   05 Jun 1997 17:09:38   Fred
// Operating system queries
// 
//    Rev 1.18   30 May 1997 15:07:36   Fred
// WindowHasTitle and FindWindowByTitle
// 
//    Rev 1.17   22 May 1997 10:29:36   Jay
// Changed hmemcpy to HMemCpy
// 
//    Rev 1.16   15 May 1997 16:49:40   Fred
// Added SafeToConvertPbox and Ppnt
// 
//    Rev 1.15   06 May 1997 17:19:28   dennisp
// Added ComputeTimeStamp
// 
//    Rev 1.14   06 May 1997 15:31:00   Jay
// Added hmemcpy
// 
//    Rev 1.13   25 Apr 1997 10:07:52   Jay
// Added some huge functions
// 
//    Rev 1.12   17 Apr 1997 13:06:02   Fred
// Added source rectangle to TransparentBltBitmap
// 
//    Rev 1.11   17 Apr 1997 12:12:30   Fred
// Added palette argument to TransparentBltBitmap
// 
//    Rev 1.10   15 Apr 1997 14:51:04   dennisp
// Added SafeStringCopy
// 
//    Rev 1.9   09 Apr 1997 17:12:46   Jay
// DoWinExec
// 
//    Rev 1.8   19 Mar 1997 10:11:36   dennisp
// Added Util::Capitalize(), Util::ReplaceChar(), and Util::RemoveChar()
// 
//    Rev 1.7   06 Sep 1996 18:15:10   Jay
// Added ChildWindowFromPointEx
// 
//    Rev 1.6   03 Sep 1996 12:35:30   Jay
// Added TrimQuotes.
// 
//    Rev 1.5   15 Aug 1996 15:56:36   Fred
// LoadResourceBitmap()
// 
//    Rev 1.4   09 Aug 1996 16:51:06   Fred
// More graphic utility programs
// 
//    Rev 1.3   31 Jul 1996 11:58:36   Fred
// 16-bit compatibility
// 
//    Rev 1.2   24 Jun 1996 00:27:38   Fred
// Added some GDI functions and IsKeyMessage()
// 
//    Rev 1.1   15 May 1996 08:51:38   Jay
// Changes from PMW2
// 
//    Rev 2.14   08 May 1996 16:59:42   FRED
// IsRootDirectory
// 
//    Rev 2.13   20 Dec 1995 09:53:54   JAY
// Removed const from BOOL&.
// 
//    Rev 2.12   15 Nov 1995 14:38:14   FRED
// Added FileSharingAvailable
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
#include <malloc.h>           // for hmemcpy
#endif

#include <afxcmn.h>				// for CTreeCtrl

#define DISK_SPACE_NOT_AVAILABLE    ((DWORD)-1)
#define CLUSTER_SIZE_NOT_AVAILABLE  ((DWORD)-1)
// Define DEBUG_ERRORBOX only for WIN32 _DEBUG
#ifdef  _DEBUG      
   #ifdef  WIN32
      #define  ERRORBOX_ALLOWED
      #define  DEBUG_ERRORBOX(s) Util::ErrorBox (s)
   #endif
#endif
// No DEBUG_ERRORBOX - define like ASSERT
#ifndef ERRORBOX_ALLOWED
   #define DEBUG_ERRORBOX(s)  ((void)0)
#endif

#define  ROUNDTO32BITS(x)  (((x) + 3) & ~3)

// Use IdentifierToString to convert identifiers such as function or variable names
// to a character string (surrounded by quotes)
// Converting an identifier to a string is a two step process so MACROs get expanded
#define IdentifierToStringHelper(x)  #x 
#define IdentifierToString(x)  IdentifierToStringHelper(x) 

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
   static BOOL FileExists(const CString& csFile,
      DWORD dwFilteredFileAttrib=FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_DIRECTORY);
	static void GetWindowsDirectory(CString& csDir);
	static void GetSystemDirectory(CString& csDir);
	static BOOL IsWindowsDirectory(const CString& csDir);
	static BOOL IsSystemDirectory(const CString& csDir);
	static void FillListFromDelimitedString(LPCSTR pszInput, LPCSTR seps, CStringList& cslStrings);
	static void FillArrayFromDelimitedString(LPCSTR pszInput, LPCSTR seps, CStringArray& csaStrings);
	static void GetProgramFilesDir(CString& csDir);
	static void FileNameToURLForm(CString& csName);
	static void NormalizeContentServerID(CString& csID);

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
//    throw( CMemoryException, CResourceException )
//
// Primitive printf() formatting utility. Can take resource ID's in place
// of string pointers.
//
// Currently supported formats:
//
//    %d - signed integer (sizeof(int) bytes on stack)
//    %s - FAR string (sizeof(LPSTR) bytes on stack)
//          if high word is zero, then low word is resource ID
*/

   static const CString& FormatString(CString &csOutput, LPCSTR lpszFormat, ...);
   static const CString& FormatStringV(CString &csOutput, LPCSTR lpszFormat, LPCSTR lpArgs);
   
/*
// MessageBox()
//    throw( CMemoryException, CResourceException )
//
// Message box function that takes printf() style arguments.
*/

   static int MessageBox(UINT uType, UINT uHelpID, LPCSTR lpszFormat, ...);
// Only for WIN32 _DEBUG
#ifdef ERRORBOX_ALLOWED
   static void ErrorBox (const char * source);
#endif

/*
// SplitPath()
//  throw( CMemoryException )
//
// Split a full file name into four components (drive, directory, file, extension).
//
// Comments:
//
//    The path is not expanded before parsing.
//    If the path does not contain a file name, then it must end with a backslash.
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
//    csDrive does not end with a backslash
//    csFile does not start with a backslash
//    csFile does not end with a dot
*/

   static CString& ConstructPath(CString &csPath, const CString &csDrive, const CString &csDirectory, const CString &csFile, const CString &csExtension);

/*
// GetDriveType()
//
// Get the type for a drive.
*/

   static UINT GetTypeOfDrive(char cDrive);

/*
// IsRootDirectory(const CString& csDirectory)
//
// Return TRUE if the directory is a root directory.
*/

   static BOOL IsRootDirectory(const CString& csDirectory);
   
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
// hmemcpy
//
// Move huge memory, allowing overlapping moves.
*/

   static void HMemCpy(
      void* pDest,               // Source buffer pointer
      const void* pSource,       // Destination buffer pointer
      unsigned long lCount)      // Count of bytes to move
#ifdef WIN32
      { memcpy(pDest, pSource, lCount); }
#else
      { ::hmemcpy(pDest, pSource, lCount); }
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
// Trim quotes off a string.
*/

   static BOOL TrimQuotes(CString& csString, BOOL fTrimFront = TRUE, BOOL fTrimBack = TRUE);
   
/*
// Capitalize words in a string.
*/
   static void Capitalize(CString& csStringToCapitalize, BOOL fRemoveExtraSpaces = TRUE);

/*
// Replaces characters of a given value with another character in a string.
*/
   static void ReplaceChar(CString& csStringToModify, char chCharToReplace, char chNewChar);

/*
// Removes characters of a given value 
*/
   enum Position
      {
         positionAll, 
      };
   enum CharType
      {
         NotAlphaNumeric, 
         NotAlpha, 
         NotDigit
      };
   static void RemoveChar(CString& csString, enum CharType enChType, enum Position enPosition=positionAll);

// SafeStringCopy copies CString contents to a char * destination 
//   for a maximum of the specified buffer size which includes NULL terminator
//   String is NULL terminated after copy
   static char * SafeStringCopy(char *szDest, CString strSource, size_t nDestBuffSize);

	// Check if str is in strArray
	static BOOL IsStrInArray(const CStringArray& strArray, const CString& str, BOOL fNoCase = TRUE);

   static void SwapWORDS(WORD* pData, long lCount);
   static void SwapWORD(WORD* w);
   static void SwapDWORD(DWORD* l);

/*
// Check if file sharing is available.
*/

   static BOOL FileSharingAvailable(void);

/*
// Some useful graphics functions.
*/

   static void FillRectangleWithColor(CDC& dc, const CRect& crRect, COLORREF clColor);
   static void DrawBorderWithColor(CDC& dc, const CRect& crRect, COLORREF clColor, int nSize = 1);
   static void DrawBrushThroughMask(CDC& dcDestination, const CRect& crDestination, CDC& dcMask, const CRect& crMask, CBrush& cbBrush, BOOL fInvertMask = FALSE);
   static void DrawColorThroughMask(CDC& dcDestination, const CRect& crDestination, CDC& dcMask, const CRect& crMask, COLORREF clColor, BOOL fInvertMask = FALSE);
   static void CreateMask(CDC& dcMask, CBitmap& bmMask, CRect& crMask, CDC& dcSource, const CRect& crSource, COLORREF clColor, BOOL fInitializeMask = TRUE);
   static void Emboss(CDC& dc, const CRect& crRect, COLORREF clBackground, COLORREF clHighlight, COLORREF clEmbossShadow, COLORREF clEmbossHighlight);
   static void TransparentBlt(CDC& dcDestination, const CRect& crDestination, CDC& dcSource, const CRect& crSource, COLORREF clTransparent);
   static void TransparentBltBitmap(CDC& dcDestination, const CRect& crDestination, CBitmap& bmBitmap, LPCRECT pSourceRect = NULL, CPalette* pPalette = NULL, UINT uTransparentX = 0, UINT uTransparentY = (UINT)-1);
   static BOOL LoadResourceBitmap(CBitmap& Bitmap, LPCSTR pszResource, CPalette* pPalette, HINSTANCE hInst=AfxGetResourceHandle(), LPCSTR pszType = RT_BITMAP);

/*
// Draw a client border.
*/

	static void DrawClientBorder(CDC& dc, CRect& crRect);

/*
// Check if a message is for a particular key.
*/

   static BOOL IsKeyMessage(LPMSG pMsg, UINT uKey = 0 /* I used to work with uKey! */, ...);
   
/*
// 16/32 bit independent functions.
*/

   static LRESULT SendWmCommand(CWnd* pWindow, UINT uID, CWnd* pControl, WORD wNotifyCode);
   static BOOL PostWmCommand(CWnd* pWindow, UINT uID, CWnd* pControl, WORD wNotifyCode);
   static int GetMenuString(CMenu* pMenu, UINT nIDItem, CString& csString, UINT nFlags);
   static int DrawText(CDC* pDC, const CString& csString, LPRECT pRect, UINT nFormat);
   static CSize GetTextExtent(CDC* pDC, const CString& csString);
   static int GetBitmap(CBitmap* pBitmap, BITMAP* pBitmapInfo);
   static BOOL ReadString(CStdioFile* pFile, CString& csString);
   static int GetDlgItemText(CWnd* pWnd, int nID, CString& csString);
   static HWND ChildWindowFromPointEx(HWND hWnd, POINT pt);
   static BOOL DoWinExec(LPCSTR pszCommandLine, UINT nShow = SW_SHOW, BOOL fModalProcess = FALSE);
   static BOOL WaitForWindow(UINT uTitle, DWORD dwMSecsToWait);
   static BOOL WaitForWindow(LPCSTR pszTitle, DWORD dwMSecsToWait);
   static BOOL WindowHasTitle(HWND hWnd, LPCSTR pszTitle, BOOL fPrefix = TRUE);
   static HWND FindWindowByTitle(LPCSTR pszTitle, BOOL fPrefix = TRUE);
   static void YieldToWindows(void);

   static LPVOID HugeAlloc(DWORD dwSize);
   static void HugeFree(LPVOID pData);

	static BOOL GetFileTime(LPCSTR pszFileName, LPFILETIME pCreationTime, LPFILETIME pLastAccessTime, LPFILETIME pLastWriteTime);
	static BOOL SetFileTime(LPCSTR pszFileName, LPFILETIME pCreationTime, LPFILETIME pLastAccessTime, LPFILETIME pLastWriteTime);

/*
// Get operating system information.
*/

   static void GetOperatingSystem(BOOL& fWin3, BOOL& fWin95, BOOL& fWinNT, WORD& wVersion);
   static BOOL IsWin3(void);
   static BOOL IsWin95(void);
   static BOOL IsWinNT(void);
   static BOOL GetRegistryString(HKEY hRootKey, LPCTSTR pszSubkey, LPCTSTR pszValue, CString& csValue);

   static DWORD
   RoundToDWORD (DWORD d)
   {
      return ((d + 3) & ~3);
   }
   
   static DWORD
   RoundToWORD (DWORD d)
   {
      return ((d + 1) & ~1);
   }

   static WORD
   RoundToWORD (WORD w)
   {
      return ((w + 1) & ~1);
   }
	// Removes menu separators with nothing between them
	static void
	RemoveExtraMenuSeparators(CMenu *pMenu);
/* Better to use new
   // Memory helper - now with chives and garlic!
   static LPVOID
   GlobalAllocPtr (UINT flags, DWORD cb)
   {
      return GlobalLock (GlobalAlloc (flags, cb));
   }

   static HGLOBAL
   GlobalPtrHandle (LPVOID lp)         \
   {
      return GlobalHandle (lp);
   }

   static BOOL
   GlobalUnlockPtr (LPVOID lp)
   {
      return GlobalUnlock (Util::GlobalPtrHandle (lp));
   }

   static HGLOBAL
   GlobalFreePtr (LPVOID lp)
   {
      Util::GlobalUnlockPtr (lp); 
      return GlobalFree (Util::GlobalPtrHandle (lp));
   }
*/

/*
// CTreeCtrl utility functions
*/

	// return the HTREEITEM in treeCtrl whose parent is hTreeCtrlParent and that matches cString, else NULL
	//
	static HTREEITEM FindTreeCtrlStringExact(
		const CTreeCtrl& treeCtrl, HTREEITEM hTreeCtrlParent, const char* pszString );

	// recursively sort treeCtrl, starting from hParent
	//
	static void SortTreeCtrl( CTreeCtrl& treeCtrl, HTREEITEM hParent = NULL );

	// add '|' delimited strings to csCategory that represent the full category "path"
	// of hTreeItem in treeCtrl, for instance "Backgrounds|Floral"
	//
	static void AddCategorySelectionStrings( const CTreeCtrl& treeCtrl, HTREEITEM hTreeItem, CString& csCategories );

	// set selected item in treeCtrl from '|' delimited full category "path" string
	//
	static void SetTreeCtrlSelection( CTreeCtrl& treeCtrl, HTREEITEM hParent, const CString& csCategories );
};

// Global function usable even by those not including util.h
extern void HMemSet(LPVOID pDst, int nValue, long lSize);

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
// Displays file names without extension
// Stores full name
#define  BAD_TRIBS   (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_TEMPORARY)
class CComboFileNames : public CComboBox
{
public:   
   
   CComboFileNames(DWORD bt = BAD_TRIBS);

   BOOL
   Fill(LPCSTR FileSpec);

   int
   AddFoundFileName(WIN32_FIND_DATA *pData);
   
   LPCSTR
   GetFilename(int idx) const
   {
      if ((idx >= 0) && (idx <= m_fileNames.GetSize()))
      {
         return m_fileNames.GetAt(idx);
      }
      return NULL;
   }

   
   LPCSTR
   GetFullName(int idx) const
   {
      if (idx != CB_ERR)
      {   
         int ComboIndex;
         if ((ComboIndex = GetItemData(idx)) != CB_ERR)
         return GetFilename(ComboIndex);
      }
      return NULL;
   }
   
   LPCSTR
   GetFullName(LPCSTR str) const
   {
      return GetFullName(FindStringExact(-1, str));
   }

   LPCSTR
   GetSelectedFullName(void) const
   {
      return GetFullName(GetCurSel());
   }

protected:
   DWORD          mdwBadTribs;
   CStringArray   m_fileNames;
   
   virtual CString
   GetFriendlyName (LPCSTR str);
};
#endif
