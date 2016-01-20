/*
// $Workfile: TutList.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TutList.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 9     12/04/98 5:31p Gbeddow
// directory reorganization in preparation for integrating Print Shop
// 
// 8     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 7     7/21/98 2:50p Johno
// Relative and "[[" paths are now used for tutorials -
// path_manager.make_full_path() will prompt for CD when they are expanded
// 
// 6     6/15/98 2:58p Hforman
// reading HD first, then CD for tutorials; fixed sorting problem
// 
// 5     5/15/98 4:42p Fredf
// 
// 4     1/20/98 10:24a Fredf
// Uses FILETIME instead of time_t in CRC generation. CTime and time_t
// break in the year 2038!
// 
// 3     11/06/97 6:06p Hforman
// added new functionality: reading list of Tutorials from CD, checking if
// more exist on HD, generating CRC, etc.
 * 
 * 2     10/30/97 6:52p Hforman
 * finishing up functionality, etc.
*/

#include "stdafx.h"
#include "pmw.h"
#include "key.h"
#include "sortarry.h"
//#include "..\Tutorial\TutInfo.h"
#include "TutRes.h"
#include "TutList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTutorialList class

CTutorialList::CTutorialList()
{
}

CTutorialList::~CTutorialList()
{
}

////////////////////////////////////////////////////////////////////////////
// CreateList()
//
// Creates the list of tutorials that are available. This includes stock
// tutorials that live on the CD, as well as any others that have been
// installed to the Harddrive.
// NOTE: be sure to call DestroyList() before exiting program.
//
/////////////////////////////////////////////////////////////////////////////
void CTutorialList::CreateList()
{
   // start clean
   DestroyList();

   CString tutDir;

   // First we check if there are tutorials on the HD. Because new tutorials
   // can be added at a later time, we need to do a bit of work here.

   // Check if "Tutorial" directory exists. If so, check if there
   // is a "tut.crc" file already created. If so, read it and compare
   // the files in the directory to the list. If they're the same, just
   // read in "tutorial.lst" file and add to m_TutorialArray.
   // If not, we'll have to generate a new CRC and list.
   CString tutFolder = "Tutorial";
	tutDir = GetGlobalPathManager()->LocatePath(tutFolder);
   CFileFind find;
   if (find.FindFile(tutDir))
   {   
      // OK, the directory exists. Check the CRC
      BOOL fOk = FALSE;

      CFile fileCrc;
      CString CrcFilename = tutDir + "\\tut.crc";
      if (fileCrc.Open(CrcFilename, CFile::modeRead))
      {
         // OK, the CRC file exists. Let's check if it's current
         DWORD crc;
         fileCrc.Read(&crc, sizeof(crc));
         fOk = CheckCrc(crc, tutDir);
         fileCrc.Close();
      }

      if (!fOk)
      {
         // well, looks like we have to generate a new CRC & list
         DWORD crc = GenerateCrc(tutDir);
         if (fileCrc.Open(CrcFilename, CFile::modeCreate|CFile::modeWrite))
         {
            fileCrc.Write(&crc, sizeof(crc));
            fileCrc.Close();
         }

         CreateNewList(tutDir);
      }

      // read in the list
      //CString listFilename = tutDir + '\\';
		CString listFilename = tutFolder + '\\';
      listFilename += TUTORIAL_INFO_FILENAME;
      ReadInTutorialList(listFilename);
   }

   // Now read in the list of stock tutorials from CD file,
	// and append to existing list from HD.
	// Note that if the same tutorial is found on the CD, the HD's
	// version will take precedence.
   //tutDir = GetGlobalPathManager()->LocatePath("[[H]]\\Tutorial\\");
	tutDir = "[[H]]\\Tutorial\\";
   CString listFilename = tutDir + TUTORIAL_INFO_FILENAME;
   ReadInTutorialList(listFilename);

   // now sort the list
   Sort();
}

void CTutorialList::DestroyList()
{
   for (int i = 0; i < m_TutorialArray.GetSize(); i++)
   {
      Tutorial* pTut = (Tutorial*)m_TutorialArray[i];
      delete pTut;
   }
}

int CTutorialList::GetSize()
{
   return m_TutorialArray.GetSize();
}

int CTutorialList::GetIDFromIndex(int nIndex)
{
   if (nIndex < 0 || nIndex > m_TutorialArray.GetUpperBound())
   {
      ASSERT(FALSE);
      return -1;
   }
   Tutorial* tut = (Tutorial*)m_TutorialArray[nIndex];
   return tut->nID;
}

int CTutorialList::GetIndexFromID(int nID)
{
   // find the tutorial with the ID passed in
   for (int i = 0; i < m_TutorialArray.GetSize(); i++)
   {
      Tutorial* tut = (Tutorial*)m_TutorialArray[i];
      if (tut->nID == nID)
		{
			return i;
		}
   }
   
   return -1;
}

BOOL CTutorialList::GetInfoFromIndex(int nIndex, LPCTSTR* ppszCaption, LPCTSTR* ppszType, LPCTSTR* ppszTitle, LPCTSTR* ppszPath)
{
	BOOL fResult = FALSE;

   if ((nIndex >= 0) && (nIndex <= m_TutorialArray.GetUpperBound()))
   {
      Tutorial* tut = (Tutorial*)m_TutorialArray[nIndex];
		if (ppszCaption != NULL)
		{
			*ppszCaption = tut->csCaption;
		}
		if (ppszType != NULL)
		{
			*ppszType = tut->csType;
		}
		if (ppszTitle != NULL)
		{
			*ppszTitle = tut->csTitle;
		}
		if (ppszPath != NULL)
		{
			*ppszPath = tut->csPath;
		}
		fResult = TRUE;
	}

	return fResult;
}

BOOL CTutorialList::GetInfoFromID(int nID, LPCTSTR* ppszCaption, LPCTSTR* ppszType, LPCTSTR* ppszTitle, LPCTSTR* ppszPath)
{
	return GetInfoFromIndex(GetIndexFromID(nID), ppszCaption, ppszType, ppszTitle, ppszPath);
}

////////////////////////////////////////////////////////////////////////////
// ReadInTutorialList()
//
// Reads the file passed in (usually tutorial.lst) and builds the internal
// list of tutorials to show on tutorial dialog/menu.
//
// Params:
//    filepath:   full path to file containing list of tutorials
//
/////////////////////////////////////////////////////////////////////////////
void CTutorialList::ReadInTutorialList(CString& fileFolder)
{
	CString filepath = GetGlobalPathManager()->LocatePath(fileFolder);
   CStdioFile File;
   BOOL fOK = File.Open(filepath, CFile::modeRead|CFile::typeText);
   if (!fOK)
   {
      TRACE("Couldn't open Tutorial List!\n");
      return;
   }

   // strip off list filename from path passed in
	filepath = fileFolder.Left(fileFolder.ReverseFind('\\') + 1);

	CString csString;
	CMlsStringArray csaLine;

	while (File.ReadString(csString))
	{
		csaLine.SetNames(csString);
		if (csaLine.GetSize() == 5)
		{
	      Tutorial* pTut = new Tutorial;
			pTut->nID = atoi(csaLine.ElementAt(1));
			pTut->csCaption = csaLine.ElementAt(2);
			pTut->csType = csaLine.ElementAt(3);
			pTut->csTitle = csaLine.ElementAt(4);
			pTut->csPath = filepath+csaLine.ElementAt(0);

			if (!IsInArray(pTut->nID))
				m_TutorialArray.Add(pTut);
			else
				delete pTut;
		}
	}
}

BOOL CTutorialList::IsInArray(int nID)
{
	for (int i = 0; i < m_TutorialArray.GetSize(); i++)
	{
		Tutorial* pTut = (Tutorial*)m_TutorialArray.GetAt(i);
		if (pTut == NULL)
		{
			ASSERT(pTut);
		}
		else
		{
			if (pTut->nID == nID)
				return TRUE;
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// CreateNewList()
//
// Create a new tutorial.lst file from all the "*.tut" files found in
// directory passed in. In order to build the list, each library found
// must be opened and read to get the ID and Title of tutorial.
//
// Params:
//    dir:     directory containing Tutorial DLLs (*.tut)
//
/////////////////////////////////////////////////////////////////////////////
void CTutorialList::CreateNewList(CString& dir)
{
   // Create the output file.
	CString csOut = dir + '\\' + TUTORIAL_INFO_FILENAME;
   CStdioFile fileOut;
   if (fileOut.Open(csOut, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
   {
	   CFileFind Find;
		if (Find.FindFile(dir + "\\*.tut"))
		{
			CString csFile;
			DWORD num = 0;
			BOOL more = TRUE;
			do
			{
				more = Find.FindNextFile();
				csFile = Find.GetFileName();
				CString csPath = dir + '\\' + csFile;
      
	         HINSTANCE handle = LoadLibrary(csPath);
				if (handle)
				{
					// Get resource string with tutorial information.
					char buf[512];
					if (LoadString(handle, IDS_TUTORIAL_INFO, buf, sizeof(buf)))
					{
						CString csInfo = csFile;
						csInfo += '|';
						csInfo += buf;

						TRY
						{
							fileOut.WriteString(csFile);
							fileOut.WriteString("|");
							fileOut.WriteString(buf);
							fileOut.WriteString("\n");
						}
						END_TRY
					}

	            FreeLibrary(handle);
				}
	      } while (more);
		}
   }
}

BOOL CTutorialList::CheckCrc(DWORD crc, CString& dir)
{
   DWORD diskCrc = GenerateCrc(dir);
   return (diskCrc == crc);
}

////////////////////////////////////////////////////////////////////////////
// GenerateCrc()
//
// Generates a DWORD based on all the "*.tut" files found in the directory
// directory passed in. Uses each file's name and last write time.
//
// Params:
//    dir:     directory containing Tutorial DLLs (*.tut)
//
/////////////////////////////////////////////////////////////////////////////
DWORD CTutorialList::GenerateCrc(CString& dir)
{
   // generate the crc for all ".tut" files in dir
   DWORD CRC = 0;
   
   CFileFind find;
   if (find.FindFile(dir + "\\*.tut"))
   {
      CString filename;
      FILETIME filetime;

      BOOL more = TRUE;
      do
      {
         more = find.FindNextFile();
         filename = find.GetFileName();
         find.GetLastWriteTime(&filetime);
         DWORD data = ::KeyFromString(filename);
         data += filetime.dwLowDateTime;
         data += filetime.dwHighDateTime;
         CRC += ::CalculateBlock((unsigned char*)&data, sizeof(data), 0);

      } while (more);
   }

   return CRC;
}

int CTutorialList::CompareTitle(const void* arg1, const void* arg2)
{
   Tutorial** ppTut1 = (Tutorial**)arg1;
   Tutorial** ppTut2 = (Tutorial**)arg2;
   return ((*ppTut1)->csTitle.Compare((*ppTut2)->csTitle));
}
int CTutorialList::CompareType(const void* arg1, const void* arg2)
{
   Tutorial** ppTut1 = (Tutorial**)arg1;
   Tutorial** ppTut2 = (Tutorial**)arg2;
   return ((*ppTut1)->csType.Compare((*ppTut2)->csType));
}
void CTutorialList::Sort()
{
	// First sort by title.
	qsort((void*)m_TutorialArray.GetData(), m_TutorialArray.GetSize(),
			sizeof(void*), CompareTitle);

	// Now sort by type.
	// NOTE: This assumes just two types (Design Tutorial/Mini Tutorial)
	int nTutorials = m_TutorialArray.GetSize();
	if (nTutorials > 2)
	{
		CPtrArray aryType1, aryType2;
		CString strType = ((Tutorial*)m_TutorialArray[0])->csType;
		for (int i = 0; i < m_TutorialArray.GetSize(); i++)
		{
			Tutorial* pTut = (Tutorial*)m_TutorialArray[i];
			if (pTut->csType == strType)
				aryType1.Add(pTut);
			else
				aryType2.Add(pTut);
		}
		m_TutorialArray.RemoveAll();
		m_TutorialArray.Append(aryType1);
		m_TutorialArray.Append(aryType2);
	}

#if 0
	// This didn't work, so it's out!

	// Primary sort key is type.
	qsort((void*)m_TutorialArray.GetData(), m_TutorialArray.GetSize(),
			sizeof(void*), CompareType);
#endif
}

