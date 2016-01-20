#ifndef _FREELIST_INC
#define _FREELIST_INC

#include "cindex.h"
#include "util.h"

// CFreeListEntries must be stored differently before changing this #define
// See SaveFileList in freelist.cpp for more info.
#define MAX_FREELIST_ENTRIES  6

// DO NOT add virtual functions to this class
// This class is used internally by CFreeLists
class CFreeListEntries
   {
   public:
      CFreeListEntries();
      void        SetNumFiles(SHORT nNumFiles)     { m_nNumFiles = nNumFiles; }
      void        SetFileName(SHORT nFileNameIndex, CString strFileName);
      CString     GetFileName(SHORT nFileNameIndex);
      USHORT      GetNumFiles()                    { return m_nNumFiles; }
   // AddEntry() Returns handle to new entry if successful, -1 if error
      SHORT       AddEntry(CString strFileName);
   protected:
      SHORT       m_nNumFiles;
      char        m_szFileName[MAX_FREELIST_ENTRIES][13];
   };

// CFreeLists provides a freelist management system
// Methods can throw exceptions of type CCIndexFileException
class CFreeLists
   {
   public:
      CFreeLists();
      virtual ~CFreeLists();

   // GetIndexList Fills in callers object with index list info. to be used
   // with opening file.
      static void GetIndexList(CCIndexIndexList *pIndexList);

   // Initialize object
      int      Init(CCIndexFile *pFreeListFile);

   // CreateList() creates a free list associated with a specific file
   // Returns handle to new entry
   // Throws CCIndexFileException on error
      SHORT    CreateList(CString strFileName);

   // FindList() locates free list associated with a specific file
   // Returns handle to list, -1 if fails
      SHORT    FindList(CString strFileName);

   // Adds a block to the free list
   // Block Size and Offset pair must be unique
      int      AddBlock(SHORT nHandle, DWORD dwBlockSize, DWORD dwOffset);

   // Locates a free block.  If successful returns offset to free block and 
   // automatically deletes block from File List
   // Returns fail if no block found matching specfied size
      int      FindBlock(SHORT nHandle, DWORD dwBlockSize, DWORD * pdwFoundOffset);

   // Removes a block from the free list.  Normally used internally by this class
      int      DeleteBlock(SHORT nHandle, DWORD dwBlockSize);

   protected:
   // Internal helpers
      int      LoadFileList();
      int      SaveFileList();
      CString  GetShortFileName(CString strFileName);
      int      DeleteKeys(SHORT nBlockHandle, DWORD dwBlockSize,
                  DWORD dwOffset, BOOL bLocateBlockKey, BOOL bLocateOffsetKey);
   protected:
      BOOL              m_bLoadedList;
      CCIndexFile *     m_pIndexFile;
      CFreeListEntries  m_FileList;
   };

#pragma pack(1)   // Pack structures 
// Internally used by free list class
class COffsetKey
   {
   public:
      COffsetKey()
         {
            m_byKeySize = sizeof(COffsetKey) - sizeof(m_byKeySize);
         }
      void     SetOffset(DWORD dwNewOffset)
         {
            m_dwOffset = dwNewOffset;
            Util::SwapDWORD(&m_dwOffset);
         }
      DWORD    GetOffset()
         {
            DWORD    dwNormal = m_dwOffset;
            Util::SwapDWORD(&dwNormal);
            return dwNormal;
         }
      void     SetID(BYTE byID)
         {
            m_byID = byID;
         }
      void     SetID(SHORT nID)
         {
            SetID((BYTE) nID);
         }
      BYTE     GetID()
         {
            return m_byID;
         }
   protected:
      BYTE     m_byKeySize;
      BYTE     m_byID;
      DWORD    m_dwOffset; // In Flipped Form
   };
#pragma pack()

#endif   // end of include

