#include "stdafx.h"
#include "compdb.h"
#include "util.h"


CSortedDatabaseObject::CSortedDatabaseObject(CCIndexFile *pCIndexFile, StorageDevice *pCFileData)
:  m_Dir(pCIndexFile), m_Data(pCFileData), 
  CSortedDatabase(&m_Dir, &m_Data)
  {
  }

CSortedDatabaseObject::~CSortedDatabaseObject()
  {
  }


