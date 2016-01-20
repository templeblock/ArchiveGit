#ifndef _COMPDB_INC_
#define _COMPDB_INC_

#include "collfile.h"
#include "UIImage.h"
 
class CPUIImageComponentDirEntry : public CSortedDBDirEntry
{
public:
  CPUIImageComponentDirEntry()
	 { }

protected:
	// Helpers
	// Get operations
	virtual StorageMethod		GetStorageMethod() 
		{ return storageInternal; }
	virtual long       GetOffsetToData()
		{ return m_dwOffset; }
	// Set operations
	virtual long       SetOffsetToData(long lNewOffset)
		{ return (m_dwOffset = lNewOffset); }
	virtual void		   SetStorageMethod(StorageMethod storageMethod) 
		{ ASSERT(0); }

protected:
  long           m_dwOffset;
};

// This class describes data format for Interface Component object
class CPUIImageComponentDataEntry : public CDataFileEntry
{
public:
   CPUIImageComponentDataEntry()
      {  }

   virtual CDataFileEntry * CreateObject() { return new CPUIImageComponentDataEntry; }

   // Override GetItemSize() to tell this object how to calc. size
   // of data object
   virtual DWORD     GetItemSize()
      { return m_DataHeader.GetSize(); }

   virtual int       GetHeaderSize()      
      { return sizeof(m_DataHeader); }
   virtual LPVOID		GetHeader()
      { return &m_DataHeader; }

   virtual void      SetHeader(CPUIImageInfo &refHeader)
      { m_DataHeader = refHeader;}
protected:
   CPUIImageInfo		m_DataHeader;
};

// This class describes data format for Interface Component object
class CPUIPageComponentDataEntry : public CDataFileEntry
{
public:
   CPUIPageComponentDataEntry()
      {  }

   virtual CDataFileEntry * CreateObject() { return new CPUIPageComponentDataEntry; }

   // Override GetItemSize() to tell this object how to calc. size
   // of data object
   virtual DWORD     GetItemSize()
      { return m_dwDataSize; }

   virtual int       GetHeaderSize()      
      { return sizeof(m_dwDataSize); }
   virtual LPVOID		GetHeader()
      { return &m_dwDataSize; }

   virtual void      SetHeader(DWORD &refDataSize)
      { m_dwDataSize = refDataSize;}
protected:
   DWORD		m_dwDataSize;
};

class CSortedDatabaseObject : public CSortedDatabase
{
public:
   CSortedDatabaseObject(CCIndexFile *pCIndexFile, StorageDevice *pDataFile);
   ~CSortedDatabaseObject();

protected:
   CSortedDir              m_Dir;
   CFileData               m_Data;
};

// Inline Section

#endif	// End of _COMPDB_INC_
