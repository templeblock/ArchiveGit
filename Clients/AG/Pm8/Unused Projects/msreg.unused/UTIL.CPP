/*
// $Workfile: UTIL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/UTIL.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 08:52:02   FRED
// Initial revision.
// 
//    Rev 1.0   25 Oct 1995 17:47:06   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "util.h"

#include <math.h>
#include <direct.h>
#include <memory.h>
#include <dos.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef WIN32
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
#ifdef WIN32
		CString csDrive;

		TRY
		{
			DirectoryDrive(csExpanded, csDrive);
			AppendBackslashToPath(csDrive);
		}
		CATCH_ALL(e)
		{
			csDrive.Empty();
		}
		END_CATCH_ALL

		DWORD dwSectorsPerCluster;
		DWORD dwBytesPerSector;
		DWORD dwFreeClusters;
		DWORD dwTotalClusters;

		if (GetDiskFreeSpace(csDrive,
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

#ifdef WIN32
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
#ifdef WIN32
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

#ifdef WIN32
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
	BOOL fResult = FALSE;
	
	CString csExpanded;
	CString csComponent;
	
//	od("MakeDirectory(%s)\n\r", (LPCSTR)csDirectory);
	
	TRY
	{
		// Expand the directory name.
		
		csExpanded = csDirectory;
		
		if (ExpandDirectory(csExpanded))
		{
			csExpanded.AnsiToOem();
			
			// Skip the first component (the root directory); drive: or \\server\share
			
			int nIndex = 0;
			
			// Collect the drive: or \\server\share portion of the path.
			
			if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
			{
				nIndex = 2;
				
				// Skip past server name.
				
				while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
				{
					nIndex++;
				}
				
				nIndex++;
				       
				// Skip past share name.
				
				while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
				{
					nIndex++;
				}
				
				fResult = TRUE;
			}
			
			else if ((csExpanded.GetLength() >= 3) && (csExpanded[1] == ':') && (csExpanded[2] == '\\'))
			{
				nIndex = 2;
				fResult = TRUE;
			}
				
			// Attempt to create each component in the path name.
			
			if (fResult)
			{
				while (nIndex < csExpanded.GetLength())
				{
					// move to the next component
						
					nIndex++;
						
					while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
					{
						nIndex++;
					}
					
					if (nIndex < csExpanded.GetLength())
					{
						// Check if the current component exists. If not, create it.
							
						csComponent = csExpanded.Left(nIndex);
							
//						od("MakeDirectory - Component |%s|\n\r", (LPCSTR)csComponent);
							
						if (!DirectoryExists(csComponent))
						{
							if (_mkdir(csComponent) != 0)
							{
								fResult = FALSE;
								break;
							}
						}
					}
				}
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
}

BOOL Util::DirectoryExists(const CString& csDirectory)
{
//	od("DirectoryExists(%s) --> ", (LPCSTR)csDirectory);

	BOOL fResult = FALSE;
	
	CString csExpanded;
	
	TRY
	{
		csExpanded = csDirectory;
		
		if (ExpandDirectory(csExpanded))
		{
			csExpanded += "*.*";
			csExpanded.AnsiToOem();
			
#ifdef WIN32
			CFileIterator Iter;

			fResult = Iter.FindFirst(csExpanded, _A_NORMAL)
								|| GetLastError() == ERROR_NO_MORE_FILES;
#else
			struct _find_t FileInfo;
			int nResult = _dos_findfirst(csExpanded, _A_NORMAL, &FileInfo);
			fResult = ((nResult == 0) || (nResult == 0x12));	// 0x12 means no more files
#endif
			
		}
	}
	CATCH_ALL(e)
	{
		csExpanded.Empty();

		THROW_LAST();
	}
	END_CATCH_ALL

//	od("%d\n\r", fResult);

	return fResult;
}

BOOL Util::DirectoryIsWritable(const CString& csDirectory)
{
	BOOL fResult = FALSE;
	
	CString csTestFile;
	CString csExpanded;
	
	TRY
	{
		csExpanded = csDirectory;
		
		if (ExpandDirectory(csExpanded))
		{
			csTestFile = "$$$$$$$$.$$$";	/* hope this doesn't already exist, should ensure unique name */
			csExpanded += csTestFile;
			csExpanded.AnsiToOem();

			LPCSTR lpszFileName = csExpanded;

			HFILE hFile;
			if ((hFile = _lcreat(csExpanded, 0)) != NULL)
			{
				_lclose(hFile);
				remove(csExpanded);
				fResult = TRUE;
			}
#ifdef OLD
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
#endif
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
}

BOOL Util::ExpandDirectory(CString &csDirectory, BOOL fAppendBackslash /* = TRUE */)
{
//	od("ExpandDirectory(%s) --> ", (LPCSTR)csDirectory);
	
	BOOL fResult = FALSE;

	CString csWork;
	CString csExpandedDirectory;
	
	TRY
	{
		csWork = csDirectory;
		csWork.AnsiToOem();
		csExpandedDirectory.Empty;

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
	
//	od(fResult ? "|%s|\n\r" : "FALSE\n\r", (LPCSTR)csDirectory);

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
		
#ifdef WIN32
			WIN32_FIND_DATA ffd;
			HANDLE hSearch;

			if ((hSearch = FindFirstFile(csExpanded, &ffd)) != INVALID_HANDLE_VALUE)
			{
				if ((ffd.dwFileAttributes & (FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_DIRECTORY)) == 0)
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
#ifdef WIN32
	char cbRoot[] = "X:\\";
	cbRoot[0] = cDrive;
	return ::GetDriveType(cbRoot);
#else
	return ::GetDriveType(cDrive - 'A');
#endif
}

/*
// Huge memory move, supporting overlapping memory blocks.
// Thanks Dr. GUI!
*/

#ifndef WIN32
void Util::HMemMove(
   char __huge  *hpSource,        // Source buffer pointer
   char __huge  *hpDest,          // Destination buffer pointer
   unsigned long  lCount)         // Count of bytes to move
{

   UINT         ucount;           // Temporary short int
   BOOL         bReverse = FALSE; // Bottom up?

// If we are moving less than 64K, we can do it all in one call.
   if (HIWORD(lCount) == 0) {
      _fmemmove(hpDest, hpSource, LOWORD(lCount));
      return;
   }

// We are now dealing with >64K of data. If the destination 
// is lower in memory than the source, we can repetitively 
// copy 64K blocks from the top of each block. If the destination
// is higher in memory than the source, we must repetitively move 
// 64K blocks from the bottom of the source and destination to
// prevent overwriting the source. We start by adjusting the source
// and destination pointers to point at the last 64K chunk in the
// block and set a flag (bReverse). After each block move, we move
// the source and destination pointers up or down appropriately.

   if (hpDest > hpSource) {
      hpSource += (lCount - 65535);  
      hpDest += (lCount - 65535);
      bReverse = TRUE;
   }
   while (TRUE) {
      if (HIWORD(lCount))
         ucount = 65535;       // >64K left; set ucount to 64K
      else
         ucount = LOWORD(lCount);
      _fmemmove(hpDest, hpSource, ucount);
      lCount -= ucount;        // Reduce the amount left to move
      if (lCount == 0)
         return;               // All done; return
      if (bReverse) {          // Update source/destination pointers
         if (HIWORD(lCount)) {
            hpDest -= ucount;
            hpSource -= ucount;
         } else {
            hpDest -= LOWORD(lCount);
            hpSource -= LOWORD(lCount);
         }
      } else {
         hpDest += ucount;
         hpSource += ucount;
      }
   }
   return;
}
#endif

BOOL Util::Draw3dBorder(
		CDC* pDC,
		const CRect& crBorder,
		COLORREF clHighlight,
		COLORREF clShadow,
		COLORREF clBorderHighlight,
		COLORREF clBorderShadow,
		COLORREF clBackground,
		int nDepth /*=2*/,
		BOOL fDrawInsideBorder /*=FALSE*/,
		BOOL fDrawBackground /*=FALSE*/,
		BOOL fMask /*=FALSE*/)
{
	BOOL fResult = FALSE;
	int nContext = 0;
	CPen cpHighlight;
	CPen cpShadow;
	CPen cpBorderHighlight;
	CPen cpBorderShadow;
	CBrush cbBackground;
	
	TRY
	{
		if (pDC != NULL)
		{
			nContext = pDC->SaveDC();
			
			if (nContext != 0)
			{
				int i;
				
				// Draw 3D border.
				
				if (nDepth > 0)
				{
					// Draw shadow.
								
					cpShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clShadow);
					pDC->SelectObject(&cpShadow);
					
					for (i = 0; i < nDepth; i++)
					{
						pDC->MoveTo(crBorder.left, crBorder.top+i);
						pDC->LineTo(crBorder.right-1-i, crBorder.top+i);
					
						pDC->MoveTo(crBorder.left+i, crBorder.top+nDepth);
						pDC->LineTo(crBorder.left+i, crBorder.bottom-1-i);
					}
					
					// Draw highlight.
					
					cpHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clHighlight);
					pDC->SelectObject(&cpHighlight);
					
					for (i = 0; i < nDepth; i++)
					{
						pDC->MoveTo(crBorder.left+i, crBorder.bottom-1-i);
						pDC->LineTo(crBorder.right, crBorder.bottom-1-i);
					
						pDC->MoveTo(crBorder.right-1-i, crBorder.top+i);
						pDC->LineTo(crBorder.right-1-i, crBorder.bottom-nDepth);
					}
				}
				
				if (fDrawInsideBorder)
				{
					// Draw inside border.
				
					cpBorderShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorderShadow);
					pDC->SelectObject(&cpBorderShadow);
				
					pDC->MoveTo(crBorder.left+nDepth, crBorder.top+nDepth);
					pDC->LineTo(crBorder.right-1-nDepth, crBorder.top+nDepth);
					
					pDC->MoveTo(crBorder.left+nDepth, crBorder.top+nDepth+1);
					pDC->LineTo(crBorder.left+nDepth, crBorder.bottom-nDepth-1);
					
					cpBorderHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorderHighlight);
					pDC->SelectObject(&cpBorderHighlight);
					
					pDC->MoveTo(crBorder.left+nDepth, crBorder.bottom-nDepth-1);
					pDC->LineTo(crBorder.right-nDepth, crBorder.bottom-nDepth-1);
					
					pDC->MoveTo(crBorder.right-nDepth-1, crBorder.top+nDepth);
					pDC->LineTo(crBorder.right-nDepth-1, crBorder.bottom-nDepth-1);
				}
				
				if (fDrawBackground)
				{
					// Fill background.
					
					if (cbBackground.CreateSolidBrush(clBackground))
					{
						CRect crBackground(crBorder);
						int nOffset = nDepth+(fDrawInsideBorder ? 1 : 0);
						crBackground.InflateRect(-nOffset, -nOffset);
						pDC->FillRect(crBackground, &cbBackground);
					}
				}
				
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


void Util::FitAspect(int* pnAspectX, int* pnAspectY, int nTargetX, int nTargetY)
{
	if (((long)(((long)(*pnAspectX))*((long)nTargetY))) < ((long)(((long)(*pnAspectY))*((long)nTargetX))))
	{
		*pnAspectX = MulDiv(*pnAspectX, nTargetY, *pnAspectY);
		*pnAspectY = nTargetY;
	}
	else
	{
		*pnAspectY = MulDiv(*pnAspectY, nTargetX, *pnAspectX);
		*pnAspectX = nTargetX;
	}
}

BOOL Util::ValidSignedNumber(LPCSTR pString, long lMinimum /*=-0x7fffffff*/, long lMaximum /*=0x7fffffff*/)
{
	BOOL fNegative = FALSE;

	while ((*pString == ' ') || (*pString == '\t'))
	{
		pString++;
	}
	
	if (*pString == '-')
	{
		fNegative = TRUE;
		pString++;
	}
	
	if (*pString == '+')
	{
		pString++;
	}
	
	BOOL fSeenDigit = FALSE;
	unsigned long u = 0;
	
	while ((*pString >= '0') && (*pString <= '9'))
	{
		fSeenDigit = TRUE;
		
		u = (u*10)+(*pString-'0');
		
		if (u > 0x7fffffff)
		{
			return FALSE;
		}
		
		pString++;
	}
	
	if (!fSeenDigit)
	{
		return FALSE;
	}
	
	while ((*pString == ' ') || (*pString == '\t'))
	{
		pString++;
	}
	
	if (*pString != '\0')
	{
		return FALSE;
	}
	
	long c = (long)u;
	
	if (fNegative)
	{
		c = -c;
	}
	
	if ((c < lMinimum) || (c > lMaximum))
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL Util::ValidUnsignedNumber(LPCSTR pString, unsigned long lMinimum /*=0*/, unsigned long lMaximum /*=0xffffffff*/)
{
	while ((*pString == ' ') || (*pString == '\t'))
	{
		pString++;
	}
	
	BOOL fSeenDigit = FALSE;
	unsigned long c = 0;
	
	while ((*pString >= '0') && (*pString <= '9'))
	{
		fSeenDigit = TRUE;
		
		unsigned long n = (c*10)+(*pString-'0');
		
		if (((n-(*pString-'0'))/10) != c)
		{
			return FALSE;
		}
		
		c = n;
		pString++;
	}
	
	if (!fSeenDigit)
	{
		return FALSE;
	}
	
	while ((*pString == ' ') || (*pString == '\t'))
	{
		pString++;
	}
	
	if (*pString != '\0')
	{
		return FALSE;
	}
	
	if ((c < lMinimum) || (c > lMaximum))
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL Util::ValidDouble(LPCSTR pString, double dMinimum /*=-1e308*/, double dMaximum /*=1e308*/)
{
	double d;
	
	TRY
	{
		d = atof(pString);
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
	
	while ((*pString == ' ') || (*pString == '\t'))
	{
		pString++;
	}
	
	if (*pString == '-')
	{
		pString++;
	}
	
	if (*pString == '+')
	{
		pString++;
	}
	
	BOOL fSeenDigit = FALSE;
	
	while ((*pString >= '0') && (*pString <= '9'))
	{
		fSeenDigit = TRUE;
		pString++;
	}
	
	if (*pString == '.')
	{
		pString++;
		
		while ((*pString >= '0') && (*pString <= '9'))
		{
			fSeenDigit = TRUE;
			pString++;
		}
	}
	
	if (!fSeenDigit)
	{
		return FALSE;
	}
	
	if ((*pString == 'd')
	 || (*pString == 'D')
	 || (*pString == 'e')
	 || (*pString == 'E'))
	{
		pString++;
		
		if (*pString == '-')
		{
			pString++;
		}
		
		if (*pString == '+')
		{
			pString++;
		}
	
		while ((*pString >= '0') && (*pString <= '9'))
		{
			fSeenDigit = TRUE;
			pString++;
		}
	}
	
	while ((*pString == ' ') || (*pString == '\t'))
	{
		pString++;
	}
	
	if (*pString != '\0')
	{
		return FALSE;
	}
	
	if ((d < dMinimum) || (d > dMaximum))
	{
		return FALSE;
	}
	
	return TRUE;
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

/////////////////////////////////////////////////////////////////////////////
// CBoolSemaphore

CBoolSemaphore::CBoolSemaphore(BOOL &rBool) : m_rBool(rBool)
{
	m_fLocked = FALSE;
}

CBoolSemaphore::~CBoolSemaphore(void)
{
	Unlock();
}

BOOL CBoolSemaphore::Lock(void)
{
	if (m_rBool)
	{
	/* Somebody else has it. */
		return FALSE;
	}
	m_rBool = m_fLocked = TRUE;
	return TRUE;
}

void CBoolSemaphore::Unlock(void)
{
	if (m_fLocked)
	{
		m_rBool = m_fLocked = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFileIterator

CFileIterator::CFileIterator()
{
#ifdef WIN32
	m_hSearch = NULL;
#endif
}

CFileIterator::~CFileIterator()
{
	Close();
}

void CFileIterator::Close(void)
{
#ifdef WIN32
	if (m_hSearch != NULL)
	{
		FindClose(m_hSearch);
	}
#endif
}

BOOL CFileIterator::FindFirst(LPCSTR pName, DWORD dwDOSAttributes)
{
#ifdef WIN32
	if (m_hSearch != NULL)
	{
	/* m_hSearch is already not NULL! Can't begin search. */
		ASSERT(FALSE);
		return FALSE;
	}

	/*
	// Construct the attribute bits to search for.
	*/

	m_dwExclude = (FILE_ATTRIBUTE_DIRECTORY
								| FILE_ATTRIBUTE_SYSTEM
								| FILE_ATTRIBUTE_HIDDEN);

	if (dwDOSAttributes & _A_SUBDIR)
	{
		m_dwExclude &= ~FILE_ATTRIBUTE_DIRECTORY;
	}

	if (dwDOSAttributes & _A_HIDDEN)
	{
		m_dwExclude &= ~FILE_ATTRIBUTE_HIDDEN;
	}

	if (dwDOSAttributes & _A_SYSTEM)
	{
		m_dwExclude &= ~FILE_ATTRIBUTE_SYSTEM;
	}

	if (dwDOSAttributes & _A_RDONLY)
	{
		m_dwExclude &= ~FILE_ATTRIBUTE_READONLY;
	}

	if ((m_hSearch = FindFirstFile(pName, &m_ffd)) == INVALID_HANDLE_VALUE)
	{
		m_hSearch = NULL;
		return FALSE;
	}

	/*
	// If this file matches our attributes, then use it.
	*/

	if ((m_ffd.dwFileAttributes & m_dwExclude) == 0)
	{
		return TRUE;
	}

	/* The first file doesn't match; keep looking. */

	return FindNext();
#else
	return _dos_findfirst(pName, (WORD)dwDOSAttributes, &m_buffer) == 0;
#endif
}

BOOL CFileIterator::FindNext(void)
{
#ifdef WIN32
	if (m_hSearch == NULL)
	{
	/* m_hSearch is NULL. What are we doing here? */
		ASSERT(FALSE);
		return FALSE;
	}

/* Look for the next file that matches our attributes. */

	while (FindNextFile(m_hSearch, &m_ffd))
	{
		if ((m_ffd.dwFileAttributes & m_dwExclude) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
#else
	return _dos_findnext(&m_buffer) == 0;
#endif
}

LPCSTR CFileIterator::Name(void)
{
#ifdef WIN32
	return m_ffd.cFileName;
#else
	return m_buffer.name;
#endif
}

DWORD CFileIterator::Attributes(void)
{
#ifdef WIN32
	DWORD dwAttributes = _A_NORMAL;
	if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
	{
		dwAttributes |= _A_RDONLY;
	}
	if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		dwAttributes |= _A_SUBDIR;
	}
	if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
	{
		dwAttributes |= _A_HIDDEN;
	}
	if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
	{
		dwAttributes |= _A_SYSTEM;
	}
	if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
	{
		dwAttributes |= _A_ARCH;
	}
	return dwAttributes;
#else
	return (DWORD)(unsigned char)m_buffer.attrib;
#endif
}
