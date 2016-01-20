//
// $Workfile: FREELIST.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1999 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/FREELIST.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 3     3/01/99 3:29p Johno
// 
// 2     3/01/99 10:41a Johno
// Added log
// 

#include "stdafx.h"
#include "freelist.h"
#include "cindex.h"

#define SORTED_BY_OFFSET_INDEX   15

CFreeLists::CFreeLists()
   {
      m_bLoadedList = FALSE;
      m_pIndexFile = NULL;
   }

CFreeLists::~CFreeLists()
   {
   }

void CFreeLists::GetIndexList(CCIndexIndexList *pIndexList)
   {
      int      i;

      ASSERT(pIndexList);

      for(i=1;i <= MAX_CCINDEX_INDEXES;i++)
         pIndexList->SetIndexType(i, CCIndexIndexList::longKey);
      pIndexList->SetIndexType(SORTED_BY_OFFSET_INDEX, CCIndexIndexList::custom1Key);
   }

int CFreeLists::Init(CCIndexFile *pFreeListFile)
   {
      m_bLoadedList = FALSE;
      m_pIndexFile = pFreeListFile;

      return ERRORCODE_None;
   }

SHORT CFreeLists::CreateList(CString strFileName)
   {
      SHORT    nNewEntry;
      CString  strShortFileName;
      SHORT    nHandle;

      strShortFileName = GetShortFileName(strFileName);
      LoadFileList();
      nHandle = FindList(strShortFileName);
      if(nHandle >= 0)
         return nHandle;

      // Assert if trying to create list that already exists
      ASSERT(nHandle < 0);

      nNewEntry = m_FileList.AddEntry(strShortFileName) + 1;
      SaveFileList();
      return nNewEntry;
   }

SHORT CFreeLists::FindList(CString strFileName)
   {
      SHORT    nCurFile;
      CString  strShortFileName;

      LoadFileList();
      strShortFileName = GetShortFileName(strFileName);
      for(nCurFile=0;nCurFile < m_FileList.GetNumFiles();nCurFile++)
         {
            if(m_FileList.GetFileName(nCurFile).CompareNoCase(strShortFileName)==0)
               return nCurFile + 1;   // Match
         }
      return -1;
   }

int CFreeLists::FindBlock(SHORT nHandle, DWORD dwBlockSize,
   DWORD * pdwFoundOffset)
   {
      int         nFindRetVal, nRetVal;
      DWORD       dwCurrentBlockSize, dwOffset, dwNewBlockSize, dwNewOffset;
      COffsetKey  offsetKey;

      ASSERT(pdwFoundOffset);
      if(pdwFoundOffset == NULL)
         return ERRORCODE_BadParameter;

      nFindRetVal = m_pIndexFile->Find(
         dwBlockSize,   // Key is block size
         NULL, 0,       // No data associated with key
         0,             // Don't know offset (record #) so supply 0
         nHandle);      // Index file ID
      if(nFindRetVal != CCIndexFile::success && nFindRetVal != CCIndexFile::keyMatch
         && nFindRetVal != CCIndexFile::keyGreaterFound)
         return ERRORCODE_Fail;

      nRetVal = m_pIndexFile->GetCurrentKey(&dwCurrentBlockSize);
      // We should be able to get current key since find was successful
      if(nRetVal != CCIndexFile::success)
         CCIndexFileException::Throw(nRetVal);

      dwOffset = m_pIndexFile->GetRecordNum();
      *pdwFoundOffset = dwOffset;

      // Delete keys thus freeing up block for client to use
      nRetVal = DeleteKeys(
         nHandle,
         dwCurrentBlockSize,
         dwOffset,
         FALSE,            // Find block key
         TRUE);            // Find offset key
      if(nRetVal != CCIndexFile::success)
         CCIndexFileException::Throw(nRetVal);

      // If requested block size doesn't exactly match requested (Most cases), 
      // Use it anyway by breaking up into 2 blocks with
      // 1st block matching requested size and second block is remainder
      // 1st block is marked as free
      if(nFindRetVal == CCIndexFile::keyGreaterFound)
         {
            dwNewBlockSize = dwCurrentBlockSize - dwBlockSize;
            dwNewOffset = dwOffset + dwBlockSize;
            nRetVal = AddBlock(nHandle, dwNewBlockSize, dwNewOffset);
            // Should always be able to add
            ASSERT(nRetVal == ERRORCODE_None);
         }
      return ERRORCODE_None;
   }

int CFreeLists::AddBlock(SHORT nHandle, DWORD dwBlockSize, DWORD dwOffset)
{
   int      nRetVal;
   BOOL     bPrevBlockMatch=FALSE, bNearMatchBlock=FALSE;
   DWORD    dwPrevOffset, dwPrevBlockSize, dwNearOffset, dwNearBlockSize;
   DWORD    dwNextBlockOffset,dwPrevBlockOffset;
   DWORD    dwNewBlockSize=dwBlockSize, dwNewOffset=dwOffset;
   COffsetKey  offsetKey;
   // Try to locate the nearest block to the one we are about to add
   // Check if nearest match has a block that is sequential 
   // to block we are about to add
   offsetKey.SetOffset(dwOffset);
   offsetKey.SetID(nHandle);
   // Find() will locate keys containing the next positional item
   // when an exact match doesn't occur
   nRetVal = m_pIndexFile->Find(
      &offsetKey,    // Special user defined key
      NULL, 0,       // No data associated with key
      0,             // Don't know Block Size (record #) so supply 0
      SORTED_BY_OFFSET_INDEX);      // Index file ID
   if(nRetVal == CCIndexFile::success || nRetVal == CCIndexFile::keyMatch
      || nRetVal == CCIndexFile::keyGreaterFound)
   {
      // Found OFFSET key, check if sequential
      nRetVal = m_pIndexFile->GetCurrentKey((LPVOID)&offsetKey);
      // We should be able to get current key since find was successful
      if(nRetVal != CCIndexFile::success)
         CCIndexFileException::Throw(nRetVal);

      dwNearBlockSize = m_pIndexFile->GetRecordNum();
      // What is the next block of the block we are about to add
      dwNextBlockOffset = dwOffset + dwBlockSize;
      // Does the beginning position of the next block of the block
      // we are about to add match the beginning position of the
      // nearest match 
      dwNearOffset = offsetKey.GetOffset();
      if(dwNextBlockOffset == dwNearOffset)
      {
         bNearMatchBlock = TRUE;
         // Block size grows, but Offset stays the same 
         dwNewBlockSize += dwNearBlockSize;
         // Delete near match key since we're merging blocks
         // We should be able to delete current offset key 
         // since find was successful
         nRetVal = DeleteKeys(
            nHandle,
            dwNearBlockSize,
            dwNearOffset,
            TRUE,             // Find block key
            FALSE);           // Find offset key
         if(nRetVal != CCIndexFile::success)
            CCIndexFileException::Throw(nRetVal);
      }
      // Get previous index entry of near match which represents
      // previous free block of near match
      nRetVal = m_pIndexFile->GetPrevEntry(NULL, NULL, SORTED_BY_OFFSET_INDEX);
      if(nRetVal == CCIndexFile::success)
      {
         nRetVal = m_pIndexFile->GetCurrentKey((LPVOID)&offsetKey);
         // We should be able to get current key since find was successful
         if(nRetVal != CCIndexFile::success)
            CCIndexFileException::Throw(nRetVal);
         dwPrevBlockSize = m_pIndexFile->GetRecordNum();
         // What is the previous blocks next block
         dwPrevOffset = offsetKey.GetOffset();
         dwPrevBlockOffset = dwPrevOffset + dwPrevBlockSize;
         // Does its position match the one we are about to add?
         // (and ID matches)
         if(dwPrevBlockOffset == dwOffset &&
            offsetKey.GetID() == nHandle)
         {
            // It matched, delete it since we are going
            // to merge free blocks
            bPrevBlockMatch = TRUE;
            dwNewBlockSize += dwPrevBlockSize;
            dwNewOffset    -= dwPrevBlockSize;
            // Should Match prev offset
            ASSERT(dwNewOffset == dwPrevOffset);
            // Delete prev offset key since we're merging blocks
            nRetVal = DeleteKeys(
               nHandle,
               dwPrevBlockSize,
               dwPrevOffset,
               TRUE,             // Find block key
               FALSE);           // Find offset key
            if(nRetVal != CCIndexFile::success)
               CCIndexFileException::Throw(nRetVal);
         }
      }
   }
   else
   {
      // Since no greater than key was found, get last and check
      // if it is sequentially positioned before new block
      nRetVal = m_pIndexFile->GetLastEntry(NULL, NULL, SORTED_BY_OFFSET_INDEX);
      if(nRetVal == CCIndexFile::success)
      {
         nRetVal = m_pIndexFile->GetCurrentKey((LPVOID)&offsetKey);
         // We should be able to get current key since find was successful
         if(nRetVal != CCIndexFile::success)
            CCIndexFileException::Throw(nRetVal);
         dwPrevBlockSize = m_pIndexFile->GetRecordNum();
         // What is the previous blocks next block
         dwPrevOffset = offsetKey.GetOffset();
         dwPrevBlockOffset = dwPrevOffset + dwPrevBlockSize;
         // Does its position match the one we are about to add?
         if(dwPrevBlockOffset == dwOffset &&
            offsetKey.GetID() == nHandle)
         {
            // It matched, delete it since we are going
            // to merge free blocks
            bPrevBlockMatch = TRUE;
            dwNewBlockSize += dwPrevBlockSize;
            dwNewOffset    -= dwPrevBlockSize;
            // Should Match prev offset
            ASSERT(dwNewOffset == dwPrevOffset);
            // Delete prev offset key since we're merging blocks
            nRetVal = DeleteKeys(
               nHandle,
               dwPrevBlockSize,
               dwPrevOffset,
               TRUE,             // Find block key
               FALSE);           // Find offset key
            if(nRetVal != CCIndexFile::success)
               CCIndexFileException::Throw(nRetVal);
         }
      }
   }

   offsetKey.SetOffset(dwNewOffset);
   offsetKey.SetID(nHandle);

   nRetVal = m_pIndexFile->Add(
      dwNewBlockSize,// Key is block size
      NULL, 0,       // No data associated with key
      dwNewOffset,   // Use record # to store offset to free block
      nHandle,       // Index file ID
      TRUE);         // Allow duplicate keys
   // Throw exception if CIndex failed to add key
   // File is probably not open
   if(nRetVal != CCIndexFile::success)
      CCIndexFileException::Throw(nRetVal);
   // Add key of Offset Index which may represent combined blocks
   nRetVal = m_pIndexFile->Add(
      (LPVOID)&offsetKey,   // Key is block size
      NULL, 0,                // No data associated with key
      dwNewBlockSize,         // Use record # to store block size
      SORTED_BY_OFFSET_INDEX, // Index file ID
      FALSE);                 // Should never have duplicate keys
   // Throw exception if CIndex failed to add key
   // File is probably not open
   if(nRetVal != CCIndexFile::success)
      CCIndexFileException::Throw(nRetVal);

   return ERRORCODE_None;
}

// This function delete block and offset keys
int CFreeLists::DeleteKeys(SHORT nBlockHandle, DWORD dwBlockSize,
DWORD dwOffset, BOOL bLocateBlockKey, BOOL bLocateOffsetKey)
{
   int            nRetVal;
   COffsetKey     offsetKey;

   ASSERT(m_pIndexFile);

   if(bLocateBlockKey)
      {
         nRetVal = m_pIndexFile->Find(
            dwBlockSize,   // Key is block size
            NULL, 0,       // No data associated with key
            dwOffset,      // offset is record #
            nBlockHandle); // Index file ID
         ASSERT(nRetVal == CCIndexFile::success);
         if(nRetVal != CCIndexFile::success)
            return ERRORCODE_Fail;
      }
   // Delete Block size key
   nRetVal = m_pIndexFile->DeleteCurrentEntry(nBlockHandle);
   // Throw exception if delete failed since find was successful
   if(nRetVal != CCIndexFile::success)
      CCIndexFileException::Throw(nRetVal);

   if(bLocateOffsetKey)
      {
         offsetKey.SetOffset(dwOffset);
         offsetKey.SetID(nBlockHandle);

         // Find() will locate keys containing the next positional item
         // when an exact match doesn't occur
         nRetVal = m_pIndexFile->Find(
            &offsetKey,    // Special user defined key
            NULL, 0,       // No data associated with key
            dwBlockSize,   // Block size in stored in record #
            SORTED_BY_OFFSET_INDEX);      // Index file ID

         ASSERT(nRetVal == CCIndexFile::success);
         if(nRetVal != CCIndexFile::success)
            return ERRORCODE_Fail;
      }
   // Delete matched key 
   nRetVal = m_pIndexFile->DeleteCurrentEntry(SORTED_BY_OFFSET_INDEX);
   // If this asserts, failed to delete pair
   ASSERT(nRetVal == CCIndexFile::success);
   // Throw exception if delete failed since find was successful
   if(nRetVal != CCIndexFile::success)
      CCIndexFileException::Throw(nRetVal);

   return ERRORCODE_None;
}

int CFreeLists::DeleteBlock(SHORT nHandle, DWORD dwBlockSize)
{
   int         nRetVal;
   COffsetKey  offsetKey;
   DWORD       dwOffset;

   nRetVal = m_pIndexFile->Find(
      dwBlockSize,   // Key is block size
      NULL, 0,       // No data associated with key
      0,             // Don't know offset (record #) so supply 0
      nHandle);      // Index file ID
   if(nRetVal != CCIndexFile::success && nRetVal != CCIndexFile::keyMatch)
      return ERRORCODE_Fail;

   // Get offset of found key used for deleting offset key
   dwOffset = m_pIndexFile->GetRecordNum();

   // Delete Block size key
   nRetVal = m_pIndexFile->DeleteCurrentEntry(nHandle);
   // Throw exception if delete failed since find was successful
   if(nRetVal != CCIndexFile::success)
      CCIndexFileException::Throw(nRetVal);

   // Prepare to Delete Offset key
   offsetKey.SetOffset(dwOffset);
   offsetKey.SetID(nHandle);

   // Delete Offset Index Key
   nRetVal = m_pIndexFile->Delete(
      (LPVOID)&offsetKey,     // Key 
      SORTED_BY_OFFSET_INDEX, // Index file ID
      dwBlockSize);           // Should never have duplicate keys
   // Throw exception if CIndex failed to delete key
   // File is probably not open
   if(nRetVal != CCIndexFile::success)
      CCIndexFileException::Throw(nRetVal);
   return ERRORCODE_None;
}

int CFreeLists::LoadFileList()
   {
      int      nRetVal;

      ASSERT(m_pIndexFile);
      if(m_pIndexFile==NULL)
         return ERRORCODE_BadParameter;
      if(m_bLoadedList)
         return ERRORCODE_None;

      memset(&m_FileList, 0, sizeof(m_FileList));
      nRetVal = m_pIndexFile->GetCustomInfo(&m_FileList, sizeof(m_FileList));
      if(nRetVal != CCIndexFile::success)
         CCIndexFileException::Throw(nRetVal);
      m_bLoadedList = TRUE;

      return ERRORCODE_None;
   }

int CFreeLists::SaveFileList()
   {
      int      nRetVal;

      // SetCustomInfo() is limited to 100 bytes before a recompile of the CIndex library is
      // required.  Even then the max = 512 - sizeof(hdrrec) in cndxwk.h
      // Use a unique key in an index and store filelist in data portion if a
      // larger file list is required.
      nRetVal = m_pIndexFile->SetCustomInfo(&m_FileList, sizeof(m_FileList));
      if(nRetVal != CCIndexFile::success)
         CCIndexFileException::Throw(nRetVal);

      m_bLoadedList = TRUE;

      return ERRORCODE_None;
   }

CString CFreeLists::GetShortFileName(CString strFileName)
{
   int      nCharIndex, nCharCount;
   CString  strShortFileName;

   // Remove directory from filename portion
   nCharIndex = strFileName.ReverseFind('\\');
   nCharCount = strFileName.GetLength();
   if(nCharIndex >= 0)
   {
      nCharCount -= (nCharIndex + 1);
      strShortFileName = strFileName.Right(nCharCount);
   }
   else
      strShortFileName = strFileName;

   // Limit to 8.3 max
   nCharCount = strShortFileName.GetLength();
   if(nCharCount > 12)
      strShortFileName = strShortFileName.Right(12);
   return strShortFileName;
}

// CFreeListEntries methods
CFreeListEntries::CFreeListEntries()
   {
      SetNumFiles(0);
   }

// Returns handle to new entry if successful
// returns -1 if error
SHORT CFreeListEntries::AddEntry(CString strFileName)
   {
      ASSERT(m_nNumFiles+1 < MAX_FREELIST_ENTRIES);
      if(m_nNumFiles+1 >= MAX_FREELIST_ENTRIES)
         return -1;

      SetFileName(m_nNumFiles, strFileName);
      m_nNumFiles++;
      return m_nNumFiles - 1; // Handle is index into filename array
   }

CString CFreeListEntries::GetFileName(SHORT nFileNameIndex)
   {
      ASSERT(nFileNameIndex < MAX_FREELIST_ENTRIES);

      return m_szFileName[nFileNameIndex];
   }

void CFreeListEntries::SetFileName(SHORT nFileNameIndex, CString strFileName)
   {
      ASSERT(nFileNameIndex < MAX_FREELIST_ENTRIES);
      // Short filenames only are currently supported
      ASSERT(strFileName.GetLength() < sizeof(m_szFileName[nFileNameIndex]));

      if(nFileNameIndex >= MAX_FREELIST_ENTRIES)
         return;

      Util::SafeStringCopy(m_szFileName[nFileNameIndex], strFileName,
         sizeof(m_szFileName[nFileNameIndex]));
   }


