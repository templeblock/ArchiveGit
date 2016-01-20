/*
// $Workfile: UTIL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/UTIL.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.1   16 Sep 1997 16:01:44   Jay
// Missing parens.
// 
//    Rev 1.0   14 Aug 1997 15:42:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:36   Fred
// Initial revision.
// 
//    Rev 1.2   09 Jul 1997 09:55:32   Fred
// Moved strings to resource file
// 
//    Rev 1.1   14 Oct 1996 13:34:46   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:42   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:57:56   JAY
// Initial revision.
// 
//    Rev 1.8   15 Dec 1995 17:59:04   FRED
// Forces tax on CA modem orders.
// 
//    Rev 1.7   29 Nov 1995 10:24:20   JAY
// Added Trim().
// 
//    Rev 1.6   19 Jan 1995 14:21:12   JAY
// New free pictures!
// 
//    Rev 1.5   13 Jan 1995 13:47:10   FRED
//  
// 
//    Rev 1.4   10 Jan 1995 16:16:20   FRED
//  
// 
//    Rev 1.3   02 Dec 1994 14:51:06   FRED
//  
// 
//    Rev 1.2   11 Nov 1994 17:03:10   JAY
// Now doesn't check the file state of a root directory (since the results
// were unpredictable).
// 
//    Rev 1.1   10 Nov 1994 17:44:46   JAY
//  
// 
//    Rev 1.0   03 Nov 1994 12:54:24   FRED
//  
// 
//    Rev 1.0   25 Oct 1994 16:21:50   JAY
// Initial revision.
// 
//    Rev 1.11   04 Aug 1994 14:29:38   JAY
// GetDriveType (WIN16) wants a drive number (0=A, 1=B, etc).
// 
//    Rev 1.10   04 Aug 1994 08:30:56   JAY
// Large model and 32-bit changes
// 
//    Rev 1.9   23 Jun 1994 11:46:34   FRED
// RemoveBackslashFromPath() now preserves the
// trailing backslash on a root directory by default.
// 
// 
//    Rev 1.8   21 Jun 1994 15:41:12   FRED
// Fixed typo.
// 
// 
//    Rev 1.7   21 Jun 1994 14:39:34   FRED
// Removed non-functioning MessageBox() variations.
// Added SplitPath() and ConstructPath() that do drive, dir, file, and ext.
// 
// 
//    Rev 1.6   17 Jun 1994 15:08:14   FRED
// No change.
// 
//    Rev 1.5   17 Jun 1994 14:57:30   FRED
// Added FormatString(), FormatStringV(), and MessageBox().
// 
// 
//    Rev 1.4   16 Jun 1994 18:13:48   FRED
// Added AppendBackslashToPath(), RemoveBackslashFromPath(),
// and ConstructPath().
// 
// 
//    Rev 1.3   16 Jun 1994 15:11:32   FRED
// Changed SplitPath() to take pointers to CStrings.
// 
// 
//    Rev 1.2   16 Jun 1994 14:38:08   JAY
// Added SplitPath().
// 
//    Rev 1.1   16 Jun 1994 09:32:10   JAY
// Added LoadString().
// 
//    Rev 1.0   15 Jun 1994 17:13:04   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "upgrade.h"
#include "util.h"

#include <math.h>
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef _WIN32
extern "C" void FAR PASCAL DOS3Call(void);
#endif

void Util::CenterWindowInWindow(CWnd *pcwndWindow, CWnd *pcwndInWindow)
{
	if (pcwndWindow != NULL)
	{
		if (pcwndInWindow == NULL)
		{
			if ((pcwndInWindow = pcwndWindow->GetParent()) == NULL)
			{
				pcwndInWindow = CWnd::GetDesktopWindow();
			}
		}

		if (pcwndInWindow != NULL)
		{
			CRect crRect;
			pcwndWindow->GetWindowRect(&crRect);
			
			CRect crInRect;
			pcwndInWindow->GetWindowRect(&crInRect);

			crRect.OffsetRect(
				((crInRect.right-crRect.right)-(crRect.left-crInRect.left))/2,
				((crInRect.bottom-crRect.bottom)-(crRect.top-crInRect.top))/2);

			pcwndWindow->MoveWindow((LPRECT)crRect, TRUE);
		}
	}
}
      
/*
// Format a double precision value into a string with a maximum
// number of places after the decimal point.
//
// If the precision is negative, then trailing zeros are stripped.
*/

void Util::FormatDouble(double dNumber, int nPrecision, CString& csOutput)
{
	csOutput.Empty();
	
	// if precision is negative, trim off trailing zeroes
	BOOL fTrim;
	
	if (fTrim = (nPrecision < 0))
	{
		nPrecision = -nPrecision;
	}
	
	// save sign
	BOOL bNegative = FALSE;
	
	if (dNumber < 0.0)
	{
		bNegative = TRUE;
		dNumber = -dNumber;
	}
	
	// compute minimum value that can be represented.	
	double dMin = 1.0;
	int i;

	for (i = 0; i < nPrecision; i++)
	{
		dMin *= 10.0;
	}
	
	// do one divide to minimize roundoff errors.
	dMin = 1.0/dMin;
	
	// round value for given precision
	dNumber += dMin/2.0;
		
	// if value is too small, return a zero
	if (dNumber < dMin)
	{
		csOutput = "0";
	}
	
	else
	{
		// check for negative number and add sign if necessary
		if (bNegative)
		{
			csOutput += '-';
			dNumber = -dNumber;
		}
	
		// put leading zero before decimal point.
		if (dNumber < 1.0)
		{
			csOutput += '0';
		}
		
		// normalize number (<1.0) and count digits to the left
		int nCount = 0;
		
		while (dNumber >= 1.0)
		{
			dNumber /= 10.0;
			nCount++;
		}
		
		// place digits into string
		
		int nTrim = csOutput.GetLength();
		
		nPrecision = -nPrecision;
		
		while (nCount > nPrecision)
		{
			if (nCount == 0)
			{
				csOutput += '.';
			}
			
			double dDigit;
			char cDigit;
			
			dNumber = modf(dNumber*10.0, &dDigit);
			csOutput += (cDigit = (char)((int)dDigit)+'0');
			
			if ((nCount > 0) || (cDigit != '0'))
			{
				nTrim = csOutput.GetLength();
			}
			
			nCount--;
		}
	
		if (fTrim)
		{	
			csOutput = csOutput.Left(nTrim);
		}
	}
}

DWORD Util::GetAvailableDiskSpace(const CString &csDirectory)
{
	CString csExpanded;
	
	DWORD dwResult = DISK_SPACE_NOT_AVAILABLE;
	
	TRY
	{
		csExpanded = csDirectory;
	}
	CATCH_ALL(e)
	{
		csExpanded.Empty();
	}
	END_CATCH_ALL
	
	if (ExpandDirectory(csExpanded, FALSE))
	{
#ifdef _WIN32
		DWORD dwSectorsPerCluster;
		DWORD dwBytesPerSector;
		DWORD dwFreeClusters;
		DWORD dwTotalClusters;

		if (GetDiskFreeSpace(csExpanded,
									&dwSectorsPerCluster,
									&dwBytesPerSector,
									&dwFreeClusters,
									&dwTotalClusters))
		{
			dwResult = dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
		}
#else

		if ((csExpanded.GetLength() >= 2) && (csExpanded[1] == ':'))
		{
			dwResult = GetAvailableDiskSpace(csExpanded[0]);
		}
			
		else if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
		{
			/*
			// We need to connect, find a drive to use.
			*/
				
			for (char cDrive = 0; cDrive < 26; cDrive++)
			{
				if (GetTypeOfDrive(cDrive) == 0)
				{
					break;
				}
			}
				
			if (cDrive < 26)
			{
				/*
				// Try to make the connection.
				*/
					
				char szDestinationPath[] = "@:";
				szDestinationPath[0] = cDrive+'A';
					
				UINT uResult;
					
				if ((uResult = ::WNetAddConnection((LPSTR)((LPCSTR)csDirectory), "", szDestinationPath)) == WN_SUCCESS)
				{
					/*
					// Get the disk space.
					*/
						
					dwResult = GetAvailableDiskSpace(szDestinationPath[0]);

					/*
					// Done with the connection.
					*/
						
					::WNetCancelConnection(szDestinationPath, TRUE);
				}
			}
		}
#endif
	}
	
	return dwResult;
}

DWORD Util::GetAvailableDiskSpace(char cDriveLetter)
{
	DWORD dwSpaceAvailable = (DWORD)(-1L);

#ifdef _WIN32
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwFreeClusters;
	DWORD dwTotalClusters;
	char cbName[] = "X:\\";

	cbName[0] = cDriveLetter;

	if (GetDiskFreeSpace(cbName,
								&dwSectorsPerCluster,
								&dwBytesPerSector,
								&dwFreeClusters,
								&dwTotalClusters))
	{
		dwSpaceAvailable = dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
	}

#else
	/*
	// Make sure there is enough disk space available on the
	// chosen drive.
	*/

	_asm
	{
		mov	dl,cDriveLetter
		cmp	dl,'a'
		jb		L0
		cmp	dl,'z'
		ja		L0
		add	dl,'A'-'a'
	L0:
		cmp	dl,'A'
		jb		L1
		cmp	dl,'Z'
		ja		L1
		sub	dl,'A'-1
		mov	ah,036h
		call	DOS3Call
		cmp	ax,-1
		je		L1
		mul	cx
		mov	cx,ax
		mov	ax,dx
		mul	bx
		xchg	ax,cx
		mul	bx
		add	dx,cx
		jmp	L2
	L1:
		mov	ax,-1
		mov	dx,-1
	L2:
		mov	word ptr dwSpaceAvailable+0,ax
		mov	word ptr dwSpaceAvailable+2,dx
	}
#endif
	
	return dwSpaceAvailable;
}

DWORD Util::GetClusterSize(const CString &csDirectory)
{
	CString csExpanded;
	
	DWORD dwResult = CLUSTER_SIZE_NOT_AVAILABLE;
	
	TRY
	{
		csExpanded = csDirectory;
	}
	CATCH_ALL(e)
	{
		csExpanded.Empty();
	}
	END_CATCH_ALL
	
	if (ExpandDirectory(csExpanded, FALSE))
	{
#ifdef _WIN32
		DWORD dwSectorsPerCluster;
		DWORD dwBytesPerSector;
		DWORD dwFreeClusters;
		DWORD dwTotalClusters;

		if (GetDiskFreeSpace(csExpanded,
									&dwSectorsPerCluster,
									&dwBytesPerSector,
									&dwFreeClusters,
									&dwTotalClusters))
		{
			dwResult = dwSectorsPerCluster * dwBytesPerSector;
		}

#else
		if ((csExpanded.GetLength() >= 2) && (csExpanded[1] == ':'))
		{
			dwResult = GetClusterSize(csExpanded[0]);
		}
			
		else if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
		{
			/*
			// We need to connect, find a drive to use.
			*/
				
			for (char cDrive = 0; cDrive < 26; cDrive++)
			{
				if (GetTypeOfDrive(cDrive) == 0)
				{
					break;
				}
			}
				
			if (cDrive < 26)
			{
				/*
				// Try to make the connection.
				*/
					
				char szDestinationPath[] = "@:";
				szDestinationPath[0] = cDrive+'A';
					
				UINT uResult;
					
				if ((uResult = ::WNetAddConnection((LPSTR)((LPCSTR)csDirectory), "", szDestinationPath)) == WN_SUCCESS)
				{
					/*
					// Get the disk space.
					*/
						
					dwResult = GetClusterSize(szDestinationPath[0]);

					/*
					// Done with the connection.
					*/
						
					::WNetCancelConnection(szDestinationPath, TRUE);
				}
			}
		}
#endif
	}
	
	return dwResult;
}

DWORD Util::GetClusterSize(char cDriveLetter)
{
	DWORD dwClusterSize = (DWORD)(-1L);

#ifdef _WIN32
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwFreeClusters;
	DWORD dwTotalClusters;
	char cbName[] = "X:\\";

	cbName[0] = cDriveLetter;

	if (GetDiskFreeSpace(cbName,
								&dwSectorsPerCluster,
								&dwBytesPerSector,
								&dwFreeClusters,
								&dwTotalClusters))
	{
		dwClusterSize = dwSectorsPerCluster * dwBytesPerSector;
	}
#else

	/*
	// Make sure there is enough disk space available on the
	// chosen drive.
	*/
	
	_asm
	{
		mov	dl,cDriveLetter
		cmp	dl,'a'
		jb		L0
		cmp	dl,'z'
		ja		L0
		add	dl,'A'-'a'
	L0:
		cmp	dl,'A'
		jb		L1
		cmp	dl,'Z'
		ja		L1
		sub	dl,'A'-1
		mov	ah,036h
		call	DOS3Call
		cmp	ax,-1
		je		L1
		mul	cx
		jmp	L2
	L1:
		mov	ax,-1
		mov	dx,-1
	L2:
		mov	word ptr dwClusterSize+0,ax
		mov	word ptr dwClusterSize+2,dx
	}
#endif
	
	return dwClusterSize;
}

BOOL Util::MakeDirectory(const CString &csDirectory)
{
#ifdef _WIN32
	AfxMessageBox("MakeDirectory is broken");
	return FALSE;
#else
	BOOL fResult = FALSE;
	
	CString csExpanded;
	CString csComponent;
	
	TRY
	{
		// expand the directory name
		
		csExpanded = csDirectory;
		
		if (ExpandDirectory(csExpanded))
		{
			csExpanded.AnsiToOem();
			
			BOOL fRootDirectory = FALSE;
			
			// move past drive: or \\server\share
			
			int nIndex = 0;
			
			/*
			// Collect the drive: or \\server\share portion of the path.
			*/
			
			if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
			{
				nIndex = 2;
				
				// skip past server name
				
				while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
				{
					nIndex++;
				}
				
				nIndex++;
				       
				// skip past share name
				
				while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
				{
					nIndex++;
				}
			}
			
			else if ((csExpanded.GetLength() >= 3) && (csExpanded[1] == ':') && (csExpanded[2] == '\\'))
			{
//				nIndex = 2;
//				fRootDirectory = TRUE;
				nIndex = 3;
			}
				
			else
			{
				nIndex = csExpanded.GetLength();
			}
			
			/*
			// Attempt to create each component in the path name.
			*/
			
			if (nIndex < csExpanded.GetLength())
			{
				while (nIndex < csExpanded.GetLength())
				{
					// skip past the component name
					
					while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
					{
						nIndex++;
					}
					
					if (fRootDirectory)
					{
						nIndex++;
						fRootDirectory = FALSE;
					}
					
					csComponent = csExpanded.Left(nIndex);
				
					LPCSTR lpszComponent = csComponent;
					
					_asm
					{
						/*
						// Check if the directory already exists using the DOS Get/Set
						// file attributes escape.
						*/
						         
				      push	ds
						         
						lds	dx,dword ptr lpszComponent	// DS:DX <-- pointer to directory name
									
						xor	al,al								// get attributes
						
						mov	ah,043h
						call	DOS3Call
									
						pop	ds
						
						jc		l0
						
						/*
						// The directory name was found, make sure it is a directory.
						*/
						
						test	cx,00010h
						jnz	l2
						
						/*
						// The directory name exists, but it is not a directory, return
						// an error.
						*/
						
						jmp	l3
						
						/*
						// There was an error. Check if the directory did not exist.
						*/
						
					l0:
						
						cmp	ax,2								// file not found
						je		l1
						cmp	ax,3								// path not found or file doesn't exist
						jne	l3
						
						/*
						// The directory does not exist, attempt to create it.
						*/
						
					l1:
						
						push	ds
						
						lds	dx,dword ptr lpszComponent	// DS:DX <-- directory name
						
						mov	ah,039h
						call	DOS3Call
						
						pop	ds
						
						jc		l3
					}
					
				l2:
				   
				   nIndex++;
				}
					
				fResult = TRUE;
					
			l3: ;
			
			}
		}
	}
	CATCH_ALL(e)
	{
		csExpanded.Empty();
		csComponent.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
#endif
}

BOOL Util::DirectoryExists(const CString& csDirectory)
{
#ifdef _WIN32
	AfxMessageBox("DirectoryExists is broken");
	return FALSE;
#else

	BOOL fResult = FALSE;
	
	CString csExpanded;
	
	TRY
	{
		csExpanded = csDirectory;
		
		if (ExpandDirectory(csExpanded, FALSE))
		{
			csExpanded.AnsiToOem();
			
			LPCSTR lpszDirectory = csExpanded;
		
			_asm
			{
				/*
				// Check if the directory already exists using the DOS Get/Set
				// file attributes escape.
				*/
				         
		      push	ds
				         
				lds	dx,dword ptr lpszDirectory	// DS:DX <-- pointer to directory name
							
				xor	al,al								// get attributes
				
				mov	ah,043h
				call	DOS3Call
							
				pop	ds
				
				jc		l1
				
				/*
				// The directory name was found, make sure it is a directory.
				*/
				
				test	cx,00010h
				jnz	l0
				
				/*
				// The directory name exists, but it is not a directory, return
				// an error.
				*/
				
				jmp	l1
			}
			
		l0:
			
			fResult = TRUE;
			
		l1: ;
		
		}
	}
	CATCH_ALL(e)
	{
		csExpanded.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
#endif
}

BOOL Util::DirectoryIsWritable(const CString& csDirectory)
{
#ifdef _WIN32
	AfxMessageBox("DirectoryIsWritable is broken");
	return FALSE;
#else
	BOOL fResult = FALSE;
	
	CString csTestFile;
	CString csExpanded;
	
	TRY
	{
		csExpanded = csDirectory;
		
		if (ExpandDirectory(csExpanded))
		{
			csTestFile = "$$$$$$$$.$$$";	// hope this doesn't already exist, should ensure unique name
			csExpanded += csTestFile;
			csExpanded.AnsiToOem();

			LPCSTR lpszFileName = csExpanded;
								
			_asm
			{
				/*
				// Attempt to create the file.
				*/
						         
		      push	ds
						         
				lds	dx,dword ptr lpszFileName	// DS:DX <-- pointer to file name
									
				xor	cx,cx								// normal file
						
				mov	ah,03ch
				call	DOS3Call
									
				pop	ds
						
				jc		l0									// error, directory not writable
						
				/*
				// The file was created, close it and erase it.
				*/
				
				mov	bx,ax
				
				mov	ah,03eh
				call	DOS3Call
				
				push	ds
				
				lds	dx,dword ptr lpszFileName	// DS:DX <-- pointer to file name
									
				mov	ah,041h
				call	DOS3Call
									
				pop	ds
			}
			
			fResult = TRUE;
			
		l0: ;
		
		}
	}
	CATCH_ALL(e)
	{
		csTestFile.Empty();
		csExpanded.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
#endif
}

BOOL Util::ExpandDirectory(CString &csDirectory, BOOL fAppendBackslash /* = TRUE */)
{
	BOOL fResult = FALSE;

	CString csWork;
	CString csExpandedDirectory;
	
	TRY
	{
		csWork = csDirectory;
		csWork.AnsiToOem();
		csExpandedDirectory.Empty();

		int nIndex = 0;
		int nEndOfDriveIndex = 0;
		
		/*
		// Collect the drive: or \\server\share portion of the path.
		*/
		
		if ((csWork.GetLength() >= 2) && (csWork[0] == '\\') && (csWork[1] == '\\'))
		{
			nIndex = 2;
			
			// skip past server name
			
			while ((nIndex < csWork.GetLength()) && (csWork[nIndex] != '\\'))
			{
				nIndex++;
			}
			
			nIndex++;
			       
			// skip past share name
			
			while ((nIndex < csWork.GetLength()) && (csWork[nIndex] != '\\'))
			{
				nIndex++;
			}
			
			if (nIndex >= csWork.GetLength())
			{
				nIndex = csWork.GetLength();
			}

			csExpandedDirectory = csWork.Left(nIndex);
			nEndOfDriveIndex = csExpandedDirectory.GetLength();
		}
		
		else
		{
			WORD wDrive;
			
			// build the drive:
			
			if ((csWork.GetLength() >= 2) && (csWork[1] == ':'))
			{
				nIndex = 2;
				csExpandedDirectory = csWork.Left(2);
				
				wDrive = (WORD)csWork[0];
				if ((wDrive >= 'a') && (wDrive <= 'z'))
				{
					wDrive += (WORD)('A'-'a');
				}
				wDrive -= '@';
			}
			
			else
			{
#ifdef NOT_PORTABLE
				_asm
				{
					mov	ah,019h
					call	DOS3Call
					xor	ah,ah
					inc	ax
		
					mov	wDrive,ax
				}
#else
			/* Portable! (JN) */
				wDrive = _getdrive();
#endif
				
				csExpandedDirectory += (char)(wDrive+'@');
				csExpandedDirectory += ':';
			}
			
			nEndOfDriveIndex = csExpandedDirectory.GetLength();
		
			// if the path is relative, append the current path
			
			if ((nIndex >= csWork.GetLength()) || (csWork[nIndex] != '\\'))
			{
				char szCurrentDirectory[256];
				
				*szCurrentDirectory = '\0';
				
#ifdef NOT_PORTABLE
				_asm
				{
					mov	dx,wDrive
					lea	si,word ptr szCurrentDirectory
		
					mov	ah,047h
					call	DOS3Call
				}
#else
			/* Portable! (JN) */
				_getdcwd(wDrive, szCurrentDirectory, sizeof(szCurrentDirectory));

				if (szCurrentDirectory[0] != '\0'
						&& szCurrentDirectory[1] == ':'
						&& szCurrentDirectory[2] == '\\')
				{
				/* We have some kind of drive letter thingamabob on the front. Strip it. */
					strcpy(szCurrentDirectory, szCurrentDirectory+3);
				}
#endif
				
				if (*szCurrentDirectory != '\0')
				{
					csExpandedDirectory += '\\';
					csExpandedDirectory += szCurrentDirectory;
					
					// Set the current directory to the same case as the drive letter.
					
					if ((csExpandedDirectory[0] >= 'a') && (csExpandedDirectory[0] <= 'z'))
					{
						csExpandedDirectory.MakeLower();
					}
					
					else
					{
						csExpandedDirectory.MakeUpper();
					}
				}
			}
		}
		
		/*
		// Copy each directory element.
		*/
		
		while (nIndex < csWork.GetLength())
		{
			// find the start of the path element
			
			while ((nIndex < csWork.GetLength()) && (csWork[nIndex] == '\\'))
			{
				nIndex++;
			}
			
			int nStart = nIndex;
			
			// find the end of the path element
			
			while ((nIndex < csWork.GetLength()) && (csWork[nIndex] != '\\'))
			{
				nIndex++;
			}
			
			// process the component (".", "..", or other)
			
			if (nStart != nIndex)
			{
				if (((nIndex-nStart) > 1) || (csWork[nStart] != '.'))
				{
					if (((nIndex-nStart) == 2) && (csWork[nStart] == '.') && (csWork[nStart+1] == '.'))
					{
						// move back to start of last component
						
						int nBackupIndex = csExpandedDirectory.GetLength()-1;
						
						while ((nBackupIndex >= nEndOfDriveIndex) && (csExpandedDirectory[nBackupIndex] != '\\'))
						{
							nBackupIndex--;
						}
						
						if (nBackupIndex >= nEndOfDriveIndex)
						{
							csExpandedDirectory = csExpandedDirectory.Left(nBackupIndex);
						}
					}
					
					else
					{
						// copy the component
						
						csExpandedDirectory += '\\';
						csExpandedDirectory += csWork.Mid(nStart, nIndex-nStart);
					}
				}
			}
		}
		
		/*
		// Trailing backslash processing:
		//
		//		At this point, the current expanded path should not have a backslash
		//		on the end.
		//
		//		If the entire expanded path is drive:, then always append a
		//		backslash (even if fAppendBackslash is FALSE) to indicate the
		//		root directory.
		//
		//		Otherwise, append a backslash onto the end if the user has requested
		//		(fAppendBackslash is TRUE).
		*/
		
		if ((csExpandedDirectory.GetLength() == 2) && (csExpandedDirectory[1] == ':'))
		{
			csExpandedDirectory += '\\';
		}
		
		else if (fAppendBackslash)
		{
			// make sure there is no backslash, just in case -- we don't want a double one.
			
			if ((csExpandedDirectory.GetLength() < 1) || (csExpandedDirectory[csExpandedDirectory.GetLength()-1] != '\\'))
			{
				csExpandedDirectory += '\\';
			}
		}
		
		csDirectory = csExpandedDirectory;
		
		fResult = TRUE;
	}
	CATCH_ALL(e)
	{
		csWork.Empty();
		csExpandedDirectory.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL

	return fResult;
}

BOOL Util::DirectoryDrive(const CString csDirectory, CString &csDrive)
{
	BOOL fResult = FALSE;
	
	/*
	// Return the drive portion of a directory path. This will
	// have the form drive: for a normal connected path and
	// \\server\share for an unconnected path.
	*/
	
	CString csExpanded;

	TRY
	{
		csDrive.Empty();
		csExpanded = csDirectory;

		if (ExpandDirectory(csExpanded, TRUE))
		{
			if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
			{
				BOOL fSeenServer = FALSE;
				BOOL fSeenShare = FALSE;
				
				int nIndex = 2;
				
				// skip past server name
					
				while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
				{
					fSeenServer = TRUE;
					nIndex++;
				}
				
				nIndex++;
					       
				// skip past share name
					
				while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
				{
					fSeenShare = TRUE;
					nIndex++;
				}
				
				// save name

				if (fSeenServer && fSeenShare)
				{
					csDrive = csExpanded.Left(nIndex);
					fResult = TRUE;
				}
			}
			
			else if ((csExpanded.GetLength() >= 2) && (csExpanded[1] == ':'))
			{
				csDrive = csExpanded.Left(2);
				fResult = TRUE;
			}
		}
		
		if (!fResult)
		{
			if (!csDirectory.IsEmpty())
			{
				csDrive = csDirectory;
				fResult = TRUE;
			}
		}
	}
	CATCH_ALL(e)
	{
		csDrive.Empty();
		csExpanded .Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
}

BOOL Util::FileExists(const CString& csFile)
{
	BOOL fResult = FALSE;
	
	CString csExpanded;
	
	TRY
	{
		csExpanded = csFile;
		
		if (ExpandDirectory(csExpanded, FALSE))
		{
			csExpanded.AnsiToOem();
		
			LPCSTR lpszDirectory = csExpanded;
		
#ifdef _WIN32
			WIN32_FIND_DATA ffd;
			HANDLE hSearch;

			if ((hSearch = FindFirstFile(csExpanded, &ffd)) != INVALID_HANDLE_VALUE)
			{
				if (!(ffd.dwFileAttributes &
								FILE_ATTRIBUTE_SYSTEM
								| FILE_ATTRIBUTE_HIDDEN
								| FILE_ATTRIBUTE_DIRECTORY))
				{
					fResult = TRUE;
				}
				FindClose(hSearch);
			}
		#else
			_asm
			{
				/*
				// Check if the file exists using the DOS Get/Set
				// file attributes escape.
				*/
				         
		      push	ds
				         
				lds	dx,dword ptr lpszDirectory	// DS:DX <-- pointer to directory name
							
				xor	al,al								// get attributes
				
				mov	ah,043h
				call	DOS3Call
							
				pop	ds
				
				jc		l1
				
				/*
				// The file name was found, make sure it is a file.
				*/
				
				test	cx,00018h
				jz	l0
				
				/*
				// The file name exists, but it is not a file, return
				// an error.
				*/
				
				jmp	l1
			}
			
		l0:
			
			fResult = TRUE;
			
		l1: ;
#endif
		
		}
	}
	CATCH_ALL(e)
	{
		csExpanded.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
}

/*
// LoadString()
//  throw( CMemoryException, CResourceException);
//
// Loads the string resource identified by 'nID' into CString 'csString'.
// Returns 'csString'.
*/

CString& Util::LoadString(UINT nID, CString& csString)
{
/* Try to load the string using the standard method. */

	if (!csString.LoadString(nID))
	{
	/* Resource not found. */
		AfxThrowResourceException();
	}

/* We got it! Return the result. */

	return csString;
}

/*
// SafeLoadString()
//  does not throw exceptions, an empty string is returned on error.
//
// Loads the string resource identified by 'nID' into CString 'csString'.
*/
BOOL Util::SafeLoadString(UINT nID, CString& csString)
{
	BOOL fResult = FALSE;

	TRY
	{
		csString.Empty();
		csString.LoadString(nID);
		fResult = TRUE;
	}
	END_TRY

	return fResult;
}

/*
// SplitPath()
//  throw( CMemoryException )
//
// Split a full file name into its two components (path and file name).
*/

void Util::SplitPath(const CString &csFullPath, CString *pcsPath, CString *pcsFilename)
{
	int nIndex;

	if ((nIndex = csFullPath.ReverseFind('\\')) == -1)
	{
		nIndex = csFullPath.ReverseFind(':');
	}
	
	nIndex++;
	
	if (pcsPath != NULL)
	{
		*pcsPath = csFullPath.Left(nIndex);
	}
	
	if (pcsFilename != NULL)
	{
		*pcsFilename = csFullPath.Mid(nIndex);
	}
}

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

void Util::SplitPath(const CString &csPath, CString *pcsDrive, CString *pcsDirectory, CString *pcsFile, CString *pcsExtension)
{
TRACE("SplitPath(%Fs) -> ", (LPCSTR)csPath);
	
	TRY
	{
		// clear out the result strings
		if (pcsDrive != NULL)		pcsDrive->Empty();
		if (pcsDirectory != NULL)	pcsDirectory->Empty();
		if (pcsFile != NULL)			pcsFile->Empty();
		if (pcsExtension != NULL)	pcsExtension->Empty();

		int nDirectoryIndex = 0;
			
		// collect the drive
			
		// check for connected syntax: \\server\share\ 
		if ((csPath.GetLength() >= 2) && (csPath[0] == '\\') && (csPath[1] == '\\'))
		{
			BOOL fSeenServer = FALSE;
			BOOL fSeenShare = FALSE;
					
			nDirectoryIndex = 2;
					
			// skip past server name
						
			while ((nDirectoryIndex < csPath.GetLength()) && (csPath[nDirectoryIndex] != '\\'))
			{
				fSeenServer = TRUE;
				nDirectoryIndex++;
			}
					
			nDirectoryIndex++;
						       
			// skip past share name
						
			while ((nDirectoryIndex < csPath.GetLength()) && (csPath[nDirectoryIndex] != '\\'))
			{
				fSeenShare = TRUE;
				nDirectoryIndex++;
			}
					
			// save drive name
	
			if (!fSeenServer || !fSeenShare)
			{
				nDirectoryIndex = 0;
			}
		}
					
		else if ((csPath.GetLength() >= 2) && (csPath[1] == ':'))
		{
			nDirectoryIndex = 2;
		}
				
		if (pcsDrive != NULL)
		{
			*pcsDrive = csPath.Left(nDirectoryIndex);
		}
			
		// collect the directory
		
		int nFileIndex = csPath.ReverseFind('\\')+1;
		
		if (nFileIndex < nDirectoryIndex)
		{
			nFileIndex = nDirectoryIndex;
		}
		
		if (pcsDirectory != NULL)
		{
			*pcsDirectory = csPath.Mid(nDirectoryIndex, nFileIndex-nDirectoryIndex);
		}
		
		// collect the file name
		
		int nExtensionIndex = csPath.ReverseFind('.');
		
		if (nExtensionIndex == -1)
		{
			nExtensionIndex = csPath.GetLength();
		}
		
		if (nExtensionIndex < nFileIndex)
		{
			nExtensionIndex = csPath.GetLength();
		}
		
		if (pcsFile != NULL)
		{
			*pcsFile = csPath.Mid(nFileIndex, nExtensionIndex-nFileIndex);
		}
		
		// collect the extension
		
		if (pcsExtension != NULL)
		{
			*pcsExtension = csPath.Mid(nExtensionIndex);
		}
	}
	CATCH_ALL(e)
	{
		if (pcsDrive != NULL)		pcsDrive->Empty();
		if (pcsDirectory != NULL)	pcsDirectory->Empty();
		if (pcsFile != NULL)			pcsFile->Empty();
		if (pcsExtension != NULL)	pcsExtension->Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
if (pcsDrive != NULL) TRACE("Drive=|%Fs| ", (LPCSTR)(*pcsDrive));
if (pcsDirectory != NULL) TRACE("Directory=|%Fs| ", (LPCSTR)(*pcsDirectory));
if (pcsFile != NULL) TRACE("File=|%Fs| ", (LPCSTR)(*pcsFile));
if (pcsExtension != NULL) TRACE("Extension=|%Fs| ", (LPCSTR)(*pcsExtension));
TRACE("\n");
}

/*
// AppendBackslashToPath()
//  throw( CMemoryException )
//
// Make sure a path ends with a backslash.
*/

CString& Util::AppendBackslashToPath(CString &csPath)
{
	if ((!csPath.IsEmpty()) && (csPath[csPath.GetLength()-1] != '\\'))
	{
		csPath += '\\';
	}
	
	return csPath;
}

/*
// RemoveBackslashFromPath()
//  throw( CMemoryException )
//
// Make sure a path ends with a backslash.
*/

CString& Util::RemoveBackslashFromPath(CString &csPath, BOOL fKeepRootDirectoryBackslash /* = TRUE */)
{
	if ((!csPath.IsEmpty()) && (csPath[csPath.GetLength()-1] == '\\'))
	{
		csPath = csPath.Left(csPath.GetLength()-1);
	}
	
	if (fKeepRootDirectoryBackslash)
	{
		if ((csPath.GetLength() == 2) && (csPath[1] == ':'))
		{
			csPath += '\\';
		}
	}
	
	return csPath;
}

/*
// ConstructPath()
//  throw( CMemoryException )
//
// Construct a full file name from its two components (path and file name).
*/

CString& Util::ConstructPath(CString &csFullPath, const CString &csPath, const CString &csFileName)
{
	csFullPath = csPath;
	AppendBackslashToPath(csFullPath);
	csFullPath += csFileName;
	
	return csFullPath;
}


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

CString& Util::ConstructPath(CString &csPath, const CString &csDrive, const CString &csDirectory, const CString &csFile, const CString &csExtension)
{
	// start with drive
	csPath = csDrive;
	
	// append directory
	csPath += csDirectory;
	
	// append file
	AppendBackslashToPath(csPath);
	csPath += csFile;
	
	// append extension
	if (!csExtension.IsEmpty())
	{
		if (csExtension[0] != '.')
		{
			csPath += '.';
		}
		
		csPath += csExtension;
	}
	
	return csPath;
}


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

const CString& Util::FormatString(CString &csOutput, LPCSTR lpszFormat, ...)
{
	return FormatStringV(csOutput, lpszFormat, (LPCSTR)(&lpszFormat)+sizeof(lpszFormat));
}

const CString& Util::FormatStringV(CString &csOutput, LPCSTR lpszFormat, LPCSTR lpArgs)
{
	#define	IS_RESOURCE_ID(s)			(HIWORD((LPVOID)(s))==0)
	#define	EXTRACT_RESOURCE_ID(s)	(LOWORD((LPVOID)(s)))
	
	CString csResource;
	CString csFormat;
	
	TRY
	{
		/*
		// Get the format string into a CString.
		*/
		
		if (IS_RESOURCE_ID(lpszFormat))
		{
			LoadString(EXTRACT_RESOURCE_ID(lpszFormat), csFormat);
		}
		
		else
		{
			// convert LPCSTR into CString
			csFormat = lpszFormat;
		}
		
		/*
		// Now, march through the format string and do the "printf" operation.
		*/
		
		int nIndex = 0;
		
		while (nIndex < csFormat.GetLength())
		{
			if (csFormat[nIndex] == '%')
			{
				// format specifier
				
				nIndex++;
				
				if (nIndex < csFormat.GetLength())
				{
					switch (csFormat[nIndex])
					{
						case 'd':
						{
							// integer
							int nArg = *((LPINT)lpArgs);
							lpArgs += sizeof(int);
							char szBuffer[20];
							itoa(nArg, szBuffer, 10);
							csOutput += szBuffer;
							break;
						}
						
						case 's':
						{
							// string
							LPCSTR lpszArg = *((LPCSTR FAR *)lpArgs);
							lpArgs += sizeof(LPCSTR);
							
							if (IS_RESOURCE_ID(lpszArg))
							{
								LoadString(EXTRACT_RESOURCE_ID(lpszArg), csResource);
								csOutput += csResource;
							}
							
							else
							{
								csOutput += lpszArg;
							}
							
							break;	
						}
						
						default:
						{
							csOutput += csFormat[nIndex];
							break;
						}
					}
				}
			}
			
			else
			{
				// normal character
				csOutput += csFormat[nIndex];
			}
			
			nIndex++;
		}
	}
	CATCH_ALL(e)
	{
		csResource.Empty();
		csFormat.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return csOutput;
}

/*
// FormatWindowText()
//		throw( CMemoryException, CResourceException )
//
// Retrieves the text of a window and uses this text as the format string
// for a call to FormatString() with the rest of the arguments.
*/

const void Util::FormatWindowText(CWnd* pWnd, ...)
{
	if (pWnd != NULL)
	{
		// Get the format string.
		
		CString csFormat;
		
		pWnd->GetWindowText(csFormat);
		
		if (!csFormat.IsEmpty())
		{
			// Format the new text.
			
			CString csOutput;
			
			FormatStringV(csOutput, csFormat, (LPCSTR)(&pWnd)+sizeof(pWnd));
			
			// Set the next text.
			
			pWnd->SetWindowText(csOutput);
		}
	}
}

/*
// MessageBox()
//		throw( CMemoryException, CResourceException )
//
// Message box function that takes printf() style arguments.
*/

int Util::MessageBox(UINT uType, UINT uHelpID, LPCSTR lpszFormat, ...)
{
	CString csMessage;
	int nResult;
	
	TRY
	{
		FormatStringV(csMessage, lpszFormat, (LPCSTR)(&lpszFormat)+sizeof(lpszFormat));
		nResult = AfxMessageBox(csMessage, uType, uHelpID);
	}
	CATCH_ALL(e)
	{
		csMessage.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return nResult;
}

/*
// GetTypeOfDrive()
//
// Get the type for a drive.
*/

UINT Util::GetTypeOfDrive(char cDrive)
{
#ifdef _WIN32
	char cbRoot[] = "X:\\";
	cbRoot[0] = cDrive;
	return ::GetDriveType(cbRoot);
#else
	return ::GetDriveType(cDrive - 'A');
#endif
}

BOOL Util::Draw3dButton(CDC* pDC, const CRect& crButton, COLORREF clBorder, COLORREF clHighlight, COLORREF clShadow, COLORREF clFace, int nDepth /*=2*/, BOOL fMask /*=FALSE*/)
{
	BOOL fResult = FALSE;
	int nContext = 0;
	CPen cpBorder;
	CPen cpHighlight;
	CPen cpShadow;
	CBrush cbFace;
	
	TRY
	{
		if (pDC != NULL)
		{
			nContext = pDC->SaveDC();
			
			if (nContext != 0)
			{
				int i;
				
				/* draw outside border */
				
				cpBorder.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorder);
				pDC->SelectObject(&cpBorder);
				
				pDC->MoveTo(crButton.left+1, crButton.top);
				pDC->LineTo(crButton.right-1, crButton.top);
				
				pDC->MoveTo(crButton.left+1, crButton.bottom-1);
				pDC->LineTo(crButton.right-1, crButton.bottom-1);
				
				pDC->MoveTo(crButton.left, crButton.top+1);
				pDC->LineTo(crButton.left, crButton.bottom-1);
				
				pDC->MoveTo(crButton.right-1, crButton.top+1);
				pDC->LineTo(crButton.right-1, crButton.bottom-1);
				
				if (nDepth > 0)
				{
					/* draw highlight */
								
					cpHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clHighlight);
					pDC->SelectObject(&cpHighlight);
					
					for (i = 0; i < nDepth; i++)
					{
						pDC->MoveTo(crButton.left+1, crButton.top+1+i);
						pDC->LineTo(crButton.right-2-i, crButton.top+1+i);
					
						pDC->MoveTo(crButton.left+1+i, crButton.top+2);
						pDC->LineTo(crButton.left+1+i, crButton.bottom-2-i);
					}
					
					/* draw shadow */
					
					cpShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clShadow);
					pDC->SelectObject(&cpShadow);
					
					for (i = 0; i < nDepth; i++)
					{
						pDC->MoveTo(crButton.left+1+i, crButton.bottom-2-i);
						pDC->LineTo(crButton.right-1, crButton.bottom-2-i);
					
						pDC->MoveTo(crButton.right-2-i, crButton.top+1+i);
						pDC->LineTo(crButton.right-2-i, crButton.bottom-2);
					}
				}
				
				/* draw face */
				
				cbFace.CreateSolidBrush(fMask ? RGB(0,0,0) : clFace);
				CRect crFace(crButton.left+nDepth+1, crButton.top+nDepth+1, crButton.right-nDepth-1, crButton.bottom-nDepth-1);
				
				pDC->FillRect(crFace, &cbFace);
				
				pDC->RestoreDC(nContext);
				nContext = 0;
				
				fResult = TRUE;
			}
		}
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
		
		if (nContext != 0)
		{
			pDC->RestoreDC(nContext);
			nContext = 0;
		}
	}
	END_CATCH_ALL
	
	return fResult;
}

void Util::PriceString(CString& csPrice, long lPrice, BOOL fDollarSign /*=TRUE*/, LPCSTR pszBlankString /*=NULL*/)
{
	if ((lPrice == 0) && (pszBlankString != NULL))
	{
		csPrice = pszBlankString;
	}
		
	else
	{
		char szPrice[20];

		long lAbsPrice = (lPrice >= 0) ? lPrice : -lPrice;
		
		CString csFormat;
		SafeLoadString(fDollarSign ? IDS_PRICE_DOLLAR_FORMAT : IDS_PRICE_NO_DOLLAR_FORMAT, csFormat);
		sprintf(szPrice, csFormat, lPrice/100L, lAbsPrice % 100L);
		csPrice = szPrice;
	}
}

BOOL Util::Trim(CString& csString, BOOL fTrimFront /*=TRUE*/, BOOL fTrimBack /*=TRUE*/)
{
	int nLength = csString.GetLength();
	int nStart = 0;
	int nEnd = nLength-1;
	
	if (fTrimFront)
	{
		for (; nStart < nLength; nStart++)
		{
			if ((csString[nStart] != ' ') && (csString[nStart] != '\t'))
			{
				break;
			}
		}
	}
	
	if (fTrimBack)
	{
		for (; nEnd >= 0; nEnd--)
		{
			if ((csString[nEnd] != ' ') && (csString[nEnd] != '\t'))
			{
				break;
			}
		}
	}
	
	if (nStart > nEnd)
	{
		csString.Empty();
	}
	else
	{
		csString = csString.Mid(nStart, nEnd-nStart+1);
	}
	
	return !csString.IsEmpty();
}

void Util::FillRectangleWithColor(CDC& dc, const CRect& crRect, COLORREF clColor)
{
	COLORREF clOld = dc.SetBkColor(clColor);
	dc.ExtTextOut(0, 0, ETO_OPAQUE, crRect, NULL, 0, NULL);
	dc.SetBkColor(clOld);
}

void Util::DrawClientBorder(CDC& dc, CRect& crRect)
{
#ifdef WIN32
	//   D---------------W
	//   |B------------BL|
	//   ||             ||
	//   ||             ||
	//   |B             ||
	//   |L-------------L|
	//   W---------------W

	CRect crSide;

	// D horizontal
	crSide.SetRect(crRect.left, crRect.top, crRect.right-1, crRect.top+1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

	// D vertical
	crSide.SetRect(crRect.left, crRect.top+1, crRect.left+1, crRect.bottom-1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

	// B horizontal
	crSide.SetRect(crRect.left+1, crRect.top+1, crRect.right-2, crRect.top+2);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// B vertical
	crSide.SetRect(crRect.left+1, crRect.top+2, crRect.left+2, crRect.bottom-2);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// W horizontal
	crSide.SetRect(crRect.left, crRect.bottom-1, crRect.right, crRect.bottom);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

	// W vertical
	crSide.SetRect(crRect.right-1, crRect.top, crRect.right, crRect.bottom-1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

	// L horizontal
	crSide.SetRect(crRect.left+1, crRect.bottom-2, crRect.right-1, crRect.bottom-1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));

	// L vertical
	crSide.SetRect(crRect.right-2, crRect.top+1, crRect.right-1, crRect.bottom-2);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));

	crRect.InflateRect(-2, -2);
#else
	//   B---------------B
	//   |               |
	//   |               |
	//   |               |
	//   |               |
	//   |               |
	//   B---------------B

	// Draw each side.
	CRect crSide;

	// Top
	crSide.SetRect(crRect.left, crRect.top, crRect.right, crRect.top+1);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// Bottom
	crSide.OffsetRect(0, crRect.Height()-1);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// Left
	crSide.SetRect(crRect.left, crRect.top+1, crRect.left+1, crRect.bottom-1);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// Right
	crSide.OffsetRect(crRect.Width()-1, 0);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	crRect.InflateRect(-1, -1);
#endif
}

