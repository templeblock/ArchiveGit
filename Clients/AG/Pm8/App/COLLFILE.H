#ifndef _COLLFILE_INC
#define _COLLFILE_INC

#include "cindex.h"
#include "dbstore.h"
#include "carray.h"
#include "freelist.h"

#pragma pack(1)   // Pack all structures used in this file

class CDataFileEntry
   {
   public:
      CDataFileEntry();
      virtual ~CDataFileEntry()                    { FreeData(); }

      virtual LPVOID    GetData()                  { return m_pData; }

   // Returns size of memory data object 
      virtual DWORD     GetDataSize()              { return m_dwDataSize; }

   // Override GetItemSize() to tell this object how to calc. size
   // of data object
      virtual DWORD     GetItemSize()              { return GetDataSize(); }

   // Does object currently own data
      BOOL              OwnData()                  { return m_bOwnData; }

   // Inits object
      virtual void      Empty()              { FreeData(); }

   // Client can own memory for data by using Attach() which
   // will assume client owns memory when called
      virtual LPVOID    Attach(LPVOID pData, DWORD dwSize);

      virtual LPVOID    Detach();

   // Allocate memory for data associated with this object, not used
   // if using Attach()
      virtual LPVOID    AllocData(DWORD dwSize);

   // Call FreeData() to free memory associated with this object
      void              FreeData();

      virtual int       GetHeaderSize()      { return 0;    }
      virtual LPVOID    GetHeader()          { return NULL; }

   // Override this to create a new object of this type
      virtual CDataFileEntry * CreateObject() { return new CDataFileEntry; }

   protected:
      LPVOID            m_pData;
      DWORD             m_dwDataSize;
      BOOL              m_bOwnData;
      // Follow with your own custom header data (if any) in derived class
   };

// Abstract for describing a sorted directory entry
class CSortedDirEntry
   {
   protected:
      CString           m_strKey;
    // This member is used for interfacing with data attached to key
      CDataFileEntry    m_DataEntry;
   public:
      CSortedDirEntry()                         { int i=0; }
   // Get operations
      virtual CString   GetKey() const
         {
            return m_strKey;
         }
      virtual long      GetOffsetToData()  = 0;
      CDataFileEntry &  GetDataEntry()          { return m_DataEntry; }

   // Set operations
      virtual void      SetKey(const CString &strKey)
         {
            m_strKey = strKey;
         }
      virtual long      SetOffsetToData(long lNewOffset)  = 0;
   };

class CSortedDir
   {
   public:
      CSortedDir(CCIndexFile *pIndexFile);
      ~CSortedDir();

   // Fills in callers CSortedDirEntry object with offset, ect.
      virtual int       Find(CSortedDirEntry *pDirEntry, BOOL bReadNow=TRUE, BOOL bAllowPartial=FALSE);

   // Read will read into directory entry object supplied with Find()
      virtual int       Read();

   // Adds current directory entry associated with this object
      virtual int       Add(CSortedDirEntry *pDirEntry);

   // Deletes entry from last Find() operation
      virtual int       Delete();

      CSortedDirEntry * GetEntry();
   protected:
      CCIndexFile *     m_pIndexFile;
      CSortedDirEntry * m_pDirEntry;
      BOOL              m_bFound;
   };

class CItemNumDir
   {
   public:
      CItemNumDir(StorageDevice *pDirFile);
      ~CItemNumDir();

      virtual int       Find(DWORD dwItemNum);
      virtual DWORD     Add(DWORD dwDataOffset); // returns new item number
      long              GetOffsetToData()    { return m_lDataOffset; }
      StorageDevice *   GetDevice()          { return m_pFile; }
      virtual int       Delete();
      virtual int       Update(DWORD dwItemNum, DWORD dwNewDataOffset);
   protected:
      StorageDevice *   m_pFile;
      ERRORCODE         m_LastError;
      long              m_lDataOffset;
      DWORD             m_dwCurItemNum;
   };

// CFileData provides access methods to a data file that may have
// a header preceed each data item.  
class CFileData
   {
   public:
      CFileData(StorageDevice *cfileObject);
      ~CFileData();

   // Find() wants file offset to start of data header
      virtual int       Find(DWORD dwStartOfData, CDataFileEntry *pDataEntry);

   // Add will use current header (if any) in CDataFileEntry object
   // and will leave data members as is
      DWORD             Add(CDataFileEntry *pDataEntry);
   
   // Read() reads specified number of bytes of data item
      virtual int       Read(DWORD dwBytesToRead=0);

   // Seek() uses a 0 based offset to data item (not file offset)
      virtual int       Seek(DWORD dwDataOffset);

   // SetFreeList()
      virtual int       SetFreeList(CFreeLists *pFreeList);

   // Delete() requires one call to SetFreeList() to init. freelist
      virtual int       Delete();

   // Update() requires one call to SetFreeList() to init. freelist
   // Successful completion updates pdwNewDataOffset parameter with 
   // new offset to data item
      virtual int       Update(DWORD dwStartOfData, CDataFileEntry *pDataEntry,
                           DWORD *pdwNewDataOffset);

   // GetItemSize() returns size of current data object
      DWORD             GetItemSize();

      int               ReadHeader();

      CDataFileEntry *  GetEntry() { return m_pDataEntry; }
      StorageDevice *   GetDevice()          { return m_pCFile; }

   protected:
      long              m_lOffsetToDataStart;
      long              m_lCurOffset;
      CDataFileEntry *  m_pDataEntry;
      StorageDevice *   m_pCFile;
      CFreeLists *      m_pFreeList;
   };

// CSortedDBDirEntry describes a Sorted directory entry that is used with
// database access
class CSortedDBDirEntry : public CSortedDirEntry
   {
   protected:
   public:
      enum StorageMethod
         {
            storageInternal = 1,
            storageExternal = 2
         };
      CSortedDBDirEntry()                         { }
      virtual void            SetStorageMethod(StorageMethod storageMethod) = 0;
   // Get operations
      virtual StorageMethod   GetStorageMethod() = 0;
   };

// CSortedDatabase provides an interface to an alphabetically sorted database 
// which has directory and data files
class CSortedDatabase
   {
   public:
      CSortedDatabase(CSortedDir *pDir, CFileData *pData);

      virtual int       Add(CSortedDBDirEntry *pDirEntry, CDataFileEntry *pDataEntry);

      virtual int       Find(CSortedDBDirEntry *pDirEntry, CDataFileEntry *pDataEntry);

   // Seeks to specified position within data item
      int               Seek(DWORD dwDataOffset);
   // Reads data into CDataFileEntry object (entire object using default parameter)
      int               Read(DWORD dwBytesToRead=0);

   // Deletes current item from last Find()
      int               Delete();

//      virtual int       Update(CSortedDBDirEntry *pDirEntry, CDataFileEntry *pDataEntry);

      DWORD             GetItemSize()        { return m_pData->GetItemSize(); }
      virtual int       SetFreeList(CFreeLists *pFreeList)
         {
            ASSERT(m_pData);
            return m_pData->SetFreeList(pFreeList);
         }
   protected:
      CSortedDir     *m_pDir;
      CFileData      *m_pData;
      long           m_lDataOffset;
   };

// Content file Classes
class CContentDirEntry : public CSortedDBDirEntry
   {
   protected:
      long           m_dwOffset;
      long           m_dwItemNum;
// FF: DWORD m_dwTimeStamp was phased out starting in PM 5.0
// This data will be a time_t in collections built with PMG 4.0
// and zero in collections built with 5.0
//		DWORD          m_dwTimeStamp;
      DWORD          m_dwUnused;
      SHORT          m_storageMethod;  // SHORT vs. enum?
   public:
      CContentDirEntry()
         {
            // This tells Directory object size of our constant data item
            m_DataEntry.Attach(&m_dwItemNum,
               sizeof(m_dwItemNum) + sizeof(m_dwUnused) +
               + sizeof(m_storageMethod));
            SetStorageMethod(storageInternal);
         }
   // Set operations
      virtual long   SetOffsetToData(long lNewOffset)
         { return (m_dwOffset = lNewOffset); }
// FF: TimeStamps in collections were phased out starting in PM 5.0
//		DWORD          SetTimeStamp(DWORD dwNewTime)
//			{ return (m_dwTimeStamp = dwNewTime); }
		DWORD          SetItemNum(DWORD dwItemNum)
			{ return m_dwItemNum = dwItemNum; }
      virtual void   SetStorageMethod(StorageMethod storageMethod)
         { m_storageMethod = storageMethod; }
   // Get operations
      virtual long      GetOffsetToData()       { return m_dwOffset; }
// FF: TimeStamps in collections were phased out starting in PM 5.0
//		DWORD             GetTimeStamp()          { return m_dwTimeStamp; }
      DWORD             GetItemNum()            { return m_dwItemNum; }
      virtual StorageMethod  GetStorageMethod()
         { return (StorageMethod) m_storageMethod; }
   };

class CCompressInfo
  {
  public:
    CCompressInfo()                    { Empty(); }
    void       Set(long lUnCompressedSize, long lCompressedSize, 
      unsigned short shCompressionType)
      {
		  ASSERT( lCompressedSize <= lUnCompressedSize );
        m_lUnCompressedSize = lUnCompressedSize;
        m_lCompressedSize   = lCompressedSize;
        m_shCompressionType = shCompressionType;
      }
    long       GetCompressedSize()     { return m_lCompressedSize; }
    long       GetUnCompressedSize()   { return m_lUnCompressedSize; }
    unsigned short GetType()           { return m_shCompressionType; }
    void Empty()
    {
       m_lUnCompressedSize = m_lCompressedSize = 0;
       m_shCompressionType = 0;
    }
  protected:
    long            m_lUnCompressedSize;
    long            m_lCompressedSize;
    unsigned short  m_shCompressionType;
  };

// This class describes data format for Content object
class CContentDataEntry : public CDataFileEntry
   {
   public:
      CCompressInfo   m_DataHeader;

   public:
      CContentDataEntry()
         { m_DataHeader.Empty(); }

      virtual CDataFileEntry * CreateObject() { return new CContentDataEntry; }

   // Override GetItemSize() to tell this object how to calc. size
   // of data object
      virtual DWORD     GetItemSize()
         { return m_DataHeader.GetCompressedSize(); }

      virtual int       GetHeaderSize()      { return sizeof(m_DataHeader); }
      virtual LPVOID    GetHeader()          { return &m_DataHeader; }
   };

// This class describes to Offset Collection interface
class CItemNumDatabase
   {
   public:
      CItemNumDatabase(StorageDevice *pCFileDir, StorageDevice *pCFileData) :
         m_Dir(pCFileDir), m_Data(pCFileData)
         { }

      virtual DWORD     Add(CDataFileEntry *pDataEntry);

      virtual int       Find(DWORD dwItemNum, CDataFileEntry *pDataEntry);

   // Use these if find is successful
   // Seeks to specified position within data item
      int               Seek(DWORD dwDataOffset);
   // Reads data
      int               Read(DWORD dwBytesToRead=0);

      virtual int       SetFreeList(CFreeLists *pFreeList)
         {
            ASSERT(pFreeList);
            return m_Data.SetFreeList(pFreeList);
         }

      virtual int       Delete ();

      virtual int       Update(DWORD dwItemNum, CDataFileEntry *pDataEntry);

      // Use these if find is successful
      DWORD             GetItemSize()     { return m_Data.GetItemSize(); }
      CDataFileEntry *  GetDataEntry()    { return m_Data.GetEntry();    }
   protected:
      CItemNumDir         m_Dir;
      CFileData           m_Data;
      long                m_lDataOffset;      
   };

// CFileContent represents the Content Database interface
class CFileContent : public CSortedDatabase
   {
   public:
      CFileContent(CCIndexFile *pCIndexFile, StorageDevice *pDataFile);
      ~CFileContent();

      virtual int Add(CContentDirEntry *pDirEntry, CContentDataEntry *pDataEntry)
         {
            return CSortedDatabase::Add((CSortedDBDirEntry *) pDirEntry, (CDataFileEntry *)pDataEntry);
         }
      virtual int Find(CContentDirEntry *pDirEntry, CContentDataEntry *pDataEntry)
         {
           return CSortedDatabase::Find((CSortedDBDirEntry *)pDirEntry, (CDataFileEntry *) pDataEntry);
         }
   protected:
      CSortedDir              m_Dir;
      CFileData               m_Data;
   };

// Thumbnail Collection Classes
class CThumbnailDataHeader
  {
  protected:
    class CCompressInfo   m_CompressInfo;
    unsigned short        m_usWidth;
    unsigned short        m_usHeight;
    unsigned short        m_usBitsPerPixel;
  public:
    CThumbnailDataHeader()       { Empty(); }
    void SetCompressInfo(long lUnCompressSize, long lCompressedSize,
      unsigned short shCompressionType)
      {
        m_CompressInfo.Set(lUnCompressSize, lCompressedSize,shCompressionType);
      }
    void SetGraphicInfo(unsigned short usWidth, unsigned short usHeight,
      unsigned short usBitsPerPixel)
      {
        m_usWidth   = usWidth;
        m_usHeight  = usHeight;
        m_usBitsPerPixel = usBitsPerPixel;
      }
	 enum
	 {
      COMPRESSION_None				= 0,
		COMPRESSION_JPEG				= 1,
		COMPRESSION_Deflate			= 2,
		COMPRESSION_ReducedDeflate	= 3		// Deflate with 256-color palette.
	 };
    unsigned short      GetWidth()      { return m_usWidth; }
    unsigned short      GetHeight()     { return m_usHeight; }
    unsigned short      GetBitsPerPixel()     { return m_usBitsPerPixel; }
    CCompressInfo * GetCompressInfo() { return &m_CompressInfo; }
    void Empty()
    {
         m_usWidth = m_usHeight = m_usBitsPerPixel = 0;
         m_CompressInfo.Empty();
    }
  };

// This class describes Thumbnail data format 
class CThumbnailDataEntry : public CDataFileEntry
   {
   protected:
   public:
      CThumbnailDataHeader  m_DataHeader;

   public:
      CThumbnailDataEntry()
         { m_DataHeader.Empty(); }
   // Override GetItemSize() to tell this object how to calc. size
   // of data object
      virtual DWORD     GetItemSize()
         {
            CCompressInfo * pCompressInfo = m_DataHeader.GetCompressInfo();
            if(pCompressInfo)
               return pCompressInfo->GetCompressedSize();
            return 0; // error
         }
      virtual void Empty()
         {
            m_DataHeader.Empty();
            CDataFileEntry::Empty();
         }

      virtual CDataFileEntry * CreateObject() { return new CThumbnailDataEntry; }

      virtual int       GetHeaderSize()      { return sizeof(m_DataHeader); }
      virtual LPVOID    GetHeader()          { return &m_DataHeader; }
		void SetThumbnailData(LPBITMAPINFO pbi, LPBYTE pDIBData, LPBYTE pTransMaskData,
									 int nWidth, int nHeight,
									 int nBits, DWORD dwBytes,
									 CString csOnlineThumbnailFile);
   };

class CFileThumbnails : public CItemNumDatabase
   {
   public:
       CFileThumbnails(StorageDevice *pDirFile, StorageDevice *pFileData);
       ~CFileThumbnails();

      
      virtual DWORD    Add(CThumbnailDataEntry* pDataEntry)
         { return CItemNumDatabase::Add((CDataFileEntry *) pDataEntry); }

      virtual int      Find(DWORD dwItemNum, CDataFileEntry *pDataEntry)
         { return CItemNumDatabase::Find(dwItemNum, (CDataFileEntry *) pDataEntry); }

      CThumbnailDataEntry *  GetDataEntry()    
         { return (CThumbnailDataEntry *) CItemNumDatabase::GetDataEntry(); }
   };

class CItemsDataHeader
   {
   protected:
      unsigned short       m_usSize;
      unsigned short       m_usFlags;
      unsigned short       m_usKeywordCount;
   public:
      CItemsDataHeader()                           { Empty(); }
      unsigned short GetSize()                     { return m_usSize; }
      unsigned short GetFlags()                    { return m_usFlags; }
      unsigned short GetKeywordCount()             { return m_usKeywordCount; }
      void     SetSize(unsigned short usSize)      { m_usSize = usSize; }
      void     SetFlags(unsigned short usNewFlags) { m_usFlags = usNewFlags; }
      void     SetKeywordCount(unsigned short usNewKeywordCount)
         {  m_usKeywordCount = usNewKeywordCount;  }
      void Empty()
         { m_usSize = m_usFlags = m_usKeywordCount = 0; }
   };

class CItemsDataEntry : public CDataFileEntry
   {
   protected:
   public:
      CItemsDataHeader  m_DataHeader;
      CVarArray            m_carrayKeywordIDs;
      CString           m_strFileName;
      CString           m_strFriendlyName;

   public:
      CItemsDataEntry()
         {
            m_DataHeader.Empty();
            m_carrayKeywordIDs.SetElementSize(sizeof(DWORD));
         }
   // Get Operations
      const DWORD  GetKeywordID(int nIndex)
         {  return (*(const DWORD *) m_carrayKeywordIDs.GetAt(nIndex)); }
      int          GetKeywordCount()
         { return m_carrayKeywordIDs.GetSize(); }
      const CString & GetFileName() const
         { return m_strFileName; }
      const CString & GetFriendlyName() const
         { return m_strFriendlyName; }

      virtual void    Empty()
         {
            m_DataHeader.Empty();
            m_carrayKeywordIDs.RemoveAll();
            m_strFileName.Empty();
            m_strFriendlyName.Empty();
            CDataFileEntry::Empty();
         }

   // Set Operations
      void SetFriendlyName(CString &strFriendlyName)
         { m_strFriendlyName = strFriendlyName; }
      void SetFileName(CString &strFileName)
         { m_strFileName = strFileName; }

      CDataFileEntry *  CreateObject()  { return new CItemsDataEntry; }

   // Override GetItemSize() to tell this object how to calc. size
   // of data object
      virtual DWORD     GetItemSize()
         {
            return m_DataHeader.GetSize();
         }
      virtual int       GetHeaderSize()      { return sizeof(m_DataHeader); }
      virtual LPVOID    GetHeader()          { return &m_DataHeader; }
   };

// CFileItems provides an interface to the Item Information Database which
// contains extended information about an item
class CFileItems : public CItemNumDatabase
   {
   public:
       CFileItems(StorageDevice *pDirFile, StorageDevice *pDataFile);
       ~CFileItems();

      virtual DWORD     Add(CItemsDataEntry *pDataEntry);

      virtual int       Find(DWORD dwItemNum, CItemsDataEntry *pDataEntry)
         { return CItemNumDatabase::Find(dwItemNum, (CDataFileEntry *) pDataEntry); }

   // Reads Item Info data into CItemsDataEntry object supplied to Find
      virtual int       Read();

	// Gets the number of items (including those deleted) in the database.
	// This number represents the upper limit of possible item numbers.
		int					GetItemRange(DWORD *pNewItemNumber);

   // GetNewItemNumber() will return the Item number that the next Add()
   // will produce which is 1 based
      int               GetNewItemNumber(DWORD *pNewItemNumber);

      virtual int       Update(DWORD dwItemNum, CItemsDataEntry *pDataEntry);

   // Helpers
   protected:
      int               BuildDataItem(CItemsDataEntry *pDataEntry);
   };

// Keyword File Classes

class CKeywordDirEntry : public CSortedDirEntry
   {
   public:
      CVarArray         m_carrayItemNumbers;
   public:
      CKeywordDirEntry()   { m_carrayItemNumbers.SetElementSize(sizeof(DWORD));}
   // Get operations
      DWORD          GetKeywordID()          { return m_dwKeywordID; }
      int            GetItemCount()
         { return m_carrayItemNumbers.GetSize(); }
      DWORD          GetItemNumber(int nIndex)
         { return (*(DWORD *)m_carrayItemNumbers[nIndex]); }
   // Set operations
      DWORD          SetKeywordID(DWORD dwKeywordID)
         { return (m_dwKeywordID = m_dwKeywordID); }
   // Used internally
      virtual long   GetOffsetToData()       { return m_dwKeywordID; }
      virtual long   SetOffsetToData(long lNewOffset)
         { return (m_dwKeywordID = lNewOffset); }
   protected:
      DWORD          m_dwKeywordID;
   };

class CKeywordItemListHeader
  {
  protected:
    DWORD     m_dwItemCount;
  public:
    DWORD     GetItemCount() { return m_dwItemCount; }
    DWORD     SetItemCount(DWORD dwNewItemCount)
      { return m_dwItemCount = dwNewItemCount; }
    void Empty() { m_dwItemCount = 0; }
  };

class CFileKeywordDir : public CSortedDir
   {
   public:
      CFileKeywordDir(CCIndexFile *pIndexFile);
      ~CFileKeywordDir();

   // GetIndexList fills in callers object with index list to be used
   // with opening file
      static void       GetIndexList(CCIndexIndexList *pIndexList);

   // Find Keyword associated with Unique Keyword ID
   //   CString parameter gets updated with Keyword string on successful find
      int               Find(DWORD dwKeywordID, CString &strKeyword);

   // Find Keyword and fill in Dir entry object 
      int               Find(CString &strKeyword,
         CKeywordDirEntry *pDirEntry, BOOL bReadNow=TRUE);
      DWORD             Add(CKeywordDirEntry *pDirEntry);
      DWORD             Add(CString &strKeyword, DWORD dwItemNum);

      int               FindFirst(CString &strKeyword,
         CKeywordDirEntry *pDirEntry, BOOL bReadNow=TRUE);
      int               FindNext(CString &strKeyword,
         CKeywordDirEntry *pDirEntry, BOOL bReadNow=TRUE);

   // Must use Find() with Keyword string before calling Delete
      int               Delete(DWORD dwItemNum);

      DWORD             GetKeywordCount();
   protected:
      virtual int       AddSecondaryKey(CString &strKeyword);
   protected:
      DWORD             m_dwKeywordID;
   };

// INLINE Section



#pragma pack()    // back to default packing
#endif            // end of include
