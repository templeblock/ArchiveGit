#include "stdafx.h"
#include "cindex.h"

#define DIR_KEY 1

IMPLEMENT_DYNAMIC(CCIndexFileException, CException)

void CCIndexFileException::Throw(DWORD dwError, CString &refStrFileName)
{
   THROW(new CCIndexFileException(dwError, refStrFileName));
}

int CCIndexFileException::ErrnoToException(int nErrno)
{
   switch(nErrno)
   {
   case CCOK:
      return CCIndexFileException::none;
   case KEYMATCH:    /* found key match but not rec match */
      return CCIndexFileException::keyMatch;
   case GREATER:     /* found next greater key - from greateq */
      return CCIndexFileException::keyGreaterFound;
   case LESSTHAN:    /* found key less than - returned on lesseq */
      return CCIndexFileException::keyLessThanFound;
   case FAIL:        /* failure on add */
      return CCIndexFileException::fail;
   case PARMERR:     /* parameter error */
      return CCIndexFileException::parameterError;
   case PSPERR:      /* bad psp structure */
      return CCIndexFileException::pspError;
   case DSKERR:      /* disk error */
      return CCIndexFileException::diskError;
   case BADINDEX:    /* bad header record */
      return CCIndexFileException::badIndex;
   case WRITERR:     /* error in write of node */
      return CCIndexFileException::writeNodeError;
   case SEEKERR:     /* error in lseek routine */
      return CCIndexFileException::badSeek;
   case NOSPACE:     /* no more disk space */
      return CCIndexFileException::diskFull;
   case READERR:     /* read error */
      return CCIndexFileException::readError;
   case OLDERR:      /* build error on opening old file */
      return CCIndexFileException::buildErrorOld;
   case NEWERR:      /* error on copen of new file */
      return CCIndexFileException::openError;
   case INCOMPLETE:  /* incomplete data record found */
      return CCIndexFileException::incompleteDataRecord;
   case BAD:         /* error found in ccheck */
      return CCIndexFileException::ccheckError;
   case BADDLIST:    /* bad datalist */
      return CCIndexFileException::badDataList;
   case DATAERROR:   /* data read error after finding key */
      return CCIndexFileException::dataReadError;
   case NOCUR:       /* no current */
      return CCIndexFileException::noCurrent;
   case BADOLDDEL:   /* bad delete of old keys (update) */
      return CCIndexFileException::oldKeyDeleteBad;
   case DATAERR:     /* error reading or adding data portion */
      return CCIndexFileException::dataError;
   case BADDATDEL:   /* bad delete of old data */
      return CCIndexFileException::oldDataDeleteBad;
   case CHKSUMERR:   /* check sums of dlist dont match */
      return CCIndexFileException::checkSumDListError;
   case CHKSUMDERR:  /* check sums of datarec dont match */
      return CCIndexFileException::checkSumDataRecordError;
   case BADFLDTYP:   /* bad field type in datalist */
      return CCIndexFileException::badFieldType;
   case BADKEYTYP:   /* bad key type in datalist */
      return CCIndexFileException::keyTypeBad;
   case BADDUPFLG:   /* bad unq/dup flag in datalist */
      return CCIndexFileException::unqDupFlagBad;
   case BADFLDLEN:   /* bad field length in datalist */
      return CCIndexFileException::fieldLenBad;
   case BADSTRLEN:   /* string passed is longer than ->fldlen in datalist */
      return CCIndexFileException::dataListStringTooLong;
   case BADFLDINX:   /* bad field index number in datalist or indexlist */
      return CCIndexFileException::fieldIndexNumberBad;
   case BUFOVER:     /* overflow of buffer */
      return CCIndexFileException::bufferOverflow;
   case FILEXIST:    /* file existed on create */
      return CCIndexFileException::fileExists;
   case BADNODE:     /* corrupted node */
      return CCIndexFileException::nodeCorrupted;
   case BADHDR:      /* bad header information */
      return CCIndexFileException::headerInfoBad;
   case BADBYTEMODE: /* invalid word order mode, bcreate or dbcreate */
      return CCIndexFileException::invalidWordOrderOnCreate;
   case BADWRITE:    /* attempted write to file opened as read only */
      return CCIndexFileException::writeAttemptOnReadOnly;
   case OUTOFMEM:    /* ran out of memory when allocating space */
      return CCIndexFileException::outOfMemory;
   case BADINDTYP:   /* index type invalid or does not match header */
      return CCIndexFileException::invalidIndexType;
   case INDEXIST:    /* index entries already exist, cannot change type */
      return CCIndexFileException::indexEntriesExist;
   case BADSEGLIST:  /* bad segment type, null list, missing terminator, etc */
      return CCIndexFileException::segmentTypeBad;
   case CMPFAIL:     /* compression failed */
      return CCIndexFileException::compressionFailed;
   case EXPFAIL:     /* expansion failed */
      return CCIndexFileException::expansionFailed;
   case WRITEINTR:   /* write to file was interupted in security level 4 */
      return CCIndexFileException::writeInterupted;
   case BADSECLEV:   /* invalid security level in effect for function (such as transbeg) */
      return CCIndexFileException::securityLevelBad;
   case PROCINTR:    /* processing was interupted by application */
      return CCIndexFileException::processInterupted;
   case INVALIDNDE:  /* invalid node number requested in diskread or diskwrte */
      return CCIndexFileException::nodeInvalid;
   case INVALIDOS:   /* invalid version of DOS, need WRTUPD */
      return CCIndexFileException::dosVersionError;
   case BYTEMISMATCH:   /* input and output file byte orders do not match */
      return CCIndexFileException::fileByteOrderMismatch;
   case ERRSHAREDIND:   /* error in cigetrel or cisetrel, more than one index in btree */
      return CCIndexFileException::sharedIndexError;
   case INVNODEREAD:    /* request to read invalid node, probably corrupted header */
      return CCIndexFileException::invalidNodeReadRequest;
   /* multi user errors */
   case FILENOTLOCKED:  /* file not locked */
      return CCIndexFileException::fileNotLocked;
   case FILELOCKED:     /* file locked - internal error */
      return CCIndexFileException::fileLocked;
   case MODEERR:        /* open mode error */
      return CCIndexFileException::openModeError;
   case RECSHLK:        /* locked by a shared process */
      return CCIndexFileException::sharedProcessLock;
   case RECSFLK:        /* locked by self */
      return CCIndexFileException::lockedBySelf;
   case RECUNLK:        /* record unlocked */
      return CCIndexFileException::recordUnlocked;
   case RECNOLK:        /* record not locked before delete/change */
      return CCIndexFileException::recordNotLocked;
   case RSTERR:         /* error reseting the share byte after extend */
      return CCIndexFileException::resetShareError;
   case USRERR:         /* too many users logged onto file */
      return CCIndexFileException::tooManyUsers;
   case BADQUE:         /* problem maintaining write queue */
      return CCIndexFileException::writeQueueError;
   case TRANSBUSY:      /* transaction processing already in use */
      return CCIndexFileException::transactionBusy;
   case TRANSINTR:      /* transaction was interupted, needs rollback */
      return CCIndexFileException::transactionInterupted;
   case TRANSALLOC:     /* transaction empty, space is allocated for this process */
      return CCIndexFileException::transactionEmpty;
   case TRANSNULL:      /* no transaction active */
      return CCIndexFileException::noTransactionActive;
   case TRANSERR:       /* invalid transaction log entry */
      return CCIndexFileException::transactionLogInvalid;
   /* additional misc errors */
   case CLRLOCKERR:     /* error clearing existing lock on record */
      return CCIndexFileException::lockClearError;
   /* errors from corrupted tree structure */
   case BADHDRREAD:     /* failed reading header */
      return CCIndexFileException::treeHeaderReadError;
   case BADROOT:        /* error reading or tesing root */
      return CCIndexFileException::rootError;
   case BADNDEFREE:     /* invalid free space in node */
      return CCIndexFileException::nodeFreeSpaceInvalid;
   case BADNDECOUNT:    /* invalid node entry count or corrupted keys */
      return CCIndexFileException::nodeEntryCountInvalid;
   case BADNDESAME:     /* forward and rev ptrs are same */
      return CCIndexFileException::nodePtrSame;
   case BADNDEFWD:      /* invalid forward ptr */
      return CCIndexFileException::nodeForwardPtrInvalid;
   case BADNDEREV:      /* invalid reverse ptr */
      return CCIndexFileException::nodeReversePtrInvalid;
   case BADNDEKEYLEN:   /* invalid key length */
      return CCIndexFileException::keyLengthInvalid;
   case BADNDENPTR:     /* invalid index node pointer */
      return CCIndexFileException::indexNodePtrInvalid;
   case BADNDEDOWN:     /* read down tree gets to wrong level */
      return CCIndexFileException::nodeLevelError;
   case BADNDESCAN:     /* error scanning node, invalid entry */
      return CCIndexFileException::nodeScanError;
   case BADNDEREAD:     /* error reading node */
      return CCIndexFileException::nodeReadError;
   case BADFREECHAIN:   /* invalid freechain node */
      return CCIndexFileException::nodeFreeChainInvalid;
   case BADHDRINFO:     /* corrupted header */
      return CCIndexFileException::nodeCorruptedHeader;
   case BADNDEKEYSEQ:   /* key sequence is invalid */
      return CCIndexFileException::nodeKeySequenceInvalid;
   default:
      return CCIndexFileException::generic;
   }
}

// CCIndexIndexList methods
CCIndexIndexList::CCIndexIndexList()
   {
      int     i;

      // One extra index in index list to indicate list end if all 
      // get used
      for(i=0;i < MAX_CCINDEX_INDEXES+1;i++)
         {
            m_indexList[i].keyn       = ENDNDXLIST;
            m_indexList[i].keytype    = 0;
            m_indexList[i].segptr     = NULL;
         }
      SetIndexType(1, stringKey);
   }

unsigned char CCIndexIndexList::KeyTypeToCIndex(IndexKeyTypes type)
   {
      unsigned char  chKeyType;

      switch(type)
         {
            case stringKey:
               chKeyType = NOCASEIND;
               break;
            case stringCaseSensitiveKey:
               chKeyType = STRINGIND;
               break;
            case longKey:
               chKeyType = LONGIND;
               break;
            case custom1Key:
               chKeyType = CUST1IND;
               break;
            default:
               chKeyType = NONKEY;
               break;
         }
      return chKeyType;
   }

// DGP-this function could check for out of sequence if called after open
int CCIndexIndexList::SetIndexType(int nID, IndexKeyTypes type)
   {
      ASSERT(!(nID < 1 || nID > MAX_CCINDEX_INDEXES));
      if(nID < 1 || nID > MAX_CCINDEX_INDEXES)
         return CCIndexFile::parameterError;

      nID--;
      m_indexList[nID].keyn       = nID + 1;
      m_indexList[nID].keytype    = KeyTypeToCIndex(type);
      m_indexList[nID].segptr     = NULL;
      return CCIndexFile::success;
   }

// CCIndexFile class member functions 
// Class wrapper for CIndex file type
CCIndexFile::CCIndexFile()
   {
      m_bOpen = FALSE;
   }

CCIndexFile::~CCIndexFile()
  {
    Close();
  }

int CCIndexFile::GetCustomInfo(LPVOID pData, int nDataSize)
   {
      int nRetVal;

      ASSERT(pData);
      ASSERT(IsOpen());

      if(pData==NULL || nDataSize < 0)
         return parameterError;
      if(!IsOpen())
         return fileNotOpen;

      nRetVal = cgetcust(&m_CFileParms, pData, nDataSize);
      CheckForException(nRetVal); // If exception exists, throw it
      return nRetVal;
   }

int CCIndexFile::SetCustomInfo(LPVOID pData, int nDataSize)
   {
      int nRetVal;

      ASSERT(pData);
      // Asserts if requested data size is greater than header size
      ASSERT(nDataSize <= MAXCUSTINFO);

      if(pData==NULL || nDataSize < 0)
         return parameterError;
      if(!IsOpen())
         return fileNotOpen;
      nRetVal = csetcust(&m_CFileParms, pData, nDataSize);
      CheckForException(nRetVal); // If exception exists, throw it
      return nRetVal;
   }

// Should return error only
int CCIndexFile::Open(CString &strFileName, CCIndexIndexList *pIndexList,
  UINT openFlag)
   {
      int         nRetVal, nShareMode, nReadMode;
      char        *pFileName = strFileName.GetBuffer(1);
      NDXLIST     *pNDXList = NULL;

      if(IsOpen())
            return TRUE;

      if(pIndexList)
         pNDXList = pIndexList->GetList();

      if( openFlag & modeCreate )
         {
            nRetVal = bcreate(&m_CFileParms, pFileName, EXCL, NATIVEMODE, pNDXList);
         }
      else
         {
            if(openFlag & shareExclusive)
               nShareMode = EXCL;
            else
               nShareMode = SHARED;
            if(openFlag & modeReadWrite)
               nReadMode = CRDWRITE;
            else
               nReadMode = CRDONLY;
            nRetVal = bopen(&m_CFileParms, pFileName, nShareMode, nReadMode, pNDXList);
         }

      strFileName.ReleaseBuffer();
      if(nRetVal == CCOK)
         m_bOpen = TRUE;
   #if 0
      if(nRetVal == CCOK)
         m_bOpen = TRUE;
      else if(pException && !IsStandardReturnValue(nRetVal))
         pException->SetError(nRetVal, strFileName);
   #endif

      m_strFileName = strFileName;
      return nRetVal;
   }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::Find(CString &strKey, LPVOID pData, USHORT usDataSize,
  DWORD dwRecordNum, int nKeyID)
   {
      int    nRetVal;

      nRetVal = Find((LPVOID) strKey.GetBuffer(1), pData, usDataSize,
         dwRecordNum, nKeyID);
      strKey.ReleaseBuffer();
      return nRetVal;
   }

int CCIndexFile::Find(long dwKey, LPVOID pData, USHORT usDataSize,
  DWORD dwRecordNum, int nKeyID)
  {
      return Find((LPVOID) &dwKey, pData, usDataSize, dwRecordNum, nKeyID);
  }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::Find(LPVOID pKey, LPVOID pData, USHORT usDataSize,
  DWORD dwRecordNum, int nKeyID)
  {
     int    nRetVal;
     char   chByte;

     ASSERT(IsOpen());
     if(!IsOpen())
        return fileNotOpen;

     // Always let cindex read at least 1 data byte to data length
     // variable is properly set
     if(pData == NULL)
      {
        pData = &chByte;
        usDataSize = 1;
      }
     nRetVal = cfind(&m_CFileParms, nKeyID,
        (char *) pKey,
        dwRecordNum,
        pData, usDataSize);
     CheckForException(nRetVal); // If exception exists, throw it
     return nRetVal;
  }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::GetFirstEntry(LPVOID pData, USHORT usDataSize, int nKeyID)
  {
     int    nRetVal;
     char   chByte;

     ASSERT(IsOpen());

     if(!IsOpen())
        return fileNotOpen;

     // Always let cindex read at least 1 data byte to data length
     // variable is properly set
     if(pData == NULL)
      {
        pData = &chByte;
        usDataSize = 1;
      }

     nRetVal = cfirst(&m_CFileParms, nKeyID, pData, usDataSize);
     CheckForException(nRetVal); // If exception exists, throw it
     return nRetVal;
  }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::GetLastEntry(LPVOID pData, USHORT usDataSize, int nKeyID)
  {
     int    nRetVal;
     char   chByte;

     ASSERT(IsOpen());

     if(!IsOpen())
        return fileNotOpen;

     // Always let cindex read at least 1 data byte to data length
     // variable is properly set
     if(pData == NULL)
      {
        pData = &chByte;
        usDataSize = 1;
      }

     nRetVal = clast(&m_CFileParms, nKeyID, pData, usDataSize);
     CheckForException(nRetVal); // If exception exists, throw it
     return nRetVal;
  }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::GetNextEntry(LPVOID pData, USHORT usDataSize, int nKeyID)
  {
     int    nRetVal;
     char   chByte;

     ASSERT(IsOpen());

     if(!IsOpen())
        return fileNotOpen;

     // Always let cindex read at least 1 data byte to data length
     // variable is properly set
     if(pData == NULL)
      {
        pData = &chByte;
        usDataSize = 1;
      }

     nRetVal = cnext(&m_CFileParms, nKeyID, pData, usDataSize);
     CheckForException(nRetVal); // If exception exists, throw it
     return nRetVal;
  }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::GetPrevEntry(LPVOID pData, USHORT usDataSize, int nKeyID)
  {
     int    nRetVal;
     char   chByte;

     ASSERT(IsOpen());
     if(!IsOpen())
        return fileNotOpen;

     // Always let cindex read at least 1 data byte to data length
     // variable is properly set
     if(pData == NULL)
      {
        pData = &chByte;
        usDataSize = 1;
      }

     nRetVal = cprev(&m_CFileParms, nKeyID, pData, usDataSize);
     CheckForException(nRetVal); // If exception exists, throw it
     return nRetVal;
  }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::Add(CString &strKey, LPVOID pData, USHORT usDataSize,
   DWORD dwRecordNum, int nID, BOOL bAllowDupes)
   {
      ASSERT(strKey.GetLength() < MAXKLEN);

      int      nRetVal;
      nRetVal = Add(strKey.GetBuffer(1), pData, usDataSize, dwRecordNum, nID,
         bAllowDupes);
      strKey.ReleaseBuffer();
      return nRetVal;
   }

int CCIndexFile::Add(long lKey, LPVOID pData, USHORT usDataSize,
   DWORD dwRecordNum, int nID, BOOL bAllowDupes)
   {
      return Add(((char *) &lKey), pData, usDataSize, dwRecordNum, nID,
         bAllowDupes);
   }

int CCIndexFile::Add(LPVOID pKey, LPVOID pData, USHORT usDataSize,
   DWORD dwRecordNum, int nID, BOOL bAllowDupes)
   {
      int   nRetVal;
      int   (EDECL *pAddFunc)(CFILE *, int, void *, long, void *, int);

      ASSERT(IsOpen());
      if(!IsOpen())
         return fileNotOpen;

      if(!bAllowDupes)
          pAddFunc = cunqadd;
      else
         pAddFunc= cdupadd;

      nRetVal = pAddFunc(
         &m_CFileParms,
         nID,
         pKey,
         dwRecordNum,
         pData,
         usDataSize);
      CheckForException(nRetVal); // If exception exists, throw it
      return nRetVal;
   }

int CCIndexFile::DeleteCurrentEntry(int nKeyID)
   {
      int nRetVal;

      ASSERT(IsOpen());
      if(!IsOpen())
         return fileNotOpen;

      nRetVal = cdelcur(&m_CFileParms, nKeyID);
      CheckForException(nRetVal); // If exception exists, throw it
      return nRetVal;
   }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::Delete(CString &strKey, int nKeyID, DWORD dwRecordNum)
   {
      int      nRetVal;
      nRetVal = Delete(strKey.GetBuffer(1), nKeyID, dwRecordNum);
      strKey.ReleaseBuffer();
      return nRetVal;
   }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::Delete(long lKey, int nKeyID, DWORD dwRecordNum)
   {
      int      nRetVal;
      nRetVal = Delete(((char *) &lKey), nKeyID, dwRecordNum);
      return nRetVal;
   }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::Delete(LPVOID pKey, int nKeyID, DWORD dwRecordNum)
   {
      int    nRetVal;
      DWORD  dwValue=(DWORD)-1L;

      ASSERT(IsOpen());
      if(!IsOpen())
         return fileNotOpen;

      // If valid record number supplied, delete with key and record 
      // number match
      if(dwRecordNum != (DWORD)-1L)
         {
            nRetVal = cdelete(&m_CFileParms, nKeyID, pKey, dwRecordNum);
         }
      else
         {
            // Delete with key match only (ignore record number)
            nRetVal = Find(pKey, NULL, 0, dwRecordNum, nKeyID);

            // Delete current key if previous find successful
            if(nRetVal == success || nRetVal == keyMatch)
            {
               nRetVal = cdelcur(&m_CFileParms, nKeyID);
               // If deleting current key failed, throw exception
               if(nRetVal != success)
                  CCIndexFileException::Throw(nRetVal, m_strFileName);
            }
         }
      CheckForException(nRetVal); // If exception exists, throw it
      return nRetVal;
   }

int CCIndexFile::GetCurrentEntry(LPVOID pData, USHORT usDataSize, int nID)
   {
      int nRetVal;

      ASSERT(IsOpen());
      if(!IsOpen())
         return fileNotOpen;

      nRetVal = cgetcur(&m_CFileParms, nID,
         pData,
         usDataSize);
      CheckForException(nRetVal); // If exception exists, throw it
      return nRetVal;
   }

int CCIndexFile::SetCurrentEntry(LPVOID pData, USHORT usDataSize, int nID)
   {
      int nRetVal;

      ASSERT(IsOpen());
      if(!IsOpen())
         return fileNotOpen;

      nRetVal = cchgcur(&m_CFileParms, nID,
         pData,
         usDataSize);
      CheckForException(nRetVal); // If exception exists, throw it
      return nRetVal;
   }

int CCIndexFile::GetCurrentKey(CString &strKey)
   {
      ASSERT(IsOpen());
      if(!IsOpen())
         return fileNotOpen;
      strKey = (char *) m_CFileParms.key;
      return success;
   }

// Gets a custom key including the length byte
int CCIndexFile::GetCurrentKey(LPVOID pCustomKey, USHORT usDestLen)
   {
      int   nLen=*((BYTE *) m_CFileParms.key);

      if(usDestLen == 0)
         usDestLen = (USHORT) nLen + 1;   // include length byte in copy
      ASSERT(usDestLen != nLen);
      memcpy(pCustomKey, m_CFileParms.key, usDestLen);
      return success;
   }

int CCIndexFile::GetCurrentKey(DWORD * pdwKey)
   {
      ASSERT(IsOpen());
      if(!IsOpen())
         return fileNotOpen;
      *pdwKey = *((DWORD *) m_CFileParms.key);
      return success;
   }

int CCIndexFile::GetEntrySize()
   {
      return m_CFileParms.retlen;
   }

DWORD CCIndexFile::GetRecordNum()
   {
      return m_CFileParms.rec;
   }

// Should return error if standard error, otherwise throw exception
int CCIndexFile::Close()
  {
     int nRetVal = success;

     if(IsOpen())
        nRetVal = cclose(&m_CFileParms);    // Close CIndex file

     endbcb(&m_CFileParms);              // Release CIndex buffers

     m_bOpen = FALSE;
     return nRetVal;    // Return what CIndex returns
  }

BOOL CCIndexFile::IsStandardReturnValue(int nValueToCheck)
{
   switch(nValueToCheck)
   {
     case success:
     case keyMatch:
     case keyGreaterFound:
     case keyLessThanFound:
     case fail:
     case diskError:
     case fileExists:
     case buildOpenError:
     case buildCreateError:
     case indexEntriesExist:
     case processInterupted:
     case sharedProcessLock:
     case lockedBySelf:
     case recordUnlocked:
     case transactionBusy:
     case transactionInterupted:
        return TRUE;
   }
  return FALSE;
}
