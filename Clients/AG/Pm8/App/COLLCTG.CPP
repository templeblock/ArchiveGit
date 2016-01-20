//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/COLLCTG.CPP 1     3/03/99 6:04p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/COLLCTG.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 14    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 13    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 12    8/25/98 1:02p Hforman
// added checks to avoid crashing if searching for unavailable collection
// 
// 11    8/10/98 4:09p Dennis
// Changes to fix Preprocessor Util
// 
// 10    7/25/98 4:38p Hforman
// fixed two bugs
// 
// 9     5/19/98 3:10p Hforman
// more DB fixes for user categories
// 
// 8     5/14/98 3:54p Hforman
// fixed crash bug in  CCategoryDatabase::Add()
// 
// 7     5/12/98 4:18p Hforman
// lots more add/delete functionality and fixes
// 
// 6     5/06/98 6:10p Hforman
// added Delete() functionality; merged Dennis' changes for FindNext()
// 
// 5     5/06/98 1:03p Dennis
// Fixed CCategoryDir::FindNext() problem when used with "Business|All"
// and "Business|Retail".
// 
// 4     4/30/98 7:02p Hforman
// Major modifications for User Categories
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "collctg.h"

#include "error.h"
#include "file.h"

#define  CATG_ARRAY_GROWBY    50

#ifdef NEW_THUMBNAILS
extern LPCSTR BrowserAllString(void);
#else
LPCSTR BrowserAllString(void)
{
	return "All";
}
#endif


void CCategoryEntry::Empty()
   {
      memset(this, 0, sizeof(CCategoryEntry)); 
      SetParent(none);
      SetFirstChild(none);
      SetNextSibling(none);
   }

CCategoryDir::CCategoryDir() :
   m_carrayCategories(sizeof(CCategoryEntry))
{
   int i;
   m_nNumLevels = 0;
   m_pFile = NULL;
   m_bWildcardSearch = FALSE;
   m_nCurLevel = m_nNumCurLevels = 0;
   for(i=0;i<MAX_CATEGORY_LEVELS;i++)
      m_naCurCategory[i] = -1;

	m_dwDataItemSize = 0;
}

// File must be open on init
int CCategoryDir::Init(CFile *pFile)
   {
      ASSERT(pFile);

      m_pFile = pFile;
      return LoadCategories();
   }

int CCategoryDir::LoadCategories()
   {
      unsigned short usNumCategoriesInFile;
      UINT           nBytesRead, nBytesToRead;
      
      ASSERT(m_pFile);
      if(!m_pFile->m_hFile)
         return ERRORCODE_NotInitialized;

      usNumCategoriesInFile = (unsigned short)(m_pFile->GetLength() / sizeof(CCategoryEntry));
      if(usNumCategoriesInFile > 0)
         {
            m_carrayCategories.SetSize(usNumCategoriesInFile, CATG_ARRAY_GROWBY);
            // Make sure we are at top of file
            DWORD dwNewOffset = m_pFile->Seek(0, CFile::begin);
            ASSERT(dwNewOffset == 0);
            if(dwNewOffset != 0)
               return ERRORCODE_Seek;
            nBytesToRead = sizeof(CCategoryEntry) * usNumCategoriesInFile;
            nBytesRead = m_pFile->Read(m_carrayCategories.ElementAt(0),
               nBytesToRead);
            // Should this throw an exception?
            ASSERT(nBytesRead == nBytesToRead);
            if(nBytesRead != nBytesToRead)
               return ERRORCODE_Read;
            UpdateCategoryDepth();
         }
      else
         {
            // Set grow by size
            m_carrayCategories.SetSize(0, CATG_ARRAY_GROWBY);
         }
      return ERRORCODE_None;
   }

CCategoryDir::~CCategoryDir()
{
   m_carrayCategories.RemoveAll();
}

void CCategoryDir::UpdateCategoryDepth()
{
   int               i;
   CCategoryEntry    *pCategory;

   // Loop through all categories to find deepest level
   m_nNumLevels = 0;
   for(i=0;i < m_carrayCategories.GetSize();i++)
   {
      pCategory = GetCategoryEntry(i);
      ASSERT(pCategory);
      if(!pCategory)
         break;
      if((pCategory->GetLevel()+1) > m_nNumLevels)
         m_nNumLevels = pCategory->GetLevel() + 1;
   }
}

CCategoryEntry * CCategoryDir::GetCategoryEntry(int nCategory)
{
	if (nCategory >= m_carrayCategories.GetSize())
	{
		ASSERT(0);
		// That item is not valid.
		return NULL;
	}

	return (CCategoryEntry*)m_carrayCategories.GetAt(nCategory);
}

int CCategoryDir::GetFirstChild(int nItemNum)
   {
      if(nItemNum < 0 || nItemNum >= m_carrayCategories.GetSize())
         return ERRORCODE_BadParameter;

      if(GetCategoryEntry(nItemNum)->GetFirstChild() < 0)
         return ERRORCODE_Fail;

      // Get Child Item
      return GetCategoryEntry(nItemNum)->GetFirstChild();
   }

int CCategoryDir::GetNextSibling(int nItemNum)
   {
      if(nItemNum < 0 || nItemNum >= m_carrayCategories.GetSize())
         return ERRORCODE_BadParameter;

      if(GetCategoryEntry(nItemNum)->GetNextSibling() < 0)
         return ERRORCODE_Fail;

      // Get Sibling
      return GetCategoryEntry(nItemNum)->GetNextSibling();
   }

int CCategoryDir::GetPrevSibling(int nItemNum)
   {
      int      nCurItem = nItemNum-1;
      int nLevelToFind = GetCategoryEntry(nItemNum)->GetLevel();

      if(nItemNum < 0 || nItemNum >= m_carrayCategories.GetSize())
         return ERRORCODE_BadParameter;

      while(nCurItem >= 0)
      {
         int nCurItemLevel = GetCategoryEntry(nCurItem)->GetLevel();
         if(nCurItemLevel == nLevelToFind)
           return nCurItem;
         // Stop when we get to parent
         if(nCurItemLevel < nLevelToFind)
            return ERRORCODE_Fail;
         nCurItem--;
      }
      return ERRORCODE_Fail;
   }

int CCategoryDir::GetParent(int nItemNum)
   {
      if(nItemNum < 0 || nItemNum >= m_carrayCategories.GetSize())
         return ERRORCODE_BadParameter;

      if(GetCategoryEntry(nItemNum)->GetParent() < 0)
         return ERRORCODE_Fail;

      return GetCategoryEntry(nItemNum)->GetParent();
   }

int CCategoryDir::FindPrevLevel(int nItemNum, int nCurLevel)
   {
      int      nCurItem = nItemNum;
      int      nItemLevel;
      
      if(nItemNum < 0 || nItemNum >= m_carrayCategories.GetSize())
         return ERRORCODE_BadParameter;

      if(nCurLevel >= 0)
         nItemLevel = nCurLevel-1;
      else
         nItemLevel = GetCategoryEntry(nItemNum)->GetLevel()-1;
      if(nItemLevel < 0)
         return ERRORCODE_Fail;

      while(nCurItem >= 0)
      {
        if(GetCategoryEntry(nCurItem)->GetLevel() == nItemLevel)
           return nCurItem;
        nCurItem--;
      }
      return ERRORCODE_Fail;
   }

int CCategoryDir::GetItem(int nEntryNum, CCategoryEntry *pCategoryItem)
   {
      ASSERT(pCategoryItem);
	if(nEntryNum < 0 || nEntryNum >= m_carrayCategories.GetSize())
         return ERRORCODE_BadParameter;

	CCategoryEntry *pRequestedCatItem = GetCategoryEntry(nEntryNum);
	if (pRequestedCatItem == NULL)
	{
		ASSERT(0);
		return ERRORCODE_Fail;
	}

	*pCategoryItem = *pRequestedCatItem;
      return ERRORCODE_None;
   }

int CCategoryDir::IncrementItemCount(int nItemNum)
   {
      if(nItemNum < 0 || nItemNum >= m_carrayCategories.GetSize())
         return ERRORCODE_BadParameter;
      GetCategoryEntry(nItemNum)->SetItemCount(GetCategoryEntry(nItemNum)->GetItemCount()+1);
      return ERRORCODE_None;
   }

int CCategoryDir::GetCategoryCount()
{
   return m_carrayCategories.GetSize();
}

// If an attempt to Add a category out of order occurs, 
// will return ERRORCODE_DataSequenceError
int CCategoryDir::Add(CString &strCategory, CString &strProductCode, DWORD dwFlags,
							 DWORD dwDataOffset)
{
   int               nRetVal, i, nNewIndex, nIndex, nSibling, nParent, nLastLevel;
   CCategoryEntry    categoryEntry;
   CString           strParentCategory;

   ASSERT(!strCategory.IsEmpty());
   if(strCategory.IsEmpty())
      return ERRORCODE_BadParameter;

   // Check if item is already in directory
   nIndex = FindFirst(strCategory);

   // If Already exists, then update with item count incremented
   // Increment item count for parents and grandparents, ect.
   if (nIndex >= 0)
   {
      nRetVal = GetItem(nIndex, &categoryEntry);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;

      // Make sure the category we found is last one.
      // If not, we're trying to add an item out of sequence 
		if((nIndex + 1) != GetCategoryCount())
         return ERRORCODE_DataSequenceError;

      categoryEntry.SetItemCount(categoryEntry.GetItemCount()+1);

      // Merge product flags with current flags - DGP
      DWORD dwNewFlags = categoryEntry.GetFlags();
      dwNewFlags |= dwFlags;
      categoryEntry.SetFlags(dwNewFlags);
      Update(nIndex, categoryEntry);

      // Increment item count for parents and grandparents, etc.
      for (nParent = GetParent(nIndex); nParent >= 0 ;)
      {
			CCategoryEntry* pCategory = GetCategoryEntry(nParent);
         IncrementItemCount(nParent);
         pCategory->SetFlags(pCategory->GetFlags() | dwFlags);
         // DGP-Change parameter to GetItem
         Update(nParent, *pCategory);

         // Get parent's parent (Grandparent)
         nParent = GetParent(nParent);
      }
	}

   // Make sure find didn't error out due to bad parameter, ect.
   if(nIndex < 0 && nIndex != ERRORCODE_Fail)
      return nIndex;    // Pass on failure code

   nLastLevel = m_nNumCurLevels - 1;
   for (i = m_nCurLevel; i < m_nNumCurLevels; i++)
   {
      // Parent, First Child, Next Sibling default to none
      categoryEntry.Empty();

      categoryEntry.SetLicenseInfo(strProductCode);
      // Get Category name from parsed fields array
      CString strCategoryName = m_csaCategoryFields.GetAt(i);
      categoryEntry.SetName(strCategoryName);
      categoryEntry.SetFlags(dwFlags);
      // Set parent
      if(i > 0)
      {
         // Get Parent Category name for validation
         strParentCategory = m_csaCategoryFields.GetAt(i - 1);
         nIndex = FindPrevLevel(m_carrayCategories.GetSize()-1, i);
         if(nIndex >= 0)
         {
            CCategoryEntry    prevLevelEntry;
            CString           strPrevLevelCategory;

            GetItem(nIndex, &prevLevelEntry);
            prevLevelEntry.GetName(&strPrevLevelCategory);
            if(strPrevLevelCategory != strParentCategory)
               return ERRORCODE_DataSequenceError;
            categoryEntry.SetParent(nIndex);
         }
         ASSERT(nIndex >= 0);
      }

      // First child should be right after this entry if
      // we aren't at lowest level, otherwise it's zero
      // Category count doesn't reflect this item we are adding yet
      if(i != nLastLevel)
         categoryEntry.SetFirstChild(m_carrayCategories.GetSize()+ 1);
      else
      {
         // Size category didn't exist, this must be 1st item
         categoryEntry.SetItemCount(1);
      }
      categoryEntry.SetOffsetToData(dwDataOffset);
      categoryEntry.SetLevel(i);

      // Add it!
      nNewIndex = AddEntryToEnd(categoryEntry);

      // Update previous category at this level to inform 
      // that they now have a sibling
      nSibling = GetPrevSibling(nNewIndex);
      if(nSibling>=0)
      {
         nRetVal = GetItem(nSibling, &categoryEntry);
         if(nRetVal == ERRORCODE_None)
         {
            categoryEntry.SetNextSibling(nNewIndex);
            Update(nSibling, categoryEntry);
         }
      }
      // Increment item count for parents and grandparents, ect.
      if(i == nLastLevel)
      {
         for(nParent = GetParent(nNewIndex);nParent >=0;)
         {
            IncrementItemCount(nParent);
            // DGP-Change parameter to GetItem
            Update(nParent, *GetCategoryEntry(nParent));
            // Get parent's parent (Grandparent)
            nParent = GetParent(nParent);
         }
      }
   }

#ifdef _DEBUG
//	CString path = GetGlobalPathManager()->ExpandPath("[[U]]\\");
//	path += "CatDirAfterAdd.txt";
//	CStdioFile dumpFile(path, CFile::modeCreate|CFile::modeWrite);
//	Dump(dumpFile);
#endif

   return ERRORCODE_None;
}

// returns index to new category item (zero based)
int CCategoryDir::AddEntryToEnd(const CCategoryEntry& newEntry)
{
	DWORD    dwNewItemNum;

	// Seek to end of file
	m_pFile->Seek(0, CFile::end);

	dwNewItemNum = ((m_pFile->GetPosition()+1) / sizeof(CCategoryEntry));

	// DGP-Check here for # of allocated entries

	m_carrayCategories.SetAtGrow(dwNewItemNum, &newEntry);

	// write throws exception if num written != num requested to write
	m_pFile->Write(&newEntry, sizeof(CCategoryEntry));
	return dwNewItemNum;
}

//////////////////////////////////////////////////////////////////////////////
// InsertEntry() -- insert an entry into directory.
//
// Params:
//		nInsertPos - 0-based index of where to insert
//		newCategory - entry to insert
//
///////////////////////////////////////////////////////////////////////////////
void CCategoryDir::InsertEntry(int nInsertPos, const CCategoryEntry& newEntry)
{
	// insert new category into array
	m_carrayCategories.InsertAt(nInsertPos, &newEntry);

	// write out new data starting at nInsertPos
	m_pFile->Seek(nInsertPos * sizeof(CCategoryEntry), CFile::begin);

	int nEntriesToWrite = m_carrayCategories.GetSize() - nInsertPos;
	int nBytesToWrite = nEntriesToWrite * sizeof(CCategoryEntry);
	m_pFile->Write(m_carrayCategories.ElementAt(nInsertPos), nBytesToWrite);

	m_pFile->Flush();
}

//////////////////////////////////////////////////////////////////////////////
// DeleteEntry()
//
// Delete an entry from the array, as well as in the file on disk. If the
// entry has a parent and this is the last child, delete the parent as well.
// If the entry is a parent, delete all of its children also.
// Updates all relationships (but see note 2 below).
//
// NOTES! NOTES! NOTES! NOTES!:
// 1) Currently implemented for two levels only (e.g. Posters|School Projects)
// 2) Does not deal with counts or offsets, just deletes entries.
//
//////////////////////////////////////////////////////////////////////////////
int CCategoryDir::DeleteEntry(int nEntryToDelete)
{
   if(nEntryToDelete < 0 || nEntryToDelete >= m_carrayCategories.GetSize())
      return ERRORCODE_BadParameter;

	CCategoryEntry* pEntryToDelete = GetCategoryEntry(nEntryToDelete);
	if (pEntryToDelete == NULL)
	{
		ASSERT(0);
		return ERRORCODE_Fail;
	}

	int nItemsDeleted = 0;

	int nParent = pEntryToDelete->GetParent();
	if (nParent >= 0)
	{
		// Check if there are other children.
		if (pEntryToDelete->GetNextSibling() < 0 && GetPrevSibling(nEntryToDelete) < 0)
		{
			// We're deleting the last child in the family - delete parent as well.

			// First check if there's a previous parent but no next parent.
			// If so, the previous parent is now the last, so its NextSib needs to be -1.
			int nPrevParent = GetPrevSibling(nParent);
			if (nPrevParent >= 0 && GetCategoryEntry(nParent)->GetNextSibling() == -1)
			{
				CCategoryEntry* pPrevParent = GetCategoryEntry(nPrevParent);
				pPrevParent->SetNextSibling(-1);
				Update(nPrevParent, *pPrevParent);
			}

			m_carrayCategories.RemoveAt(nParent);
			nEntryToDelete--;	// move location back 1
			nItemsDeleted++;
		}
	}
	else
	{
		// Entry isn't a child -- check if it has children
		int nChild = pEntryToDelete->GetFirstChild();
		if (nChild >= 0)
		{
			// Entry is a parent -- delete all children
			CCategoryEntry* pEntry = GetCategoryEntry(nChild);
			while (pEntry && pEntry->GetParent() == nEntryToDelete)
			{
				m_carrayCategories.RemoveAt(nChild);
				nItemsDeleted++;
				pEntry = GetCategoryEntry(nChild);
			}
		}
	}

	// Check if we're deleting the last item in the array or the last child in a group.
	// If so, set the previous item's Next Sibling to -1
	if (nEntryToDelete == m_carrayCategories.GetSize() - 1 ||
		 pEntryToDelete->GetNextSibling() == -1)
	{
		int nPrevSib = GetPrevSibling(nEntryToDelete);
		if (nPrevSib >= 0)
		{
			CCategoryEntry* pPrevSib = GetCategoryEntry(nPrevSib);
			ASSERT(pPrevSib);
			pPrevSib->SetNextSibling(-1);
			Update(nPrevSib, *pPrevSib);
		}
	}

	// delete entry
	m_carrayCategories.RemoveAt(nEntryToDelete);
	m_carrayCategories.FreeExtra();
	nItemsDeleted++;

	// now update all the relationships below the item(s) we deleted
	UpdatePositions(nEntryToDelete, -nItemsDeleted);

	// remove any old stuff from end of file
	m_pFile->SetLength(GetCategoryCount() * sizeof(CCategoryEntry));

	return ERRORCODE_None;
}

//////////////////////////////////////////////////////////////////////////////
// UpdatePositions()
//
// Update all entries' positions (starting with nStartIndex), adjusting the
// Parent/FirstChild/NextSibling relationships by nChange.
// Note that if entries have been deleted, nChange should be negative.
//
//////////////////////////////////////////////////////////////////////////////
int CCategoryDir::UpdatePositions(int nStartIndex, int nChange)
{
	// Update the entries in the array.

	// Note that if nStartIndex is greater than or equal to nMaxIndex,
	// there's nothing to do.

	CCategoryEntry entry;
	int nMaxIndex = GetCategoryCount() - 1;
	if (nStartIndex < nMaxIndex)
	{
		int nStartParent = GetCategoryEntry(nStartIndex)->GetParent();

		for (int nEntry = nStartIndex; nEntry < GetCategoryCount(); nEntry++)
		{
			if (GetItem(nEntry, &entry) != ERRORCODE_None)
			{
				ASSERT(0);
				return ERRORCODE_Fail;
			}

			if (entry.GetNextSibling() > 0)
				entry.SetNextSibling(entry.GetNextSibling() + nChange);
			if (entry.GetFirstChild() > 0)
				entry.SetFirstChild(entry.GetFirstChild() + nChange);

			// Change entry's parent unless the entry is a sibling of nStartIndex
			int nParent = entry.GetParent();
			if (nParent != nStartParent && nParent > 0)
				entry.SetParent(nParent + nChange);

			m_carrayCategories.SetAt(nEntry, &entry);
		}
	}

	// Update the file on disk
	m_pFile->Seek(nStartIndex * sizeof(CCategoryEntry), CFile::begin);

	int nBytesToWrite = (GetCategoryCount() - nStartIndex) * sizeof(CCategoryEntry);
	if (nBytesToWrite > 0)
		m_pFile->Write(m_carrayCategories.ElementAt(nStartIndex), nBytesToWrite);

	return ERRORCODE_None;
}

//////////////////////////////////////////////////////////////////////////////
// UpdateChildOffsets()
//
// Update all child offsets of parent.
//
//////////////////////////////////////////////////////////////////////////////
int CCategoryDir::UpdateChildOffsets(int nParentEntry, DWORD dwNewOffset)
{
	int nChild = GetCategoryEntry(nParentEntry)->GetFirstChild();
	int nCount = 0;
	while (nChild >= 0)
	{
		CCategoryEntry* pChild = GetCategoryEntry(nChild);
		if (pChild == NULL)
		{
			ASSERT(0);
			return ERRORCODE_Fail;
		}
		pChild->SetOffsetToData(dwNewOffset + nCount * sizeof(CCategoryDataItem));
		Update(nChild, *pChild);

		nCount += pChild->GetItemCount();
		nChild = GetNextSibling(nChild);
	}
	return ERRORCODE_None;
}

// Update an entry in the array
int CCategoryDir::Update(int nIndex, CCategoryEntry & refNewCategory)
{
   if(nIndex < 0 || nIndex >= m_carrayCategories.GetSize())
      return ERRORCODE_BadParameter;

   m_carrayCategories.SetAt(nIndex, &refNewCategory);

   // Seek to proper location in file
   m_pFile->Seek(nIndex * sizeof(CCategoryEntry), CFile::begin);

   // write throws exception if num written != num requested to write
   m_pFile->Write(&refNewCategory, sizeof(CCategoryEntry));

   return ERRORCODE_None;
}

// returns index to found category item
int CCategoryDir::FindFirst(CString &strCategoryToFind, int nStartIndex)
{
   for(int i = 0; i < MAX_CATEGORY_LEVELS; i++)
      m_naCurCategory[i] = -1;

   m_nCurCategory = -1;
   m_nCurLevel = 0;
   m_bWildcardSearch = FALSE;

   // Break up Category and SubCategories into separate fields
   m_csaCategoryFields.SetNames(strCategoryToFind);

   if(m_csaCategoryFields[0].Find(BrowserAllString()) == 0)
   {
      m_bWildcardSearch = TRUE;
   }

   // m_nNumCurLevels represents number of levels of categories 
   // ie.  if strCategoryToFind is "Animals|Birds" then m_nNumCurLevels = 2
   m_nNumCurLevels = m_csaCategoryFields.GetSize();
   if(m_nNumCurLevels < 1)
      return ERRORCODE_BadParameter;

   int nCurIndex = nStartIndex;

   if((nCurIndex + 1) > m_carrayCategories.GetSize())
      return ERRORCODE_Fail;

   // Current level starts with level of item we start search at
   if(!m_bWildcardSearch)
      m_nStartLevel = GetCategoryEntry(nCurIndex)->GetLevel();
   else
   {
      m_nStartLevel = 1;

      nCurIndex = FindCategory(m_nStartLevel);
      // This can fail for user collections since they have only 1 category
      if(nCurIndex < 0)
         return ERRORCODE_Fail;
   }
   m_nCurLevel = m_nStartLevel; 

   do
   {
      // Find category string at same level as current index
		if (m_nCurLevel > m_nNumCurLevels - 1)
		{
			ASSERT(0);
			return ERRORCODE_Fail;
		}

      nCurIndex = FindCategory(nCurIndex, m_csaCategoryFields[m_nCurLevel]);

      // if a match occurred, prepare to find category at next level
      if(nCurIndex >= 0)
      {
         // Bump current level
         m_nCurLevel++;
         // If we have matched all categories, return index of item
         if((m_nCurLevel + 1) > m_nNumCurLevels)
         {
            m_nCurCategory = nCurIndex;
            return nCurIndex;
         }

         // Get 1st child of current item in preparation for next category find
         nCurIndex = GetFirstChild(nCurIndex);
      }
   }
   while(nCurIndex >= 0);

   return nCurIndex;
}

int CCategoryDir::FindNext()
{
	int      nCurIndex, nCurLevel, nCurCategory;

	ASSERT(m_csaCategoryFields.GetSize() > 0);
	if(!m_bWildcardSearch || GetCurrentCategory() < 0 || m_nCurLevel < 0 ||
		m_csaCategoryFields.GetSize() <= 0)
		return ERRORCODE_Fail;

	// Depending on what category depth Find() got to, we may need
	// to recursively get current categories parent, grandparent until
	// we are at the proper starting level
	nCurLevel = m_nCurLevel - 1;
	if(nCurLevel < 0)
		return ERRORCODE_Fail;

	// Get parent of this Category so we can get parents sibling, then 
	// repeat category search at same level of this category except 
	// in a different family
	nCurCategory = GetCurrentCategory();
	while(nCurLevel != 0)
	{
		nCurCategory = GetParent(nCurCategory);
		nCurLevel--;
		ASSERT(nCurLevel >= 0);
		ASSERT(nCurCategory >= 0);
		if(nCurCategory < 0 || nCurLevel < 0)
			return ERRORCODE_Fail;
	}

	// Now get next category at this level (Sibling of top level)
	nCurCategory = GetNextSibling(nCurCategory);
	// This category may not have a next sibling, fail if none
	if(nCurCategory < 0)
		return ERRORCODE_Fail;

	// Now get children until level matches starting level (normally 1 for "All|Business")
	while(nCurLevel != m_nStartLevel && nCurLevel < m_nNumCurLevels)
	{
		nCurCategory = GetFirstChild(nCurCategory);
		nCurLevel++;
		ASSERT(nCurLevel >= 0);
		ASSERT(nCurCategory >= 0);
		if(nCurCategory < 0 || nCurLevel < 0 || nCurLevel >= m_nNumCurLevels)
			return ERRORCODE_Fail;
	}

	m_nCurLevel = nCurLevel;
	nCurIndex = nCurCategory;

	do
	{
		// Find category string at same level as current index
		nCurIndex = FindCategory(nCurIndex, m_csaCategoryFields[m_nCurLevel]);

		// if a match occurred, prepare to find category at next level
		if(nCurIndex >= 0)
		{
			// Bump current level
			m_nCurLevel++;
			// If we have matched all categories, return index of item
			if ((m_nCurLevel + 1) > m_nNumCurLevels)
			{
				m_nCurCategory = nCurIndex;
				return nCurIndex;
			}

			// Get 1st child of current item in preparation for next
			// category find
			nCurIndex = GetFirstChild(nCurIndex);
		}
	}
	while(nCurIndex >= 0);

	return ERRORCODE_Fail;
}


// Searches within a level for a category name 
int CCategoryDir::FindCategory(int nStartIndex, CString &strCategoryToFind)
{
	if (strCategoryToFind.IsEmpty())
	{
		ASSERT(0);
		return ERRORCODE_Fail;
	}

   int      i, nNextCategorySibling;
   CString  strCurCategoryName;

   for(i=nStartIndex;i < m_carrayCategories.GetSize();)
   {
      GetCategoryEntry(i)->GetName(&strCurCategoryName);
      // If found category, return item 
      if(strCategoryToFind.CompareNoCase(strCurCategoryName) == 0)
      {
         return i;   // Found it!
      }
      if(!m_bWildcardSearch)
         i = nNextCategorySibling = GetNextSibling(i);
      else
		{
//			int level = m_nCurLevel > 1 ? m_nCurLevel - 1 : m_nCurLevel;
         i = nNextCategorySibling = FindCategory(m_nCurLevel, i+1);
		}
      if(nNextCategorySibling <= 0)
         break;   // Failed
   }
   return ERRORCODE_Fail;
}

int CCategoryDir::FindCategory(int nLevelToFind, int nStartIndex)
{
   int      i;

   for(i=nStartIndex;i < m_carrayCategories.GetSize();i++)
   {
      if(GetCategoryEntry(i)->GetLevel() == nLevelToFind)
         return i;   // Found it!
   }
   return ERRORCODE_Fail;
}

// NOTE: this WILL NOT WORK for user collections! It assumes
// that item numbers are sequential in the category database,
// which is not the case for user collections.
// Use CCategoryDatabase::GetCategory() for user collections.
CString CCategoryDir::GetCategoryStrInReadOnly(DWORD dwItem)
{
	CString csCategory;

	CCategoryEntry* pCategory = FindInReadOnly(dwItem);

	if(pCategory)
	{
		// Recurse from lowest level category back up to top level
		// building full category name
		csCategory = pCategory->GetName();
		int nParent;
		while( (nParent = pCategory->GetParent()) >= 0)
		{
			pCategory = GetCategoryEntry(nParent);
			ASSERT(pCategory);
			if(pCategory == NULL)
				break;

			// Insert delimiter before category
			csCategory = DELIMITER_CHAR + csCategory;

			// Insert parent category name before delimiter
			csCategory = pCategory->GetName() + csCategory;
		}
	}
	return csCategory;
}

/////////////////////////////////////////////////////////////////////////////
// RenameCategory()
//
// Rename all entries with category "strOld" to "strNew".
/////////////////////////////////////////////////////////////////////////////
ERRORCODE CCategoryDir::RenameCategory(CString& strOld, CString& strNew)
{
	for (int nEntry = 0; nEntry < m_carrayCategories.GetSize(); nEntry++)
	{
		CCategoryEntry* pEntry = GetCategoryEntry(nEntry);
		if (pEntry == NULL)
			return ERRORCODE_Fail;

		CString strName = pEntry->GetName();
		if (strName == strOld)
		{
			pEntry->SetName(strNew);
			Update(nEntry, *pEntry);
		}
	}

	return ERRORCODE_None;
}

CString CCategoryDir::GetProductCode(DWORD dwItem)
{
   CCategoryEntry* pCategory = FindInReadOnly(dwItem);
   if(pCategory)
      return pCategory->GetProductCode();
   return "";
}

// This method assumes that item numbers are sequential in the
// category database (so DON'T USE for User collections)
CCategoryEntry* CCategoryDir::FindInReadOnly(DWORD dwItem)
{
	DWORD dwStartItem, dwEndItem;
   dwStartItem = dwEndItem = 1;
   for(int i = 0; i < m_carrayCategories.GetSize();i++)
   {
      CCategoryEntry* pCategory = GetCategoryEntry(i);
      ASSERT(pCategory);
      if(!pCategory)
         break;
      // Skip top level categories, look at sub-categories only
      if(pCategory->GetLevel() != (m_nNumLevels-1))
        continue;
      dwEndItem += (pCategory->GetItemCount() - 1);
      // Check if item is in this category
      if(dwItem >= dwStartItem && dwItem <= dwEndItem)
      {
         return pCategory;
      }
      dwStartItem += pCategory->GetItemCount();
      dwEndItem = dwStartItem;
   }

   return NULL;
}


DWORD CCategoryDir::GetItemCount(void)
   {
      DWORD             dwCount = 0;

      for(int i=0;i < m_carrayCategories.GetSize();i++)
         {
            CCategoryEntry* pCategory = GetCategoryEntry(i);
            if(pCategory->GetLevel())
              continue;
            dwCount += pCategory->GetItemCount();
         }

      return dwCount;
  }

int CCategoryDir::Dump(CStdioFile &refStdioDumpFile)
   {
      CString  strLine, strCategory;
      int      i;

      for(i=0;i<m_carrayCategories.GetSize();i++)
      {
			CCategoryEntry* pCat = GetCategoryEntry(i);
         strLine.Empty();
         pCat->GetName(&strCategory);
         strLine.Format("%2i Category: %16s NextSib: %3i 1stChild: %3i Parent: %3i Count: %5i Offset: %li Level: %i\n",
				i,
            strCategory.GetBuffer(1), 
            (int)pCat->GetNextSibling(),
            (int)pCat->GetFirstChild(),
            (int)pCat->GetParent(),
            (int)pCat->GetItemCount(),
            (long)pCat->GetOffsetToData(),
            (int)pCat->GetLevel());
         refStdioDumpFile.WriteString(strLine);
      }
      return ERRORCODE_None;
   }

/////////////////////////////////////////////////////////////////////////////
// CCategoryData methods
/////////////////////////////////////////////////////////////////////////////

int CCategoryData::Set(CString &strCategory,
   CString &strProductCode, CCategoryDataItem *pCDItem, DWORD dwFlags)
{
   // There should only be one Category data item after this member completes
   Empty();
   SetCategory(strCategory);
   SetProductCode(strProductCode);
   SetFlags(dwFlags);
   return AddItem(pCDItem);
}

void CCategoryData::Empty()
{
   m_strCategory.Empty();
   m_strProductCode.Empty();
   m_carrayItems.RemoveAll();
   m_dwFlags = 0;
}

//////////////////////////////////////////////////////////////////////////////
// DeleteItem()
//
// Remove an item from the array
//
//////////////////////////////////////////////////////////////////////////////
int CCategoryData::DeleteItem(DWORD dwItemToDelete)
{
	// find the right data to delete
	BOOL fFound = FALSE;
	for (int i = 0; i < (int)m_carrayItems.GetSize(); i++)
	{
		CCategoryDataItem* pData = (CCategoryDataItem*)m_carrayItems.GetAt(i);
		ASSERT(pData);
		if (pData->GetItemNumber() == dwItemToDelete)
		{
			m_carrayItems.RemoveAt(i);
			fFound = TRUE;
			break;
		}
	}
	if (!fFound)
	{
		ASSERT(0);	// didn't find the item for some reason
		return ERRORCODE_Fail;
	}

	Detach();						// Detach since data size will change
	m_carrayItems.FreeExtra();	// Reallocate memory space
	Attach(m_carrayItems.GetData(),
			 m_carrayItems.GetSize() * m_carrayItems.GetElementSize());

	return ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// CCategoryDatabase methods
/////////////////////////////////////////////////////////////////////////////

CCategoryDatabase::CCategoryDatabase(StorageDevice *pCFileData)
: m_Data(pCFileData)
{
   m_pCategoryData = NULL;
   m_nCategoryIndex = -1;
   m_bReadItems = FALSE;
   m_bOkToFindNext = FALSE;
}

CCategoryDatabase::~CCategoryDatabase()
{
}

//
// Find and Read the category from the category DB
// (Note: this didn't use to read, but does now - hforman)
//
int CCategoryDatabase::FindFirst(CString &strCategory, CCategoryData *pCategoryData)
{
   int      nRetVal;
   DWORD    dwItemCount;

   // Make a copy of callers string since we might modify it
   CString strCategoryToFind = strCategory;

   m_pCategoryData = NULL;
   m_nCategoryIndex = -1;
   m_bReadItems = FALSE;
   m_bOkToFindNext = FALSE;

   // If user is supplies "All" for category, then get all items
   BOOL bGetAllItems = FALSE;
	CString	csSubCategoryAll = CString('|') + BrowserAllString();
   if(strCategoryToFind.CompareNoCase(BrowserAllString()) == 0)
      bGetAllItems = TRUE;

   // Search for "|All" in category string, if a match then
   // strip off trailing "|All" which indicates search should
   // find all items in this subcategory
	//
	// NOTE: this is currently never the case -- the trailing "All" doesn't
	// get appended to the category string (hforman)
	int nIndex;
   if( (nIndex = strCategoryToFind.Find(csSubCategoryAll)) >= 0)
   {
      strCategoryToFind.SetAt(nIndex, 0);
      // Compute new length            
      strCategoryToFind.ReleaseBuffer();
   }

   pCategoryData->SetCategory(strCategoryToFind);
   if (!bGetAllItems)
   {
      m_nCategoryIndex = m_Dir.FindFirst(strCategoryToFind);
      if(m_nCategoryIndex < 0)
         return ERRORCODE_Fail;

      // Get Category Data
      nRetVal = m_Dir.GetItem(m_nCategoryIndex, &m_categoryItem);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;

      dwItemCount = m_categoryItem.GetItemCount();
      m_bOkToFindNext = TRUE;
   }
   else // get all
   {
      // Get item count for entire database
		dwItemCount = m_Dir.GetItemCount();
      m_nCategoryIndex = 0;

      // Get 1st Category for product code info
      nRetVal = m_Dir.GetItem(0, &m_categoryItem);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;
   }

   pCategoryData->SetProductCode(m_categoryItem.GetProductCode());
   pCategoryData->SetFlags(m_categoryItem.GetFlags());

   // Grow huge item array to match size return by category directory find
   pCategoryData->m_carrayItems.SetSize(dwItemCount);

	DWORD dwElementSize = pCategoryData->m_carrayItems.GetElementSize();

	if (bGetAllItems)
	{
		// We need to step through the directory to get offsets to the data.
		// We do this for each top-level entry
		int nTotalCount = 0;
		int nNextSibling = 0;
		while (nNextSibling >= 0)
		{
			CCategoryEntry* pEntry = m_Dir.GetCategoryEntry(nNextSibling);
			if (pEntry == NULL)
			{
				ASSERT(0);
				return ERRORCODE_Fail;
			}
			ASSERT(pEntry->GetLevel() == 0); // sanity

			DWORD dwOffset = pEntry->GetOffsetToData();
			int nCount = pEntry->GetItemCount();

			// Attach piece of array to data object
			pCategoryData->Attach(pCategoryData->m_carrayItems.ElementAt(nTotalCount),
										 nCount * dwElementSize);

			m_Data.Find(dwOffset, pCategoryData);
			m_Data.Read();

			nTotalCount += nCount;
			nNextSibling = pEntry->GetNextSibling();
		}
	}
	else
	{
		// Attach array of items to data object
		pCategoryData->Attach(pCategoryData->m_carrayItems.GetData(),
									 pCategoryData->m_carrayItems.GetSize() * dwElementSize);

		DWORD dwOffsetToData = m_categoryItem.GetOffsetToData();
		m_Data.Find(dwOffsetToData, pCategoryData);
		Read();
	}

   m_pCategoryData = pCategoryData;

   return ERRORCODE_None;
}

int CCategoryDatabase::FindNext(CCategoryData *pCategoryData)
{
   int            nRetVal;
   DWORD          dwItemCount, dwOffsetToData;
   CString        strCategoryToFind, csSubCategoryAll = "|";

   // We may get called when FirstFirst was called with "All"
   if(!m_bOkToFindNext)
      return ERRORCODE_Fail;

   m_pCategoryData = NULL;
   m_nCategoryIndex = -1;
   m_bReadItems = FALSE;

//      pCategoryData->SetCategory();
   m_nCategoryIndex = m_Dir.FindNext();
   if(m_nCategoryIndex < 0)
      return ERRORCODE_Fail;

   // Get Category Data
   nRetVal = m_Dir.GetItem(m_nCategoryIndex, &m_categoryItem);
   if(nRetVal != ERRORCODE_None)
      return ERRORCODE_Fail;
   dwItemCount = m_categoryItem.GetItemCount();
   dwOffsetToData = m_categoryItem.GetOffsetToData();

   pCategoryData->SetProductCode(m_categoryItem.GetProductCode());
   pCategoryData->SetFlags(m_categoryItem.GetFlags());

   // Grow huge item array to match size return by category directory find
   pCategoryData->m_carrayItems.SetSize(dwItemCount);

   // Attach huge array of items to data object
   pCategoryData->Attach(pCategoryData->m_carrayItems.GetData(),
      pCategoryData->m_carrayItems.GetSize() *
      pCategoryData->m_carrayItems.GetElementSize());

   m_Data.Find(dwOffsetToData, pCategoryData);
	Read();

   m_pCategoryData = pCategoryData;

   return ERRORCODE_None;
}

int CCategoryDatabase::Read(DWORD dwBytesToRead)
{
   int      nRetVal;

   nRetVal = m_Data.Read(dwBytesToRead);
   if(nRetVal == ERRORCODE_None)
      m_bReadItems = TRUE;
   return nRetVal;
}

CString CCategoryDatabase::GetCategory(DWORD dwItem, BOOL fUserColl)
{
	CString strCat;

	if (fUserColl)
	{
		int nEntry = GetCatEntryNumFromItemNum(dwItem);
		if (nEntry >= 0)
		{
			CCategoryEntry* pCatEntry = m_Dir.GetCategoryEntry(nEntry);
			if (pCatEntry)
			{
				// See if this is a child entry. If so, create the full category string
				if (pCatEntry->GetParent() >= 0)
				{
					CCategoryEntry* pParent = m_Dir.GetCategoryEntry(pCatEntry->GetParent());
					strCat = pParent->GetName();
							strCat += DELIMITER_CHAR;
					strCat += pCatEntry->GetName();
				}
				else
				{
					strCat = pCatEntry->GetName();
				}
			}
		}

		return strCat;
	}
	else
	{
		return m_Dir.GetCategoryStrInReadOnly(dwItem);
	}
}

//////////////////////////////////////////////////////////////////////////////
// AddToExisting()
//
// Add an item to an existing category. We must read/write the entire
// "family" (supercategory/categories) to new location in DB.
//
// Params:	nItemEntry - index of category entry we're adding to
//				pCategoryData - data we're adding
//
//////////////////////////////////////////////////////////////////////////////
int CCategoryDatabase::AddToExisting(int nItemEntry, CCategoryData *pCategoryData)
{
	if (nItemEntry < 0 || pCategoryData == NULL)
	{
		ASSERT(0);
		return ERRORCODE_BadParameter;
	}

	int nRetVal = ERRORCODE_None;
	CCategoryData newCatData;

	CCategoryEntry* pItemEntry = m_Dir.GetCategoryEntry(nItemEntry);
	if (pItemEntry == NULL)
		return ERRORCODE_Fail;

	// 0-based child position of entry relative to parent
	int nItemPosInFamily = 0;

	BOOL fHasParent = FALSE;
	CCategoryEntry* pParentEntry = NULL;
	int nParent = -1;
	if ((nParent = pItemEntry->GetParent()) != -1)
	{
		fHasParent = TRUE;

		pParentEntry = m_Dir.GetCategoryEntry(nParent);
		if (pParentEntry == NULL)
			return ERRORCODE_Fail;

		// do FindFirst() at parent level
		CString strParent = pParentEntry->GetName();
		FindFirst(strParent, &newCatData);

		// Insert new item into data array. We read items until we find our category,
		// then insert at end.
		int nChild = pParentEntry->GetFirstChild();
		int nItemIndex = 0;
		while (nChild >= 0)
		{
			CCategoryEntry* pCurEntry = m_Dir.GetCategoryEntry(nChild);
			if (pCurEntry == NULL)
				return ERRORCODE_Fail;

			nItemIndex += pCurEntry->GetItemCount();
			if (nChild == nItemEntry)
			{
				newCatData.m_carrayItems.InsertAt(nItemIndex, pCategoryData->m_carrayItems[0]);
				break;
			}

			nChild = pCurEntry->GetNextSibling();
		}
	}
	else
	{
		// no parent -- just read the entry
		CString strCat = pItemEntry->GetName();
		FindFirst(strCat, &newCatData);
		newCatData.m_carrayItems.Add(pCategoryData->m_carrayItems[0]);
	}

	// Delete old data
	nRetVal = m_Data.Delete();
	if(nRetVal != ERRORCODE_None)
	{
		ASSERT(0);
		return ERRORCODE_Fail;
	}

	// and Add new
	newCatData.Attach(newCatData.m_carrayItems.GetData(),
							newCatData.m_carrayItems.GetSize() * newCatData.m_carrayItems.GetElementSize());
	DWORD dwNewOffset = m_Data.Add(&newCatData);

	// update item's count
	pItemEntry->SetItemCount(pItemEntry->GetItemCount() + 1);

	if (fHasParent)
	{
		// update parent entry
		pParentEntry->SetItemCount(pParentEntry->GetItemCount() + 1);
		pParentEntry->SetOffsetToData(dwNewOffset);
		nRetVal = m_Dir.Update(nParent, *pParentEntry);

		// also update all children offsets
		m_Dir.UpdateChildOffsets(nParent, dwNewOffset);
	}
	else
	{
		pItemEntry->SetOffsetToData(dwNewOffset);
		m_Dir.Update(nItemEntry, *pItemEntry);
	}

#ifdef _DEBUG
//	CString path = GetGlobalPathManager()->ExpandPath("[[U]]\\");
//	path += "AfterAddToExisting.txt";
//	CStdioFile dumpFile(path, CFile::modeCreate|CFile::modeWrite);
//	m_Dir.Dump(dumpFile);
#endif

	return nRetVal;
}

//////////////////////////////////////////////////////////////////////////////
// InsertSubcategory()
//
// Insert a new subcategory into an existing parent category. All items in
// the directory below the inserted item must be adjusted.
// NOTE: Do not call if only one level! If that's the case, just do AddToEnd()
//
// Params:	nParentIndex - index of parent category entry we're adding to
//				pCategoryData - data we're adding
//
//////////////////////////////////////////////////////////////////////////////

int CCategoryDatabase::InsertSubcategory(int nParentEntry, CCategoryData* pCategoryData)
{
	// check that our params are good, and that there's at least 1 child of parent
	if (nParentEntry < 0 || pCategoryData == NULL ||
		 m_Dir.GetCategoryEntry(nParentEntry)->GetFirstChild() < 0)
	{
		ASSERT(0);
		return ERRORCODE_BadParameter;
	}

	//
	// Update the data file, adding a new entry and rewriting
	// the entire category group to a new location so it's contiguous
	//

	// Read in the category "family" (parent and all children)
	CCategoryData newCatData;
	CString strParent = m_Dir.GetCategoryEntry(nParentEntry)->GetName();
	FindFirst(strParent, &newCatData);

	// add the new data item
	newCatData.m_carrayItems.Add(pCategoryData->m_carrayItems[0]);

	// Delete old data
	if(m_Data.Delete() != ERRORCODE_None)
	{
		ASSERT(0);
		return ERRORCODE_Fail;
	}

	// and Add new
	newCatData.Attach(newCatData.m_carrayItems.GetData(),
							newCatData.m_carrayItems.GetSize() * newCatData.m_carrayItems.GetElementSize());
	DWORD dwNewOffset = m_Data.Add(&newCatData);

	//
	// Now update the directory...
	//

	// Update parent info
	CCategoryEntry* pParentEntry = m_Dir.GetCategoryEntry(nParentEntry);
	if (pParentEntry->GetNextSibling() != -1)
		pParentEntry->SetNextSibling(pParentEntry->GetNextSibling() + 1);
	pParentEntry->SetItemCount(pParentEntry->GetItemCount() + 1);
	pParentEntry->SetOffsetToData(dwNewOffset);
	m_Dir.Update(nParentEntry, *pParentEntry);

	// Update the offsets of all the current children
	m_Dir.UpdateChildOffsets(nParentEntry, dwNewOffset);

	// Figure out where we're going to do the insert. We want to be the last child.
	int nChild = m_Dir.GetCategoryEntry(nParentEntry)->GetFirstChild();
	ASSERT(nChild >= 0);	// better be at least 1 child
	int nInsertPos = nChild + 1;
	while ((nChild = m_Dir.GetNextSibling(nChild)) >= 0)
		nInsertPos++;

	// Update sibling value for last leaf, since it's going to have a sibling now
	CCategoryEntry* pLastChild = m_Dir.GetCategoryEntry(nInsertPos - 1);
	pLastChild->SetNextSibling(nInsertPos);
	m_Dir.Update(nInsertPos - 1, *pLastChild);

	// build the new entry
	CCategoryEntry newEntry;

	CMlsStringArray catstrs(pCategoryData->GetCategory());
	newEntry.SetName(catstrs[1]);

	newEntry.SetLicenseInfo(pCategoryData->GetProductCode());
	newEntry.SetFlags(pCategoryData->GetFlags());
	newEntry.SetParent(nParentEntry);
	newEntry.SetFirstChild(-1);
	newEntry.SetNextSibling(-1);
	newEntry.SetItemCount(1);
	int nCount = pParentEntry->GetItemCount() - 1;
	newEntry.SetOffsetToData(dwNewOffset + nCount * sizeof(CCategoryDataItem));
	newEntry.SetLevel(1);
	// and insert it
	m_Dir.InsertEntry(nInsertPos, newEntry);

	// Now update the sibling/child/parent entries of every item
	// below the new entry, since everything has shifted by 1
	m_Dir.UpdatePositions(nInsertPos + 1, 1);

#ifdef _DEBUG
//	CString path = GetGlobalPathManager()->ExpandPath("[[U]]\\");
//	path += "AfterInsert.txt";
//	CStdioFile dumpFile(path, CFile::modeCreate|CFile::modeWrite);
//	m_Dir.Dump(dumpFile);
#endif

	return ERRORCODE_None;
}

int CCategoryDatabase::AddToEnd(CCategoryData *pCategoryData)
{
   // Add data to file and get offset to data
	pCategoryData->Attach(pCategoryData->m_carrayItems.GetData(),
								 pCategoryData->m_carrayItems.GetSize() *
										pCategoryData->m_carrayItems.GetElementSize());
	DWORD dwItemOffset = m_Data.Add(pCategoryData);
	if (dwItemOffset < 0)
	{
		// error occurred
		return (int)dwItemOffset;
	}

	// If Data item successfully added, then add/update directory entry
	return m_Dir.Add(pCategoryData->GetCategory(),
						  pCategoryData->GetProductCode(), pCategoryData->GetFlags(),
						  dwItemOffset);
}

int CCategoryDatabase::Add(CCategoryData *pCategoryData, BOOL fUserCollection)
{
#ifdef _DEBUG
//	CString path = GetGlobalPathManager()->ExpandPath("[[U]]\\");
//	path += "CatDirBeforeAdd.txt";
//	CStdioFile dumpFile(path, CFile::modeCreate|CFile::modeWrite);
//	m_Dir.Dump(dumpFile);
#endif

	// set current category to none when adding
	m_nCategoryIndex = -1;

	// Non-user collections should always use AddToEnd(), since
	// everything must be in sequential order.
	if (!fUserCollection)
		return AddToEnd(pCategoryData);

	// For User collections, see if we're adding items into existing category, etc.

	CMlsStringArray CategoryArray(pCategoryData->GetCategory());
	int nLevels = CategoryArray.GetSize();
	if (nLevels == 0)
	{
		ASSERT(0);
		return ERRORCODE_BadParameter;
	}

	// First check full category. If found, and this is a user collection,
	// we add this item to existing ones.
	int nIndex = m_Dir.FindFirst(pCategoryData->GetCategory());
	if (nIndex >= 0)
	{
		return AddToExisting(nIndex, pCategoryData);
	}

	// Next see if we have a "supercategory" that we're adding a new subcategory to
	if (nLevels == 2)
	{
		int nParent = m_Dir.FindFirst(CategoryArray[0]);

		// If this item's parent (supercategory) is not the last one in the
		// directory, we need to do an insert. Otherwise, fall through.
		if (nParent >= 0)// && m_Dir.GetCategoryEntry(nParent)->GetNextSibling() >= 0)
		{
			return InsertSubcategory(nParent, pCategoryData);
		}
	}

	// We haven't found existing items of this type, or the existing items
	// are at the end of the list -- add this item to end of DB
	return AddToEnd(pCategoryData);
}

int CCategoryDatabase::Delete(DWORD dwItemToDelete)
{
#ifdef _DEBUG
{
//	CString path = GetGlobalPathManager()->ExpandPath("[[U]]\\");
//	path += "CatDirBeforeDelete.txt";
//	CStdioFile dumpFile(path, CFile::modeCreate|CFile::modeWrite);
//	m_Dir.Dump(dumpFile);
}
#endif

	int nRetVal = ERRORCODE_None;

	// Get the directory entry of the item we're trying to delete.
	CCategoryEntry* pItemEntry;
	int nItemEntry = GetCatEntryNumFromItemNum(dwItemToDelete);
	if (nItemEntry >= 0)
	{
		pItemEntry = m_Dir.GetCategoryEntry(nItemEntry);
		if (pItemEntry == NULL)
		{
			ASSERT(0);
			return ERRORCODE_Fail;
		}
	}

	// Read in the DB data into memory, once we figure out how much to read
	CCategoryData catData;

	BOOL fHasParent = FALSE;
	int nParentEntry = pItemEntry->GetParent();
	if (nParentEntry >= 0)
	{
		fHasParent = TRUE;

		// If item has a parent, we need to read the entire family in,
		// since we will have to write out this whole chunk after the delete.
		CCategoryEntry* pParent = m_Dir.GetCategoryEntry(nParentEntry);
		if (pParent == NULL)
		{
			ASSERT(0);
			return ERRORCODE_Fail;
		}

		catData.m_carrayItems.SetSize(pParent->GetItemCount());
		catData.Attach(catData.m_carrayItems.GetData(),
							catData.m_carrayItems.GetSize() *
								catData.m_carrayItems.GetElementSize());

		m_Data.Find(pParent->GetOffsetToData(), &catData);
	}
	else
	{
		// no parent - just read in single category entry
		catData.m_carrayItems.SetSize(pItemEntry->GetItemCount());
		catData.Attach(catData.m_carrayItems.GetData(),
							catData.m_carrayItems.GetSize() *
								catData.m_carrayItems.GetElementSize());

		m_Data.Find(pItemEntry->GetOffsetToData(), &catData);
	}

	m_Data.Read(m_Data.GetItemSize());

	if (pItemEntry->GetItemCount() == 1)
	{
		// We're deleting the last item in this category.

		// First delete the data in the datafile
		nRetVal = m_Data.Delete();
		ASSERT(nRetVal == ERRORCODE_None);

		// See if we have siblings to worry about (before we delete the entry)
		BOOL fHasSiblings =
			( fHasParent &&
			 (m_Dir.GetNextSibling(nItemEntry) >= 0 || m_Dir.GetPrevSibling(nItemEntry) >= 0) );

		// Now delete the category entry in the directory
		m_Dir.DeleteEntry(nItemEntry);

		if (fHasSiblings)
		{
			// If there are other siblings, we have to write them back out after we
			// delete the data item.
			if (catData.DeleteItem(dwItemToDelete) != ERRORCODE_None)
				return ERRORCODE_Fail;

			DWORD dwNewOffset = m_Data.Add(&catData);

			// Update parent entry
			CCategoryEntry* pParent = m_Dir.GetCategoryEntry(nParentEntry);
			pParent->SetItemCount(pParent->GetItemCount() - 1);
			pParent->SetNextSibling(pParent->GetNextSibling() - 1);
			pParent->SetOffsetToData(dwNewOffset);
			nRetVal = m_Dir.Update(nParentEntry, *pParent);

			// Update all other children offsets of deleted item's parent
			m_Dir.UpdateChildOffsets(nParentEntry, dwNewOffset);
		}
	}
	else // not last item in this category
	{
		// Delete old data (it's either a single entry or full family)
		nRetVal = m_Data.Delete();
		ASSERT(nRetVal == ERRORCODE_None);

		// Remove the item from the array
		if (catData.DeleteItem(dwItemToDelete) != ERRORCODE_None)
			return ERRORCODE_Fail;

		// Write out the block (minus the deleted item) to new location
		DWORD dwNewOffset = m_Data.Add(&catData);

		// 
		// Now update the Directory...
		//

		pItemEntry->SetItemCount(pItemEntry->GetItemCount() - 1);

		if (fHasParent)
		{
			// update parent entry
			CCategoryEntry* pParent = m_Dir.GetCategoryEntry(nParentEntry);
			pParent->SetItemCount(pParent->GetItemCount() - 1);
			pParent->SetOffsetToData(dwNewOffset);
			nRetVal = m_Dir.Update(nParentEntry, *pParent);

			// also update all children offsets
			m_Dir.UpdateChildOffsets(nParentEntry, dwNewOffset);
		}
		else
		{
			// no other kids to worry about
			pItemEntry->SetOffsetToData(dwNewOffset);
			m_Dir.Update(nItemEntry, *pItemEntry);
		}
	}

#ifdef _DEBUG
{
//	CString path = GetGlobalPathManager()->ExpandPath("[[U]]\\");
//	path += "CatDirAfterDelete.txt";
//	CStdioFile dumpFile(path, CFile::modeCreate|CFile::modeWrite);
//	m_Dir.Dump(dumpFile);
}
#endif

   return ERRORCODE_None;
}

// GetCategoryDataItem retrieves item specific information stored in the Category database
// such as Tone and Custom flags 
ERRORCODE CCategoryDatabase::GetCategoryDataItem(DWORD dwItem, CCategoryDataItem *pCatDataItem)
{
   CDataFileEntry cdfeFileData;
   DWORD          dwOffsetToData;
   ERRORCODE      errorcode;

   dwOffsetToData = (dwItem-1) * sizeof(CCategoryDataItem);
   cdfeFileData.Attach(pCatDataItem, sizeof(CCategoryDataItem));
   errorcode = (ERRORCODE) m_Data.Find(dwOffsetToData, &cdfeFileData);
   if(errorcode != ERRORCODE_None)
      return errorcode;
   errorcode = (ERRORCODE)m_Data.Read();
   cdfeFileData.Detach();

   return ERRORCODE_None;
}

int CCategoryDatabase::GetCatEntryNumFromItemNum(DWORD dwItem)
{
	// Since user collection categories are not necessarily sequential,
	// we need to read in the database and look for the item number, then
	// find the category in the directory.
	int nCatEntries = m_Dir.GetCategoryCount();
	for (int nEntry = 0; nEntry < nCatEntries; nEntry++)
	{
		CCategoryEntry* pCatEntry = m_Dir.GetCategoryEntry(nEntry);
		if (pCatEntry->GetParent() != -1)
			continue;	// only look at top level

		DWORD dwOffset = pCatEntry->GetOffsetToData();
		DWORD dwCount = pCatEntry->GetItemCount();

		CCategoryData catData;
		catData.m_carrayItems.SetSize(dwCount);
		catData.Attach(catData.m_carrayItems.GetData(),
							catData.m_carrayItems.GetSize() *
								catData.m_carrayItems.GetElementSize());

		m_Data.Find(dwOffset, &catData);
		m_Data.Read(m_Data.GetItemSize());

		for (DWORD dwItemInGroup = 0; dwItemInGroup < catData.m_carrayItems.GetSize(); dwItemInGroup++)
		{
			CCategoryDataItem* pCatItem;
			pCatItem = (CCategoryDataItem*)catData.m_carrayItems.GetAt(dwItemInGroup);
			ASSERT(pCatItem);
			if (pCatItem->GetItemNumber() == dwItem)
			{
				// Found it!

				// If the entry has no children, we're done
				int nChild = pCatEntry->GetFirstChild();
				if (nChild == -1)
					return nEntry;

				// Otherwise, we need to find the right child entry
				dwItemInGroup++;	// since we have to start at 1
				DWORD dwStartItem, dwEndItem;
				dwStartItem = dwEndItem = 1;
				while (nChild != -1)
				{
					CCategoryEntry* pChildEntry = m_Dir.GetCategoryEntry(nChild);
					ASSERT(pChildEntry);
					if(!pChildEntry)
						break;

					dwEndItem += (pChildEntry->GetItemCount() - 1);

					// Check if item is in this category
					if(dwItemInGroup >= dwStartItem && dwItemInGroup <= dwEndItem)
					{
						// Got the child entry
						return nChild;
					}

					dwStartItem += pChildEntry->GetItemCount();
					dwEndItem = dwStartItem;
					nChild = pChildEntry->GetNextSibling();
				}
			}
		}
	}

	return ERRORCODE_Fail;
}

