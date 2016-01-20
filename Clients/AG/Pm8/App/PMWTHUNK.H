/*
// $Workfile: PMWTHUNK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWTHUNK.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:36   FRED
// Initial revision.
// 
//    Rev 1.6   26 Dec 1995 10:23:08   FRED
// Project structure changes for mac port
// 
//    Rev 1.5   04 Oct 1995 10:56:28   FRED
// 32 bit TrueType engine
// 
//    Rev 1.4   27 Sep 1995 14:38:14   FRED
// BDE Thunking
// 
//    Rev 1.3   25 Sep 1995 11:38:24   FRED
// TrueType speedups
// 
//    Rev 1.2   21 Sep 1995 12:29:18   FRED
// Position Dialog
// 
//    Rev 1.1   20 Sep 1995 10:01:14   FRED
// PmwThunk support for PMWTT.DLL
// 
//    Rev 1.0   15 Sep 1995 14:52:28   FRED
// Initial revision.
*/

class CPmwThunk;

#ifndef __PMWTHUNK_H__
#define __PMWTHUNK_H__

#include "thunkmsg.h"

class CPmwThunk
{

public:

	CPmwThunk();
	virtual ~CPmwThunk();

public:

	BOOL Initialize(void);

	LRESULT SendMessage(WORD wClass, WORD wOp, DWORD dwCount = 0, LPVOID lpData = NULL);

	BOOL SetMessage(PCOPYDATASTRUCT pData);
	void FreeMessage(void);

	WORD MessageContext(void)
		{ return THUNK_MESSAGE_CONTEXT(m_dwMessage); }
	WORD MessageClass(void)
		{ return THUNK_MESSAGE_CLASS(m_dwMessage); }
	WORD MessageOperation(void)
		{ return THUNK_MESSAGE_OP(m_dwMessage); }

	DWORD MessageDataLength(void)
		{ return m_dwDataLength; }
	PVOID MessageData(void)
		{ return m_pData; }

protected:

	BOOL Connect(void);
	BOOL Disconnect(void);

protected:

	WORD m_wContext;					// our context returned by Connect()

	WORD Context(void)
		{ return m_wContext; }

	HWND m_hPmwClientWindow;		// window that will receive messages
	HWND m_hPmwThunkWindow;			// window to which we will send messages

	static LRESULT PASCAL HandleMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

	DWORD m_dwMessage;
	DWORD m_dwDataLength;
	PVOID m_pData;

public:

	long PmwdlgsResult(void)
		{ return m_lPmwdlgsResult; }
	void PmwdlgsResult(long lResult)
		{ m_lPmwdlgsResult = lResult; }

protected:

	long m_lPmwdlgsResult;

public:

	DBIResult ThunkDbiGetErrorContext(INT16 eContext, pCHAR pszContext);
	DBIResult ThunkDbiGetErrorEntry(UINT16 uEntry, pUINT32 pulNativeError, pCHAR pszError);
	DBIResult ThunkDbiGetErrorInfo(BOOL bFull, pDBIErrInfo pErrInfo);
	DBIResult ThunkDbiGetErrorString(DBIResult rslt, pCHAR pszError);

	DBIResult ThunkDbiOpenDatabase(pCHAR pszDbName, pCHAR pszDbType, DBIOpenMode eOpenMode, DBIShareMode eShareMode, pCHAR pszPassword, UINT16 iOptFields, pFLDDesc16 pOptFldDesc, pBYTE pOptParams, phDBIDb phDb);
	DBIResult ThunkDbiCloseDatabase(phDBIDb phDb);

	DBIResult ThunkDbiOpenTable(hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, pCHAR pszIndexName, pCHAR pszIndexTagName, UINT16 iIndexId,
		DBIOpenMode eOpenMode, DBIShareMode eShareMode, XLTMode eXltMode, BOOL bUniDirectional, pBYTE pOptParams, phDBICur phCursor);
	DBIResult ThunkDbiGetCursorProps(hDBICur hCursor, pCURProps16 pCurProps);
	DBIResult ThunkDbiGetFieldDescs(hDBICur hCursor, pFLDDesc16 pFldDesc, WORD wNumberOfFields);
	DBIResult ThunkDbiCloseCursor(phDBICur phCursor);
	DBIResult ThunkDbiGetNextRecord(hDBICur hCursor, DBILockType eLock, pBYTE pRecBuf, pRECProps16 pRecProps, WORD wRecordLength);
	DBIResult ThunkDbiGetPriorRecord(hDBICur hCursor, DBILockType eLock, pBYTE pRecBuf, pRECProps16 pRecProps, WORD wRecordLength);
	DBIResult ThunkDbiSetToBegin(hDBICur hCursor);
	DBIResult ThunkDbiSetToKey(hDBICur hCursor, DBISearchCond eSearchCond, BOOL bDirectKey, UINT16 iFields, UINT16 iLen, pBYTE pBuf, WORD wRecordLength);
	DBIResult ThunkDbiGetField(hDBICur hCursor, UINT16 iField, pBYTE pRecBuf, pBYTE pDest, pBOOL pbBlank, WORD wRecordLength, WORD wFieldLength);
	DBIResult ThunkDbiPutField(hDBICur hCursor, UINT16 iField, pBYTE pRecBuf, pBYTE pSrc, WORD wRecordLength, WORD wFieldLength);

	BOOL ThunkReadNextLabelRecord(THUNK_DATA_BDE_ReadNextLabelRecord* pData);
};

// Extern declaration for global CPmwThunk object.

extern CPmwThunk PmwThunk;

#endif
