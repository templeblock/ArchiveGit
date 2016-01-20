#ifndef _CINDEX_INC
#define _CINDEX_INC

#pragma pack(1)   // Pack all structures used in this file

#define  MAX_CCINDEX_INDEXES  15

#ifdef   WIN32
  #ifndef  NT
    #define NT    // For C-Index specification of 32 bit platform
    #define  MSC  // For C-Index specification of Microsoft usage
    extern "C"{
      #include "cndx.h"
    }
  #endif
#else
    #define  VC
    #define  DLL  // For C-Index specification of Microsoft usage
    extern "C"{
      #include "cndx.h"
    }
#endif

class CCIndexFileException : public CException
  {
  	DECLARE_DYNAMIC(CCIndexFileException)
  public:
     // Error code values match cindex error code #defines in cndx.h
    enum ErrorCodes
      {
		    none =                    000,
		    keyMatch =                001,
		    keyGreaterFound =         002,
		    keyLessThanFound =        003,
		    fail  =                   -1,
		    parameterError  =         -2,
		    pspError =                -3,
		    diskError =               -4,
		    badIndex  =               -5,
		    writeNodeError  =         -6,
		    badSeek  =                -7,
		    diskFull  =               -8,
		    readError  =              -9,
		    buildErrorOld  =          -10,
		    openError  =              -11,
		    incompleteDataRecord =    -12,
		    ccheckError =             -13,
		    badDataList =             -14,
		    dataReadError  =          -15,
		    noCurrent =               -16,
		    oldKeyDeleteBad =         -17,
		    dataError =               -18,
		    oldDataDeleteBad =        -19,
		    checkSumDListError =      -20,
		    checkSumDataRecordError = -21,
		    badFieldType =            -22,
		    keyTypeBad =              -23,
		    unqDupFlagBad =           -24,
		    fieldLenBad =             -25,
		    dataListStringTooLong =   -26,
		    fieldIndexNumberBad =     -27,
		    bufferOverflow =          -28,
		    fileExists =              -29,
		    nodeCorrupted =           -30,
		    headerInfoBad =           -31,
		    invalidWordOrderOnCreate =-32 ,
		    writeAttemptOnReadOnly =  -33,
		    outOfMemory =             -34,
		    invalidIndexType =        -35,
		    indexEntriesExist =       -36,
		    segmentTypeBad =          -37,
		    compressionFailed =       -38,
		    expansionFailed =         -39,
		    writeInterupted =         -40,
		    securityLevelBad =        -41,
		    processInterupted =       -42,
		    nodeInvalid =             -43,
		    dosVersionError =         -44,
		    fileByteOrderMismatch =   -45,
		    sharedIndexError =        -46,
		    invalidNodeReadRequest =  -47,
/* multi user errors */
		    fileNotLocked =           -101,
		    fileLocked =              -102,
		    openModeError =           -103,
		    sharedProcessLock =       -104,
		    lockedBySelf =            -105,
		    recordUnlocked =          -106,
		    recordNotLocked =         -107,
		    resetShareError =         -108,
		    tooManyUsers =            -109,
		    writeQueueError =         -110,
		    transactionBusy =         -111,
		    transactionInterupted =   -112,
		    transactionEmpty =        -113,
		    noTransactionActive =     -114,
		    transactionLogInvalid =   -115,
		    lockClearError =          -116,
    /* errors from corrupted tree structure */
		    treeHeaderReadError =     -201,
		    rootError =               -202,
		    nodeFreeSpaceInvalid =    -203,
		    nodeEntryCountInvalid =   -204,
		    nodePtrSame =             -205,
		    nodeForwardPtrInvalid =   -206,
		    nodeReversePtrInvalid =   -207,
		    keyLengthInvalid =        -208,
		    indexNodePtrInvalid =     -209,
		    nodeLevelError =          -210,
		    nodeScanError =           -211,
		    nodeReadError =           -212,
		    nodeFreeChainInvalid =    -213,
		    nodeCorruptedHeader =     -214,
		    nodeKeySequenceInvalid =  -216,
          generic                =  -1000
      };                          
// Constructor
	CCIndexFileException(int cause = CCIndexFileException::none,
		CString &refStrFileName = CString(""))
	{ SetError(cause, refStrFileName); }

// Attributes
	int     m_cause;
	CString m_strFileName;

// Operations
	// convert a CIndex dependent error code to a Cause
  static int        ErrnoToException(int nErrno);
  void SetError(int cause, CString &refStrFileName)
	{ m_cause = cause; m_strFileName = refStrFileName; }

	// helper functions to throw exception after converting to a Cause
	static void Throw(DWORD dwError, CString &refStrFileName = CString(""));

// Implementation
public:
	virtual ~CCIndexFileException() {};
//	virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
//		PUINT pnHelpContext = NULL);
  };

// CCIndexIndexList class is used with a CCIndexFile class to describe
// the key types of each index in a given index file
class CCIndexIndexList
   {
   public:
      enum IndexKeyTypes
         {
         stringKey,
         stringCaseSensitiveKey,
         longKey,
         custom1Key
         };
      CCIndexIndexList();
      int            SetIndexType(int nID, IndexKeyTypes type);
      NDXLIST *      GetList() { return &m_indexList[0]; }
   protected:
   // Internal helpers
      unsigned char  KeyTypeToCIndex(IndexKeyTypes type);

   protected:
      NDXLIST     m_indexList[MAX_CCINDEX_INDEXES+1];
   };

class CCIndexFile
   {
   public:
      enum ReturnValues
         {
		      success =                 000,
         // Key search return values - mapped to CIndex values
		      keyMatch =                001,
		      keyGreaterFound =         002,
		      keyLessThanFound =        003,

         // NON CIndex specific return values, specific to this class
            fileNotOpen, 

         // More return values - mapped to CIndex values
		      fail  =                   -1,   // Operation failed

		      buildOpenError =          -10,
		      buildCreateError =        -11,
		      ccheckError =             -13,
		      indexEntriesExist =       -36,
		      pspError =                -3,
		      incompleteDataRecord =    -12,
		      bufferOverflow =          -28,
		      diskError =               -4,
		      parameterError  =         -2,
		      processInterupted =       -42,
		      fileExists =              -29,
		      noCurrent =               -16,
		      checkSumDListError =      -20,
		      checkSumDataRecordError = -21,
		      oldKeyDeleteBad =         -17,
		      dataError =               -18,

         // File sharing return values
		      sharedProcessLock =       -104,
		      lockedBySelf =            -105,
		      recordUnlocked =          -106,
		      transactionBusy =         -111,
		      transactionInterupted =   -112,
		      transactionEmpty =        -113,
		      securityLevelBad =        -41,
		      noTransactionActive =     -114,
         };
	   enum OpenFlags
         {
		      modeRead =          0x0000,
		      modeReadWrite =     0x0002,
      		shareExclusive =    0x0010,
		      modeCreate =        0x1000
		   };

         CCIndexFile();
         virtual ~CCIndexFile();

      // Opens/Creates index file
      // All key types default to Case Sensitive string keys if no indexlist
         virtual int     Open(CString &strFileName,
            CCIndexIndexList *pIndexList = NULL, UINT openFlag=modeRead);

         BOOL            IsOpen();
         virtual int     Close();
         virtual int     Find(CString &strKey, LPVOID pData, USHORT usDataSize,
                           DWORD dwRecordNum=0, int nID=1);
         virtual int     Find(long lKey, LPVOID pData, USHORT usDataSize,
                           DWORD dwRecordNum=0, int nID=1);
         virtual int     Find(LPVOID pKey, LPVOID pData, USHORT usDataSize,
                           DWORD dwRecordNum=0, int nID=1);
         int             GetFirstEntry(LPVOID pData, USHORT usDataSize, int nKeyID);
         int             GetLastEntry(LPVOID pData, USHORT usDataSize, int nKeyID);
         virtual int     GetPrevEntry(LPVOID pData, USHORT usDataSize, int nID=1);
         virtual int     GetNextEntry(LPVOID pData, USHORT usDataSize, int nID=1);
      // Add() adds a new key to index.
      // If Allowing dupe keys, a unique record number must be supplied.
         int             Add(CString &strKey, LPVOID pData, USHORT usDataSize,
                           DWORD dwRecordNum=0, int nID=1, BOOL bAllowDupes=FALSE);
         int             Add(long lKey, LPVOID pData, USHORT usDataSize,
                           DWORD dwRecordNum=0, int nID=1, BOOL bAllowDupes=FALSE);
         int             Add(LPVOID pKey, LPVOID pData, USHORT usDataSize,
                           DWORD dwRecordNum=0, int nID=1, BOOL bAllowDupes=FALSE);
      // Most implementions of Delete won't care about matching the
      // record number associated with a key
         int             Delete(CString &strKey, int nKeyID, DWORD dwRecordNum=-1L);
         int             Delete(long lKey, int nKeyID, DWORD dwRecordNum=-1L);
         int             Delete(LPVOID pKey, int nKeyID=1, DWORD dwRecordNum=-1L);
         int             DeleteCurrentEntry(int nKeyID=1);
         int             SetCustomInfo(LPVOID pData, int nDataSize);
         int             GetCustomInfo(LPVOID pData, int nDataSize);

      // GetCurrentEntry retrieves data associated with current key
         int             GetCurrentEntry(LPVOID pData, USHORT usDataSize, int nID=1);

      // SetCurrentEntry updates data associate with current key
         int             SetCurrentEntry(LPVOID pData, USHORT usDataSize, int nID=1);

      // GetEntrySize() return size of data item associated with current key
         int             GetEntrySize();
         DWORD           GetRecordNum();
         int             GetCurrentKey(CString &strKey);
         int             GetCurrentKey(DWORD * plKey);
         int             GetCurrentKey(LPVOID pCustomKey, USHORT usDestLen=0);

   protected:
      CFILE             m_CFileParms;		// CIndex parameters
      CString           m_strFileName;
      BOOL              m_bOpen;
      NDXLIST           m_indexList[MAX_CCINDEX_INDEXES];

   // Internal helpers
   protected:
      BOOL IsStandardReturnValue(int nValueToCheck);
      void CheckForException(int nValueToCheck)
         {
         if(!IsStandardReturnValue(nValueToCheck))
            CCIndexFileException::Throw(nValueToCheck, m_strFileName);
         }
   };

// Inline section
inline BOOL CCIndexFile::IsOpen()
   {
      return m_bOpen;
   }

#endif    // end of include
