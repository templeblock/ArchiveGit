/*
// $Workfile: CBDEDB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CBDEDB.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:26   Fred
// Initial revision.
// 
//    Rev 1.2   18 Jul 1996 17:39:34   FRED
// No PMWTHUNK.H for WIN32.
// 
//    Rev 1.1   24 May 1996 16:13:20   Fred
// TRACEx
// 
//    Rev 1.0   14 Mar 1996 13:25:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:26:50   FRED
// Initial revision.
// 
//    Rev 1.6   18 Oct 1995 15:59:54   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 1.5   27 Sep 1995 14:41:18   FRED
// BDE Thunking
// 
//    Rev 1.4   13 Sep 1995 11:47:24   JAY
// New 32-bit changes
// 
//    Rev 1.3   09 Aug 1995 17:31:48   FRED
// Static variable to turn off exception reporting
// 
//    Rev 1.2   08 Aug 1995 12:10:36   FRED
// Open databases read-only
// 
//    Rev 1.1   21 Jun 1995 10:25:48   FRED
// Enabled BDE error dialog in release build.
// 
// 
//    Rev 1.0   19 Apr 1995 12:37:54   JAY
// Initial revision.
*/

#include "stdafx.h"

#ifdef WIN32
#include "pmwthunk.h"
#endif
#include "cbdedb.h"

/////////////////////////////////////////////////////////////////////////////
// CBdeException

IMPLEMENT_DYNAMIC(CBdeException, CException)
IMPLEMENT_DYNAMIC(CBdeSystemException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeNotFoundException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeDataCorruptException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeIoException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeLimitException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeIntegrityException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeInvalidReqException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeLockConflictException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeSecurityException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeInvalidContextException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeOsException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeNetworkException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeOptParamException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeQueryException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeVersionException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeCapabilityException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeConfigException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeWarningException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeOtherException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeCompatibilityException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeDriverException, CBdeException)
IMPLEMENT_DYNAMIC(CBdeInternalException, CBdeException)

BOOL CBdeException::m_fQuiet = FALSE;

CBdeException::CBdeException(const DBIResult nResult)
	: m_nResult(nResult)
{
}

INT16 CBdeException::Category(void) const
{
	return ErrCat(m_nResult);
}

INT16 CBdeException::Code(void) const
{
	return ErrCode(m_nResult);
}

CString CBdeException::Context(const INT16 nContext) const
{
	char szContext[DBIMAXMSGLEN+1];

#ifdef WIN32
	PmwThunk.ThunkDbiGetErrorContext(nContext, szContext);
#else
	DbiGetErrorContext(nContext, szContext);
#endif

	return CString(szContext);
}

CString CBdeException::Entry(const UINT16 uEntry, UINT32& ulNativeError) const
{
	char szEntry[DBIMAXMSGLEN+1];

#ifdef WIN32
	PmwThunk.ThunkDbiGetErrorEntry(uEntry, &ulNativeError, szEntry);
#else
	DbiGetErrorEntry(uEntry, &ulNativeError, szEntry);
#endif

	return CString(szEntry);
}

void CBdeException::GetInfo(DBIErrInfo& errInfo, const BOOL fFull) const
{
#ifdef WIN32
	PmwThunk.ThunkDbiGetErrorInfo(fFull, &errInfo);
#else
	DbiGetErrorInfo(fFull, &errInfo);
#endif
}

CString CBdeException::Message(void) const
{
	char szMessage[DBIMAXMSGLEN+1];

#ifdef WIN32
	PmwThunk.ThunkDbiGetErrorString(m_nResult, szMessage);
#else
	DbiGetErrorString(m_nResult, szMessage);
#endif

	return CString(szMessage);
}

DBIResult CBdeException::Result(void) const
{
	return m_nResult;
}

DBIResult CBdeException::CheckResult(const DBIResult nResult)
{
	return CheckResult(nResult, DBIERR_NONE);
}

DBIResult CBdeException::CheckResult(const DBIResult nResult, const DBIResult nIgnore, ...)
{
	if (ErrCat(nResult) == ERRCAT_NONE)
	{
		return nResult;
	}

	const DBIResult* pIgnore = &nIgnore;
	
	while (*pIgnore != 0)
	{
		if (*pIgnore == nResult)
		{
			return nResult;
		}
		
		if (ErrCode(*pIgnore) == 0)
		{
			if (ErrCat(*pIgnore) == ErrCat(nResult))
			{
				return nResult;
			}
		}
		
		pIgnore++;
	}

	if (!m_fQuiet)
	{	
		TRACE1("CheckResult(%04x)\n", nResult);

		char szMessage[256];
#ifdef WIN32
		PmwThunk.ThunkDbiGetErrorString(nResult, szMessage);
#else
		DbiGetErrorString(nResult, szMessage);
#endif

		char szReport[256];
		wsprintf(szReport, "IDAPI Error #%04x '%s'", nResult, szMessage);

		AfxMessageBox(szReport);
	}

	switch (ErrCat(nResult))
	{
		case ERRCAT_SYSTEM:         THROW(new CBdeSystemException(nResult));
		case ERRCAT_NOTFOUND:       THROW(new CBdeNotFoundException(nResult));
		case ERRCAT_DATACORRUPT:    THROW(new CBdeDataCorruptException(nResult));
		case ERRCAT_IO:             THROW(new CBdeIoException(nResult));
		case ERRCAT_LIMIT:          THROW(new CBdeLimitException(nResult));
		case ERRCAT_INTEGRITY:      THROW(new CBdeIntegrityException(nResult));
		case ERRCAT_INVALIDREQ:     THROW(new CBdeInvalidReqException(nResult));
		case ERRCAT_LOCKCONFLICT:   THROW(new CBdeLockConflictException(nResult));
		case ERRCAT_SECURITY:       THROW(new CBdeSecurityException(nResult));
		case ERRCAT_INVALIDCONTEXT: THROW(new CBdeInvalidContextException(nResult));
		case ERRCAT_OS:             THROW(new CBdeOsException(nResult));
		case ERRCAT_NETWORK:        THROW(new CBdeNetworkException(nResult));
		case ERRCAT_OPTPARAM:       THROW(new CBdeOptParamException(nResult));
		case ERRCAT_QUERY:          THROW(new CBdeQueryException(nResult));
		case ERRCAT_VERSION:        THROW(new CBdeVersionException(nResult));
		case ERRCAT_CAPABILITY:     THROW(new CBdeCapabilityException(nResult));
		case ERRCAT_CONFIG:         THROW(new CBdeConfigException(nResult));
		case ERRCAT_WARNING:        THROW(new CBdeWarningException(nResult));
		case ERRCAT_OTHER:          THROW(new CBdeOtherException(nResult));
		case ERRCAT_COMPATIBILITY:  THROW(new CBdeCompatibilityException(nResult));
		case ERRCAT_DRIVER:         THROW(new CBdeDriverException(nResult));
		case ERRCAT_RC:             THROW(new CBdeInternalException(nResult));
		default:                    THROW(new CBdeException(nResult));
	}
}


#ifndef WIN32

/////////////////////////////////////////////////////////////////////////////
// CBdeEngine

CBdeEngine::CBdeEngine()
{
//	TRACE1("%08lx Engine::Construct\n", this);
	m_fOpen = FALSE;
}

CBdeEngine::~CBdeEngine()
{
//	TRACE1("%08lx Engine::Destruct\n", this);
	
	TRY
	{
		Close();
		ASSERT(!m_fOpen);
	}
	END_TRY
}

void CBdeEngine::Open(LPCSTR pszIdapiDllPath /*=NULL*/, pDBIEnv pEnvironment /*=NULL*/)
{
//	TRACE1("%08lx Engine::Open\n", this);
	
	Close();
	ASSERT(!m_fOpen);
	
	SetIdapiDllPath(pszIdapiDllPath);
	
	CBdeException::CheckResult(DbiInit(pEnvironment));
	DbiDebugLayerOptions(DEBUGON | OUTPUTTOFILE, "trace.inf");

	m_fOpen = TRUE;
}

void CBdeEngine::Close()
{
//	TRACE1("%08lx Engine::Close\n", this);
	
#ifdef _DEBUG	
	if (!m_fOpen)
	{
//		TRACE1("%08lx Engine::Close called with engine not open\n", this);
	}
#endif

	if (m_fOpen)
	{
		DbiDebugLayerOptions(0, NULL);
		CBdeException::CheckResult(DbiExit());
		m_fOpen = FALSE;
	}
	
	ASSERT(!m_fOpen);
}

void CBdeEngine::SetIdapiDllPath(LPCSTR pszIdapiDllPath)
{
	if (pszIdapiDllPath != NULL)
	{
		char szCurrentPath[_MAX_PATH+1];
		
		if (::GetProfileString("IDAPI", "DLLPATH", "", &(szCurrentPath[0]), sizeof(szCurrentPath)-1) == 0)
		{
			::WriteProfileString("IDAPI", "DLLPATH", pszIdapiDllPath);
			TRACE1("Set IDAPI DLL Path to |%s|\n", pszIdapiDllPath);
		}
#ifdef _DEBUG
		else
		{
			TRACE1("Using previously set IDAPI DLL path |%s|\n", &(szCurrentPath[0]));
		}
#endif
	}
}

#endif


/////////////////////////////////////////////////////////////////////////////
// CBdeDatabase

CBdeDatabase::CBdeDatabase()
{
//	TRACE1("%08lx Database::Construct\n", this);
	m_hDb = NULL;
}

CBdeDatabase::~CBdeDatabase()
{
//	TRACE1("%08lx Database::Destruct\n", this);
	
	TRY
	{
		Close();
		ASSERT(m_hDb == NULL);
	}
	END_TRY
}

void CBdeDatabase::Open(DBIOpenMode nOpenMode /*=dbiREADWRITE*/, LPCSTR pszName /*=NULL*/, LPCSTR pszDbType /*=NULL*/)
{
//	TRACE1("%08lx Database::Open\n", this);
	
	Close();
	ASSERT(m_hDb == NULL);
	
#ifdef WIN32
	CBdeException::CheckResult(PmwThunk.ThunkDbiOpenDatabase((pCHAR)pszName, (pCHAR)pszDbType, nOpenMode, dbiOPENSHARED, NULL, 0, NULL, NULL, &m_hDb));
#else
	CBdeException::CheckResult(DbiOpenDatabase((pCHAR)pszName, (pCHAR)pszDbType, nOpenMode, dbiOPENSHARED, NULL, 0, NULL, NULL, &m_hDb));
#endif
	ASSERT(m_hDb != NULL);
}

void CBdeDatabase::Close(void)
{
//	TRACE1("%08lx Database::Close\n", this);
	
#ifdef _DEBUG	
	if (m_hDb == NULL)
	{
//		TRACE1("%08lx Database::Close called with NULL database\n", this);
	}
#endif

	if (m_hDb != NULL)
	{
#ifdef WIN32
		CBdeException::CheckResult(PmwThunk.ThunkDbiCloseDatabase(&m_hDb));
#else
		CBdeException::CheckResult(DbiCloseDatabase(&m_hDb));
#endif
	}
	
	ASSERT(m_hDb == NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CBdeCursor

CBdeCursor::CBdeCursor()
{
//	TRACE1("%08lx Cursor::Construct\n", this);
	
	m_hCursor = NULL;
	m_pRecordBuffer = NULL;
	m_pFieldDescriptions = NULL;
}
					
CBdeCursor::~CBdeCursor()
{
//	TRACE1("%08lx Cursor::Destruct\n", this);
	
	TRY
	{
		Close();
		ASSERT(m_hCursor == NULL);	
		ASSERT(m_pRecordBuffer == NULL);	
		ASSERT(m_pFieldDescriptions == NULL);	
	}
	END_TRY
	
	if (m_pRecordBuffer != NULL)
	{
		delete [] m_pRecordBuffer;
		m_pRecordBuffer = NULL;
	}
	
	if (m_pFieldDescriptions != NULL)
	{
		delete [] m_pFieldDescriptions;
		m_pFieldDescriptions = NULL;
	}
}

void CBdeCursor::Open(CBdeDatabase* pDatabase, LPCSTR pszTableName, DBIOpenMode nOpenMode /*=dbiREADWRITE*/, LPCSTR pszDriverType /*=NULL*/, LPCSTR pszIndexName /*=NULL*/, LPCSTR pszIndexTagName /*=NULL*/, UINT16 uIndexId /*=0*/)
{
//	TRACE1("%08lx Cursor::Open\n", this);
	
	ASSERT(pDatabase != NULL);
	ASSERT(pszTableName != NULL);

	Close();
	ASSERT(m_hCursor == NULL);	
	ASSERT(m_pRecordBuffer == NULL);	
	ASSERT(m_pFieldDescriptions == NULL);	

#ifdef WIN32
	CBdeException::CheckResult(PmwThunk.ThunkDbiOpenTable(pDatabase->Handle(), (pCHAR)pszTableName, (pCHAR)pszDriverType, (pCHAR)pszIndexName, (pCHAR)pszIndexTagName, uIndexId, (DBIOpenMode)nOpenMode, dbiOPENSHARED, xltFIELD, FALSE, NULL, &m_hCursor));
	CBdeException::CheckResult(PmwThunk.ThunkDbiGetCursorProps(m_hCursor, &m_CursorProperties));

	m_pRecordBuffer = new BYTE[m_CursorProperties.iRecBufSize];
	ASSERT(m_pRecordBuffer != NULL);
	
	m_pFieldDescriptions = new FLDDesc16[m_CursorProperties.iFields];
	ASSERT(m_pFieldDescriptions != NULL);
	
	CBdeException::CheckResult(PmwThunk.ThunkDbiGetFieldDescs(m_hCursor, m_pFieldDescriptions, m_CursorProperties.iFields));
#else
	CBdeException::CheckResult(DbiOpenTable(pDatabase->Handle(), (pCHAR)pszTableName, (pCHAR)pszDriverType, (pCHAR)pszIndexName, (pCHAR)pszIndexTagName, uIndexId, nOpenMode, dbiOPENSHARED, xltFIELD, FALSE, NULL, &m_hCursor));
	CBdeException::CheckResult(DbiGetCursorProps(m_hCursor, &m_CursorProperties));
	
	m_pRecordBuffer = new BYTE[m_CursorProperties.iRecBufSize];
	ASSERT(m_pRecordBuffer != NULL);
	
	m_pFieldDescriptions = new FLDDesc[m_CursorProperties.iFields];
	ASSERT(m_pFieldDescriptions != NULL);
	
	CBdeException::CheckResult(DbiGetFieldDescs(m_hCursor, m_pFieldDescriptions));
#endif
}
					
void CBdeCursor::Close(void)
{
//	TRACE1("%08lx Cursor::Close\n", this);

#ifdef _DEBUG	
	if (m_hCursor == NULL)
	{
//		TRACE1("%08lx Cursor::Close called with NULL cursor\n", this);
	}
#endif

	if (m_pRecordBuffer != NULL)
	{
		delete [] m_pRecordBuffer;
		m_pRecordBuffer = NULL;
	}
	
	if (m_pFieldDescriptions != NULL)
	{
		delete [] m_pFieldDescriptions;
		m_pFieldDescriptions = NULL;
	}
		
	if (m_hCursor != NULL)
	{
#ifdef WIN32
		CBdeException::CheckResult(PmwThunk.ThunkDbiCloseCursor(&m_hCursor));
#else
		CBdeException::CheckResult(DbiCloseCursor(&m_hCursor));
#endif
		ASSERT(m_hCursor == NULL);
	}
}

BOOL CBdeCursor::AtStart(void)
{
#ifdef WIN32
	ASSERT(FALSE);
	return FALSE;
#else
	BOOL fResult = DbiGetPriorRecord(m_hCursor, dbiNOLOCK, NULL, 0) == DBIERR_BOF;
	DbiGetNextRecord(m_hCursor, dbiNOLOCK, NULL, 0);
	return fResult;
#endif
}

BOOL CBdeCursor::AtEnd(void)
{
#ifdef WIN32
	BOOL fResult = PmwThunk.ThunkDbiGetNextRecord(m_hCursor, dbiNOLOCK, NULL, 0, m_CursorProperties.iRecBufSize) == DBIERR_EOF;
	PmwThunk.ThunkDbiGetPriorRecord(m_hCursor, dbiNOLOCK, NULL, 0, m_CursorProperties.iRecBufSize);
	return fResult;
#else
	BOOL fResult = DbiGetNextRecord(m_hCursor, dbiNOLOCK, NULL, 0) == DBIERR_EOF;
	DbiGetPriorRecord(m_hCursor, dbiNOLOCK, NULL, 0);
	return fResult;
#endif
}

void CBdeCursor::ToStart(void)
{
#ifdef WIN32
	CBdeException::CheckResult(PmwThunk.ThunkDbiSetToBegin(m_hCursor));
#else
	CBdeException::CheckResult(DbiSetToBegin(m_hCursor));
#endif
}

void CBdeCursor::ToEnd(void)
{
#ifdef WIN32
	ASSERT(FALSE);
#else
	CBdeException::CheckResult(DbiSetToEnd(m_hCursor));
#endif
}

BOOL CBdeCursor::GetRecord(void)
{
#ifdef WIN32
	ASSERT(FALSE);
	return FALSE;
#else
	return CBdeException::CheckResult(DbiGetRecord(m_hCursor, dbiNOLOCK, m_pRecordBuffer, &m_RecordProperties), DBIERR_BOF, DBIERR_EOF, 0) == DBIERR_NONE;
#endif
}

BOOL CBdeCursor::GetNextRecord(void)
{
#ifdef WIN32
	return CBdeException::CheckResult(PmwThunk.ThunkDbiGetNextRecord(m_hCursor, dbiNOLOCK, m_pRecordBuffer, &m_RecordProperties, m_CursorProperties.iRecBufSize), DBIERR_BOF, DBIERR_EOF, 0) == DBIERR_NONE;
#else
	return CBdeException::CheckResult(DbiGetNextRecord(m_hCursor, dbiNOLOCK, m_pRecordBuffer, &m_RecordProperties), DBIERR_BOF, DBIERR_EOF, 0) == DBIERR_NONE;
#endif
}

BOOL CBdeCursor::GetPriorRecord(void)
{
#ifdef WIN32
	ASSERT(FALSE);
	return FALSE;
#else
	return CBdeException::CheckResult(DbiGetPriorRecord(m_hCursor, dbiNOLOCK, m_pRecordBuffer, &m_RecordProperties), DBIERR_BOF, DBIERR_EOF, 0) == DBIERR_NONE;
#endif
}

BOOL CBdeCursor::SeekToRecord(UINT16 uField, pBYTE pSrc)
{
#ifdef WIN32
/* Stick the key into the record and seek to it. */
	PutField(uField, pSrc);
	return CBdeException::CheckResult(PmwThunk.ThunkDbiSetToKey(m_hCursor, keySEARCHEQ, FALSE, 0, 0, m_pRecordBuffer, m_CursorProperties.iRecBufSize)) == DBIERR_NONE;
#else
/* Stick the key into the record and seek to it. */
	PutField(uField, pSrc);
	return CBdeException::CheckResult(DbiSetToKey(m_hCursor, keySEARCHEQ, FALSE, 0, 0, m_pRecordBuffer)) == DBIERR_NONE;
#endif
}

void CBdeCursor::GetField(UINT16 uField, pBYTE pDest, pBOOL pbBlank /*=NULL*/) const
{
#ifdef WIN32
	ValidateFieldNumber(uField);
	CBdeException::CheckResult(PmwThunk.ThunkDbiGetField(m_hCursor, uField, m_pRecordBuffer, pDest, (pBOOL)pbBlank, m_CursorProperties.iRecBufSize, FieldDescription(uField)->iLen));
#else
	ValidateFieldNumber(uField);
	CBdeException::CheckResult(DbiGetField(m_hCursor, uField, m_pRecordBuffer, pDest, pbBlank));
#endif
}

void CBdeCursor::GetField(LPCSTR pszField, pBYTE pDest, pBOOL pbBlank /*=NULL*/) const
{
	GetField(FieldNumber(pszField), pDest, pbBlank);
}
	
void CBdeCursor::PutField(UINT16 uField, pBYTE pSrc)
{
#ifdef WIN32
	ValidateFieldNumber(uField);
	CBdeException::CheckResult(PmwThunk.ThunkDbiPutField(m_hCursor, uField, m_pRecordBuffer, pSrc, m_CursorProperties.iRecBufSize, FieldDescription(uField)->iLen));
#else
	ValidateFieldNumber(uField);
	CBdeException::CheckResult(DbiPutField(m_hCursor, uField, m_pRecordBuffer, pSrc));
#endif
}

void CBdeCursor::PutField(LPCSTR pszField, pBYTE pSrc)
{
	PutField(FieldNumber(pszField), pSrc);
}
	
UINT16 CBdeCursor::Fields(void) const
{
	return m_CursorProperties.iFields;
}

#ifdef WIN32
const FLDDesc16* CBdeCursor::FieldDescription(UINT16 uField) const
#else
const FLDDesc* CBdeCursor::FieldDescription(UINT16 uField) const
#endif
{
	ValidateFieldNumber(uField);
	return m_pFieldDescriptions+(uField-1);
}

#ifdef WIN32
const FLDDesc16* CBdeCursor::FieldDescription(LPCSTR pszField) const
#else
const FLDDesc* CBdeCursor::FieldDescription(LPCSTR pszField) const
#endif
{
	return FieldDescription(FieldNumber(pszField));
}

UINT16 CBdeCursor::FieldNumber(LPCSTR pszField) const
{
	for (UINT16 uField = 1; uField <= Fields(); uField++)
	{
#ifdef WIN32
		const FLDDesc16* pFieldDescription = FieldDescription(uField);
#else
		const FLDDesc* pFieldDescription = FieldDescription(uField);
#endif
		
		if (strcmp(pFieldDescription->szName, pszField) == 0)
		{
			return uField;
		}
	}
	
	return 0;
}

void CBdeCursor::ValidateFieldNumber(UINT16 uField) const
{
	if ((uField < 1) || (uField > Fields()))
	{
		CBdeException::CheckResult(DBIERR_INVALIDFIELDNAME);
	}
}
