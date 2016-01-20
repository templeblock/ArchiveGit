//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/COLLFILE.CPP 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/COLLFILE.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 5     2/09/99 1:48p Gbeddow
// Support in collection building for creating online collection GIF and
// JPEG thumbnail files, and related changes, per spec from Alexei
// 
// 4     1/28/99 4:47p Mwilson
// added special colleciton builders
// 
// 3     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "collfile.h"
#include "util.h"
#include "gif.h"

#ifdef NEW_THUMBNAILS
// The code ifdef'd NEW_THUMBNAILS is not PrintMaster-dependent.
// However, you do need to link with things like the JPEG code and the
// Pegasus library.
// Hence, the define remains.
#include "file.h"
#include "jpegimpl.h"
#include "cdeflate.h"
#endif

#define KEYWORDID_INDEX    2

CDataFileEntry::CDataFileEntry()                             
   {
      m_pData = NULL;
      m_bOwnData = TRUE;
      m_dwDataSize = 0;
   }

LPVOID CDataFileEntry::Attach(LPVOID pData, DWORD dwSize)
   {
      FreeData();
      m_pData = pData;
      m_dwDataSize = dwSize;
      m_bOwnData=FALSE;
      return pData;
   }

LPVOID CDataFileEntry::Detach()
   {
      LPVOID pOldData = GetData();
      FreeData();
      return pOldData;
   }

LPVOID CDataFileEntry::AllocData(DWORD dwSize)
   {
      FreeData();
      m_bOwnData = TRUE;
      m_dwDataSize = dwSize;
      return (m_pData = Util::HugeAlloc(dwSize));
   }

void CDataFileEntry::FreeData()
   {
      if(m_pData && m_bOwnData)
      {
			Util::HugeFree(m_pData);
      }
      m_pData = NULL;
      m_bOwnData = TRUE;
      m_dwDataSize = 0;
   }

// Directory data access methods
CSortedDir::CSortedDir(CCIndexFile *pIndexFile)
   {
      ASSERT(pIndexFile);

      m_pIndexFile = pIndexFile;
      m_pDirEntry = NULL;
      m_bFound = FALSE;
   }

CSortedDir::~CSortedDir()
   {
   }

CSortedDirEntry * CSortedDir::GetEntry()
   {
  	   return m_pDirEntry;
   }

// If Logical errors then Find Fails with return value
// CCIndexFile can throw exception
int CSortedDir::Find(CSortedDirEntry *pDirEntry, BOOL bReadNow, BOOL bAllowPartial /*=FALSE*/)
   {
      int   nRetVal = CCIndexFile::fail;
      int   nThisRetVal = ERRORCODE_Fail;
   
      ASSERT(pDirEntry);
      ASSERT(m_pIndexFile);

      m_bFound = FALSE;

      // Callers directory entry is current entry
      m_pDirEntry = pDirEntry;

      if(m_pIndexFile == NULL)
         return ERRORCODE_BadParameter;

      // If Data object owns memory, release it so we can allocate a new one
      // with new data
      if(m_pDirEntry->GetDataEntry().OwnData())
         m_pDirEntry->GetDataEntry().FreeData();

      // Don't know record number (Data Offset)
      nRetVal = m_pIndexFile->Find(CString(pDirEntry->GetKey()), NULL, 0);

      if(nRetVal == CCIndexFile::success
			|| nRetVal == CCIndexFile::keyMatch
			|| (bAllowPartial && nRetVal == CCIndexFile::keyGreaterFound))
      {
         m_bFound = TRUE;
         // Fill in directory with keyword and offset contained
         // in directory
         m_pDirEntry->SetOffsetToData(m_pIndexFile->GetRecordNum());

         if(bReadNow)
			{
				// Do the read.
				int nReadVal = Read();
				if (nReadVal != ERRORCODE_None)
				{
					return nReadVal;
				}
			}

		   return (nRetVal == CCIndexFile::keyGreaterFound) ? ERRORCODE_KeyGreaterFound : ERRORCODE_None;
      }
      else if(nRetVal == CCIndexFile::fileNotOpen)
         return ERRORCODE_Open;

      return nThisRetVal;
   }

int CSortedDir::Read()
   {
      int   nRetVal;
      int   nDataSize;

      ASSERT(m_pDirEntry);
      ASSERT(m_pIndexFile);

      if(!m_bFound)
         ERRORCODE_Fail;

      // Fill in users data item, allocate memory for it if user
      // didn't supply own
      nDataSize = m_pIndexFile->GetEntrySize();

      // If data size is 0, assume client has no data associated with this key
      if(nDataSize <= 0)
   		return ERRORCODE_None;

      // If we own data object, we can allocate memory for data object, 
      // then fill it in
      if(m_pDirEntry->GetDataEntry().OwnData())
         {
            m_pDirEntry->GetDataEntry().AllocData(nDataSize);
         }
      nRetVal = m_pIndexFile->GetCurrentEntry(m_pDirEntry->GetDataEntry().GetData(), 
         (USHORT) m_pDirEntry->GetDataEntry().GetDataSize());

      if(nRetVal != CCIndexFile::success)
   		return ERRORCODE_Read;

		return ERRORCODE_None;
   }

// Error types
// Keyword too big
// Will return fail if key already exists
int CSortedDir::Add(CSortedDirEntry *pDirEntry)
   {
      int   nRetVal;

      // Store users data without keyword portion in data record
      // since key will contain the keyword
      nRetVal = m_pIndexFile->Add(CString(pDirEntry->GetKey()),
         pDirEntry->GetDataEntry().GetData(), 
         (USHORT) pDirEntry->GetDataEntry().GetDataSize(),
         pDirEntry->GetOffsetToData());
      if(nRetVal != CCIndexFile::success)
         return ERRORCODE_Fail;

      return ERRORCODE_None;
   }

int CSortedDir::Delete()
   {
      int      nRetVal;

      ASSERT(m_pDirEntry);
      ASSERT(m_pIndexFile);

      ASSERT(m_bFound == TRUE);
      if(!m_bFound)
         return ERRORCODE_BadParameter;

      nRetVal = m_pIndexFile->DeleteCurrentEntry();

      if(nRetVal != CCIndexFile::success)
         return ERRORCODE_Fail;

      return ERRORCODE_None;
   }

// Offset Directory Methods
CItemNumDir::CItemNumDir(StorageDevice *pDirFile)
  {
      m_pFile = pDirFile;
      m_lDataOffset = 0;
  }

CItemNumDir::~CItemNumDir()
  {
  }

int CItemNumDir::Find(DWORD dwItemNum)
  {
      ST_DEV_IO_SIZE nBytesRead;

      if(m_pFile == NULL)
         return ERRORCODE_BadParameter;

      m_pFile->seek((dwItemNum-1) * sizeof(DWORD), ST_DEV_SEEK_SET);

      // read offset directly into dest (dwDataOffset)
      m_pFile->read(&m_lDataOffset, sizeof(DWORD), &nBytesRead);
      if(nBytesRead != sizeof(DWORD))
         return ERRORCODE_Read;

      m_dwCurItemNum = dwItemNum;
      return ERRORCODE_None;
  }

// Returns new item number
DWORD CItemNumDir::Add(DWORD dwDataOffset)
  {
     DWORD                 dwNewItemNum;
     ST_DEV_POSITION       stdevCurPosition;
 
     // seek to end of file
     m_pFile->seek(0, ST_DEV_SEEK_END);
     m_pFile->tell(&stdevCurPosition);
     dwNewItemNum = ((stdevCurPosition+1) / sizeof(DWORD)) + 1;

     // write throws exception if num written != num requested to write
     m_pFile->write(&dwDataOffset, sizeof(DWORD));

     return dwNewItemNum;
  }


// Deletes current item (set from last Find())
int CItemNumDir::Delete()
   {
      DWORD    dwDataOffset = (DWORD)-1L;

      m_pFile->seek((m_dwCurItemNum-1) * sizeof(DWORD), ST_DEV_SEEK_SET);

      // Mark as deleted
      m_pFile->write(&dwDataOffset, sizeof(DWORD));
      return ERRORCODE_None;
   }

int CItemNumDir::Update(DWORD dwItemNum, DWORD dwNewDataOffset)
   {
      int         nRetVal;

      nRetVal = Find(dwItemNum);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      // Seek to item we just found
      m_pFile->seek((m_dwCurItemNum-1) * sizeof(DWORD), ST_DEV_SEEK_SET);

      // Update directory item with new position to data item
      m_pFile->write(&dwNewDataOffset, sizeof(DWORD));

      return ERRORCODE_None;
   }

// File data methods
CFileData::CFileData(StorageDevice *cfileObject)
   {
      m_pCFile = cfileObject;
      m_pDataEntry = NULL;
      m_lCurOffset = m_lOffsetToDataStart = 0;
      m_pFreeList = NULL;
   }

CFileData::~CFileData()
  {
  }

// Find()
// Data header is automatically read on a successful find
int CFileData::Find(DWORD dwFileOffsetToDataStart, CDataFileEntry *pDataEntry)
   {
      int   nRetVal;

      ASSERT(pDataEntry);
      ASSERT(m_pCFile);

      m_pDataEntry = pDataEntry;

      if(m_pDataEntry == NULL || m_pCFile == NULL)
         return ERRORCODE_BadParameter;

//      if(m_pCFile->m_hFile == CFile::hFileNull)
//         return BrowserFileRC_fileNotOpen;

      // Throws exception if seek fails
      m_pCFile->seek(dwFileOffsetToDataStart, ST_DEV_SEEK_SET);
      m_lCurOffset = m_lOffsetToDataStart = dwFileOffsetToDataStart;

      // read header (if any)
      if(m_pDataEntry->GetHeader())
         {
            nRetVal = ReadHeader();
            if(nRetVal != ERRORCODE_None)
               return ERRORCODE_Read;
         }

      return ERRORCODE_None;
   }

int CFileData::Read(DWORD dwBytesToRead)
   {
      DWORD             dwBytesRead, dwDataEndPosition;
      ST_DEV_POSITION   stdevCurFilePosition;

      ASSERT(m_pDataEntry);
      ASSERT(m_pCFile);

      if(m_pDataEntry == NULL || m_pCFile == NULL)
         return ERRORCODE_BadParameter;

      // Default indicates read entire data object
      if(dwBytesToRead == 0)
         {
            // If user didn't attach own memory, allocate memory for entire
            // data item found from last find
            if(m_pDataEntry->OwnData())
               {
                  m_pDataEntry->AllocData(m_pDataEntry->GetItemSize());
                  dwBytesToRead = m_pDataEntry->GetItemSize();
               }
            else
               {
                  // We are only read size of attached memory object or
                  // file item, which ver is smaller
                  dwBytesToRead = min(m_pDataEntry->GetDataSize(),
                     m_pDataEntry->GetItemSize());
               }
         }
      else
         {
            // If user didn't attach own memory, allocate memory for entire
            // data item found from last find
            if(m_pDataEntry->OwnData())
               {
                  m_pDataEntry->AllocData(dwBytesToRead);
               }
            else
               {
                  // Will read data into callers buffer
                  // make sure it's big enough for request
                  ASSERT(m_pDataEntry->GetDataSize() >= dwBytesToRead);
                  if(m_pDataEntry->GetDataSize() < dwBytesToRead)
                     return ERRORCODE_BadParameter;
               }
         }

      ASSERT(dwBytesToRead);
      if(dwBytesToRead == 0)
         return ERRORCODE_BadParameter;

      // Check if client is attempting read past end of data object
      dwDataEndPosition = m_lOffsetToDataStart +
         m_pDataEntry->GetHeaderSize() + GetItemSize();

      // If this ASSERTS, this call is trying to read past end of the
      // current data object
      ASSERT( !((m_lCurOffset + dwBytesToRead) > dwDataEndPosition) );
      if( (m_lCurOffset + dwBytesToRead) > dwDataEndPosition )
         return ERRORCODE_BadParameter;

//      if(m_pCFile->m_hFile == CFile::hFileNull)
//         return BrowserFileRC_fileNotOpen;

      m_pCFile->tell(&stdevCurFilePosition);
      if(m_lCurOffset != stdevCurFilePosition)
         {
            m_pCFile->seek(stdevCurFilePosition, ST_DEV_SEEK_SET);
            m_lCurOffset = stdevCurFilePosition;
         }

      m_pCFile->huge_read(m_pDataEntry->GetData(), dwBytesToRead, &dwBytesRead);
      if(dwBytesRead != dwBytesToRead)
         return ERRORCODE_Read;

      m_pCFile->tell(&stdevCurFilePosition);
      m_lCurOffset = stdevCurFilePosition;

      return ERRORCODE_None;
   }

int CFileData::Seek(DWORD dwDataOffset)
   {
      DWORD    dwNewFileOffset, dwRequestedFileOffset;

      ASSERT(m_pDataEntry);
      ASSERT(m_pCFile);

      if(m_pDataEntry == NULL || m_pCFile == NULL || dwDataOffset == -1L)
         return ERRORCODE_BadParameter;

//      if(m_pCFile->m_hFile == CFile::hFileNull)
//         return BrowserFileRC_fileNotOpen;

      dwRequestedFileOffset = m_lOffsetToDataStart +
         m_pDataEntry->GetHeaderSize() + dwDataOffset;

      // seek within data item
      dwNewFileOffset = m_pCFile->seek(dwRequestedFileOffset, ST_DEV_SEEK_SET);
      m_lCurOffset = dwRequestedFileOffset;

      return ERRORCODE_None;
   }

DWORD CFileData::GetItemSize()
   {
      ASSERT(m_pDataEntry);
      ASSERT(m_pCFile);

      if(m_pDataEntry == NULL || m_pCFile == NULL)
         return ERRORCODE_BadParameter;

      return m_pDataEntry->GetItemSize();
   }

int CFileData::ReadHeader()
   {
      ST_DEV_IO_SIZE    nBytesRead;
      ST_DEV_POSITION   stdevCurFilePosition;

      ASSERT(m_pDataEntry);
      ASSERT(m_pCFile);
      if(m_pDataEntry->GetHeaderSize() == 0 ||
         m_pDataEntry->GetHeader() == NULL  ||
         m_pCFile == NULL)
      	   return ERRORCODE_BadParameter;

      // If current file position isn't at data start (data header if one exists)
      // then seek to that position
      m_pCFile->tell(&stdevCurFilePosition);
      if(stdevCurFilePosition != m_lOffsetToDataStart)
        m_pCFile->seek(m_lOffsetToDataStart, ST_DEV_SEEK_SET);
      // Read the header
      m_pCFile->read(m_pDataEntry->GetHeader(),
         m_pDataEntry->GetHeaderSize(), &nBytesRead);

      // Update this objects state of where current file pointer is
      m_pCFile->tell(&stdevCurFilePosition);
      m_lCurOffset = stdevCurFilePosition;

      if(nBytesRead != m_pDataEntry->GetHeaderSize())
         return ERRORCODE_Read;
      return ERRORCODE_None;
   }

DWORD CFileData::Add(CDataFileEntry *pDataEntry)
   {
      int               nRetVal;
      BOOL              bUsingFreeBlock=FALSE;
      SHORT             nListHandle;
      DWORD             dwBlockSize, dwFreeBlockOffset;
      ST_DEV_POSITION   stdevItemPosition, stdevCurFilePosition;

      ASSERT(pDataEntry);
      ASSERT(m_pCFile);

      if(pDataEntry == NULL ||
         pDataEntry->GetData() == NULL ||
         pDataEntry->GetDataSize() == 0 || m_pCFile == NULL)
            return ERRORCODE_BadParameter;

      // Data size should match item size
      ASSERT(pDataEntry->GetDataSize() == pDataEntry->GetItemSize());

      dwBlockSize = pDataEntry->GetHeaderSize() + pDataEntry->GetDataSize();

      // If free list is registered, check it first for free blocks
      if(m_pFreeList)
         {
            nListHandle = m_pFreeList->FindList(m_pCFile->get_name());
            if(nListHandle >= 0)
               {
                  nRetVal = m_pFreeList->FindBlock(nListHandle,
                     dwBlockSize, &dwFreeBlockOffset);
                  if(nRetVal == ERRORCODE_None)
                     {
                        m_pCFile->seek(dwFreeBlockOffset, ST_DEV_SEEK_SET);
                        bUsingFreeBlock = TRUE;
                     }
               }
         }

      // If no free block found, append new data to end of file
      if(!bUsingFreeBlock)
         m_pCFile->seek(0, ST_DEV_SEEK_END);
      m_pCFile->tell(&stdevItemPosition);

      // Write header if data object has one
      if(pDataEntry->GetHeader())
         m_pCFile->write(pDataEntry->GetHeader(), pDataEntry->GetHeaderSize());

      // Write data item
      m_pCFile->huge_write(pDataEntry->GetData(), pDataEntry->GetDataSize());

      m_pCFile->tell(&stdevCurFilePosition);
      m_lCurOffset = stdevCurFilePosition;

      return stdevItemPosition;
   }

int CFileData::SetFreeList(CFreeLists *pFreeList)
   {
      ASSERT(pFreeList);

      m_pFreeList = pFreeList;
      return ERRORCODE_None;
   }

int CFileData::Delete()
   {
      DWORD       dwDataOffset, dwBlockSize;
      SHORT       nListHandle;
      int         nRetVal;
      CString     strThisFile;


      // ASSERTS of SetFreeList() wasn't called
      ASSERT(m_pFreeList);

      if(m_pFreeList == NULL)
         return ERRORCODE_BadParameter;

      dwDataOffset   = m_lOffsetToDataStart;
      dwBlockSize    = m_pDataEntry->GetHeaderSize() +
         m_pDataEntry->GetItemSize();

      ASSERT(dwBlockSize);

      ASSERT(m_lOffsetToDataStart != -1L);
      if(m_lOffsetToDataStart == -1L)
         return ERRORCODE_Fail;

      strThisFile = m_pCFile->get_name();
      nListHandle = m_pFreeList->FindList(strThisFile);
      // if file isn't registered in free list database, do it
      if(nListHandle < 0)
         {
            // This should throw a CIndex exception on error
            nListHandle = m_pFreeList->CreateList(strThisFile);
            if(nListHandle < 0)
               {
                  // Failed to create a free list
                  ASSERT(0);
                  return ERRORCODE_Fail;
               }
         }
      // Register this block as available in free list database
      // Block size / offset pair must be unique
      nRetVal = m_pFreeList->AddBlock(nListHandle, dwBlockSize, dwDataOffset);
      return nRetVal;
   }

int CFileData::Update(DWORD dwFileOffsetToDataStart, CDataFileEntry *pDataEntry,
   DWORD *pNewOffsetToData)
   {
      int               nRetVal;
      DWORD             dwNewDataOffset;
      CDataFileEntry    *pCurrentDataEntry;

      // Locate item to update
      pCurrentDataEntry = pDataEntry->CreateObject();
      ASSERT(pCurrentDataEntry);
      // Both objects should have same size headers
      ASSERT(pCurrentDataEntry->GetHeaderSize() == pDataEntry->GetHeaderSize());
      nRetVal = Find(dwFileOffsetToDataStart, pCurrentDataEntry);
      if(nRetVal != ERRORCODE_None)
         {
            delete pCurrentDataEntry;
            return nRetVal;
         }

      // Found it, remove old one
      nRetVal = Delete();
      // Release temp object
      delete pCurrentDataEntry;
      if(nRetVal != ERRORCODE_None)
         {
            return nRetVal;
         }

      // Now add new data
      dwNewDataOffset = Add(pDataEntry);
      *pNewOffsetToData = dwNewDataOffset;

      return ERRORCODE_None;
   }

CSortedDatabase::CSortedDatabase(CSortedDir *pDir, CFileData *pData)
   {
      ASSERT(pDir);
      ASSERT(pData);
      m_pDir = pDir;
      m_pData = pData;
   }

int CSortedDatabase::Add(CSortedDBDirEntry *pDirEntry, CDataFileEntry *pDataEntry)
   {
      DWORD dwItemPosition;
      int   nRetVal;

      ASSERT(pDirEntry);

      if(pDirEntry == NULL)
         return ERRORCODE_BadParameter;

      // Check if item is already in directory
      pDirEntry->SetOffsetToData(0);
      nRetVal = m_pDir->Find(pDirEntry);

      // If Already exists, return success
      if(nRetVal == ERRORCODE_None)
         return ERRORCODE_Exists;

      // If pDataEntry is NULL then assume client wants to store data external
      if(pDataEntry == NULL)
         pDirEntry->SetStorageMethod(CSortedDBDirEntry::storageInternal);

      // Client can have a directory entry with data incase they want
      // to store data item external to database.
      if(pDataEntry &&
         pDirEntry->GetStorageMethod() == CSortedDBDirEntry::storageInternal)
         {
            // Should have data attached
            ASSERT(pDataEntry->GetData());
            if(!pDataEntry->GetData())
               return ERRORCODE_BadParameter;
            dwItemPosition = m_pData->Add(pDataEntry);
         }
      else
         dwItemPosition = 0L;

      pDirEntry->SetOffsetToData(dwItemPosition);

      // If Data item successfully added, then add entry to directory
      nRetVal = m_pDir->Add(pDirEntry);

      return nRetVal;
   }

int CSortedDatabase::Find(CSortedDBDirEntry *pDirEntry, CDataFileEntry *pDataEntry)
   {
      int   nRetVal;

      ASSERT(pDirEntry);

      nRetVal = m_pDir->Find(pDirEntry);

      if(nRetVal == ERRORCODE_None)
         {
            // If pDataEntry is NULL, assume client wants to store data external
            m_lDataOffset = pDirEntry->GetOffsetToData();
            if(pDataEntry &&
               pDirEntry->GetStorageMethod() == CSortedDBDirEntry::storageInternal)
               {
                  if(m_lDataOffset != -1L)
                     nRetVal = m_pData->Find(m_lDataOffset, pDataEntry);
               }
         }
      else
         m_lDataOffset = 0L;

      return nRetVal;
   }

int CSortedDatabase::Seek(DWORD dwDataOffset)
   {
      ASSERT(m_pData);
      return m_pData->Seek(dwDataOffset);
   }

int CSortedDatabase::Read(DWORD dwBytesToRead)
   {
      ASSERT(m_pData);
      return m_pData->Read(dwBytesToRead);
   }

// Deletes current item from last Find()
int CSortedDatabase::Delete()
   {
      int                  nRetVal;
      CSortedDBDirEntry    *pDirEntry;

      ASSERT(m_pDir);
      ASSERT(m_pData);

      nRetVal = m_pDir->Delete();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      pDirEntry = (CSortedDBDirEntry *) m_pDir->GetEntry();
      ASSERT(pDirEntry);

      // Delete data item if one is associated with directory entry
      if(pDirEntry->GetStorageMethod() == CSortedDBDirEntry::storageInternal)
            nRetVal = m_pData->Delete();
      return nRetVal;
   }

#if 0
int CSortedDatabase::Update(CSortedDBDirEntry *pDirEntry, CDataFileEntry *pDataEntry)
   {
      int         nRetVal;

      nRetVal = Find(pDirEntry, pDataEntry);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      // Delete data item if one is associated with directory entry
      if(pDirEntry->GetStorageMethod() == CSortedDBDirEntry::storageInternal)
            nRetVal = m_pData->Delete();

      // Update directory with new data offset
   }
#endif

// Itemnum database consists of a directory containing file offsets
// sorted by item numbers and an associated data file
DWORD CItemNumDatabase::Add(CDataFileEntry *pDataEntry)
   {
      DWORD dwItemPosition;

      ASSERT(pDataEntry);

      dwItemPosition = m_Data.Add(pDataEntry);

      // If Data item successfully added, then add entry to directory
      return m_Dir.Add(dwItemPosition);
   }

int CItemNumDatabase::Find(DWORD dwItemNum, CDataFileEntry *pDataEntry)
   {
      int    nRetVal;

      nRetVal = m_Dir.Find(dwItemNum);
      if(nRetVal == ERRORCODE_None)
         {
            m_lDataOffset = m_Dir.GetOffsetToData();
            nRetVal = m_Data.Find(m_lDataOffset, pDataEntry);
         }
      return nRetVal;
   }

int CItemNumDatabase::Seek(DWORD dwDataOffset)
   {
      return m_Data.Seek(dwDataOffset);
   }

int CItemNumDatabase::Read(DWORD dwBytesToRead)
   {
     return m_Data.Read(dwBytesToRead);
   }

// Delete() requires a call to Find() first
int CItemNumDatabase::Delete()
   {
      int      nRetVal;

      // Deletes current directory entry (from last Find())
      nRetVal = m_Dir.Delete();
      ASSERT(nRetVal == ERRORCODE_None);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;

      // Delete data
      nRetVal = m_Data.Delete();
      ASSERT(nRetVal == ERRORCODE_None);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;
      return ERRORCODE_None;
   }

int CItemNumDatabase::Update(DWORD dwItemNum, CDataFileEntry *pDataEntry)
   {
      int               nRetVal;
      DWORD             dwItemPosition;
      CDataFileEntry    *pCurrentDataEntry;

      ASSERT(pDataEntry);
      pCurrentDataEntry = pDataEntry->CreateObject();
      ASSERT(pCurrentDataEntry->GetHeaderSize() == pDataEntry->GetHeaderSize());
      nRetVal = Find(dwItemNum, pCurrentDataEntry);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      // Delete data
      nRetVal = m_Data.Delete();
      delete pCurrentDataEntry;
      ASSERT(nRetVal == ERRORCODE_None);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;

      dwItemPosition = m_Data.Add(pDataEntry);
      
      // If Data item successfully added, then update directory
      return m_Dir.Update(dwItemNum, dwItemPosition);
   }

// Thumbnail methods
//
// The code ifdef'd NEW_THUMBNAILS is not PrintMaster-dependent.
// However, you do need to link with things like the JPEG code and the
// Pegasus library.
// Hence, the define remains.

#ifdef NEW_THUMBNAILS
static BOOL ReduceColors(BYTE* pDIBData, int nWidth, int nHeight,
								 CDWordArray& cdaColors, BYTE** ppNewData,
								 DWORD* pdwNewSize)
{
	int nMaxColors = 255;	// leave room for transparent entry
	int nPalEntries = 256;
	int nColors = 0;
	ASSERT(cdaColors.GetSize() == 0);

	int nSrcByteWidth = (nWidth*3 + 3) & ~3;
	int nDstByteWidth = (nWidth + 3) & ~3;

	//
	// Format of the data is:
	//
	// WORD wColorsInPalette
	// DWORD[] Palette Entries
	// BYTE[] Data
	//

	int nPaletteSize = sizeof(WORD) + nPalEntries*sizeof(DWORD);
	DWORD dwDataSize = (DWORD)nDstByteWidth*(DWORD)nHeight;
	DWORD dwNewSize = (DWORD)nPaletteSize + dwDataSize;
	BYTE* pNewData = (BYTE*)Util::HugeAlloc(dwNewSize);
	if (pNewData == NULL)
	{
		return FALSE;
	}

	BYTE*  pSrcRow = pDIBData;
	BYTE* pDstRow = pNewData + nPaletteSize;

	ASSERT(sizeof(RGBQUAD) == sizeof(DWORD));
	RGBQUAD c;
	c.rgbReserved = 0;

	for (int nY = 0; nY < nHeight; nY++)
	{
		BYTE* pSrc = pSrcRow;
		BYTE* pDst = pDstRow;
		for (int nX = 0; nX < nWidth; nX++)
		{
			// Get the source color.
			c.rgbBlue = pSrc[0];
			c.rgbGreen = pSrc[1];
			c.rgbRed = pSrc[2];
			pSrc += 3;

			// Try to find this in the array.
			for (int nColor = 0; nColor < nColors; nColor++)
			{
				if (*(DWORD*)&c == cdaColors.GetAt(nColor))
				{
					break;
				}
			}

			// See if we found the color.
			if (nColor == nColors)
			{
				// Did not find it. Add it if not at max.
				if (nColors == nMaxColors)
				{
					TRACE("ReduceColors: Exceeded %d colors.", nMaxColors);
					Util::HugeFree((LPVOID)pNewData);
					return FALSE;
				}
				// Not at max. Add it.
				cdaColors.Add(*(DWORD*)&c);
				nColors++;
			}

			// See if we found the color.
			*pDst++ = (BYTE)nColor;
		}
		pSrcRow += nSrcByteWidth;
		pDstRow += nDstByteWidth;
	}

	// Copy the palette data over.
	memset(pNewData, 0, nPaletteSize);		// Set to zero first. (Better compression)
	*(WORD*)pNewData = nPalEntries;
	LPBYTE pPalette = pNewData + sizeof(WORD);
	memcpy(pPalette, &cdaColors.ElementAt(0), cdaColors.GetSize()*sizeof(DWORD));

	// Plug in the return values.
	*ppNewData = pNewData;
	*pdwNewSize = dwNewSize;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// Use the Pegasus library to create an optimal palette from a 24-bit image.
// This is currently only getting called after we determine that the
// image has more than 255 colors in it.
// NOTE: Pegasus does some less-than-ideal things with saturated colors,
// turning color components of 255 to 248. Since we then dither these
// colors into the PrintMaster palette, images with large areas of pure
// colors could potentially look rather ugly.
////////////////////////////////////////////////////////////////////////////

// Little inline function to avoid the "macro-ness" of min().
static inline int MyMin(int a, int b)
	{ return (a < b) ? a : b; }
static BOOL DoPegasusReduction(LPBITMAPINFO pbi, BYTE* pDIBData,
										 int nWidth, int nHeight,
										 BYTE** ppNewData, DWORD* pdwNewSize)
{
	int nMaxColors = 255;	// leave room for transparent entry
	int nPalEntries = 256;

	int nSrcByteWidth = (nWidth*3 + 3) & ~3;
	DWORD dwSrcDataSize = (DWORD)nSrcByteWidth * (DWORD)nHeight;

	int nDstByteWidth = (nWidth + 3) & ~3;
	DWORD dwDstDataSize = (DWORD)nDstByteWidth*(DWORD)nHeight;

	// create the get/put queues
	DWORD inputsize = dwSrcDataSize + 1;
	DWORD outputsize = dwDstDataSize + 1;
	BYTE* input_queue = NULL;
	BYTE* output_queue = NULL;
	TRY
	{
		input_queue = new BYTE[inputsize];
		output_queue = new BYTE[outputsize];
	}
	END_TRY

	if (!input_queue || !output_queue)
	{
		ASSERT(0);
		delete[] input_queue;
		delete[] output_queue;
		return FALSE;
	}

	PIC_PARM* pPicParm = new PIC_PARM;
	memset(pPicParm, 0, sizeof(PIC_PARM));

   // Set the standard fields.
	pPicParm->ParmSize = sizeof(PIC_PARM);
	pPicParm->ParmVer = CURRENT_PARMVER;
	pPicParm->ParmVerMinor = 1;

	pPicParm->Op = OP_UTL;	// opcode 82
	pPicParm->u.UTL.Subcode = 7;

	pPicParm->Head = pbi->bmiHeader;
	pPicParm->u.UTL.OutBpp = 8;
	pPicParm->u.UTL.NumColors = nMaxColors;
	pPicParm->u.UTL.AllocType = 1;	// we allocate full buffers for input/output

	// setup buffers
	pPicParm->Get.Start = input_queue;
	pPicParm->Get.Front = pPicParm->Get.Rear = pPicParm->Get.Start;
	pPicParm->Get.End = pPicParm->Get.Start + inputsize;

	pPicParm->Put.Start = output_queue;
	pPicParm->Put.Front = pPicParm->Put.Rear = pPicParm->Put.Start;
	pPicParm->Put.End = pPicParm->Put.Start + outputsize;

	// the Pegasus loop
	BOOL success = FALSE;
	RESPONSE res = Pegasus(pPicParm, REQ_INIT);
	if (res == RES_DONE)
	{
		res = Pegasus(pPicParm, REQ_EXEC);
		while (res != RES_DONE)
		{
			if (res == RES_GET_NEED_DATA || res == RES_GET_DATA_YIELD)
			{
				// we always copy the whole thing, so set pointers to start
				QUEUE* q = &pPicParm->Get;
				q->Front = q->Rear = q->Start;
				memcpy(q->Rear, pDIBData, dwSrcDataSize);
				q->Rear = q->Rear + dwSrcDataSize;
			}
			else if (res != RES_SEEK)
			{
				// NOTE: pegasus reads the data twice, with a RES_SEEK in between.
				// We don't need to do anything for the RES_SEEK.
				
				// any other response means we failed
				success = FALSE;
				goto end_loop;
			}
			res = Pegasus(pPicParm, REQ_CONT);
		}
		success = TRUE;
	}

end_loop:
	Pegasus(pPicParm, REQ_TERM);

	if (success)
	{
		// we now have a colormap and new bitmap data

		//
		// Format of the data to return is:
		//
		// WORD wColorsInPalette
		// DWORD[] Palette Entries
		// BYTE[] Data
		//

		int nPaletteSize = sizeof(WORD) + nPalEntries * sizeof(DWORD);
		DWORD dwNewSize = (DWORD)nPaletteSize + dwDstDataSize;

		BYTE* pNewData = new BYTE[dwNewSize];
		memset(pNewData, 0, dwNewSize);	// 0 everything to begin with

		// Scale the palette to full-range.
		RGBQUAD* pq = pPicParm->ColorTable;
		for (int j = 0; j < nMaxColors; j++, pq++)
		{
			pq->rgbRed = (BYTE)MyMin(MulDiv(pq->rgbRed, 0x00ff, 0x00fc), 255);
			pq->rgbGreen = (BYTE)MyMin(MulDiv(pq->rgbGreen, 0x00ff, 0x00fc), 255);
			pq->rgbBlue = (BYTE)MyMin(MulDiv(pq->rgbBlue, 0x00ff, 0x00fc), 255);
		}

		// stuff all our info into buffer
		*(WORD*)pNewData = nPalEntries;
		memcpy(pNewData + sizeof(WORD), pPicParm->ColorTable, nMaxColors * sizeof(DWORD));
		LPBYTE pData = pNewData + nPaletteSize;
		memcpy(pData, pPicParm->Put.Front, dwDstDataSize);

		// Plug in the return values.
		*ppNewData = pNewData;
		*pdwNewSize = dwNewSize;
	}

	delete pPicParm;
	delete [] input_queue;
	delete [] output_queue;

	return success;
}
#endif

void CThumbnailDataEntry::SetThumbnailData(LPBITMAPINFO pbi, LPBYTE pDIBData, LPBYTE pTransMaskData,
														 int nWidth, int nHeight,
														 int nBits, DWORD dwBytes,
														 CString csOnlineThumbnailFile)
				
{
#ifdef NEW_THUMBNAILS
	ASSERT(pbi != NULL);
	ASSERT(pDIBData != NULL);
	ASSERT(nBits == 24);

	// Construct the thumbnail data.

	CDWordArray cdaColors;
	BYTE* pNewData = NULL;
	DWORD dwNewSize = 0;

	// GCB 2/5/99 - support for creating online thumbnail files
  	CString csDrive, csDirectory, csFilename, csExtension;
	if (!csOnlineThumbnailFile.IsEmpty())
	{
		Util::SplitPath(csOnlineThumbnailFile, &csDrive, &csDirectory, &csFilename, &csExtension);
		Util::MakeDirectory(csDirectory);
	}

	// See if we can reduce the thumbnail to 8-bit. If there's more than 255 colors
	// in the image, only reduce it to 8-bit if the image has transparency. If not,
	// use JPEG compression (it's smaller than the pegasus 8-bit reduction).

	if (ReduceColors(pDIBData, nWidth, nHeight, cdaColors, &pNewData, &dwNewSize) ||
		 (pTransMaskData &&
		  DoPegasusReduction(pbi, pDIBData, nWidth, nHeight, &pNewData, &dwNewSize)))
	{
		// We now have a 256 entry colormap, with 255 or less colors used.
		nBits = 8;

		if (pTransMaskData)
		{
			// If we have a transparency mask, index 255 will be set to -1 as a flag,
			// and we set the transparent pixels in the bitmap.
			COLORREF* pPalette = (COLORREF*)(pNewData + sizeof(WORD));
			pPalette[255] = (COLORREF)-1;

			int nEndOfPalette = sizeof(WORD) + 256 * sizeof(DWORD);

			int nByteWidth = (nWidth + 3) & ~3;
			int nMaskByteWidth = nWidth;

			BYTE* pSrcRow = pNewData + nEndOfPalette;
			BYTE* pMskRow = pTransMaskData;

			for (int nY = 0; nY < nHeight; nY++)
			{
				BYTE* pSrc = pSrcRow;
				BYTE* pMsk = pMskRow;
				for (int nX = 0; nX < nWidth; nX++)
				{
					if (*pMsk)
						*pSrc = 255;	// set to transparent index
					pSrc++;
					pMsk++;
				}

				pSrcRow += nByteWidth;
				pMskRow += nMaskByteWidth;
			}
		}

		// Set the graphic info.
		m_DataHeader.SetGraphicInfo(nWidth, nHeight, nBits);

		// Compress the data.
		CDeflate Deflator;
		LPBYTE pCompressedData;
		DWORD dwCompressedSize;
		Deflator.Compress((LPBYTE)pNewData, dwNewSize, &pCompressedData, &dwCompressedSize);

		// Set the compression info.
		m_DataHeader.SetCompressInfo(dwNewSize, dwCompressedSize, CThumbnailDataHeader::COMPRESSION_ReducedDeflate);
		// Allocate memory for the compressed data in the thumbnail data.
		LPVOID pData = AllocData(dwCompressedSize);
		// Copy the data over.
		memcpy(pData, pCompressedData, dwCompressedSize);
		// Free the compressed data.
		Deflator.FreeData(pCompressedData);

		// GCB 2/5/99 - if we need to save an online thumbnail file, do it now
		if (!csOnlineThumbnailFile.IsEmpty())
		{
			csExtension = ".GIF";
			Util::ConstructPath(csOnlineThumbnailFile, csDrive, csDirectory, csFilename, csExtension);
			StorageFile File(csOnlineThumbnailFile);
			File.set_open_flags(CFile::modeCreate | CFile::modeWrite);
			if (File.initialize() == ERRORCODE_None)
			{
				int nBytesPerPixel = 1;	// 8-bit
				int nByteWidth = ((nWidth * nBytesPerPixel) + 3) & ~3;
				DWORD dwDataBytes = (DWORD)nByteWidth * (DWORD)nHeight;
				int nHeaderBytes = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
				LPBITMAPINFO pbi = (LPBITMAPINFO) Util::HugeAlloc(nHeaderBytes);
				if (pbi != NULL)
				{
					// Initialize the DIB header.
					pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					pbi->bmiHeader.biWidth = (LONG)nWidth;
					pbi->bmiHeader.biHeight = (LONG)nHeight;
					pbi->bmiHeader.biPlanes = 1;
					pbi->bmiHeader.biBitCount = 8;
					pbi->bmiHeader.biCompression = BI_RGB;
					pbi->bmiHeader.biSizeImage = dwDataBytes;
					pbi->bmiHeader.biXPelsPerMeter =
						pbi->bmiHeader.biYPelsPerMeter = 0;
					pbi->bmiHeader.biClrUsed =
						pbi->bmiHeader.biClrImportant = 0;
					COLORREF* pPalette = (COLORREF*)(pNewData + sizeof(WORD));
					memcpy(pbi->bmiColors, pPalette, 256*sizeof(RGBQUAD));
					int nEndOfPalette = sizeof(WORD) + 256 * sizeof(DWORD);
					BYTE* pDIBData = pNewData + nEndOfPalette;

					CGIFDataSinkDevice Sink(&File);
					CDIBtoGIFWriter Writer;
					TRY
					{
						Writer.CreateGIF(pbi, pDIBData, &Sink, FALSE, 255);
					}
					CATCH_ALL(e)
					{
					}
					END_CATCH_ALL

					Util::HugeFree((LPVOID)pbi);
				}
			}
		}

		// Free the data.
		Util::HugeFree((LPVOID)pNewData);
	}
	else
	{
		// we have a non-transparent bitmap with more than 255 colors,
		// or the reduction failed. Use JPEG compression.

		// Set the graphic info.
		m_DataHeader.SetGraphicInfo(nWidth, nHeight, nBits);
		// Compress it!
		TmpFile File("comp");
		if (File.initialize() == ERRORCODE_None)
		{
			CJPEGCompressor Compressor;
			if (Compressor.Init(&File, nWidth, nHeight) == ERRORCODE_None  &&
				 Compressor.WriteLines(pDIBData, nHeight) == ERRORCODE_None &&
				 Compressor.Finish() == ERRORCODE_None)
			{
				// We have the data in our temp file.
				ST_DEV_POSITION lLength;
				File.length(&lLength);
				m_DataHeader.SetCompressInfo(dwBytes, lLength, CThumbnailDataHeader::COMPRESSION_JPEG);
				LPVOID pData = AllocData(lLength);
				File.seek(0, ST_DEV_SEEK_SET);
				File.huge_read(pData, lLength);
			}
		}

		// GCB 2/5/99 - if we need to save an online thumbnail file, do it now
		if (!csOnlineThumbnailFile.IsEmpty())
		{
			csExtension = ".JPG";
			Util::ConstructPath(csOnlineThumbnailFile, csDrive, csDirectory, csFilename, csExtension);
			StorageFile File(csOnlineThumbnailFile);
			File.set_open_flags(CFile::modeCreate | CFile::modeWrite);
			if (File.initialize() == ERRORCODE_None)
			{
				TRY
				{
					CJPEGCompressor JPEGCompressor;   
					if (JPEGCompressor.Init(&File, pbi->bmiHeader.biWidth, pbi->bmiHeader.biHeight, 3) == ERRORCODE_None &&
						JPEGCompressor.WriteLines(pDIBData, pbi->bmiHeader.biHeight, pbi->bmiHeader.biHeight > 0) == ERRORCODE_None &&
						JPEGCompressor.Finish() == ERRORCODE_None)
						NULL;
				}
				CATCH_ALL(e)
				{
				}
				END_CATCH_ALL
			}
		}
	}
#endif
}

CFileThumbnails::CFileThumbnails(StorageDevice *pDirFile, StorageDevice *pDataFile)
: CItemNumDatabase(pDirFile, pDataFile)
  {
  }

CFileThumbnails::~CFileThumbnails()
  {
  }

// Content Collection methods
CFileContent::CFileContent(CCIndexFile *pCIndexFile, StorageDevice *pCFileData)
:  m_Dir(pCIndexFile), m_Data(pCFileData), 
  CSortedDatabase(&m_Dir, &m_Data)
  {
  }

CFileContent::~CFileContent()
  {
  }

// Items methods
CFileItems::CFileItems(StorageDevice *pDirFile, StorageDevice *pDataFile)
: CItemNumDatabase(pDirFile, pDataFile)
  {
  }

CFileItems::~CFileItems()
  {
  }

int CFileItems::GetItemRange(DWORD *pNewItemNumber)
   {
      ST_DEV_POSITION   stdevFileSize;
      ERRORCODE         stdevErrorCode;
      StorageDevice *   pDevice;

      pDevice = m_Dir.GetDevice();
      ASSERT(pDevice);
      stdevErrorCode = pDevice->length(&stdevFileSize);
      if(stdevErrorCode != ERRORCODE_None)
         ThrowErrorcodeException(stdevErrorCode);
      *pNewItemNumber = (stdevFileSize / sizeof(DWORD));
      return ERRORCODE_None;
   }

int CFileItems::GetNewItemNumber(DWORD *pNewItemNumber)
   {
		int error;
		if ((error = GetItemRange(pNewItemNumber)) == ERRORCODE_None)
			{
				(*pNewItemNumber)++;
			}
      return error;
   }

// DGP-Change return type to return code
DWORD CFileItems::Add(CItemsDataEntry *pDataEntry)
   {
      int      nRetVal;

      nRetVal = BuildDataItem(pDataEntry);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      return CItemNumDatabase::Add((CDataFileEntry *) pDataEntry);
   }

int CFileItems::Update(DWORD dwItemNum, CItemsDataEntry *pDataEntry)
   {
      int            nRetVal;

      nRetVal = BuildDataItem(pDataEntry);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      return CItemNumDatabase::Update(dwItemNum, pDataEntry);
   }

int CFileItems::Read()
   {
      char *            pData;
      int               nRetVal;
      USHORT            usKeywordCount, usDataSize, usKeywordDataSize;
      USHORT            usFileNameSize, usBufferSize;
      CString           strTemp;
      CItemsDataEntry   *pDataEntry = (CItemsDataEntry *)GetDataEntry();

      ASSERT(pDataEntry);

      if(pDataEntry == NULL)
         return ERRORCODE_BadParameter;

      usKeywordCount = pDataEntry->m_DataHeader.GetKeywordCount();
      usDataSize     = pDataEntry->m_DataHeader.GetSize();
      usKeywordDataSize = 0;

      // Check if any keywords associated with this item
      if(usKeywordCount > 0)
         {
            pDataEntry->m_carrayKeywordIDs.SetSize(usKeywordCount);
            usKeywordDataSize = pDataEntry->m_carrayKeywordIDs.GetSize() *
               pDataEntry->m_carrayKeywordIDs.GetElementSize();
            pDataEntry->Attach(pDataEntry->m_carrayKeywordIDs.GetData(), usKeywordDataSize);
//            nRetVal = CItemNumDatabase::Read(pDataEntry->GetDataSize());
            nRetVal = CItemNumDatabase::Read(usKeywordDataSize);

            // Detach data pointer associated with data object
            pDataEntry->FreeData();
            if(nRetVal != ERRORCODE_None)
               return nRetVal;
         }
      if(usDataSize)
         {
            usBufferSize = usDataSize - usKeywordDataSize;
            ASSERT(usBufferSize);

            // usBuffersSize is size of FileName and FriendlyName
            if(usBufferSize == 0)
               return ERRORCODE_Fail;    // Where's the rest of Data?

            // Read Filename and Friendly name into buffer            
            pData = new char [usBufferSize];
            pDataEntry->Attach(pData, usBufferSize);
            nRetVal = CItemNumDatabase::Read(usBufferSize);
            if(nRetVal != ERRORCODE_None)
               {
                  delete pData;
                  return nRetVal;
               }
            
            // Copy FileName portion which is first part of buffer, 
            // Assume it's NULL terminated
            pDataEntry->m_strFileName = (char *) pData;
            usFileNameSize = pDataEntry->m_strFileName.GetLength() + 1;

            // Maybe there is no friendly name ?, better check
            if(usBufferSize > usFileNameSize)
               {
                  // Copy friendly name
                  pDataEntry->m_strFriendlyName = (char *) &pData[usFileNameSize];
               }
            else
               pDataEntry->m_strFriendlyName.Empty();
            delete pData;
         }
      return ERRORCODE_None;
   }

// BuildDataItem allocates and fills in pDataEntry with prepared memory 
// structure containing Keyword IDs, filename, and friendly name
// in preparation for a Add/Update operation
int CFileItems::BuildDataItem(CItemsDataEntry *pDataEntry)
   {
      char     *pItemData;
      USHORT   usKeywordDataSize, usDataSize;
      int      nFileNameBuffSize;

      ASSERT(pDataEntry);
      ASSERT(!pDataEntry->m_strFileName.IsEmpty());
      if(pDataEntry == NULL || pDataEntry->m_strFileName.IsEmpty())
         return ERRORCODE_BadParameter;

      // Data size is size of Keyword IDs plus Friendly Name
      usKeywordDataSize = pDataEntry->m_carrayKeywordIDs.GetSize() *
         pDataEntry->m_carrayKeywordIDs.GetElementSize();
      usDataSize = usKeywordDataSize +
         (pDataEntry->m_strFileName.GetLength() + 1) +
         (pDataEntry->m_strFriendlyName.GetLength() + 1);

      pDataEntry->m_DataHeader.SetKeywordCount(
         pDataEntry->m_carrayKeywordIDs.GetSize());

      pDataEntry->AllocData(usDataSize);
      pItemData = (char *) pDataEntry->GetData();
      ASSERT(pItemData);
      if(!pItemData)
         return ERRORCODE_BadParameter;

      // Copy Keyword IDs into buffer
      if(usKeywordDataSize)
         memcpy(pItemData, pDataEntry->m_carrayKeywordIDs.GetData(),
            usKeywordDataSize);

      // Copy Filename into next portion of buffer
      nFileNameBuffSize = pDataEntry->m_strFileName.GetLength() + 1;
      memcpy(&pItemData[usKeywordDataSize],
         pDataEntry->m_strFileName.GetBuffer(1),
         nFileNameBuffSize);
      pDataEntry->m_strFileName.ReleaseBuffer();

      // Copy Friendly name into next portion of buffer
      memcpy(&pItemData[usKeywordDataSize + nFileNameBuffSize],
         pDataEntry->m_strFriendlyName.GetBuffer(1),
         pDataEntry->m_strFriendlyName.GetLength() + 1);
      pDataEntry->m_strFriendlyName.ReleaseBuffer();
      pItemData[usDataSize-1] = 0;

      pDataEntry->m_DataHeader.SetSize(usDataSize);

      return ERRORCODE_None;
   }

// Keyword methods
CFileKeywordDir::CFileKeywordDir(CCIndexFile *pIndexFile) :
   CSortedDir(pIndexFile)
   {
      m_dwKeywordID = 0;
   }

CFileKeywordDir::~CFileKeywordDir()
  {
  }

void CFileKeywordDir::GetIndexList(CCIndexIndexList *pIndexList)
   {
      ASSERT(pIndexList);
      pIndexList->SetIndexType(1, CCIndexIndexList::stringKey);
      pIndexList->SetIndexType(2, CCIndexIndexList::longKey);
   }

int CFileKeywordDir::Find(CString &strKeyword, CKeywordDirEntry *pDirEntry,
   BOOL bReadNow)
   {
      int               nRetVal;
      int               nThisRetVal = ERRORCODE_Fail;
      int               nDataSize;
      CDataFileEntry    &clientDataEntry=pDirEntry->GetDataEntry();
   
      ASSERT(m_pIndexFile);

      if(m_pIndexFile == NULL)
         return ERRORCODE_BadParameter;

      pDirEntry->SetKey(strKeyword);

      nRetVal = CSortedDir::Find((CSortedDirEntry *) pDirEntry, FALSE);

      if(nRetVal == ERRORCODE_None)
         {
            // Release memory through CVarArray 
            pDirEntry->m_carrayItemNumbers.RemoveAll();
         
            nDataSize = m_pIndexFile->GetEntrySize();

            // If No items associated with keyword, nothing to read
            if(nDataSize <= 0)
               {
                  pDirEntry->m_carrayItemNumbers.RemoveAll();
                  return ERRORCODE_None;
               }

            // Allocate memory through CVarArray 
            pDirEntry->m_carrayItemNumbers.SetSize( nDataSize /
               pDirEntry->m_carrayItemNumbers.GetElementSize());

            // Let data object directly fill in carray of items
            clientDataEntry.Attach(
               pDirEntry->m_carrayItemNumbers.GetData(),
               nDataSize);

            nRetVal = CSortedDir::Find((CSortedDirEntry *)pDirEntry, bReadNow);
            nThisRetVal = ERRORCODE_None;
         }
      else if(nRetVal == CCIndexFile::fileNotOpen)
         return ERRORCODE_Open;

      return nThisRetVal;
   }

//
// Find the first keyword match.
//

int CFileKeywordDir::FindFirst(CString &strKeyword, CKeywordDirEntry *pDirEntry, BOOL bReadNow /*=TRUE*/)
{
	// Determine whether we have a wildcard or not.
	int nLength = strKeyword.GetLength();
	if (nLength == 0)
	{
		// Empty string.
		ASSERT(FALSE);
		return ERRORCODE_BadParameter;
	}

	BOOL fWildCard = (strKeyword[nLength-1] == '*');
	CString csSearch = fWildCard ? strKeyword.Left(nLength-1) : strKeyword;

   if (csSearch.IsEmpty())
   {
		return ERRORCODE_BadParameter;
   }

	int               nRetVal;
	int               nThisRetVal = ERRORCODE_Fail;
	int               nDataSize;
	CDataFileEntry    &clientDataEntry=pDirEntry->GetDataEntry();

   ASSERT(m_pIndexFile);

	if (m_pIndexFile == NULL)
	{
		return ERRORCODE_BadParameter;
	}

	pDirEntry->SetKey(csSearch);

	nRetVal = CSortedDir::Find((CSortedDirEntry *) pDirEntry, FALSE, fWildCard);

	CString csFoundKey = csSearch;

	if (nRetVal == ERRORCODE_KeyGreaterFound)
	{
		// We found a key greater than our search string.
		// See if it is a match or not.
		m_pIndexFile->GetCurrentKey(csFoundKey);

		// Our wildcard stuff is really simple at this stage.
		// The wildcard character ('*') must always come at the end.
		// This resolves to a prefix search.
		if (csFoundKey.Left(nLength-1).CompareNoCase(csSearch) == 0)
		{
			nRetVal = ERRORCODE_None;
		}
	}

	if (nRetVal == ERRORCODE_None)
	{
//		TRACE(">>> (First of '%s') Found '%s' <<<\n", (LPCSTR)csSearch, (LPCSTR)csFoundKey);

		// Release memory through CVarArray 
		pDirEntry->m_carrayItemNumbers.RemoveAll();

		nDataSize = m_pIndexFile->GetEntrySize();

		// If No items associated with keyword, nothing to read
		if (nDataSize <= 0)
		{
			pDirEntry->m_carrayItemNumbers.RemoveAll();
			return ERRORCODE_None;
		}

		// Allocate memory through CVarArray 
		pDirEntry->m_carrayItemNumbers.SetSize(nDataSize /
															pDirEntry->m_carrayItemNumbers.GetElementSize());

		// Let data object directly fill in carray of items
		clientDataEntry.Attach(pDirEntry->m_carrayItemNumbers.GetData(), nDataSize);

		if (bReadNow)
		{
			nRetVal = Read();
		}
//		nRetVal = CSortedDir::Find((CSortedDirEntry *)pDirEntry, bReadNow);
		nThisRetVal = ERRORCODE_None;
	}
	else if (nRetVal == CCIndexFile::fileNotOpen)
	{
		nThisRetVal = ERRORCODE_Open;
	}

	return nThisRetVal;
}

//
// Find the next keyword match.
//

int CFileKeywordDir::FindNext(CString &strKeyword, CKeywordDirEntry *pDirEntry, BOOL bReadNow /*=TRUE*/)
{
	int nLength = strKeyword.GetLength();
	if (nLength == 0)
	{
		// Empty string.
		ASSERT(FALSE);
		return ERRORCODE_BadParameter;
	}

	// We can only do a "next" if we have a wildcard in the string.
	// Otherwise, you only get one match.

	int nThisRetVal = ERRORCODE_Fail;
	BOOL fWildCard = (strKeyword[nLength-1] == '*');
	if (fWildCard)
	{
   	CDataFileEntry    &clientDataEntry=pDirEntry->GetDataEntry();
		CString csSearch = strKeyword.Left(nLength-1);

      // If Data object owns memory, release it so we can allocate a new one
      // with new data
      if (pDirEntry->GetDataEntry().OwnData())
		{
         pDirEntry->GetDataEntry().FreeData();
		}

		int nRetVal = m_pIndexFile->GetNextEntry(NULL, 0);

		if (nRetVal == CCIndexFile::success)
		{
			// We found a key greater than our search string.
			// See if it is a match or not.
			CString csFoundKey;
			m_pIndexFile->GetCurrentKey(csFoundKey);

			// Our wildcard stuff is really simple at this stage.
			// The wildcard character ('*') must always come at the end.
			// This resolves to a prefix search.
			if (csFoundKey.Left(nLength-1).CompareNoCase(csSearch) == 0)
			{
//				TRACE(">>> (Next of '%s') Found '%s' <<<\n", (LPCSTR)csSearch, (LPCSTR)csFoundKey);

				// Fill in directory with keyword and offset contained
				// in directory
				pDirEntry->SetOffsetToData(m_pIndexFile->GetRecordNum());

				// Release memory through CVarArray 
				pDirEntry->m_carrayItemNumbers.RemoveAll();

				int nDataSize = m_pIndexFile->GetEntrySize();

				// If No items associated with keyword, nothing to read
				if (nDataSize <= 0)
				{
					pDirEntry->m_carrayItemNumbers.RemoveAll();
					return ERRORCODE_None;
				}

				// Allocate memory through CVarArray 
				pDirEntry->m_carrayItemNumbers.SetSize(nDataSize /
																	pDirEntry->m_carrayItemNumbers.GetElementSize());

				// Let data object directly fill in carray of items
				clientDataEntry.Attach(pDirEntry->m_carrayItemNumbers.GetData(), nDataSize);

				nThisRetVal = ERRORCODE_None;

				if (bReadNow)
				{
					nThisRetVal = Read();
				}
			}
		}
	}

	return nThisRetVal;
}

int CFileKeywordDir::Find(DWORD dwKeywordID, CString &strKeyword)
   {
      int               nRetVal;
      int               nThisRetVal = ERRORCODE_Fail;
      int               nDataSize;
      CDataFileEntry    tempDataEntry;
   
      ASSERT(m_pIndexFile);

      if(m_pIndexFile == NULL)
         return ERRORCODE_BadParameter;

      // Don't know record number (Data Offset)
      nRetVal = m_pIndexFile->Find(dwKeywordID, NULL, 0, 0L, KEYWORDID_INDEX);

      if(nRetVal == CCIndexFile::success || nRetVal == CCIndexFile::keyMatch)
      {
         // Get size of data item associated with Keyword ID key
         nDataSize = m_pIndexFile->GetEntrySize();

         // Missing data item
         ASSERT( !(nDataSize <= 0) );
         if(nDataSize <= 0)
            return ERRORCODE_Fail;

         // Allocate memory for temporary data object
         tempDataEntry.AllocData(nDataSize);

         // Fill in temp data object with keyword stored in data portion of key
         nRetVal = m_pIndexFile->GetCurrentEntry(tempDataEntry.GetData(), 
            (USHORT)tempDataEntry.GetDataSize(), KEYWORDID_INDEX);

         // Copy keyword and unique keyword ID into callers data object
         if(nRetVal == CCIndexFile::success)
            {
               strKeyword = CString((char *)(tempDataEntry.GetData()));
               nThisRetVal = ERRORCODE_None;
            }
      }
      else if(nRetVal == CCIndexFile::fileNotOpen)
         return ERRORCODE_Open;

      return nThisRetVal;
   }

// Deletes an Item associated with the current keyword from last Find()
int CFileKeywordDir::Delete(DWORD dwItemNumToDelete)
   {
      int                     nNumItems, nDataSize, i, nRetVal;
      DWORD                   dwItemNum;
      CKeywordDirEntry *      pCurDirEntry = (CKeywordDirEntry *)GetEntry();
      LPVOID                  pData;

      ASSERT(pCurDirEntry);
      ASSERT(m_pIndexFile);
      if(pCurDirEntry == NULL || m_pIndexFile == NULL)
         return ERRORCODE_BadParameter;
      nNumItems = pCurDirEntry->m_carrayItemNumbers.GetSize();
      for(i=0;i<nNumItems;i++)
         {
            pCurDirEntry->m_carrayItemNumbers.GetData(),
            dwItemNum = (*(DWORD *)pCurDirEntry->m_carrayItemNumbers[i]);
            // Delete item in Keyword file
            if(dwItemNum == dwItemNumToDelete)
               {
                  // Match, remove it from list
                  pCurDirEntry->m_carrayItemNumbers.RemoveAt(i);
                  // break out since same item should occur twice
                  break;
               }
         }
      nDataSize = pCurDirEntry->m_carrayItemNumbers.GetElementSize() * 
         pCurDirEntry->m_carrayItemNumbers.GetSize();

      // Reattach updated items list
#if 0
      pCurDirEntry->Attach(
         pCurDirEntry->m_carrayItemNumbers.GetData(),
         nDataSize);
#endif

      // Update data associate with current key (from last Find call)
      if(nDataSize)
         pData = pCurDirEntry->m_carrayItemNumbers.GetData();
      else
         pData = NULL;
      nRetVal = m_pIndexFile->SetCurrentEntry(pData, nDataSize);

      return ERRORCODE_None;
   }

DWORD CFileKeywordDir::Add(CString &strKeyword, DWORD dwItemNum)
   {
      CKeywordDirEntry  tempDirEntry;
      tempDirEntry.SetKey(strKeyword);
      tempDirEntry.m_carrayItemNumbers.Add(&dwItemNum);
      return Add(&tempDirEntry);
   }

// DGP-Change return type to return code
DWORD CFileKeywordDir::Add(CKeywordDirEntry *pDirEntry)
   {
      DWORD             *pdwaItems=NULL, dwKeywordID;
      int               nRetVal, i, nDataSize, nCurEntryIndex, nCurEntrySize;
      CKeywordDirEntry  curDirEntry;
      CDataFileEntry    &clientDataEntry=pDirEntry->GetDataEntry();
      CString           strKeyword = pDirEntry->GetKey();

      // Find without trying to read data associated with key
      nRetVal = Find(strKeyword, &curDirEntry, FALSE);

      if(nRetVal != ERRORCODE_None)
         {
            // New Keyword, add it with item(s)

            // Must add secondary key first to get unique keyword ID
            nRetVal = AddSecondaryKey(strKeyword);
            if(nRetVal != CCIndexFile::success)
               return ERRORCODE_Fail;
            
            // Let data object directly fill in carray of items
            clientDataEntry.Attach(
               pDirEntry->m_carrayItemNumbers.GetData(),
               pDirEntry->m_carrayItemNumbers.GetElementSize() * 
               pDirEntry->m_carrayItemNumbers.GetSize());

            pDirEntry->SetOffsetToData(m_dwKeywordID);
            nRetVal = CSortedDir::Add((CSortedDirEntry *)pDirEntry);
            clientDataEntry.Empty();
            if(nRetVal != CCIndexFile::success)
               return ERRORCODE_Fail;

            dwKeywordID = pDirEntry->GetOffsetToData();
         }
      else
         {
            // Key exists, update data item associated with it
            nDataSize = m_pIndexFile->GetEntrySize();

            // If keyword has item numbers associated with it, read them
            if(nDataSize > 0)
               {
                  // Allocate memory for current data object associated with key
                  curDirEntry.m_carrayItemNumbers.SetSize( nDataSize /
                     curDirEntry.m_carrayItemNumbers.GetElementSize());
      #if 0
                  // Let data object directly fill in carray of items
                  clientDataEntry.Attach(
                     (void *)pDirEntry->m_carrayItemNumbers.GetData(),
                     nDataSize);
      #endif
                  // Fill in data and CVarArray objects
                  Find(strKeyword, &curDirEntry, TRUE);
               }
            else
               curDirEntry.m_carrayItemNumbers.RemoveAll();

            dwKeywordID = curDirEntry.GetOffsetToData();

            nCurEntrySize = curDirEntry.m_carrayItemNumbers.GetSize();
            for(i=0;i<pDirEntry->m_carrayItemNumbers.GetSize();i++)
               {
                  // First check to make sure item number isn't already in list
                  for(nCurEntryIndex = 0; nCurEntryIndex < nCurEntrySize;nCurEntryIndex++)
                     {
                        // If already number already in list, no need to add it
                        if(pDirEntry->m_carrayItemNumbers[i] ==
                           curDirEntry.m_carrayItemNumbers[nCurEntryIndex])
                              continue;   
                     }
                  curDirEntry.m_carrayItemNumbers.Add(
                     pDirEntry->m_carrayItemNumbers[i]);
               }

            nRetVal = m_pIndexFile->SetCurrentEntry(
               curDirEntry.m_carrayItemNumbers.GetData(),
               curDirEntry.m_carrayItemNumbers.GetSize() *
                 curDirEntry.m_carrayItemNumbers.GetElementSize());

            if(nRetVal != CCIndexFile::success)
               return ERRORCODE_Fail;
         }
      if(nRetVal == CCIndexFile::success)
         return dwKeywordID;
      return (DWORD) -1L;
   }

// This member adds a new key and data item to keyword index file
int CFileKeywordDir::AddSecondaryKey(CString &strKeyword)
   {
      int   nRetVal;

      m_dwKeywordID = 0;

      // Get unique key ID from index header
      nRetVal = m_pIndexFile->GetCustomInfo(&m_dwKeywordID, sizeof(m_dwKeywordID));
      m_dwKeywordID++;
      nRetVal = m_pIndexFile->SetCustomInfo(&m_dwKeywordID,
         sizeof(m_dwKeywordID));

      // Store users data without keyword portion in data record
      // since key will contain the keyword
      nRetVal = m_pIndexFile->Add(m_dwKeywordID, strKeyword.GetBuffer(1),
         strKeyword.GetLength()+1, 1, KEYWORDID_INDEX);
      strKeyword.ReleaseBuffer();
      if(nRetVal != CCIndexFile::success)
         return ERRORCODE_Fail;

      return ERRORCODE_None;
   }

DWORD CFileKeywordDir::GetKeywordCount()
   {
      DWORD       dwKeywordCount=0;
      int         nRetVal;

      // Get unique key ID from index header
      nRetVal = m_pIndexFile->GetCustomInfo(&dwKeywordCount, sizeof(DWORD));
      return dwKeywordCount;
   }

