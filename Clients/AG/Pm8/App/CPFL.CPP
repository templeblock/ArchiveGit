/*
// $Workfile: CPFL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CPFL.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     8/05/98 10:55a Mwilson
// added a compresssion option to the installer
// 
//    Rev 1.0   14 Aug 1997 15:19:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:36:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:44   FRED
// Initial revision.
// 
//    Rev 2.3   08 Aug 1995 09:01:00   FRED
// Publishing Suite Installation
// 
//    Rev 2.2   07 Aug 1995 17:12:36   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:35:56   JAY
// Reverted. New series.
// 
//    Rev 1.3   22 Jun 1994 14:51:52   FRED
// Supports program file lines (in PMW.PFL) with
// elements separated by white space instead of '|'
// characters. The '|' character is replaced by space
// in file names after they have been parsed.
// 
// 
//    Rev 1.2   21 Jun 1994 14:40:30   FRED
// Changed old "flags" to "groups".
// Added "groups" field.
// 
// 
//    Rev 1.1   16 Jun 1994 18:13:32   FRED
// Version that copies files.
// 
// 
//    Rev 1.0   15 Jun 1994 19:01:38   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "install.h"
#include "chdirdlg.h"
#include "cpfl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgramFileList

CProgramFileList::CProgramFileList()
{
}

CProgramFileList::~CProgramFileList()
{
	DeleteAll();
}

BOOL CProgramFileList::Init(const CString &csFileName)
{
	BOOL fResult = FALSE;
	
	CStdioFile cfList;
	LPSTR lpszLine = NULL;
	CProgramFile *pProgramFile = NULL;
	
	TRY
	{
		#define	LINE_BUFFER_SIZE	(1024)
		
		// open the file
		if (cfList.Open(csFileName, CFile::modeRead))
		{
			if ((lpszLine = new far char[LINE_BUFFER_SIZE]) != NULL)
			{
				// read lines and construct CProgramFile objects until the end of file is reached
				for (;;)
				{
					// read the line
					if (cfList.ReadString(lpszLine, LINE_BUFFER_SIZE) == NULL)
					{
						// end of file
						fResult = TRUE;
						break;
					}
					
					// ignore blank and comment lines
					if ((*lpszLine != '\0') && (*lpszLine != '\n') && (*lpszLine != '#'))
					{
						// allocate the file item			
						if ((pProgramFile = new CProgramFile) == NULL)
						{
							break;
						}
	
						// initialize the item from the file data
						if (!pProgramFile->Init(lpszLine))
						{
							break;
						}
						
						// add it to the CPtrArray
						Add(pProgramFile);
						
						pProgramFile = NULL;
					}
				}

				delete [] lpszLine;				
				delete pProgramFile;
			}
			
			cfList.Close();
		}
	}
	CATCH_ALL(e)
	{
		cfList.Close();
		delete [] lpszLine;
		delete pProgramFile;
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return fResult;
}

void CProgramFileList::DeleteAll(void)
{
	/*
	// Free all the elements in the array.
	*/
	
	for (int i = 0; i <= GetUpperBound(); i++)
	{
		CProgramFile *pProgramFile;
		
		if ((pProgramFile = (CProgramFile *)GetAt(i)) != NULL)
		{
			SetAt(i, NULL);
			delete pProgramFile;
		}
	}
}

DWORD CProgramFileList::GetFileSize(DWORD dwGroups, DWORD dwClusterSize) const
{
	DWORD dwSize = 0;
	                 
	/*
	// Sum the sizes of all files who belong to the classes
	// specified by dwGroups.
	*/
	
	for (int i = 0; i < GetSize(); i++)
	{
		CProgramFile *pProgramFile;
		
		if ((pProgramFile = (CProgramFile *)GetAt(i)) != NULL)
		{
			if ((pProgramFile->GetGroups() & dwGroups) != 0)
			{
				dwSize += pProgramFile->GetFileSize(dwClusterSize);
			}
		}
	}
	
	return dwSize;
}

/////////////////////////////////////////////////////////////////////////////
// CProgramFile

CProgramFile::CProgramFile()
{
	m_dwGroups = 0;
	m_csSource.Empty();
	m_csDestination.Empty();
	m_dwSize = (DWORD)-1;
	m_dwFlags = 0;
	m_dwCompression = 0;
}

CProgramFile::~CProgramFile()
{
}

BOOL CProgramFile::Init(LPCSTR lpData)
{
	BOOL fResult = FALSE;
	
	/*
	// Parse the input line. It should have the form:
	//
	//		<groups>|<source name>|<destination name>|<size>|<flags> [|<extra>] <null>
	//
	//		<groups>					the decimal DWORD group membership flags
	//		<source name>			the source directory relative path for the file
	//		<destination name>	the destination directory relative path for the file
	//		<size>					the decimal DWORD byte size of the file
	//		<flags>					the decimal DWORD flags value
	//
	//	
	*/
	
	if (ParseNumber(lpData, m_dwGroups)
	 && ParseString(lpData, m_csSource)
	 && ParseString(lpData, m_csDestination)
	 && ParseNumber(lpData, m_dwSize)
	 && ParseNumber(lpData, m_dwFlags)
	 && ParseNumber(lpData, m_dwCompression))
	 
	{
		NormalizeFileName(m_csSource);
		NormalizeFileName(m_csDestination);
		
		fResult = TRUE;
	}
	
	return fResult;
}

char CProgramFile::ParseChar(LPCSTR &lpData)
{
	char cResult;
	
	/*
	// Collect a character from the input line.
	// Translate field separators (|) into NULL's.
	*/
	
	if ((cResult = *lpData) != '\0')
	{
		lpData++;
		
		if (IsWhite(cResult))
		{
			cResult = '\0';
		}
	}
	
	return cResult;
}

BOOL CProgramFile::ParseNumber(LPCSTR &lpData, DWORD &dwResult)
{
	BOOL fResult = TRUE;
	
	/*
	// Collect a number from the input line.
	*/
	
	dwResult = 0;
	BOOL fSeenDigit = FALSE;
	char ch;
	
	SkipWhite(lpData);
	
	while ((ch = ParseChar(lpData)) != '\0')
	{
		if ((ch < '0') || (ch > '9'))
		{
			fResult = FALSE;
			dwResult = 0;
			break;	
		}
	
		fSeenDigit = TRUE;	
		dwResult = dwResult*10+(ch-'0');
	}
	
	return fSeenDigit && fResult;
}

BOOL CProgramFile::ParseString(LPCSTR &lpData, CString &dwResult)
{
	/*
	// Collect a string from the input line.
	*/
	
	BOOL fSeenCharacter = FALSE;
	
	dwResult.Empty();
		
	SkipWhite(lpData);
	
	char ch;

	while ((ch = ParseChar(lpData)) != '\0')
	{
		fSeenCharacter = TRUE;
		dwResult += ch;
	}
	
	return fSeenCharacter;
}
	
BOOL CProgramFile::IsWhite(char c)
{
	return
			(c == ' ')
		|| (c == '\t')
		|| (c == '\r')
		|| (c == '\n');
}

void CProgramFile::SkipWhite(LPCSTR &lpData)
{
	while (IsWhite(*lpData))
	{
		lpData++;
	}
}

void CProgramFile::NormalizeFileName(CString &dwName)
{
	for (int nIndex = 0; nIndex < dwName.GetLength(); nIndex++)
	{
		if (dwName[nIndex] == '|')
		{
			dwName.SetAt(nIndex, ' ');
		}
	}
}

DWORD CProgramFile::GetFileSize(DWORD dwClusterSize) const
{
	DWORD dwNumberOfClusters = GetSize()/dwClusterSize;
	
	if (dwNumberOfClusters*dwClusterSize < GetSize())
	{
		dwNumberOfClusters++;
	}
	
	return dwNumberOfClusters*dwClusterSize;
}
