/*
// $Workfile: CBDEDB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CBDEDB.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:27:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:32:46   FRED
// Initial revision.
// 
//    Rev 1.8   05 Feb 1996 08:07:18   JAY
// Bonus Pack/Classic support
// 
//    Rev 1.7   26 Dec 1995 10:58:48   FRED
// 
//    Rev 1.6   26 Dec 1995 10:58:32   FRED
// References database\thunkmsg.h
// 
// 
//    Rev 1.5   26 Dec 1995 10:28:22   FRED
// Project structure changes for mac port
// 
//    Rev 1.4   18 Oct 1995 16:00:02   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 1.3   27 Sep 1995 14:41:24   FRED
// BDE Thunking
// 
//    Rev 1.2   09 Aug 1995 17:31:48   FRED
// Static variable to turn off exception reporting
// 
//    Rev 1.1   08 Aug 1995 12:10:36   FRED
// Open databases read-only
// 
//    Rev 1.0   19 Apr 1995 12:37:54   JAY
// Initial revision.
*/

class CBdeException;
class CBdeSystemException;
class CBdeNotFoundException;
class CBdeDataCorruptException;
class CBdeIoException;
class CBdeLimitException;
class CBdeIntegrityException;
class CBdeInvalidReqException;
class CBdeLockConflictException;
class CBdeSecurityException;
class CBdeInvalidContextException;
class CBdeOsException;
class CBdeNetworkException;
class CBdeOptParamException;
class CBdeQueryException;
class CBdeVersionException;
class CBdeCapabilityException;
class CBdeConfigException;
class CBdeWarningException;
class CBdeOtherException;
class CBdeCompatibilityException;
class CBdeDriverException;
class CBdeInternalException;

#ifndef WIN32
class CBdeEngine;
#endif

class CBdeDatabase;
class CBdeCursor;

#ifndef __CBDEDB_H__
#define __CBDEDB_H__

#ifdef WIN32
#include "thunkmsg.h"
#else
#include "idapi.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CBdeException

class CBdeException : public CException
{
	DECLARE_DYNAMIC(CBdeException);
	
public:

	CBdeException(const DBIResult nResult = DBIERR_NONE);

	INT16 Category(void) const;
	INT16 Code(void) const;
	CString Context(const INT16) const;
	CString Entry(const UINT16, UINT32&) const;
	void GetInfo(DBIErrInfo&, const BOOL = TRUE) const;
	CString Message(void) const;
	DBIResult Result(void) const;

	static DBIResult CheckResult(const DBIResult nResult);
	static DBIResult CheckResult(const DBIResult nResult, const DBIResult nIgnore, ...);

	static BOOL m_fQuiet;
	
private:

	const DBIResult m_nResult;
};

// Exception subclasses.

class CBdeSystemException:         public CBdeException { DECLARE_DYNAMIC(CBdeSystemException);         public: CBdeSystemException(const DBIResult r):         CBdeException(r) {} };
class CBdeNotFoundException:       public CBdeException { DECLARE_DYNAMIC(CBdeNotFoundException);       public: CBdeNotFoundException(const DBIResult r):       CBdeException(r) {} };
class CBdeDataCorruptException:    public CBdeException { DECLARE_DYNAMIC(CBdeDataCorruptException);    public: CBdeDataCorruptException(const DBIResult r):    CBdeException(r) {} };
class CBdeIoException:             public CBdeException { DECLARE_DYNAMIC(CBdeIoException);             public: CBdeIoException(const DBIResult r):             CBdeException(r) {} };
class CBdeLimitException:          public CBdeException { DECLARE_DYNAMIC(CBdeLimitException);          public: CBdeLimitException(const DBIResult r):          CBdeException(r) {} };
class CBdeIntegrityException:      public CBdeException { DECLARE_DYNAMIC(CBdeIntegrityException);      public: CBdeIntegrityException(const DBIResult r):      CBdeException(r) {} };
class CBdeInvalidReqException:     public CBdeException { DECLARE_DYNAMIC(CBdeInvalidReqException);     public: CBdeInvalidReqException(const DBIResult r):     CBdeException(r) {} };
class CBdeLockConflictException:   public CBdeException { DECLARE_DYNAMIC(CBdeLockConflictException);   public: CBdeLockConflictException(const DBIResult r):   CBdeException(r) {} };
class CBdeSecurityException:       public CBdeException { DECLARE_DYNAMIC(CBdeSecurityException);       public: CBdeSecurityException(const DBIResult r):       CBdeException(r) {} };
class CBdeInvalidContextException: public CBdeException { DECLARE_DYNAMIC(CBdeInvalidContextException); public: CBdeInvalidContextException(const DBIResult r): CBdeException(r) {} };
class CBdeOsException:             public CBdeException { DECLARE_DYNAMIC(CBdeOsException);             public: CBdeOsException(const DBIResult r):             CBdeException(r) {} };
class CBdeNetworkException:        public CBdeException { DECLARE_DYNAMIC(CBdeNetworkException);        public: CBdeNetworkException(const DBIResult r):        CBdeException(r) {} };
class CBdeOptParamException:       public CBdeException { DECLARE_DYNAMIC(CBdeOptParamException);       public: CBdeOptParamException(const DBIResult r):       CBdeException(r) {} };
class CBdeQueryException:          public CBdeException { DECLARE_DYNAMIC(CBdeQueryException);          public: CBdeQueryException(const DBIResult r):          CBdeException(r) {} };
class CBdeVersionException:        public CBdeException { DECLARE_DYNAMIC(CBdeVersionException);        public: CBdeVersionException(const DBIResult r):        CBdeException(r) {} };
class CBdeCapabilityException:     public CBdeException { DECLARE_DYNAMIC(CBdeCapabilityException);     public: CBdeCapabilityException(const DBIResult r):     CBdeException(r) {} };
class CBdeConfigException:         public CBdeException { DECLARE_DYNAMIC(CBdeConfigException);         public: CBdeConfigException(const DBIResult r):         CBdeException(r) {} };
class CBdeWarningException:        public CBdeException { DECLARE_DYNAMIC(CBdeWarningException);        public: CBdeWarningException(const DBIResult r):        CBdeException(r) {} };
class CBdeOtherException:          public CBdeException { DECLARE_DYNAMIC(CBdeOtherException);          public: CBdeOtherException(const DBIResult r):          CBdeException(r) {} };
class CBdeCompatibilityException:  public CBdeException { DECLARE_DYNAMIC(CBdeCompatibilityException);  public: CBdeCompatibilityException(const DBIResult r):  CBdeException(r) {} };
class CBdeDriverException:         public CBdeException { DECLARE_DYNAMIC(CBdeDriverException);         public: CBdeDriverException(const DBIResult r):         CBdeException(r) {} };
class CBdeInternalException:       public CBdeException { DECLARE_DYNAMIC(CBdeInternalException);       public: CBdeInternalException(const DBIResult r):       CBdeException(r) {} };

#ifndef WIN32

/////////////////////////////////////////////////////////////////////////////
// CBdeEngine

class CBdeEngine
{

public:

	CBdeEngine();
	~CBdeEngine();
	
public:

	void Open(LPCSTR pszIdapiDllPath = NULL, pDBIEnv pEnvironment = NULL);
	void Close(void);
	
	BOOL IsOpen(void) const
		{ return m_fOpen; }

protected:

	void SetIdapiDllPath(LPCSTR pszIdapiDllPath);
	
	BOOL m_fOpen;
};

#endif

/////////////////////////////////////////////////////////////////////////////
// CBdeDatabase

class CBdeDatabase
{

public:

	CBdeDatabase();
	~CBdeDatabase();
	
public:

	void Open(DBIOpenMode nOpenMode = dbiREADWRITE, LPCSTR pszName = NULL, LPCSTR pszDbType = NULL);
	void Close(void);
	
public:

	hDBIDb Handle(void)
		{ return m_hDb; }
		
protected:

	hDBIDb m_hDb;
};

/////////////////////////////////////////////////////////////////////////////
// CBdeCursor

class CBdeCursor
{

#ifdef WIN32

public:

	CBdeCursor();
	~CBdeCursor();
	
public:

	void Open(CBdeDatabase* pDatabase, LPCSTR pszTableName, DBIOpenMode nOpenMode = dbiREADWRITE, LPCSTR pszDriverType = NULL, LPCSTR pszIndexName = NULL, LPCSTR pszIndexTagName = NULL, UINT16 uIndexId = 0);
//	void Open(CBdeDatabase* pDatabase, LPCSTR pszTableName, short /*DBIOpenMode*/ nOpenMode = dbiREADWRITE, LPCSTR pszDriverType = NULL, LPCSTR pszIndexName = NULL, LPCSTR pszIndexTagName = NULL, UINT16 uIndexId = 0);
	void Close(void);
	
public:

	BOOL AtStart(void);
	BOOL AtEnd(void);
	void ToStart(void);
	void ToEnd(void);
	BOOL GetRecord(void);
	BOOL GetNextRecord(void);
	BOOL GetPriorRecord(void);
	BOOL SeekToRecord(UINT16 uField, pBYTE pSrc);
	
	void GetField(UINT16 uField, pBYTE pDest, pBOOL pbBlank = NULL) const;
	void GetField(LPCSTR pszField, pBYTE pDest, pBOOL pbBlank = NULL) const;
//	void GetField(UINT16 uField, pBYTE pDest, BOOL* /*pBOOL*/ pbBlank = NULL) const;
//	void GetField(LPCSTR pszField, pBYTE pDest, BOOL* /*pBOOL*/ pbBlank = NULL) const;
	void PutField(UINT16 uField, pBYTE pSrc);
	void PutField(LPCSTR pszField, pBYTE pSrc);
	
	UINT16 Fields(void) const;
	const FLDDesc16* FieldDescription(UINT16 uField) const;
	const FLDDesc16* FieldDescription(LPCSTR pszField) const;
	UINT16 FieldNumber(LPCSTR pszField) const;
	void ValidateFieldNumber(UINT16 uField) const;
	
public:

	hDBICur Handle(void) const
		{ return m_hCursor; }
		
	const CURProps16* Properties(void) const
		{ return &m_CursorProperties; }
	
	const RECProps16* RecordProperties(void) const
		{ return &m_RecordProperties; }

protected:

	hDBICur m_hCursor;
	CURProps16 m_CursorProperties;
	pBYTE m_pRecordBuffer;
	pFLDDesc16 m_pFieldDescriptions;
	RECProps16 m_RecordProperties;

#else

public:

	CBdeCursor();
	~CBdeCursor();
	
public:

	void Open(CBdeDatabase* pDatabase, LPCSTR pszTableName, DBIOpenMode nOpenMode = dbiREADWRITE, LPCSTR pszDriverType = NULL, LPCSTR pszIndexName = NULL, LPCSTR pszIndexTagName = NULL, UINT16 uIndexId = 0);
	void Close(void);
	
public:

	BOOL AtStart(void);
	BOOL AtEnd(void);
	void ToStart(void);
	void ToEnd(void);
	BOOL GetRecord(void);
	BOOL GetNextRecord(void);
	BOOL GetPriorRecord(void);
	BOOL SeekToRecord(UINT16 uField, pBYTE pSrc);
	
	void GetField(UINT16 uField, pBYTE pDest, pBOOL pbBlank = NULL) const;
	void GetField(LPCSTR pszField, pBYTE pDest, pBOOL pbBlank = NULL) const;
	void PutField(UINT16 uField, pBYTE pSrc);
	void PutField(LPCSTR pszField, pBYTE pSrc);
	
	UINT16 Fields(void) const;
	const FLDDesc* FieldDescription(UINT16 uField) const;
	const FLDDesc* FieldDescription(LPCSTR pszField) const;
	UINT16 FieldNumber(LPCSTR pszField) const;
	void ValidateFieldNumber(UINT16 uField) const;
	
public:

	hDBICur Handle(void) const
		{ return m_hCursor; }
		
	const CURProps* Properties(void) const
		{ return &m_CursorProperties; }
	
	const RECProps* RecordProperties(void) const
		{ return &m_RecordProperties; }

protected:

	hDBICur m_hCursor;
	CURProps m_CursorProperties;
	pBYTE m_pRecordBuffer;
	pFLDDesc m_pFieldDescriptions;
	RECProps m_RecordProperties;

#endif

};

#endif
