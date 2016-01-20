#include "stdafx.h"
#include "srchart.h"
#include "inifile.h"

////////////////////////////////////////////////////////////////////
// CSearchArtCollectionItem Methods

CSearchArtCollectionItem::CSearchArtCollectionItem()
   {
   }

void CSearchArtCollectionItem::SetItem(LPCSTR szBaseName, int nSubDirs/*=0*/)
   {
      m_csBaseName = szBaseName;
      m_nSubDirs = nSubDirs;
   }

////////////////////////////////////////////////////////////////////
// CSearchArtCollections Methods
CSearchArtCollections::CSearchArtCollections(LPCSTR pszIniName)
   {
		m_pszIniName = pszIniName;
   }

CSearchArtCollections::~CSearchArtCollections()
   {
      RemoveAll();
   }

void CSearchArtCollections::RemoveAll()
   {
      int   nArraySize = m_cpaCollItem.GetSize();
      const CSearchArtCollectionItem *pItem;

      // Run through list and free objects
      for(int i=0;i < nArraySize;i++)
         {
            pItem = m_cpaCollItem.GetAt(i);
            delete pItem;
         }
      m_cpaCollItem.RemoveAll();
   }

void CSearchArtCollections::Reset()
   {
      RemoveAll();

      // These are always searched
      Add("PM7ART");
   }

ERRORCODE CSearchArtCollections::Add(LPCSTR szBaseName, int nNumSubDirs/*=0*/)
   {
      CSearchArtCollectionItem *pItem;

      // Don't allow duplicates
      if(Find(szBaseName) >= 0)
         return ERRORCODE_Exists;

      pItem = new CSearchArtCollectionItem;
      ASSERT(pItem);
      // Fill Item Item
      pItem->SetItem(szBaseName, nNumSubDirs);
      // Add it to typed pointer array
      m_cpaCollItem.Add(pItem);

      CIniFile    IniFile(m_pszIniName);
      IniFile.WriteString("Search Art", szBaseName, "");

      return ERRORCODE_None;
   }

int CSearchArtCollections::Find(LPCSTR szBaseName)
   {
      int   nArraySize = m_cpaCollItem.GetSize();
      int   nItemIndex = -1;
      const CSearchArtCollectionItem *pItem;

      // Run through list searching for match
      for(int i=0;i < nArraySize && nItemIndex < 0;i++)
         {
            pItem = m_cpaCollItem.GetAt(i);
            if(pItem)
               {
                  if((pItem->GetBaseName()).CompareNoCase(szBaseName) == 0)
                     nItemIndex = i;
               }
         }
      return nItemIndex;
   }

ERRORCODE CSearchArtCollections::Update()
   {
      Reset();

      CIniFile    IniFile(m_pszIniName);

		// Get the list of Collection Base Names to 
		LPSTR pSections = IniFile.GetSectionKeys("Search Art");
      if(pSections == NULL)
		{
			// No keys. Nothing to do.
         return ERRORCODE_None;
		}

		for (LPCSTR pEntry = pSections;
         *pEntry != '\0';
         pEntry += strlen(pEntry)+1)
		{
         Add(pEntry);
		}
		delete [] pSections;

      return ERRORCODE_None;
   }

