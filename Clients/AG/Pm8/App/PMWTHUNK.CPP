/*
// $Workfile: PMWTHUNK.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWTHUNK.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:26   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:44:58   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:07:02   FRED
// Initial revision.
// 
//    Rev 1.8   04 Oct 1995 10:56:32   FRED
// 32 bit TrueType engine
// 
//    Rev 1.7   28 Sep 1995 14:51:08   JAY
// Now calls AttachThreadInput() to join the PMWTHUNK thread and the main one.
// 
//    Rev 1.6   27 Sep 1995 14:38:10   FRED
// BDE Thunking
// 
//    Rev 1.5   25 Sep 1995 11:38:24   FRED
// TrueType speedups
// 
//    Rev 1.4   22 Sep 1995 10:36:00   FRED
// Address and Sender Books
// 
//    Rev 1.3   21 Sep 1995 12:29:18   FRED
// Position Dialog
// 
//    Rev 1.2   20 Sep 1995 10:01:22   FRED
// PmwThunk support for PMWTT.DLL
// 
//    Rev 1.1   15 Sep 1995 15:25:50   FRED
// Does not use path manager (not available to DB stuff).
// 
// 
//    Rev 1.0   15 Sep 1995 14:52:28   FRED
// Initial revision.
*/

#include "stdafx.h"

#include <stdarg.h>

#include "pmgdb.h"
#include "geronimo.h"
#include "fengine.h"
#include "tfengine.h"
#include "pmwthunk.h"


// Global CPmwThunk object

CPmwThunk PmwThunk;


// CPmwThunk implementation

CPmwThunk::CPmwThunk()
{
	m_wContext = 0;
	m_hPmwClientWindow = NULL;
	m_hPmwThunkWindow = NULL;
	m_dwMessage = 0;
	m_dwDataLength = 0;
	m_pData = NULL;
}

CPmwThunk::~CPmwThunk()
{
	Disconnect();

	if (m_hPmwClientWindow != NULL)
	{
		::DestroyWindow(m_hPmwClientWindow);
		m_hPmwClientWindow = NULL;
	}

	FreeMessage();
}

BOOL CPmwThunk::Initialize(void)
{
	BOOL fResult = FALSE;

	CString csClass;
	CString csWindow;

	TRY
	{
		// First, connect to PMWTHUNK.EXE. This includes checking if the PmwThunkWindow
		// exists and performing a connext operation. If there is no PmwThunkWindow, then
		// WinExec() is called to load the thunking application.

		if (Connect())
		{
			// We're connected! It's time to create our window that PMWTHUNK will use to send
			// information back to us.

			wsprintf(csClass.GetBuffer(100), PmwClientClassName, Context());
			csClass.ReleaseBuffer();
		
			wsprintf(csWindow.GetBuffer(100), PmwClientWindowName, Context());
			csWindow.ReleaseBuffer();

			// Register the class for the that will be used for the window.
			WNDCLASS WndClass;

			WndClass.style = CS_GLOBALCLASS;
			WndClass.lpfnWndProc = CPmwThunk::HandleMessage;
			WndClass.cbClsExtra = 0;
			WndClass.cbWndExtra = 0;
			WndClass.hInstance = AfxGetInstanceHandle();
			WndClass.hIcon = NULL;
			WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
			WndClass.lpszMenuName = NULL;
			WndClass.lpszClassName = csClass;
			
			// Register the class
			if (RegisterClass(&WndClass))
			{
				m_hPmwClientWindow = CreateWindow(
					csClass,
					csWindow,
					0,
					0, 0, 0, 0,
					NULL,
					NULL,
					AfxGetInstanceHandle(),
					NULL);
					
				if (m_hPmwClientWindow != NULL)
				{
					// Everything worked, return TRUE.
					fResult = TRUE;
				}
			}
		}
	}
	END_TRY

	if (!fResult)
	{
		// Something went wrong, disconnect.
		Disconnect();
	}

	return fResult;
}

BOOL CPmwThunk::Connect(void)
{
#ifdef _MAC
   // TODO:MAC -- CPmwThunk::Connect, probably need to re-think all of this
   // some stuff down here not supported on MAC
   return FALSE;
#else // _MAC
	BOOL fResult = FALSE;

	// Check if we're already connected.
	if (m_hPmwThunkWindow == NULL)
	{
		// Not yet connected, see if the window already exists.
		m_hPmwThunkWindow = FindWindow(PmwThunkClassName, PmwThunkWindowName);

		if (m_hPmwThunkWindow == NULL)
		{
			// Window does not exist, WinExec PMWTHUNK.EXE. Of course, there
			// are some race conditions here most of which we are ignoring for
			// the moment.

			LPCSTR pszPmwThunkExe = StorageFile::TranslateFileName("PMWTHUNK.EXE", FALSE);

			PROCESS_INFORMATION ProcessInformation;

			STARTUPINFO StartupInfo;
			StartupInfo.cb = sizeof(StartupInfo);
			StartupInfo.lpReserved = NULL;
			StartupInfo.lpDesktop = NULL;
			StartupInfo.lpTitle = NULL;
			StartupInfo.dwX = 0;
			StartupInfo.dwY = 0;
			StartupInfo.dwXSize = 0;
			StartupInfo.dwYSize = 0;
			StartupInfo.dwXCountChars = 0;
			StartupInfo.dwYCountChars = 0;
			StartupInfo.dwFillAttribute = 0;
			StartupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK;
			StartupInfo.wShowWindow = SW_SHOWNORMAL;
			StartupInfo.cbReserved2 = 0;
			StartupInfo.lpReserved2 = NULL;
			StartupInfo.hStdInput = NULL;
			StartupInfo.hStdOutput = NULL;
			StartupInfo.hStdError = NULL;

			if (CreateProcess(
						NULL,
						(LPTSTR)pszPmwThunkExe,			// use command line for 16 bit process in NT
						NULL,
						NULL,
						FALSE,
						NORMAL_PRIORITY_CLASS,
						NULL,
						NULL,
						&StartupInfo,
						&ProcessInformation))
			{
				// Wait until the process has finished it's initialization (or 10 seconds, whichever is less).
				WaitForInputIdle(ProcessInformation.hProcess, 10*1000);

				AttachThreadInput(ProcessInformation.dwThreadId,
										GetCurrentThreadId(),
										TRUE);

				// Try again to find the window.
				m_hPmwThunkWindow = FindWindow(PmwThunkClassName, PmwThunkWindowName);
			}
		}

		if (m_hPmwThunkWindow != NULL)
		{
			// We have the window to talk to (m_hPmwThunkWindow), send a connect
			// message to try and connect.

			m_wContext = (WORD)SendMessage(THUNK_CLASS_Control, THUNK_OP_CONTROL_Connect);

			if (m_wContext != 0)
			{
				// We all connected up.
				fResult = TRUE;
			}
		}
	}

	return fResult;
#endif // else _MAC
}

BOOL CPmwThunk::Disconnect(void)
{
	BOOL fResult = FALSE;

	// Check if we are connected. If not, then there's nothing to do.
	if (m_hPmwThunkWindow != NULL)
	{
		// Send a disconnect message to PMWTHUNK.EXE
		SendMessage(THUNK_CLASS_Control, THUNK_OP_CONTROL_Disconnect);
		m_hPmwThunkWindow = NULL;
	}

	return fResult;
}

LRESULT CPmwThunk::SendMessage(WORD wClass, WORD wOp, DWORD dwCount /*=0*/, LPVOID lpData /*=NULL*/)
{
	LRESULT lResult = 0;
	
	ASSERT(m_hPmwThunkWindow != NULL);
	ASSERT((dwCount == 0) || (lpData != NULL));
	
	if ((m_hPmwThunkWindow != NULL)
	 && ((dwCount == 0) || (lpData != NULL)))
	{
		COPYDATASTRUCT CopyData;
		
		CopyData.dwData = THUNK_MESSAGE(Context(), wClass, wOp);
		CopyData.cbData = dwCount;
		CopyData.lpData = lpData;
		
		lResult = ::SendMessage(m_hPmwThunkWindow, WM_COPYDATA, (WPARAM)m_hPmwClientWindow, (LPARAM)(&CopyData));
	}
	
	return lResult;
}

LRESULT PASCAL CPmwThunk::HandleMessage(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;

	if (uMsg == WM_COPYDATA)
	{
		ASSERT(lParam != NULL);
		if (lParam != NULL)
		{
			// Check for special messages processing.
			// By default, we just a make a copy of the data.
			
			WORD wContext = THUNK_MESSAGE_CONTEXT(((PCOPYDATASTRUCT)lParam)->dwData);
			WORD wClass = THUNK_MESSAGE_CLASS(((PCOPYDATASTRUCT)lParam)->dwData);
			WORD wOp = THUNK_MESSAGE_OP(((PCOPYDATASTRUCT)lParam)->dwData);
			DWORD dwCount = ((PCOPYDATASTRUCT)lParam)->cbData;
			BOOL fMessageHandled = FALSE;
			
	//od("PMW.32 - COPYDATA Context %d Class %d Op %d Count %08lx Data %08lx\n\r", wContext, wClass, wOp, dwCount, (DWORD)(((PCOPYDATASTRUCT)lParam)->lpData));

			switch (wClass)
			{
				case THUNK_CLASS_Pmwdlgs:
				{
	//od("PMW.32 - PMWDLGS Message\n\r", wOp);
					switch (wOp)
					{
						case THUNK_OP_PMWDLGS_EndDialog:
						{
							// Process an EndDialog call.

	//od("PMW.32 - PMWDLGS EndDialog\n\r");

							TRY
							{
								ASSERT((dwCount == sizeof(THUNK_DATA_PMWDLGS_EndDialog)) && (((PCOPYDATASTRUCT)lParam)->lpData != NULL));
								if (dwCount == sizeof(THUNK_DATA_PMWDLGS_EndDialog))
								{
									THUNK_DATA_PMWDLGS_EndDialog* pData = (THUNK_DATA_PMWDLGS_EndDialog*)(((PCOPYDATASTRUCT)lParam)->lpData);

									if (pData != NULL)
									{
	//od("PMW.32 - PMWDLGS EndDialog lResult %08lx\n\r", pData->lResult);
										PmwThunk.PmwdlgsResult(pData->lResult);

										// Post a message to ourselves so we will wake up!
										::PostMessage(PmwThunk.m_hPmwClientWindow, WM_CLEAR, 0, 0);

										lResult = TRUE;
									}
								}
							}
							END_TRY

							fMessageHandled = TRUE;
							break;
						}
					}

					break;
				}
			}
		
			if (!fMessageHandled)
			{
	//od("PMW.32 - Save message\n\r");
				// Default action: save the new message data.
				if (PmwThunk.SetMessage((PCOPYDATASTRUCT)lParam))
				{
					lResult = TRUE;
				}
			}
		}
	}
	else
	{
		lResult = DefWindowProc(hWindow, uMsg, wParam, lParam);
	}

	return lResult;
}

BOOL CPmwThunk::SetMessage(PCOPYDATASTRUCT pData)
{
	BOOL fResult = FALSE;

	// Free any current message data.

	FreeMessage();

	// Save the current data.

	m_dwMessage = pData->dwData;

	if ((pData->cbData != 0) && (pData->lpData != NULL))
	{
		TRY
		{
			// Make a copy of the data.

			PVOID pNewData = new char[pData->cbData];
			ASSERT(pNewData != NULL);

			if (pNewData != NULL)
			{
				memcpy(pNewData, pData->lpData, pData->cbData);
				m_dwDataLength = pData->cbData;
				m_pData = pNewData;

				fResult = TRUE;
			}
		}
		END_TRY
	}

	return fResult;
}

void CPmwThunk::FreeMessage(void)
{
	m_dwMessage = 0;
	m_dwDataLength = 0;

	if (m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
}

DBIResult CPmwThunk::ThunkDbiGetErrorContext(INT16 eContext, pCHAR pszContext)
{
//od("PMW.32 - ThunkDbiGetErrorContext\n\r");
	THUNK_DATA_BDE_DbiGetErrorContext Data;
	Data.eContext = eContext;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetErrorContext, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorContext));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorContext))
		{
			strcpy(pszContext, ((THUNK_DATA_BDE_DbiGetErrorContext*)MessageData())->szContext);
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetErrorContext Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetErrorEntry(UINT16 uEntry, pUINT32 pulNativeError, pCHAR pszError)
{
//od("PMW.32 - ThunkDbiGetErrorEntry\n\r");
	THUNK_DATA_BDE_DbiGetErrorEntry Data;
	Data.uEntry = uEntry;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetErrorEntry, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorEntry));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorEntry))
		{
			*pulNativeError = ((THUNK_DATA_BDE_DbiGetErrorEntry*)MessageData())->ulNativeError;
			strcpy(pszError, ((THUNK_DATA_BDE_DbiGetErrorEntry*)MessageData())->szError);
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetErrorEntry Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetErrorInfo(BOOL bFull, pDBIErrInfo pErrInfo)
{
//od("PMW.32 - ThunkDbiGetErrorInfo\n\r");
	THUNK_DATA_BDE_DbiGetErrorInfo Data;
	Data.bFull = bFull;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetErrorInfo, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorInfo));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorInfo))
		{
			*pErrInfo = ((THUNK_DATA_BDE_DbiGetErrorInfo*)MessageData())->ErrInfo;
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetErrorInfo Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetErrorString(DBIResult rslt, pCHAR pszError)
{
//od("PMW.32 - ThunkDbiGetErrorString\n\r");
	THUNK_DATA_BDE_DbiGetErrorString Data;
	Data.rslt = rslt;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetErrorString, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorString));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetErrorString))
		{
			strcpy(pszError, ((THUNK_DATA_BDE_DbiGetErrorString*)MessageData())->szError);
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetErrorString Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiOpenDatabase(pCHAR pszDbName, pCHAR pszDbType, DBIOpenMode eOpenMode, DBIShareMode eShareMode, pCHAR pszPassword, UINT16 iOptFields, pFLDDesc16 pOptFldDesc, pBYTE pOptParams, phDBIDb phDb)
{
//od("PMW.32 - ThunkDbiOpenDatabase\n\r");
	ASSERT(pszPassword == NULL);
	ASSERT(iOptFields == 0);
	ASSERT(pOptFldDesc == NULL);
	ASSERT(pOptParams == NULL);
	THUNK_DATA_BDE_DbiOpenDatabase Data;
	strcpy(Data.szDbName, pszDbName != NULL ? pszDbName : "");
	strcpy(Data.szDbType, pszDbType != NULL ? pszDbType : "");
	Data.eOpenMode = eOpenMode;
	Data.eShareMode = eShareMode;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiOpenDatabase, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiOpenDatabase));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiOpenDatabase))
		{
			*phDb = ((THUNK_DATA_BDE_DbiOpenDatabase*)MessageData())->hDb;
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiOpenDatabase Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiCloseDatabase(phDBIDb phDb)
{
//od("PMW.32 - ThunkDbiCloseDatabase\n\r");
	THUNK_DATA_BDE_DbiCloseDatabase Data;
	Data.hDb = *phDb;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiCloseDatabase, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiCloseDatabase));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiCloseDatabase))
		{
			*phDb = ((THUNK_DATA_BDE_DbiCloseDatabase*)MessageData())->hDb;
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiCloseDatabase Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiOpenTable(hDBIDb hDb, pCHAR pszTableName, pCHAR pszDriverType, pCHAR pszIndexName, pCHAR pszIndexTagName, UINT16 iIndexId,
	DBIOpenMode eOpenMode, DBIShareMode eShareMode, XLTMode eXltMode, BOOL bUniDirectional, pBYTE pOptParams, phDBICur phCursor)
{
//od("PMW.32 - ThunkDbiOpenTable\n\r");
	ASSERT(pOptParams == NULL);
	THUNK_DATA_BDE_DbiOpenTable Data;
	Data.hDb = hDb;
	strcpy(Data.szTableName, pszTableName != NULL ? pszTableName : "");
	strcpy(Data.szDriverType, pszDriverType != NULL ? pszDriverType : "");
	strcpy(Data.szIndexName, pszIndexName != NULL ? pszIndexName : "");
	strcpy(Data.szIndexTagName, pszIndexTagName != NULL ? pszIndexTagName : "");
	Data.iIndexId = iIndexId;
	Data.eOpenMode = eOpenMode;
	Data.eShareMode = eShareMode;
	Data.eXltMode = eXltMode;
	Data .bUniDirectional = bUniDirectional;
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiOpenTable, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiOpenTable));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiOpenTable))
		{
			*phCursor = ((THUNK_DATA_BDE_DbiOpenTable*)MessageData())->hCursor;
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiOpenTable Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetCursorProps(hDBICur hCursor, pCURProps16 pCurProps)
{
//od("PMW.32 - ThunkDbiGetCursorProps\n\r");
	THUNK_DATA_BDE_DbiGetCursorProps Data;
	Data.hCursor = hCursor;
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetCursorProps, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetCursorProps));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetCursorProps))
		{
			*pCurProps = ((THUNK_DATA_BDE_DbiGetCursorProps*)MessageData())->CurProps;
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetCursorProps Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetFieldDescs(hDBICur hCursor, pFLDDesc16 pFldDesc, WORD wNumberOfFields)
{
//od("PMW.32 - ThunkDbiGetFieldDescs\n\r");
	DBIResult Result = DBIERR_INVALIDPARAM;
	WORD wSize = sizeof(THUNK_DATA_BDE_DbiGetFieldDescs)+wNumberOfFields*sizeof(FLDDesc16);
	THUNK_DATA_BDE_DbiGetFieldDescs* pData = (THUNK_DATA_BDE_DbiGetFieldDescs*)(new BYTE[wSize]);
	ASSERT(pData != NULL);
	if (pData != NULL)
	{
		pData->hCursor = hCursor;
		FreeMessage();
		Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetFieldDescs, wSize, pData);
		if (MessageData() != NULL)
		{
			ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetFieldDescs));
			if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetFieldDescs))
			{
				ASSERT(pFldDesc != NULL);
				if (pFldDesc != NULL)
				{
					memcpy(pFldDesc, &(((THUNK_DATA_BDE_DbiGetFieldDescs*)MessageData())->FldDesc[0]), wNumberOfFields*sizeof(FLDDesc16));
				}
			}
		}

		delete [] (LPBYTE)pData;
		pData = NULL;
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetFieldDescs Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiCloseCursor(phDBICur phCursor)
{
//od("PMW.32 - ThunkDbiCloseCursor\n\r");
	THUNK_DATA_BDE_DbiCloseCursor Data;
	Data.hCursor = *phCursor;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiCloseCursor, sizeof(Data), &Data);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiCloseCursor));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiCloseCursor))
		{
			*phCursor = ((THUNK_DATA_BDE_DbiCloseCursor*)MessageData())->hCursor;
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiCloseCursor Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetNextRecord(hDBICur hCursor, DBILockType eLock, pBYTE pRecBuf, pRECProps16 pRecProps, WORD wRecordLength)
{
//od("PMW.32 - ThunkDbiGetNextRecord\n\r");
	DBIResult Result = DBIERR_INVALIDPARAM;
	WORD wSize = sizeof(THUNK_DATA_BDE_DbiGetNextRecord)+wRecordLength;
	THUNK_DATA_BDE_DbiGetNextRecord* pData = (THUNK_DATA_BDE_DbiGetNextRecord*)(new BYTE[wSize]);
	ASSERT(pData != NULL);
	if (pData != NULL)
	{
		pData->hCursor = hCursor;
		pData->eLock = eLock;
		FreeMessage();
		Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetNextRecord, wSize, pData);
		if (MessageData() != NULL)
		{
			ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetNextRecord));
			if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetNextRecord))
			{
				if (pRecBuf != NULL)
				{
					memcpy(pRecBuf, ((THUNK_DATA_BDE_DbiGetNextRecord*)MessageData())->RecBuf, wRecordLength);
				}

				if (pRecProps != NULL)
				{
					*pRecProps = ((THUNK_DATA_BDE_DbiGetNextRecord*)MessageData())->RecProps;
				}
			}
		}

		delete [] (LPBYTE)pData;
		pData = NULL;
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetNextRecord Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetPriorRecord(hDBICur hCursor, DBILockType eLock, pBYTE pRecBuf, pRECProps16 pRecProps, WORD wRecordLength)
{
//od("PMW.32 - ThunkDbiGetPriorRecord\n\r");
	DBIResult Result = DBIERR_INVALIDPARAM;
	WORD wSize = sizeof(THUNK_DATA_BDE_DbiGetPriorRecord)+wRecordLength;
	THUNK_DATA_BDE_DbiGetPriorRecord* pData = (THUNK_DATA_BDE_DbiGetPriorRecord*)(new BYTE[wSize]);
	ASSERT(pData != NULL);
	if (pData != NULL)
	{
		pData->hCursor = hCursor;
		pData->eLock = eLock;
		FreeMessage();
		Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetPriorRecord, wSize, pData);
		if (MessageData() != NULL)
		{
			ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetPriorRecord));
			if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetPriorRecord))
			{
				if (pRecBuf != NULL)
				{
					memcpy(pRecBuf, ((THUNK_DATA_BDE_DbiGetPriorRecord*)MessageData())->RecBuf, wRecordLength);
				}

				if (pRecProps != NULL)
				{
					*pRecProps = ((THUNK_DATA_BDE_DbiGetPriorRecord*)MessageData())->RecProps;
				}
			}
		}

		delete [] (LPBYTE)pData;
		pData = NULL;
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetPriorRecord Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiSetToBegin(hDBICur hCursor)
{
//od("PMW.32 - ThunkDbiSetToBegin\n\r");
	THUNK_DATA_BDE_DbiSetToBegin Data;
	Data.hCursor = hCursor;
	FreeMessage();
	DBIResult Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiSetToBegin, sizeof(Data), &Data);
	FreeMessage();
//od("PMW.32 - ThunkDbiSetToBegin Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiSetToKey(hDBICur hCursor, DBISearchCond eSearchCond, BOOL bDirectKey, UINT16 iFields, UINT16 iLen, pBYTE pBuf, WORD wRecordLength)
{
//od("PMW.32 - ThunkDbiSetToKey\n\r");
	DBIResult Result = DBIERR_INVALIDPARAM;
	WORD wSize = sizeof(THUNK_DATA_BDE_DbiSetToKey)+wRecordLength;
	THUNK_DATA_BDE_DbiSetToKey* pData = (THUNK_DATA_BDE_DbiSetToKey*)(new BYTE[wSize]);
	ASSERT(pData != NULL);
	if (pData != NULL)
	{
		pData->hCursor = hCursor;
		pData->eSearchCond = eSearchCond;
		pData->bDirectKey = bDirectKey;
		pData->iFields = iFields;
		pData->iLen = iLen;
		memcpy(pData->Buf, pBuf, wRecordLength);
		FreeMessage();
		Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiSetToKey, wSize, pData);
		delete [] (LPBYTE)pData;
		pData = NULL;
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiSetToKey Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiGetField(hDBICur hCursor, UINT16 iField, pBYTE pRecBuf, pBYTE pDest, pBOOL pbBlank, WORD wRecordLength, WORD wFieldLength)
{
//od("PMW.32 - ThunkDbiGetField\n\r");
	DBIResult Result = DBIERR_INVALIDPARAM;
	WORD wSize = sizeof(THUNK_DATA_BDE_DbiGetField)+wRecordLength+wFieldLength;
	THUNK_DATA_BDE_DbiGetField* pData = (THUNK_DATA_BDE_DbiGetField*)(new BYTE[wSize]);
	ASSERT(pData != NULL);
	if (pData != NULL)
	{
		pData->hCursor = hCursor;
		pData->iField = iField;
		pData->wRecLen = wRecordLength;
		memcpy(pData->Buf, pRecBuf, wRecordLength);
		FreeMessage();
		Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiGetField, wSize, pData);
		if (MessageData() != NULL)
		{
			ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetField));
			if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiGetField))
			{
				if (pDest != NULL)
				{
					memcpy(pDest, &(((THUNK_DATA_BDE_DbiGetField*)MessageData())->Buf[wRecordLength]), wFieldLength);
				}
				if (pbBlank != NULL)
				{
					*pbBlank = ((THUNK_DATA_BDE_DbiGetField*)MessageData())->bBlank;
				}
			}
		}

		delete [] (LPBYTE)pData;
		pData = NULL;
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiGetField Result %ld\n\r", (DWORD)Result);
	return Result;
}

DBIResult CPmwThunk::ThunkDbiPutField(hDBICur hCursor, UINT16 iField, pBYTE pRecBuf, pBYTE pSrc, WORD wRecordLength, WORD wFieldLength)
{
//od("PMW.32 - ThunkDbiPutField\n\r");
	DBIResult Result = DBIERR_INVALIDPARAM;
	WORD wSize = sizeof(THUNK_DATA_BDE_DbiPutField)+wRecordLength+wFieldLength;
	THUNK_DATA_BDE_DbiPutField* pData = (THUNK_DATA_BDE_DbiPutField*)(new BYTE[wSize]);
	ASSERT(pData != NULL);
	if (pData != NULL)
	{
		pData->hCursor = hCursor;
		pData->iField = iField;
		pData->wRecLen = wRecordLength;
		memcpy(&(pData->Buf[0]), pRecBuf, wRecordLength);
		memcpy(&(pData->Buf[wRecordLength]), pSrc, wFieldLength);
		FreeMessage();
		Result = (DBIResult)SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_DbiPutField, wSize, pData);
		if (MessageData() != NULL)
		{
			ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiPutField));
			if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_DbiPutField))
			{
				if (pRecBuf != NULL)
				{
					memcpy(pRecBuf, &(((THUNK_DATA_BDE_DbiPutField*)MessageData())->Buf[0]), wRecordLength);
				}
			}
		}

		delete [] (LPBYTE)pData;
		pData = NULL;
	}
	FreeMessage();
//od("PMW.32 - ThunkDbiPutField Result %ld\n\r", (DWORD)Result);
	return Result;
}

BOOL CPmwThunk::ThunkReadNextLabelRecord(THUNK_DATA_BDE_ReadNextLabelRecord* pData)
{
//od("PMW.32 - ThunkReadNextLabelRecord\n\r");
	ASSERT(pData != NULL);
	BOOL fResult = FALSE;
	FreeMessage();
	SendMessage(THUNK_CLASS_Bde, THUNK_OP_BDE_ReadNextLabelRecord, sizeof(*pData), pData);
	if (MessageData() != NULL)
	{
		ASSERT((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_ReadNextLabelRecord));
		if ((MessageClass() == THUNK_CLASS_Bde) && (MessageOperation() == THUNK_OP_BDE_ReadNextLabelRecord))
		{
			memcpy(pData, (THUNK_DATA_BDE_ReadNextLabelRecord*)MessageData(), sizeof(*pData));
			fResult = TRUE;
		}
	}
	FreeMessage();
//od("PMW.32 - ThunkReadNextLabelRecord Result %ld\n\r", (DWORD)Result);
	return fResult;
}
