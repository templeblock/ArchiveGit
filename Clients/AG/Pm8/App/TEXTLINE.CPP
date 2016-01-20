/*
// $Workfile: TEXTLINE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TEXTLINE.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.1   13 Mar 1997 13:17:14   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.0   14 Mar 1996 13:35:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:16   FRED
// Initial revision.
// 
//    Rev 1.0   15 Nov 1995 14:41:14   FRED
// Initial revision.
*/

#include "stdafx.h"
#include <io.h>

#include "util.h"
#include "textline.h"

////////////////////////////////////////////////////////////////
// CTextLineFile

CTextLineFile::CTextLineFile()
{
}

CTextLineFile::~CTextLineFile()
{
	TRY
	{
		Done(FALSE, NULL);
	}
	END_TRY
}

BOOL CTextLineFile::Name(LPCSTR pszFileName)
{
	BOOL fResult = FALSE;
	
	TRY
	{
		CloseFiles();	// does not throw an exception
		m_csRead = pszFileName;
		fResult = TRUE;
	}
	END_TRY
	
	ASSERT(!m_csRead.IsEmpty());
	
	return fResult;
}
	
BOOL CTextLineFile::ReadLine(CString& csLine)
{
	#define	READ_MAX		(1024)	// total buffer size including NULL
	
	BOOL fResult = FALSE;
	
	TRY
	{
		// Set up for reading.
		StartReading();
		ASSERT(m_cfRead.m_hFile != CFile::hFileNull);
	
		// Read the line.
		fResult = (m_cfRead.ReadString(csLine.GetBuffer(READ_MAX-1), READ_MAX) != NULL);
		csLine.ReleaseBuffer();
		
		if (fResult)
		{
			// Make sure we did not overflow.	
			int nLength = csLine.GetLength();
			if (nLength >= READ_MAX-1)
			{
				AfxThrowFileException(CFileException::invalidFile);
			}
			
			// Make sure there's a newline at the end.
			if ((nLength == 0) || (csLine[nLength-1] != '\n'))
			{
				csLine += '\n';
			}
		}
	}
	CATCH_ALL(e)
	{
		csLine.ReleaseBuffer();
		csLine.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
}

void CTextLineFile::WriteLine(CString& csLine)
{
	// Set up for writing.
	StartWriting();
	ASSERT(m_cfWrite.m_hFile != CFile::hFileNull);
	
	// Write the string.
	m_cfWrite.WriteString(csLine);
}

BOOL CTextLineFile::CopyLine(void)
{
	BOOL fResult = FALSE;
	
	CString csLine;
	
	TRY
	{
		if (ReadLine(csLine))
		{
			WriteLine(csLine);
			fResult = TRUE;
		}
	}
	CATCH_ALL(e)
	{
		csLine.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
}

void CTextLineFile::CopyRestOfLines(void)
{
	while (CopyLine()) ;
}

BOOL CTextLineFile::Done(BOOL fCommit /*=TRUE*/, CString* pcsBackupName /*=NULL*/)
{
	BOOL fResult = FALSE;
	
	if (pcsBackupName != NULL)
	{
		pcsBackupName->Empty();
	}

	// Compute a flag indicating whether any writing occurred.
	BOOL fWriting = m_cfWrite.m_hFile != CFile::hFileNull;
			
	// Check if we are committing the results. If not, then simply close
	// the files and remove the write file if it exists.
		
	if (!fCommit)
	{
		CloseFiles();	// does not throw an exception
		
		TRY
		{
			if (!m_csWrite.IsEmpty())
			{
				CFile::Remove(m_csWrite);
			}
		}
		END_TRY
		
		fResult = TRUE;
	}
	
	else
	{
		TRY
		{
			// We are not aborting and want to keep the results of all that
			// reading and writing.
			
			// We want to be very careful so the original file is never
			// destroyed:
			//
			//		1) Close the files.
			//		1)	Rename the old file to a different name.
			//		2) Rename the new file to the old name.
			//		3) Delete the renamed old file.
			//
			// If any of these fail, we try to clean up to the original state
			// and return FALSE.
			
			// Close the files.
			CloseFiles();	// does not throw an exception
			
			// If we were only reading, then we're done. Otherwise, then we have a bunch
			// of work to do to get the new data back to the original file.
			
			if (fWriting)
			{
				// Rename the old file to its backup name.
				
				CString csPath;
				CString csDrive;
				CString csDirectory;
				CString csFile;
				CString csExtension;
				
				TRY
				{
					// If the original file exists, then make a backup (X.BAK)
					if (Util::FileExists(m_csRead))
					{
						// First try ".BAQ". If it exists, then force ".BAK".
						Util::SplitPath(m_csRead, &csDrive, &csDirectory, &csFile, NULL);
						csExtension = "BAQ";
						Util::ConstructPath(csPath, csDrive, csDirectory, csFile, csExtension);
						
						if (Util::FileExists(csPath))
						{
							csExtension = "BAK";
							Util::ConstructPath(csPath, csDrive, csDirectory, csFile, csExtension);
						}
						
						TRY
						{
							// Make sure the backup file does not exist.
							CFile::Remove(csPath);
						}
						END_TRY
						
						// If the user wants to keep the backup file, then
						// pass the file name back.
						
						if (pcsBackupName != NULL)
						{
							*pcsBackupName = csPath;
						}
						
						CFile::Rename(m_csRead, csPath);
					}
				}
				CATCH_ALL(e)
				{
					// A string exception or the rename failed. Delete the new file.
					// The original file should still be intact with its original name.
					TRY
					{
						CFile::Remove(m_csWrite);
					}
					END_TRY
					
					csPath.Empty();
					csDrive.Empty();
					csDirectory.Empty();
					csFile.Empty();
					csExtension.Empty();
					
					THROW_LAST();
				}
				END_CATCH_ALL
				
				csDrive.Empty();
				csDirectory.Empty();
				csFile.Empty();
				csExtension.Empty();
				
				// Rename the new file to the old name.
				
				TRY
				{
					CFile::Rename(m_csWrite, m_csRead);
				}
				CATCH_ALL(e)
				{
					// The rename failed. Rename the original file back and delete the
					// new file.
					
					TRY
					{
						if (!csPath.IsEmpty())
						{
							CFile::Rename(csPath, m_csRead);
						}
					}
					END_TRY
					
					TRY
					{
						CFile::Remove(m_csWrite);
					}
					END_TRY
					
					csPath.Empty();
					
					THROW_LAST();
				}
				END_CATCH_ALL
				
				// At this point the new data is in place. The only
				// thing left to do is to delete the renamed old file
				// unless the user wants to keep the backup file.
				// If this fails, we ignore it.
				
				TRY
				{
					if (pcsBackupName == NULL)
					{
						if (!csPath.IsEmpty())
						{
							CFile::Remove(csPath);
						}
					}
				}
				END_TRY
				
				fResult = TRUE;
			}
		}
		CATCH_ALL(e)
		{
			// Some sort of error occured.
			THROW_LAST();
		}
		END_CATCH_ALL
	}
	
	return fResult;
}

void CTextLineFile::CloseFiles(void)
{
	TRY
	{
		if (m_cfRead.m_hFile != CFile::hFileNull)
		{
			m_cfRead.Close();
		}
	}
	END_TRY
		
	TRY
	{
		if (m_cfWrite.m_hFile != CFile::hFileNull)
		{
			m_cfWrite.Close();
		}
	}
	END_TRY
}

void CTextLineFile::StartReading(void)
{
	ASSERT(!m_csRead.IsEmpty());
	
	if (m_cfRead.m_hFile == CFile::hFileNull)
	{
		TRY
		{
			// We will be reading, open the file named by m_csRead.
			// If the file does not currently exist, then create it.
			
			if (Util::FileExists(m_csRead))
			{
				// Open the file for reading.
				CFileException e;
				if (!m_cfRead.Open(m_csRead, CFile::typeText|CFile::modeRead, &e))
				{
					AfxThrowFileException(e.m_cause, e.m_lOsError);
				}
			}
			else
			{
				// The file does not exist, attempt to create it.
				// Note we have to use modeReadWrite. Using modeRead causes
				// the first call to ReadString to throw an exception.
				CFileException e;
				if (!m_cfRead.Open(m_csRead, CFile::typeText|CFile::modeReadWrite|CFile::modeCreate, &e))
				{
					AfxThrowFileException(e.m_cause, e.m_lOsError);
				}
			}
		}
		CATCH_ALL(e)
		{
			CloseFiles();
			THROW_LAST();
		}
		END_CATCH_ALL
	}
}

void CTextLineFile::StartWriting(void)
{
	ASSERT(!m_csRead.IsEmpty());
	
	if (m_cfWrite.m_hFile == CFile::hFileNull)
	{
		CString csDrive;
		CString csDirectory;
		CString csFile;
		CString csExtension;
		
		// Create another file which will be written to as the new file is created.
		
		TRY
		{
			// Construct the name of the temporary file to which to write.
			Util::SplitPath(m_csRead, &csDrive, &csDirectory, &csFile, NULL);
			
			for (int nCount = 0; nCount < 500; nCount++)
			{
				wsprintf(csExtension.GetBuffer(3), "%03d", nCount);
				csExtension.ReleaseBuffer();
				Util::ConstructPath(m_csWrite, csDrive, csDirectory, csFile, csExtension);
				if (!Util::FileExists(m_csWrite))
				{
					TRACE("Use temp file '%s'\n", m_csWrite);
					break;
				}
				m_csWrite.Empty();
			}
			
			if (m_csWrite.IsEmpty())
			{
				AfxThrowUserException();
			}
			
			CFileException e;
			if (!m_cfWrite.Open(m_csWrite, CFile::typeText|CFile::modeCreate|CFile::modeWrite, &e))
			{
				AfxThrowFileException(e.m_cause, e.m_lOsError);
			}
		}
		CATCH_ALL(e)
		{
			csDrive.Empty();
			csDirectory.Empty();
			csFile.Empty();
			csExtension.Empty();
			
			CloseFiles();
			THROW_LAST();
		}
		END_CATCH_ALL
	}
}
