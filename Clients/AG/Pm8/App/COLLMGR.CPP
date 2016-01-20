/*
// $Workfile: COLLMGR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/COLLMGR.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 33    3/01/99 2:26p Gbeddow
// fix for bug 155 (your own art is missing) which also fixed problems
// with "your own projects"
// 
// 32    2/27/99 6:31p Gbeddow
// Fixed PM8 bug #155 (your own art is missing)
// 
// 31    1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 30    1/05/99 5:31p Gbeddow
// art gallery (graphic browser) support for Print Shop-style graphic type
// filtering
// 
// 29    11/02/98 5:14p Jayn
// 
// 28    10/26/98 6:47p Jayn
// Download manager.
// 
// 27    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 26    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 25    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 24    8/25/98 1:05p Hforman
// checking for NULL collection passed in to FindCollection()
// 
// 23    7/24/98 4:44p Hforman
// quiting out of search if errorcode returned is ERRORCODE_Open, in case
// user cancels out of "please insert CD..."
// 
// 22    7/15/98 6:56p Hforman
// oops, needed to check nResult before checking if 4.0 projects
// 
// 21    7/15/98 6:03p Hforman
// calling function to convert 4.0 user project collection to new
// structure
// 
// 20    7/14/98 3:49p Hforman
// sourcesafe merge problems
// 
// 19    7/14/98 3:45p Hforman
// speed up Search() w/unlicensed items
// 
// 18    7/13/98 12:41p Jayn
// 
// 17    7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 16    7/06/98 5:42p Rlovejoy
// Update with added project types.
// 
// 15    6/25/98 7:35p Psasse
// added filtering of projects/collections/packs capability
// 
// 14    6/15/98 6:55p Hforman
// add "Match All" functionality for keyword searching
// 
// 13    6/01/98 5:44p Rlovejoy
// Hidden projects are now a set type.
// 
// 12    5/22/98 5:34p Hforman
// CPMWCollectionSearchContext now has an array of collections
// 
// 11    5/21/98 1:54p Dennis
// Added Searchable Art Collections
// 
// 10    5/21/98 10:07a Rlovejoy
// Look for the 'Hidden' attribute.
// 
// 9     5/20/98 2:27p Dennis
// Added Close() functionality
// 
// 8     4/15/98 5:52p Hforman
// supply default category in ImportContent()
// 
// 7     4/14/98 10:08a Dennis
// a) Removed Media and Style
// b) Added UpdateSharedPaths() method for shared collection support.
// c) Enhanced OpenReadOnlyCollections() to look for INI files in shared
// path(s)
// 
// 6     4/13/98 12:03p Hforman
// modified params to ImportItem()
// 
// 5     4/08/98 6:44p Hforman
// interim checkin - more later
// 
// 4     4/06/98 11:18a Hforman
// 
// 3     1/30/98 5:40p Hforman
// 
// 2     1/13/98 11:10a Hforman
// fixed signed/unsigned warning in Search()
// 
//    Rev 1.1   16 Sep 1997 16:14:50   Jay
// Minor fix
// 
//    Rev 1.0   14 Aug 1997 15:19:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:42   Fred
// Initial revision.
// 
//    Rev 1.22   18 Jul 1997 11:11:06   Jay
// Now closes user collection streams at appropriate times.
// 
//    Rev 1.21   14 Jul 1997 13:01:30   Jay
// Made old directories (PICTURES and DOCUMENT) relative to the PMW directory.
// 
//    Rev 1.20   11 Jul 1997 09:38:00   Jay
// Free collection stuff
// 
//    Rev 1.19   09 Jul 1997 08:55:44   dennisp
// Misc fixes for 16 bit (hmemcpy parameter changed from short to long size)
// 
//    Rev 1.18   24 Jun 1997 15:17:24   Jay
// Support for multiple file paths and CD friendly names
// 
//    Rev 1.17   17 Jun 1997 12:42:26   Jay
// Import; OpenStreams()
// 
//    Rev 1.16   13 Jun 1997 13:35:38   dennisp
// Added FindCollection by Collection *
// 
//    Rev 1.15   11 Jun 1997 15:51:06   johno
// Add on support
// 
// 
//    Rev 1.14   10 Jun 1997 14:47:02   Jay
// Improved unlicensed sort
// 
//    Rev 1.13   10 Jun 1997 14:20:46   Jay
// Moves unlicensed content to end
// 
//    Rev 1.12   10 Jun 1997 09:12:46   Jay
// Sorted collections
// 
//    Rev 1.11   03 Jun 1997 11:32:10   dennisp
// Added Find() for locating an item
// 
//    Rev 1.10   22 May 1997 10:29:46   Jay
// Changed hmemcpy to HMemCpy.
// 
//    Rev 1.9   07 May 1997 12:10:46   Jay
// User directory and collections
// 
//    Rev 1.8   06 May 1997 17:21:56   Jay
// Multi-collection stream management
// 
//    Rev 1.7   06 May 1997 15:32:04   Jay
// Initial multi-collection searches
// 
//    Rev 1.6   06 May 1997 10:30:48   Jay
// User collections
// 
//    Rev 1.5   29 Apr 1997 13:54:48   Jay
// Got rid of a memory leak.
// 
//    Rev 1.4   25 Apr 1997 11:51:32   Jay
// 16-bit changes.
// 
//    Rev 1.3   23 Apr 1997 14:47:00   Jay
// success -> ERRORCODE_None.
// 
//    Rev 1.2   22 Apr 1997 10:22:42   Jay
// Added some dummy functions to make the 16-bit version build.
// 
//    Rev 1.1   21 Apr 1997 18:06:32   Jay
// Now creates collection objects.
// 
//    Rev 1.0   21 Apr 1997 14:49:38   Jay
// Collection manager
*/

#include "stdafx.h"
#include "pmw.h"
//#include "resource.h"
#include "collmgr.h"
#include "collimp.h"
#include "colldrw.h"
#include "inifile.h"
#include "util.h"
#include "file.h"
#include "pmwcfg.h"

static char BASED_CODE SECTION_FriendlyNames[] = "Friendly Names";
static char BASED_CODE FreeArtCollectionName[] = "PMG4FREE";

extern LPCSTR BrowserAllString(void);

/*
// The collection manager.
//
// Some notes about user collections:
// 1) Collections are INI-file based; user collections have INI files as well.
//  a) User project collection INI files have the name _xxxPROJ.INI
//  b) User art collection INI files have the name _xxxART.INI
// 2) The user collection INI files are dynamically created when they don't
//    exist. Each type has a file-based "definition" image which is copied
//    to the correct INI file to create it. These files live in the content
//    subdirectory
//  a) The user project collection "def" file is USERPROJ.DEF.
//  b) The user art collection "def" file is USERART.DEF.
// 3) The user collection INI files do NOT have a "Contents" section. They
//    aren't linked in via that mechanism. The user section name is pre-defined.
//  a) The user project collection section name is "USERPROJ".
//  b) The user art collection section name is "USERART".
*/

struct CUserCollectionStatics
{
	LPCSTR m_pDirectory;
	LPCSTR m_pIniName;
	LPCSTR m_pDefinitionFile;
	LPCSTR m_pSection;
	UINT m_uCorruptMessage;
} UserStatics[] =
{
	// Project.
	{
		"[[D]]",
		"USERPROJ.INI",
		"USERPROJ.DEF",
		"USERPROJ",
		IDS_CorruptUserProjects
	},
	// Art.
	{
		"[[P]]",
		"USERART.INI",
		"USERART.DEF",
		"USERART",
		IDS_CorruptUserArt
	},
#if 0
	// Sentiment (not used)
	{
		"[[?]]",
		"USERSENT.INI",
		"USERSENT.DEF",
		"USERSENT",
		IDS_CorruptUserSentiments
	}
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CContentCollectionArray
CContentCollectionArray::CContentCollectionArray()
	: m_fDeleteElementsOnDestruction(TRUE)
{
}

CContentCollectionArray::~CContentCollectionArray()
{
   Empty();
}

void CContentCollectionArray::Empty()
{
	if (m_fDeleteElementsOnDestruction)
	{
		// Delete all the pointers.
		for (int n = 0; n < GetSize(); n++)
		{
			delete GetAt(n);
		}
	}
	// Remove all pointers.
	RemoveAll();
}

void CContentCollectionArray::SetDeleteElementsOnDestruction( BOOL fDeleteElementsOnDestruction )
{
	m_fDeleteElementsOnDestruction = fDeleteElementsOnDestruction;
}

/////////////////////////////////////////////////////////////////////////////
// CPMWCollectionSearchContext

CPMWCollectionSearchContext::CPMWCollectionSearchContext()
{
	// Is this correct? The SetSize creates an empty string...
//	m_strAryCategories.SetSize(1);
	m_strAryCategories.Add(BrowserAllString());
	m_strKeywords = "";
	m_nTone = CCategoryDataItem::toneNone;
	m_nGraphicType = CCategoryDataItem::cfNone;
	m_fKeywordsMatchAll = FALSE;
}

CPMWCollectionSearchContext::~CPMWCollectionSearchContext()
{
}

long CPMWCollectionSearchContext::Find(USHORT nID, DWORD dwItemNumber)
   {
      long     lResultCount = GetResultCount(), lCurResult;

      for(lCurResult = 0; lCurResult < lResultCount; lCurResult++)
			{
				CPMWCollectionResult Result = GetSearchResult(lCurResult);
				if (Result.GetCollectionID() == nID &&
					 Result.GetItemNumber() == dwItemNumber)
					{
						return lCurResult;
					}
         }

      // Failed to find item
      return -1;
   }

/////////////////////////////////////////////////////////////////////////////
// CPMCollectionManager

CPMCollectionManager::CPMCollectionManager()
{
	// NULL out the pointers to the user collections.
	memset(m_pUserCollections, 0, sizeof(m_pUserCollections));
	m_fRegistered = FALSE;
	m_pPathManager = NULL;

	// holds aliased pointers that get deleted elsewhere
	for (CPMWCollection::CollectionSubTypes nSubType = CPMWCollection::subTypeListStart;
		  nSubType < CPMWCollection::subTypeListEnd;
		  nSubType++)
	{
		m_SubTypeCollections[nSubType].SetDeleteElementsOnDestruction( FALSE );
	}
}

CPMCollectionManager::~CPMCollectionManager()
{
	// Close the path manager if not already done.
	Close();
}

/*
// Open the collection manager.
*/

BOOL CPMCollectionManager::Open(IPathManager* pPathManager, BOOL fRegistered, BOOL fAllowUserCollections /*=TRUE*/)
{
	// Set our pointer to the path manager.
	m_pPathManager = pPathManager;
	m_pPathManager->AddRef();

	m_fAllowUserCollections = fAllowUserCollections;

	DoOpen(fRegistered);

	return TRUE;
}

void CPMCollectionManager::Reopen(void)
{
	DoClose();
	DoOpen(m_fRegistered);
}

void CPMCollectionManager::DoOpen(BOOL fRegistered)
{
   UpdateSharedPaths();

	OpenReadOnlyCollections();
	GET_PMWAPP()->UpdateLicensedProducts();
   if (m_fAllowUserCollections)
   {
	   // Open the user collections.
	   if (OpenUserCollection(CPMWCollection::typeProjects) != ERRORCODE_None)
	   {
		   // We could not open or create the user project collection.
		   AfxMessageBox(IDS_CantInitializeUserProjects);
	   }
	   else
	   {
		   ASSERT(GetUserCollection(CPMWCollection::typeProjects) != NULL);
	   }

	   if (OpenUserCollection(CPMWCollection::typeArt) != ERRORCODE_None)
	   {
		   // We could not open or create the user art collection.
		   AfxMessageBox(IDS_CantInitializeUserArt);
	   }
	   else
	   {
		   ASSERT(GetUserCollection(CPMWCollection::typeArt) != NULL);
	   }
   }

	// Sort the collections.
	SortCollections();
}

BOOL CPMCollectionManager::OpenReadOnlyCollections(BOOL fNewOnly /*=FALSE*/)
{
	BOOL fAddedSomething = FALSE;
   CStringList    cslFileList;

	// Loop through all the INI files in the content directory.
   // Populate CStringList with INI filenames in the content directory
	CFileIterator fit;
	BOOL fRet = fit.FindFirst(m_pPathManager->ExpandPath("[[S]]\\*.INI"), _A_NORMAL|_A_RDONLY);
	while (fRet)
	{
		// We have an INI file.
		CString csIniFile;
		Util::ConstructPath(csIniFile, "[[S]]", fit.Name());
      cslFileList.AddTail(csIniFile);

		// Go to the next file.
		fRet = fit.FindNext();
   }
   fit.Close();


   // Add INI filenames located in the shared content directories
   const CStringArray *pcsaSharedPaths = GetSharedPaths();
   if(pcsaSharedPaths)
      {
         CString  csFileToFind, csSharedFile;
         for(int i=0;i < pcsaSharedPaths->GetSize();i++)
            {
               csFileToFind = pcsaSharedPaths->GetAt(i);
               csFileToFind += "\\*.INI";
	            fRet = fit.FindFirst(m_pPathManager->ExpandPath(csFileToFind), _A_NORMAL|_A_RDONLY);
	            while (fRet)
	            {
		            // We have an INI file.
                  csSharedFile = pcsaSharedPaths->GetAt(i) + '\\';
                  csSharedFile += fit.Name();
                  if (m_pPathManager->FileExists(csSharedFile))
                     cslFileList.AddTail(csSharedFile);

		            // Go to the next file.
		            fRet = fit.FindNext();
               }
            }
      }
   fit.Close();

	for (POSITION posFileList = cslFileList.GetHeadPosition();
		  posFileList != NULL && cslFileList.GetCount() > 0;
		  cslFileList.GetNext(posFileList))
	{
		// Setup the INI file so we can get the section names.
		CIniFile IniFile(m_pPathManager->ExpandPath(cslFileList.GetAt(posFileList)));
#ifdef _DEBUG
      afxDump << "INI FileName: " << cslFileList.GetAt(posFileList) << "\n";
#endif

		// Add normal collections.
		fAddedSomething |= AddIniCollections(IniFile, "Contents", fNewOnly);

		// Add online collections.
		fAddedSomething |= AddIniCollections(IniFile, "Online Contents", fNewOnly);

		if (!fNewOnly)
		{
			// Get the list of friendly names in this INI file.
			LPSTR pEntries = IniFile.GetSectionKeys(SECTION_FriendlyNames);
			if (pEntries != NULL)
			{
				for (LPCSTR pEntry = pEntries; *pEntry != '\0'; pEntry += strlen(pEntry)+1)
				{
					// Read the next friendly name.
					// The entry is the volume label, and the value is the friendly name.
					CString csFriendlyName = IniFile.GetString(SECTION_FriendlyNames, pEntry);
					if (!csFriendlyName.IsEmpty())
					{
						m_mapFriendlyNames.SetAt(pEntry, csFriendlyName);
					}
				}	
				delete [] pEntries;
			}
		}
	}
   cslFileList.RemoveAll();

	return fAddedSomething;
}

BOOL CPMCollectionManager::AddIniCollections(CIniFile& IniFile, LPCSTR pszSection, BOOL fNewOnly /*=FALSE*/)
{
	BOOL fAddedSomething = FALSE;

	// Get the list of content entries.
	LPSTR pSections = IniFile.GetSectionKeys(pszSection);
	if (pSections != NULL)
	{
		for (LPCSTR pSection = pSections; *pSection != '\0'; pSection += strlen(pSection)+1)
		{
			TRACE("Got section: %s\n", pSection);
			CString csSection = pSection;

			CString csFilters = IniFile.GetString(csSection, "Filters");

			if(!GetConfiguration()->ExcludeFilters(csFilters))
			{
				if (!fNewOnly
					 || FindCollection(csSection) == NULL)
				{

					// If this is not an "unregistered" collection, add it.
					if (m_fRegistered
							|| csSection.CompareNoCase(FreeArtCollectionName) != 0)
					{
						CString csName = IniFile.Name();
						CPMWCollection* pNewCollection = new CPMWCollection;
						int nReturn;
						if ((nReturn = pNewCollection->Init(m_pPathManager,
																		NULL,
																		csName,
																		csSection,
																		CPMWCollection::originGeneric,
																		CPMWCollection::modeRead)) == ERRORCODE_None)
						{
							CPMWCollection::CollectionTypes nType = pNewCollection->GetType();
							TRACE("Got type %d for collection %s\n", nType, pSection);
							if (nType >= CPMWCollection::typeListStart
									&& nType < CPMWCollection::typeListEnd)
							{
								m_Collections[nType].Add(pNewCollection);
								fAddedSomething = TRUE;
							}
							else
							{
								// Bad type!
								ASSERT(FALSE);
								delete pNewCollection;
							}

							// GCB 3/1/99 - subtype collection support
							CPMWCollection::CollectionSubTypes nSubType = pNewCollection->GetSubType();
							TRACE("Got subtype %d for collection %s\n", nSubType, csName);
							if (nSubType >= CPMWCollection::subTypeListStart
									&& nSubType < CPMWCollection::subTypeListEnd)
							{
								// add pointer alias to m_SubTypeCollections that won't get deleted
								// (it will be deleted from m_Collections)
								m_SubTypeCollections[nSubType].Add(pNewCollection);
							}
							else if (nSubType != CPMWCollection::subTypeNone)
							{
								// Bad type!
								ASSERT(FALSE);
							}
						}
						else
						{
							TRACE("Got error %d opening '%s|%s'\n", nReturn, IniFile.Name(), pSection);
							delete pNewCollection;
						}
					}
				}
			}
		}
		delete [] pSections;
	}
	return fAddedSomething;
}

void CPMCollectionManager::UpdateSharedPaths()
   {
   char     chKey[MAX_PATH], szSharedPath[MAX_PATH];
   HKEY     hKey = NULL, hSharedKey = NULL;
   DWORD    retCode;
   DWORD    dwLength = 0;
   LPCSTR   lpcstrSharedKey = "SOFTWARE\\Mindscape\\Shared";
   CString  csSharedPathKey;

   m_csaSharedPaths.RemoveAll();

   TRY
   {
      if ( (retCode = RegOpenKeyEx(
               HKEY_LOCAL_MACHINE,
               lpcstrSharedKey,
               0,
               KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
               &hKey)) == ERROR_SUCCESS)
      {
         for(int i=0;retCode == ERROR_SUCCESS;i++)
            {
               retCode = RegEnumKey(hKey, i, chKey, MAX_PATH);
               if(retCode == (DWORD) ERROR_SUCCESS)
                  {
                     hSharedKey = NULL;
                     csSharedPathKey = lpcstrSharedKey;
                     csSharedPathKey += "\\";
                     csSharedPathKey += chKey;
                     if ( (retCode = RegOpenKeyEx(
                              HKEY_LOCAL_MACHINE,
                              (LPCSTR)csSharedPathKey,
                              0,
                              KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
                              &hSharedKey)) == ERROR_SUCCESS)
                     {
                        dwLength = sizeof(szSharedPath);
                        if ((retCode = RegQueryValueEx(
                                 hSharedKey,
                                 NULL,
                                 NULL,
                                 NULL,
                                 (LPBYTE)szSharedPath,
                                 &dwLength)) == ERROR_SUCCESS)
                           {
                              m_csaSharedPaths.Add(szSharedPath);
                           }
                        RegCloseKey(hSharedKey);
                     }
                  }

            }
         RegCloseKey(hKey);
      }
   }
   CATCH_ALL(e)
   {
      if (hKey != NULL)
      {
         RegCloseKey(hKey);
      }
      if (hSharedKey != NULL)
      {
         RegCloseKey(hSharedKey);
      }
      csSharedPathKey.Empty();
   }
   END_CATCH_ALL
   }

/*
// Update the free collection.
*/

void CPMCollectionManager::UpdateFreeCollection(BOOL fRegistered)
{
	if (fRegistered != m_fRegistered)
	{
		// Set the new registered state.
		m_fRegistered = fRegistered;

		// Update with the new registered state.
		UpdateCollections();
	}
}

/*
// Update collections.
*/

void CPMCollectionManager::UpdateCollections(void)
{
	if (OpenReadOnlyCollections(TRUE))
	{
		// We added something. Re-sort the collections.
		SortCollections();
	}
}

/*
// Open a user collection.
// Note that the input parameter is modified (NOT LPCSTR).
*/

int CPMCollectionManager::OpenUserCollection(CPMWCollection::CollectionTypes nType)
{
	// Make sure the directory exists.
	CString csCollectionDir = m_pPathManager->ExpandPath(UserStatics[nType].m_pDirectory);
	if (!Util::MakeDirectory(csCollectionDir))
	{
		// Not able to make the directory.
		// LPCSTR pFormat = "Can't create the directory\n%s";
		CString csFormat;
		csFormat.LoadString(IDS_ErrCreateDirectory);
		CString csMessage;
		csMessage.Format(csFormat, (LPCSTR)csCollectionDir);
		AfxMessageBox(csMessage);
		return ERRORCODE_Fail;
	}

	// Set up the INI file name.
	CString csIniFile;
	Util::ConstructPath(csIniFile,
							  UserStatics[nType].m_pDirectory,
							  UserStatics[nType].m_pIniName);
   csIniFile = m_pPathManager->ExpandPath(csIniFile);

	// Set up the section name.
   CString csUserSection = UserStatics[nType].m_pSection;

	// Assume the following are valid until proven otherwise.
	BOOL fIniFileInvalid = FALSE;
	BOOL fDefFileInvalid = FALSE;

	int nResult;
	BOOL fCreated = FALSE;

	for (;;)
	{
		// Try to setup the INI file.
		// There are a number of situations (in the following, "bad" means
		// no INI file or INI file has invalid entries):
		// (1) INI file is bad; valid DEF is available.
		// (3) INI file is invalid, and local DEF file is invalid.
		// (4) INI file is invalid and no valid DEf file can be found.
		// We try it both ways (local and CD). If it totally fails, we
		// return ERRORCODE_NoINIFile.

		if (SetupUserIni(csIniFile, nType, fIniFileInvalid, fDefFileInvalid) != ERRORCODE_None)
		{
			// The DEF file doesn't exist or the INI file can't be written.
			nResult = ERRORCODE_NoINIFile;
			// Leave the 'for' loop.
			break;
		}

		// Now open the collection in read/write mode.
		CPMWCollection* pNewCollection = new CPMWCollection;

		// Create a collection draw object to pass in.
		CPMCollectionDraw* pCollectionDraw = new CPMCollectionDraw;
		pCollectionDraw->Init(m_pPathManager);

		if ((nResult = pNewCollection->Init(m_pPathManager,
														(ICollectionDraw*)pCollectionDraw->GetInterface(&IID_ICollectionDraw),
														csIniFile,
														csUserSection,
														CPMWCollection::originUser,
														CPMWCollection::modeReadWrite,
														csCollectionDir)) == ERRORCODE_None)
		{
			// As an extra check, make sure the type is correct.
			ASSERT(nType == pNewCollection->GetType());
			if (nType != pNewCollection->GetType())
			{
				// Bad user! Nasty user!
				nResult = ERRORCODE_NoINIFile;
			}
		}

		// Release our creation ref. The collection will have its own ref if
		// we succeeded.
		pCollectionDraw->ExternalRelease();

		// If we had a successful open, validate the streams.
		if (nResult == ERRORCODE_None)
		{
			if ((nResult = pNewCollection->OpenStreams()) != ERRORCODE_None)
			{
				if (nResult == ERRORCODE_Corrupt)
            {
					if (AfxMessageBox(UserStatics[nType].m_uCorruptMessage,
										MB_YESNO | MB_ICONEXCLAMATION) != IDYES)
				   {
					   // Leave with the error.
					   break;
				   }
            }
				// Fall through to down below where we handle the error.
			}
		}

		// See if this is a 4.0 Project collection. If so, convert to new category structure
		if (nResult == ERRORCODE_None && 
			 nType == CPMWCollection::typeProjects &&
			 pNewCollection->Is40UserProjects())
		{
			nResult = pNewCollection->Convert40UserProjectCategories();
		}

		// Check the result code.
		if (nResult == ERRORCODE_None)
		{
			//
			// Success! We have a valid collection to add.
			//

			ASSERT(nType >= CPMWCollection::typeListStart
				&& nType < CPMWCollection::typeListEnd);

			// Add the collection to the list.
			m_Collections[nType].Add(pNewCollection);

			// Add a pointer to the collection to the user collection list.
			m_pUserCollections[nType] = pNewCollection;

			// And close the streams so we get file handles back.
 			pNewCollection->CloseAllStreams();

			// GCB 3/1/99 - subtype collection support
			CPMWCollection::CollectionSubTypes nSubType = pNewCollection->GetSubType();
			TRACE("Got subtype %d for collection %s\n", nSubType, csIniFile);
			if (nSubType >= CPMWCollection::subTypeListStart
					&& nSubType < CPMWCollection::subTypeListEnd)
			{
				// add pointer alias to m_SubTypeCollections that won't get deleted
				// (it will be deleted from m_Collections)
				m_SubTypeCollections[nSubType].Add(pNewCollection);
			}
			else if (nSubType != CPMWCollection::subTypeNone)
			{
				// Bad type!
				ASSERT(FALSE);
			}

			// Leave the 'for' loop.
			break;
		}
		else
		{
			//
			// General error handling section.
			//

			TRACE("Got error %d opening '%s|%s'\n", nResult, csIniFile, csUserSection);

			// Out with the collection.
			delete pNewCollection;
			pNewCollection = NULL;

			// See what the error is.
			// (A switch will not work here since we want to be able to break).
			if (nResult == ERRORCODE_NoINIFile
				 || nResult == ERRORCODE_InvalidINIEntry)
			{
				if (!fIniFileInvalid)
				{
					// The INI file is bad.
					fIniFileInvalid = TRUE;
				}
				else if (!fDefFileInvalid)
				{
					// The (hopefully local) DEF file is bad.
					fDefFileInvalid = TRUE;
				}
				else
				{
					// We can not find a valid INI or DEF file.
					// All we can do is return an error.
					break;
				}
			}
			else if (nResult == ERRORCODE_Corrupt || nResult == ERRORCODE_Open)
			{
				// We want to rebuild the collection.
				CPMWCollection Collection;
				if ((nResult = Collection.Init(m_pPathManager,
														 NULL,
														 csIniFile,
														 csUserSection,
														 CPMWCollection::originUser,
														 CPMWCollection::modeCreate,
														 csCollectionDir)) != ERRORCODE_None
					 || (nResult = Collection.OpenStreams()) != ERRORCODE_None)
				{
					break;
				}

				// Assume the INI file is valid now.
				fIniFileInvalid = FALSE;

				// Remember that we created this guy.
				fCreated = TRUE;

				// Collection closes here at destruct time.
			}
			else
			{
				// Some other kind of error.
				break;
			}
		}
	}

	// We have just successfully created the user collection.
	// Now, we want to see if there are any existing items to add to it.
	if (nResult == ERRORCODE_None && fCreated)
	{
		// Check for 3.0 items.
		AutoImport30Content(nType);
		// Check for 4.0 items
		AutoImport40Content(nType);
	}

	return nResult;
}

CPMWCollectionImportContext* CPMCollectionManager::NewImportContext(CPMWCollection::CollectionTypes nType)
{
	CPMWCollectionImportContext* pContext = NULL;

	// Establish a pointer to the user collection.
	// If none, then we have no place to import.
	CPMWCollection* pUserCollection = GetUserCollection(nType);
	if (pUserCollection != NULL)
	{
		// Create the context.
		TRY
		{
			switch (nType)
			{
				case CPMWCollection::typeProjects:
				{
					pContext = new CPMWProjectImportContext;
					break;
				}
				case CPMWCollection::typeArt:
				{
					pContext = new CPMWArtImportContext;
					break;
				}
				default:
				{
					break;
				}
			}
			pContext->m_pCollection = pUserCollection;
		}
		END_TRY
	}
	return pContext;
}

void CPMCollectionManager::AutoImport30Content(CPMWCollection::CollectionTypes nType)
{
	// Try to find the path to a 3.0 installation.
	// The way this is written, it will only work if the user installs 4.0 over
	// the existing 3.0 directory. This could be changed to look in the registry
	// (32-bit) or MSREGUSR.INI (16-bit), but it is not written that way right now.

	// Create the import context.
	CPMWCollectionImportContext* pContext = NewImportContext(nType);

	if (pContext != NULL)
	{
		// Construct the directory to look in.
		switch (nType)
		{
			case CPMWCollection::typeProjects:
			{
				// Choose the old default document path.
				pContext->m_csDirectory = m_pPathManager->ExpandPath("DOCUMENT");
				pContext->m_uConfirmMessage = IDS_Confirm30ProjectImport;
				break;
			}
			case CPMWCollection::typeArt:
			{
				// Choose the old default picture path.
				pContext->m_csDirectory = m_pPathManager->ExpandPath("PICTURES");
				pContext->m_uConfirmMessage = IDS_Confirm30ArtImport;
				break;
			}
			default:
			{
				// Invalid type.
				ASSERT(FALSE);
				return;
			}
		}

		pContext->ImportFromDirectory();

		delete pContext;
	}
}

void CPMCollectionManager::AutoImport40Content(CPMWCollection::CollectionTypes nType)
{
	// Create the import context.
	CPMWCollectionImportContext* pContext = NewImportContext(nType);

	if (pContext != NULL)
	{
		// Construct the directory to look in.
		switch (nType)
		{
			case CPMWCollection::typeProjects:
			{
				pContext->m_csDirectory = m_pPathManager->ExpandPath("[[D]]");
				pContext->m_uConfirmMessage = IDS_Confirm40ProjectImport;
				break;
			}
			case CPMWCollection::typeArt:
			{
				pContext->m_csDirectory = m_pPathManager->ExpandPath("[[P]]");
				pContext->m_uConfirmMessage = IDS_Confirm40ArtImport;
				break;
			}
			default:
			{
				// Invalid type.
				ASSERT(FALSE);
				return;
			}
		}

		pContext->ImportFromDirectory();
		delete pContext;
	}
}

/*
// Setup the user INI file.
*/

int CPMCollectionManager::SetupUserIni(CString csIniFile, CPMWCollection::CollectionTypes nType, BOOL fForceCopy /*=FALSE*/, BOOL fForceCDDef /*=FALSE*/)
{
   int nResult = ERRORCODE_None;
	// See if the INI file exists.
	if (fForceCopy || !m_pPathManager->FileExists(csIniFile))
	{
		// We need to create the INI file.
		// See if we have access to the DEF file.

		LPCSTR pDefinitionFile = UserStatics[nType].m_pDefinitionFile;

		// Construct a path to the local copy of the DEF file.
		CString csDefFile;
		Util::ConstructPath(csDefFile, "[[S]]", pDefinitionFile);

		// Does the local copy exist?
		if (fForceCDDef || !m_pPathManager->FileExists(csDefFile))
		{
			// Construct a path to the CD copy of the DEF file.
			Util::ConstructPath(csDefFile, "[[H]]\\CONTENT", pDefinitionFile);
		}

		// Copy the DEF file to the INI file.
		ReadOnlyFile Source(csDefFile);
		StorageFile Dest(csIniFile);

      nResult = copy_file(&Source, &Dest);
      Dest.flush();
	}
	return nResult;
}

/*
// Sort all collections.
*/

void CPMCollectionManager::SortCollections(void)
{
	for (CPMWCollection::CollectionTypes nType = CPMWCollection::typeProjects;
		  nType < CPMWCollection::typeListEnd;
		  nType++)
	{
		SortContentCollectionArray(&m_Collections[nType]);
	}

	for (CPMWCollection::CollectionSubTypes nSubType = CPMWCollection::subTypeListStart;
		  nSubType < CPMWCollection::subTypeListEnd;
		  nSubType++)
	{
		SortContentCollectionArray(&m_SubTypeCollections[nSubType]);
	}
}

void CPMCollectionManager::SortContentCollectionArray(CContentCollectionArray* pArray)
{
	// Run through all the collections of this type.
	int nCollections = pArray->GetSize();
	CContentCollectionArray SortArray;

	// Copy the pointers over to the sort array.
	for (int nCollection = 0; nCollection < nCollections; nCollection++)
	{
		SortArray.Add(pArray->GetAt(nCollection));
	}

	// Empty the source array.
	pArray->RemoveAll();

	// Now re-fill the source array in sorted order.
	while ((nCollections = SortArray.GetSize()) > 0)
	{
		CPMWCollection* pNext = NULL;
		int nNext;

		// Compute the next collection to move over.
		for (int nCollection = 0; nCollection < nCollections; nCollection++)
		{
			CPMWCollection* pCollection = SortArray.GetAt(nCollection);
			BOOL fSetNext = FALSE;
			if (pNext == NULL)
			{
				// Always set next if first one.
				fSetNext = TRUE;
			}
			else
			{
				// We need to do a comparison.
				int n = pCollection->GetSortIndex() - pNext->GetSortIndex();
				if (n == 0)
				{
					// Same index. Compare the names.
					// We set it if the new name is less than the current one.
					fSetNext = pCollection->GetFriendlyName().CompareNoCase(pNext->GetFriendlyName()) < 0;
				}
				else if (n > 0)
				{
					// Greater index. Use this one.
					fSetNext = TRUE;
				}
			}

			// If we need to set the next variables, do it now.
			if (fSetNext)
			{
				pNext = pCollection;
				nNext = nCollection;
			}
		}

		// Move the next one over.
		ASSERT(pNext != NULL);
		SortArray.RemoveAt(nNext);
		pArray->Add(pNext);
	}
}

/*
// Close the collection manager.
*/

void CPMCollectionManager::Close(void)
{
	DoClose();

	// Get rid of the path manager.
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Release();
		m_pPathManager = NULL;
	}
}

void CPMCollectionManager::DoClose(void)
{
	for (CPMWCollection::CollectionTypes nType = CPMWCollection::typeListStart;
		  nType < CPMWCollection::typeListEnd;
		  nType++)
	{
      m_Collections[nType].Empty();
	}

	for (CPMWCollection::CollectionSubTypes nSubType = CPMWCollection::subTypeListStart;
		  nSubType < CPMWCollection::subTypeListEnd;
		  nSubType++)
	{
      m_SubTypeCollections[nSubType].Empty();
	}

   m_csaSharedPaths.RemoveAll();

	// NULL out the pointers to the user collections.
	memset(m_pUserCollections, 0, sizeof(m_pUserCollections));

   m_mapFriendlyNames.RemoveAll();
}

/*
// How many collections are there of a particular type or subtype?
*/

int CPMCollectionManager::NumberOfCollections(int nType, int nSubType)
{
	if (nSubType != CPMWCollection::subTypeNone)
	{
		// Return the size of m_SubTypeCollections[nSubType].
		return m_SubTypeCollections[nSubType].GetSize();
	}
	else
	{
		// Return the size of collection[nType].
		if (nType < CPMWCollection::typeListEnd)
		{
			return m_Collections[nType].GetSize();
		}
	}

	// Invalid type.
	ASSERT(FALSE);
	return 0;
}

/*
// Get a particular collection of a particular type or subtype.
*/

CPMWCollection* CPMCollectionManager::GetCollection(int nCollection, int nType, int nSubType)
{
	if (nSubType != CPMWCollection::subTypeNone)
	{
		// Return the nth entry of m_SubTypeCollections[nSubType].
		return m_SubTypeCollections[nSubType].GetAt(nCollection);
	}
	else
	{
		// Return the nth entry of collection[nType].
		if (nType < CPMWCollection::typeListEnd)
		{
			return m_Collections[nType].GetAt(nCollection);
		}
	}

	// Invalid type.
	ASSERT(FALSE);
	return NULL;
}

/*
// Look up a collection by name.
*/

CPMWCollection* CPMCollectionManager::FindCollection(LPCSTR pszCollection)
{
	// Look through all the types.
	for (CPMWCollection::CollectionTypes nType = CPMWCollection::typeProjects;
		  nType < CPMWCollection::typeListEnd;
		  nType++)
	{
		CContentCollectionArray* pArray = &m_Collections[nType];
		// Run through all the collections of this type.
		int nCollections = pArray->GetSize();
		for (int nCollection = 0; nCollection < nCollections; nCollection++)
		{
			CPMWCollection* pCollection = pArray->GetAt(nCollection);
			// Compare the base name.
			if (pCollection->GetBaseName().CompareNoCase(pszCollection) == 0)
			{
				// Found the collection.
				return pCollection;
			}
		}
	}

	// Not found.
	return NULL;
}

int CPMCollectionManager::FindCollection(CPMWCollection * pCollectionToFind)
{
	if (pCollectionToFind == NULL)
	{
		ASSERT(pCollectionToFind);
		return -1;
	}

	CContentCollectionArray* pArray = &m_Collections[pCollectionToFind->GetType()];
	// Run through all the collections of this type.
	int nCollections = pArray->GetSize();
	for (int nCollection = 0; nCollection < nCollections; nCollection++)
	{
		CPMWCollection* pCollection = pArray->GetAt(nCollection);
		// Compare the base name.
		if (pCollection == pCollectionToFind) 
		{
			// Found the collection.
			return nCollection;
		}
	}

	// Not found.
	return -1;
}

/*
// Look up a collection by ID (across all types and subtypes).
*/

CPMWCollection* CPMCollectionManager::FindCollection(WORD wID)
{
	CPMWCollection* pCollection = NULL;

	// Look through all the types.
	for (CPMWCollection::CollectionTypes nType = CPMWCollection::typeProjects;
		  nType < CPMWCollection::typeListEnd;
		  nType++)
	{
		if ((pCollection = FindCollection(wID, nType, CPMWCollection::subTypeNone)) != NULL)
		{
			break;
		}
	}

	// If not found, look through all the subtypes.
	if (pCollection == NULL)
	{
		for (CPMWCollection::CollectionSubTypes nSubType = CPMWCollection::subTypeListStart;
			  nSubType < CPMWCollection::subTypeListEnd;
			  nSubType++)
		{
			if ((pCollection = FindCollection(wID, CPMWCollection::typeListStart, nSubType)) != NULL)
			{
				break;
			}
		}
	}

	return pCollection;
}

/*
// Look up a collection of a particular type or subtype by ID.
*/

CPMWCollection* CPMCollectionManager::FindCollection(WORD wID, int nType, int nSubType)
{
	if (nSubType != CPMWCollection::subTypeNone)
	{
		CContentCollectionArray* pArray = &m_SubTypeCollections[nSubType];
		// Run through all the collections of this subtype.
		int nCollections = pArray->GetSize();
		for (int nCollection = 0; nCollection < nCollections; nCollection++)
		{
			CPMWCollection* pCollection = pArray->GetAt(nCollection);
			// Compare the ID.
			if (pCollection->GetID() == wID)
			{
				// Found the collection.
				return pCollection;
			}
		}
	}
	else
	{
		CContentCollectionArray* pArray = &m_Collections[nType];
		// Run through all the collections of this type.
		int nCollections = pArray->GetSize();
		for (int nCollection = 0; nCollection < nCollections; nCollection++)
		{
			CPMWCollection* pCollection = pArray->GetAt(nCollection);
			// Compare the ID.
			if (pCollection->GetID() == wID)
			{
				// Found the collection.
				return pCollection;
			}
		}
	}

	return NULL;
}

// Get the friendly name for a volume.
BOOL CPMCollectionManager::VolumeToFriendlyName(LPCSTR pszVolume, CString& csFriendlyName) const
{
	return m_mapFriendlyNames.Lookup(pszVolume, csFriendlyName);
}

//
// Main entry for conducting a search.
//
int CPMCollectionManager::Search(CPMWCollectionSearchContext& Context)
{
	// See if we were passed in any collections.
	if (Context.m_ptrAryCollections.GetSize() == 0)
	{
		// If we're not passed any collections, search all of them
		for (int i = 0; i < NumberOfCollections(Context.m_nType, Context.m_nSubType); i++)
			Context.m_ptrAryCollections.Add(GetCollection(i, Context.m_nType, Context.m_nSubType));
	}

	int nCategories = Context.m_strAryCategories.GetSize();
	int nCollections = Context.m_ptrAryCollections.GetSize();

	// arrays to hold licensed and unlicensed items
	CPMWCollectionResultArray licensedArray;
	CPMWCollectionResultArray unlicensedArray;

	// array to hold CPMWLicensedInfo for each collection
	CPtrArray LicensedInfoArray;
	LicensedInfoArray.SetSize(nCollections);

	// We want things sorted by category, then collection, with all
	// licensed items at the top and all unlicensed items at the bottom.

	// NOTE: We sort first by category so that project types all get lumped
	// together. So "category" is really "super-category" - art should
	// only have a single category.
	for (int nCategory = 0; nCategory < nCategories; nCategory++)
	{
		// Search all collections
		for (int nCollection = 0; nCollection < nCollections ; nCollection++)
		{
			CPMWCollection* pCollection = (CPMWCollection*)Context.m_ptrAryCollections[nCollection];
			if (!pCollection)
				continue;

			// Try the search.
			CPMWCollectionSearchContext SubContext;

			SubContext.m_strAryCategories.RemoveAll();
			SubContext.m_strAryCategories.Add(Context.m_strAryCategories[nCategory]);
			SubContext.m_strKeywords = Context.m_strKeywords;
			SubContext.m_fKeywordsMatchAll = Context.m_fKeywordsMatchAll;
			SubContext.m_nTone = Context.m_nTone;
			SubContext.m_nGraphicType = Context.m_nGraphicType;

			int nResult = pCollection->Search(SubContext);

			// this is the case if user Cancels in the "insert CD" prompt
			if (nResult == ERRORCODE_Open)
				goto QuitLoop;

			if (nResult == ERRORCODE_None)
			{
				int nResultCnt = SubContext.GetResultCount();
				if (nResultCnt > 0)
				{
					// get license info for collection if we haven't yet
					CPMWLicensedInfo* pLicInfo = (CPMWLicensedInfo*)LicensedInfoArray[nCollection];
					if (pLicInfo == NULL)
					{
						pLicInfo = new CPMWLicensedInfo;	// remember to delete!
						pCollection->GetLicensedInfo(*pLicInfo);
						LicensedInfoArray.SetAt(nCollection, pLicInfo);
					}

					// separate licensed from unlicensed items
					if (pLicInfo->HasUnlicensedItems())
					{
						// check each item one at a time
						for (int i = 0; i < nResultCnt; i++)
						{
							CPMWCollectionResult Result = SubContext.GetSearchResult(i);
							if (pLicInfo->IsLicensed(Result.GetItemNumber()) == TRUE)
								licensedArray.Add(Result);
							else
								unlicensedArray.Add(Result);
						}
					}
					else
					{
						// no unlicensed items -- the fast case
						licensedArray.Append(SubContext.GetSearchResultArray());
					}
				}
			}
		} // end for (nCollection = 0; ...)
	} // end for (int nCategory = 0; ...)

QuitLoop:

	// clean up any License info
	for (int i = 0; i < LicensedInfoArray.GetSize(); i++)
	{
		CPMWLicensedInfo* pLicInfo = (CPMWLicensedInfo*)LicensedInfoArray[i];
		if (pLicInfo)
			delete pLicInfo;
	}

	// add items to search results - first licensed, then unlicensed
	Context.GetSearchResultArray().Copy(licensedArray);
	Context.GetSearchResultArray().Append(unlicensedArray);

	return ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CPMCollectionManager.

BEGIN_INTERFACE_MAP(CPMCollectionManager, CCmdTarget)
	INTERFACE_PART(CPMCollectionManager, IID_ICollectionManager, CollectionManager)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CPMCollectionManager.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CPMCollectionManager::XCollectionManager::AddRef()
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPMCollectionManager::XCollectionManager::Release()
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->ExternalRelease();
}

STDMETHODIMP CPMCollectionManager::XCollectionManager::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

//
// How many collections are there of a particular type or subtype?
//

STDMETHODIMP_(int) CPMCollectionManager::XCollectionManager::NumberOfCollections(int nType, int nSubType)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->NumberOfCollections(nType, nSubType);
}

//
// Get a particular collection of a particular type or subtype.
//

STDMETHODIMP_(CPMWCollection*) CPMCollectionManager::XCollectionManager::GetCollection(int nCollection, int nType, int nSubType)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->GetCollection(nCollection, nType, nSubType);
}

//
// Look up a collection by name.
//

STDMETHODIMP_(CPMWCollection*) CPMCollectionManager::XCollectionManager::FindCollection(LPCSTR pszCollection)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->FindCollection(pszCollection);
}

//
// Look up a collection by ID (across all types).
//

STDMETHODIMP_(CPMWCollection*) CPMCollectionManager::XCollectionManager::FindCollection(WORD wID)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->FindCollection(wID);
}

//
// Look up a collection by ID of a particular type.
//

STDMETHODIMP_(CPMWCollection*) CPMCollectionManager::XCollectionManager::FindCollection(WORD wID, int nType, int nSubType)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->FindCollection(wID, nType, nSubType);
}

//
// Look up a collection by Collection * to get it's index.
//

STDMETHODIMP_(int) CPMCollectionManager::XCollectionManager::FindCollection(CPMWCollection* pCollectionToFind)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->FindCollection(pCollectionToFind);
}

//
// Get a user collection of a particular type.
//

STDMETHODIMP_(CPMWCollection*) CPMCollectionManager::XCollectionManager::GetUserCollection(int nType)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->GetUserCollection(nType);
}

//
// Get the friendly name for a volume.
//

STDMETHODIMP_(BOOL) CPMCollectionManager::XCollectionManager::VolumeToFriendlyName(LPCSTR pszVolume, CString& csFriendlyName)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->VolumeToFriendlyName(pszVolume, csFriendlyName);
}

//
// Perform a search.
//

STDMETHODIMP_(int) CPMCollectionManager::XCollectionManager::Search(CPMWCollectionSearchContext& Context)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->Search(Context);
}

//
// Get the shared paths.
//

STDMETHODIMP_(const CStringArray*) CPMCollectionManager::XCollectionManager::GetSharedPaths(void)
{
	METHOD_PROLOGUE(CPMCollectionManager, CollectionManager)
	return pThis->GetSharedPaths();
}
