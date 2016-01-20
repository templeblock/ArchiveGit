/******************************************************************************
* Includes
******************************************************************************/
/* main application */
#include "stdafx.h"
//#include "Profiler.h"
#include "CommDevice.h"
#include "MeasureDialog.h"
#include "resource.h"

/******************************************************************************
* Macros
******************************************************************************/
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
* CLASS CCommRequest
******************************************************************************/

IMPLEMENT_DYNCREATE( CCommRequest, CObject)

/******************************************************************************
* Name: CCommRequest::CCommRequest
*
* Purpose: constructor, initialized data.
******************************************************************************/
CCommRequest::CCommRequest()
{
	m_dwRequestType = REQUEST_CALLBACK;
	m_dwRequestSpecial = 0;
	m_dwRequestError = ERR_NOERROR;
	m_lpszData = NULL;
	m_dwDataLength = 0;
	m_lpszResult = NULL;
	m_dwResultLength = 0;
	m_pWindow = NULL;
	m_pCompleteFunction = NULL;
	m_dwTimeout = COM_OPERATIONTIMEOUT;
	m_dwMaxTries = 1;
}

/******************************************************************************
* Name: CCommRequest::~CCommRequest
*
* Purpose: destructor
******************************************************************************/
CCommRequest::~CCommRequest()
{
	if( m_lpszData){
		delete m_lpszData;
		m_lpszData = NULL;
	}
	if( m_lpszResult){
		delete m_lpszResult;
		m_lpszResult = NULL;
	}
}

/******************************************************************************
* CLASS CCommDevice
******************************************************************************/

IMPLEMENT_DYNCREATE( CCommDevice, CObject)

/******************************************************************************
* Name: CCommDevice::CCommDevice
*
* Purpose: constructor, initialized data.
******************************************************************************/
CCommDevice::CCommDevice()
{
	m_hPortMutex = NULL;
	m_hPort = INVALID_HANDLE_VALUE;
	m_hThread = INVALID_HANDLE_VALUE;
	m_hEvent = NULL;
	m_pThread = NULL;
	m_pRequest = NULL;
	m_bTerminate = FALSE;
	m_bCancel = FALSE;
	m_dwCommState = ERR_NOERROR;
	m_bParseCRLF = TRUE;
	m_EOLChar = '\r';
	m_LFChar = -1;

	m_dwReadIntervalTimeout = COM_READINTERVALTIMEOUT;
	m_dwReadTotalTimeoutMultiplier = COM_READTOTALTIMEOUTMULTIPLIER;
	m_dwReadTotalTimeoutConstant = COM_READTOTALTIMEOUTCONSTANT;
	m_dwWriteTotalTimeoutMultiplier = COM_WRITETOTALTIMEOUTMULTIPLIER;
	m_dwWriteTotalTimeoutConstant = COM_WRITETOTALTIMEOUTCONSTANT;

	m_dwRequestTimeout = COM_OPERATIONTIMEOUT;
}

/******************************************************************************
* Name: CCommDevice::~CCommDevice
*
* Purpose: destructor
******************************************************************************/
CCommDevice::~CCommDevice()
{
	Close();
}

/******************************************************************************
* Name: CCommDevice::OpenPort
*
* Purpose: opens the com port
******************************************************************************/
HANDLE CCommDevice::OpenPort( int port)
{
	CString	sPort;
	sPort.Format( "COM%d", port);
	HANDLE hPort = CreateFile( (LPCTSTR)sPort, GENERIC_READ | GENERIC_WRITE, 0, 
												NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
	return hPort;
}

/******************************************************************************
* Name: CCommDevice::InitPort
*
* Purpose: initializes the com port
******************************************************************************/
BOOL CCommDevice::InitPort( HANDLE hPort, int baud, char parity, int data, int stop)
{
	DCB	dcb;
	if( !GetCommState( hPort, (LPDCB)&dcb))
		return FALSE;

	CString sMode;
	sMode.Format( "baud=%d parity=%c data=%d stop=%d", baud, parity, data, stop);
	if( !BuildCommDCB( (LPCTSTR)sMode, &dcb))
		return FALSE;

  dcb.fBinary					= 1;
  dcb.fErrorChar 			= 0;
	dcb.fAbortOnError		= TRUE;
  //dcb.XonLim					= 2048;
  //dcb.XoffLim					= 512;
  dcb.XonChar					= 17;		// '\x11'
  dcb.XoffChar				= 19;		// '\x13'
  dcb.EofChar					= '\r';
  dcb.EvtChar					= '\r';
	dcb.fDsrSensitivity	= 0;
	dcb.fNull						= 0;

	if( !SetCommState( hPort, (LPDCB)&dcb))
		return FALSE;

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::Open
*
* Purpose: open a connection to the device on the com port
******************************************************************************/
BOOL CCommDevice::Open( INT port, DWORD baud, CHAR parity, INT data, INT stop)
{
	// clear any previous errors
	m_dwCommState = ERR_NOERROR;

	// close any open port and terminate any open thread
	if( !Close()) return FALSE;

	// open the port
	m_hPort = OpenPort( port);
	if( m_hPort == INVALID_HANDLE_VALUE) {
		m_dwCommState = ERR_NOPORT;
		return FALSE;
	}

	// initialize the com port to read and write to the device.
	if( !InitPort( m_hPort, baud, parity, data, stop)) {
		m_dwCommState = ERR_NOPORT;
		CloseHandle( m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// set the port timeouts
	COMMTIMEOUTS comTime;
	comTime.ReadIntervalTimeout = m_dwReadIntervalTimeout;
	comTime.ReadTotalTimeoutMultiplier = m_dwReadTotalTimeoutMultiplier;
	comTime.ReadTotalTimeoutConstant = m_dwReadTotalTimeoutConstant;
	comTime.WriteTotalTimeoutMultiplier = m_dwWriteTotalTimeoutMultiplier;
	comTime.WriteTotalTimeoutConstant = m_dwWriteTotalTimeoutConstant;
	if( !SetCommTimeouts( m_hPort, &comTime)) {
		m_dwCommState = ERR_PORTERROR;
		CloseHandle( m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// create a mutex to control access to the port
	if( !(m_hPortMutex = CreateMutex( NULL, FALSE, NULL))) {
		m_dwCommState = ERR_NOTHREAD;
		CloseHandle( m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// initialize the terminate and cancel flags
	m_bTerminate = FALSE;
	m_bCancel = FALSE;

	// create an event to signal when requests are received
	if( !(m_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL))) {
		m_dwCommState = ERR_NOTHREAD;
		CloseHandle( m_hPort);
		CloseHandle( m_hPortMutex);
		m_hPort = INVALID_HANDLE_VALUE;
		m_hPortMutex = 0;
		return FALSE;
	}

	// create a thread to process port requests
	m_pThread = AfxBeginThread( ProcessCommRequests, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
	if( !m_pThread) {
		m_dwCommState = ERR_NOTHREAD;
		CloseHandle( m_hPort);
		CloseHandle( m_hPortMutex);
		m_hPort = INVALID_HANDLE_VALUE;
		m_hPortMutex = 0;
		return FALSE;
	}
	if(!::DuplicateHandle(::GetCurrentProcess(), m_pThread->m_hThread,
				::GetCurrentProcess(), &m_hThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		m_bTerminate = TRUE;
		m_pThread->ResumeThread();
		Sleep(40); // turn processor over to thread for termination
		m_dwCommState = ERR_NOTHREAD;
		CloseHandle( m_hPort);
		CloseHandle( m_hPortMutex);
		m_hPort = INVALID_HANDLE_VALUE;
		m_hPortMutex = 0;
		return FALSE;
	}
	m_pThread->ResumeThread();

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::Close
*
* Purpose: closes a connection to the device on the com port
******************************************************************************/
BOOL CCommDevice::Close()
{
	// flag termination of the device connection
	m_bTerminate = TRUE;

	// if the thread hasn't already terminated, terminate it now
	if( m_hThread != INVALID_HANDLE_VALUE)	{
		// wait for thread to terminate
		DWORD dwExitCode;	
		while( ::GetExitCodeThread( m_hThread, &dwExitCode) &&
					 (dwExitCode == STILL_ACTIVE)
				 ) 
		{
			Sleep(40); // turn processing time over to thread
		}
		m_pThread = NULL;

		CloseHandle( m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
	if( m_pRequest)
		delete m_pRequest;

	// if the port is still open, close the port
	if( m_hPort != INVALID_HANDLE_VALUE) {

		// clear the terminate and cancel flags so
		// that we can talk to the device
		m_bCancel = FALSE;
		m_bTerminate = FALSE;

		// reset the device
		Reset();

		// close the port
		if( !CloseHandle( m_hPort)) {
			m_dwCommState = ERR_OPENPORT;
			return FALSE;
		}
		m_hPort = INVALID_HANDLE_VALUE;
	}

	// close the request event semaphore
	if( m_hEvent) {
		CloseHandle( m_hEvent);
		m_hEvent = 0;
	}

	// close the port mutex semaphore
	if( m_hPortMutex) {
		CloseHandle( m_hPortMutex);
		m_hPortMutex = 0;
	}

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::Request (event driven)
*
* Purpose: send a request to the port
******************************************************************************/
BOOL CCommDevice::Request( CCommRequest* pRequest)
{
	DWORD dwWait;
	DWORD dwExitCode;

	// clear any previous errors
	m_dwCommState = ERR_NOERROR;

	// make sure thread is still around
	if( (m_hThread == INVALID_HANDLE_VALUE) ||
			(!::GetExitCodeThread( m_hThread, &dwExitCode)) ||
			(dwExitCode != STILL_ACTIVE))
	{
		m_dwCommState = ERR_NOTHREAD;
		return FALSE;
	}

	// make sure a thread is not already processing
	// a request.  By not using a timeout we can verify 
	// that the thread has not terminated prematurely
	dwWait = WaitForSingleObject( m_hEvent, 0);
	while( dwWait == WAIT_OBJECT_0) {
		Sleep(40); // turn processing time over to thread
		dwWait = WaitForSingleObject( m_hEvent, 0);
	}
	if( dwWait==WAIT_FAILED) {
		m_dwCommState = ERR_NOTHREAD;
		return FALSE;
	}

	// check for termination
	if( m_bTerminate) {
		m_dwCommState = ERR_TERMINATED;
		return FALSE;
	}

	m_pRequest = pRequest;

	SetEvent( m_hEvent);
	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::RequestWrite (event driven)
*
* Purpose: request data to be written to the port - send the result to a 
*          window when finished
******************************************************************************/
BOOL CCommDevice::RequestWrite( LPCTSTR lpszData, CWnd* pWnd)
{
	CCommRequest* pRequest = NewRequest();

	pRequest->m_dwRequestType = REQUEST_WRITE;

	if( lpszData && lpszData[0]) {
		int len = lstrlen( lpszData);
		pRequest->m_lpszData = new char[ len + 1];
		lstrcpy( pRequest->m_lpszData, lpszData);
		pRequest->m_dwDataLength = len;
	}

	pRequest->m_pWindow = pWnd;
	
	if( !Request( pRequest)) {
		delete pRequest;
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::RequestWrite (event driven)
*
* Purpose: request data to be written to the port - call a callback function
*          when finished
******************************************************************************/
BOOL CCommDevice::RequestWrite( LPCTSTR lpszData, LPREQUESTPROC pProc)
{
	CCommRequest* pRequest = NewRequest();

	pRequest->m_dwRequestType = REQUEST_WRITE;

	if( lpszData && lpszData[0]) {
		int len = lstrlen( lpszData);
		pRequest->m_lpszData = new char[ len + 1];
		lstrcpy( pRequest->m_lpszData, lpszData);
		pRequest->m_dwDataLength = len;
	}

	pRequest->m_pCompleteFunction = pProc;

	if( !Request( pRequest)) {
		delete pRequest;
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::RequestRead (event driven)
*
* Purpose: request data to be read from the port - send the result to a 
*					 window when finished
******************************************************************************/
BOOL CCommDevice::RequestRead( DWORD dwResultLen, CWnd* pWnd)
{
	CCommRequest* pRequest = NewRequest();

	pRequest->m_dwRequestType = REQUEST_READ;
	pRequest->m_lpszResult = new char[ dwResultLen];
	pRequest->m_dwResultLength = dwResultLen;
	pRequest->m_pWindow = pWnd;

	if( !Request( pRequest)) {
		delete pRequest;
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::RequestRead (event driven)
*
* Purpose: request data to be read from the port - call a callback function
*					 when finished
******************************************************************************/
BOOL CCommDevice::RequestRead( DWORD dwResultLen, LPREQUESTPROC pProc)
{
	CCommRequest* pRequest = NewRequest();

	pRequest->m_dwRequestType = REQUEST_READ;
	pRequest->m_lpszResult = new char[ dwResultLen];
	pRequest->m_pCompleteFunction = pProc;

	if( !Request( pRequest)) {
		delete pRequest;
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::RequestCommand (event driven)
*
* Purpose: execute a device command - send the result to a window when 
*          finished
******************************************************************************/
BOOL CCommDevice::RequestCommand( LPCTSTR lpcszData, DWORD dwResultLen, CWnd* pWnd)
{
	CCommRequest* pRequest = NewRequest();

	pRequest->m_dwRequestType = REQUEST_COMMAND;

	if( lpcszData && lpcszData[0]) {
		int len = lstrlen( lpcszData);
		pRequest->m_lpszData = new char[ len + 1];
		lstrcpy( pRequest->m_lpszData, lpcszData);
		pRequest->m_dwDataLength = len;
	}

	if( dwResultLen) {
		pRequest->m_lpszResult = new char[ dwResultLen];
		pRequest->m_dwResultLength = dwResultLen;
	}

	pRequest->m_pWindow = pWnd;

	if( !Request( pRequest)) {
		delete pRequest;
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::RequestCommand (event driven)
*
* Purpose: execute a device command - call a callback function when finished
******************************************************************************/
BOOL CCommDevice::RequestCommand( LPCTSTR lpcszData, DWORD dwResultLen, LPREQUESTPROC pProc)
{
	CCommRequest* pRequest = NewRequest();

	pRequest->m_dwRequestType = REQUEST_COMMAND;

	if( lpcszData && lpcszData[0]) {
		int len = lstrlen( lpcszData);
		pRequest->m_lpszData = new char[ len + 1];
		lstrcpy( pRequest->m_lpszData, lpcszData);
		pRequest->m_dwDataLength = len;
	}

	if( dwResultLen) {
		pRequest->m_lpszResult = new char[ dwResultLen];
		pRequest->m_dwResultLength = dwResultLen;
	}

	pRequest->m_pCompleteFunction = pProc;

	if( !Request( pRequest)) {
		delete pRequest;
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::CancelOperation
*
* Purpose: cancel an operation
******************************************************************************/
BOOL CCommDevice::CancelOperation()
{
	m_bCancel = TRUE;
	if( m_hThread != INVALID_HANDLE_VALUE)
		Sleep(40); // turn processing time over to thread
	return TRUE;
}

/******************************************************************************
* Name: CCommDevice::NewRequest
*
* Purpose: create a new request object
******************************************************************************/
CCommRequest* CCommDevice::NewRequest()
{
	return new CCommRequest();
}

/******************************************************************************
* Name: CCommDevice::Write (poll driven)
*
* Purpose: write data directly to the port
******************************************************************************/
DWORD CCommDevice::Write( LPCTSTR data, DWORD dataLength)
{
	DWORD dwBytesWritten = 0;
	
	// clear any previous error
	m_dwCommState = ERR_NOERROR;

	// make sure the port is not in use
	DWORD dwWait = WaitForSingleObject( m_hPortMutex, INFINITE);
	if( dwWait==WAIT_FAILED) {
		m_dwCommState = ERR_NOPORTLOCK;
		ReleaseMutex( m_hPortMutex);
		return 0;
	}

	//P	added to purge the port before read & write
	PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	// check for termination
	if( m_bCancel) {
		m_bCancel = FALSE;
		m_dwCommState = ERR_CANCELLED;
		PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		ReleaseMutex( m_hPortMutex);
		return 0;
	}
	if( m_bTerminate) {
		m_dwCommState = ERR_TERMINATED;
		PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		ReleaseMutex( m_hPortMutex);
		return 0;
	}

	// clear any previous errors
	DWORD errors;
	if( !ClearCommError( m_hPort, &errors, NULL)) {
		m_dwCommState = ERR_PORTERROR;
		ReleaseMutex( m_hPortMutex);
		return 0;
	}
	ASSERT( errors==NULL);

	// write the data to the port
	if( !WriteFile( m_hPort, data, dataLength, &dwBytesWritten, NULL)) {
		m_dwCommState = ERR_PORTERROR;
		ReleaseMutex( m_hPortMutex);
		return dwBytesWritten;
	}

	TRACE( "Sent %ld bytes: %s\n", dwBytesWritten, data);

	ReleaseMutex( m_hPortMutex);
	return dwBytesWritten;
}

/******************************************************************************
* Name: CCommDevice::Read (poll driven)
*
* Purpose: read data directly from the port
******************************************************************************/
DWORD CCommDevice::Read( LPTSTR data, DWORD dataLength, DWORD dwMaxTries, DWORD dwTimeout)
{
	DWORD dwBytesRead;
	DWORD dwTries = 1;
	LPTSTR pByte = data;

	// clear any previous error
	m_dwCommState = ERR_NOERROR;

	// initialize the data buffer
	memset( data, 0, dataLength);

	// make sure the port is not in use
	DWORD dwWait = WaitForSingleObject( m_hPortMutex, INFINITE);
	if( dwWait!=WAIT_OBJECT_0) {
		m_dwCommState = ERR_NOPORTLOCK;
		ReleaseMutex( m_hPortMutex);
		return 0;
	}

	//P	added to purge the port before read & write
	PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	// check for termination
	if( m_bCancel) {
		m_bCancel = FALSE;
		m_dwCommState = ERR_CANCELLED;
		PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		ReleaseMutex( m_hPortMutex);
		return 0;
	}
	if( m_bTerminate) {
		m_dwCommState = ERR_TERMINATED;
		PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		ReleaseMutex( m_hPortMutex);
		return 0;
	}

	// clear any previous errors
	DWORD errors;
	if( !ClearCommError( m_hPort, &errors, NULL)) {
		m_dwCommState = ERR_PORTERROR;
		ReleaseMutex( m_hPortMutex);
		return 0;
	}
	ASSERT( errors == NULL);

RetryRead:

	// get the data
	do
	{
		if( !ReadFile( m_hPort, pByte, 1, &dwBytesRead, NULL)) {
			m_dwCommState = ERR_PORTERROR;
			ReleaseMutex( m_hPortMutex);
			return (DWORD)(pByte - data);
		}

		// if we timed out and haven't read anything check terminate 
		// and cancel operation flags otherwise just stay in the loop
		// until we receive something
		if( !dwBytesRead && !(pByte - data)) {
			if( m_bCancel) {
				m_bCancel = FALSE;
				m_dwCommState = ERR_CANCELLED;
				PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
				ReleaseMutex( m_hPortMutex);
				return 0;
			}
			if( m_bTerminate) {
				m_dwCommState = ERR_TERMINATED;
				PurgeComm( m_hPort, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
				ReleaseMutex( m_hPortMutex);
				return 0;
			}

			if( !dwMaxTries || dwTries < dwMaxTries) {
				dwTries++;
				dwBytesRead = 1; // don't let loop terminate
				Sleep( dwTimeout);
			}
			else m_dwCommState = ERR_MAXTRIES;
		}
		else if (dwBytesRead) { // We got a character
			if (m_bParseCRLF) {

				if (*pByte==m_EOLChar) // Use EOL's to terminate event
					break;
				else if (*pByte!= m_LFChar)
					pByte++;
			}
			else
				pByte++;
		}

	} while( (dwBytesRead == 1) &&
					 (dataLength ? ((DWORD)(pByte - data) < dataLength) : TRUE));

	if (m_bParseCRLF) {
		*pByte = '\0';
		for(;pByte>data;pByte--) {
			if (*(pByte-1)=='\r' || *(pByte-1)=='\n')
				*(pByte-1) = '\0';
			else
				break;
		}
	}

	DWORD len = (pByte - data);

	if (m_bParseCRLF && len==0 && (!dwMaxTries || dwTries<dwMaxTries))
		goto RetryRead;

	TRACE( "Received %ld bytes: %s\n", len, data);

	ReleaseMutex( m_hPortMutex);
	return len;
}

/******************************************************************************
* Name: CCommDevice::Command (poll driven)
*
* Purpose: execute a device command
******************************************************************************/
DWORD CCommDevice::Command( LPCTSTR lpcszData, DWORD dwDataLength, LPTSTR lpszResult, 
													  DWORD dwResultLen, DWORD dwMaxTries, DWORD dwTimeout)
{
	if( !Write( lpcszData, dwDataLength))
		return 0;

	return Read( lpszResult, dwResultLen, dwMaxTries, dwTimeout);
}

DWORD CCommDevice::GetLastError()
{
	return m_dwCommState;
}


/******************************************************************************
* THREAD
******************************************************************************/

/******************************************************************************
* Macros
******************************************************************************/
#define COMMEVENT				(((CCommDevice*)lParam)->m_hEvent)
#define PORTMUTEX				(((CCommDevice*)lParam)->m_hPortMutex)
#define TERMINATE_FLAG	(((CCommDevice*)lParam)->m_bTerminate)
#define CANCEL_FLAG			(((CCommDevice*)lParam)->m_bCancel)
#define HPORT						(((CCommDevice*)lParam)->m_hPort)
#define TIMEOUT					(((CCommDevice*)lParam)->m_dwRequestTimeout)
#define COMMSTATE				(((CCommDevice*)lParam)->m_dwCommState)
#ifdef _TEST_PLATFORM
#define CONSOLEDOC			(((CCommDevice*)lParam)->m_pConsoleDoc)
#endif
#define READFILE( result, resultlength, timeout, maxtries) \
												(((CCommDevice*)lParam)->Read( result, resultlength, maxtries, timeout))
#define WRITEFILE( data, datalength) \
												(((CCommDevice*)lParam)->Write( data, datalength))
#define COMMAND( data, datalength, result, resultlength, maxtries, timeout) \
												(((CCommDevice*)lParam)->Command( data, datalength, result, resultlength, maxtries, timeout))
#define HANDLESPECIALREQUEST( request) \
												(((CCommDevice*)lParam)->HandleSpecialRequest( request))

/******************************************************************************
* Name: ProcessCommRequests
*
* Purpose: worker thread function that communicates with the com port
******************************************************************************/
UINT AFX_CDECL ProcessCommRequests( LPVOID lParam)
{
	if( !lParam) return 2;
	
	DWORD dwWait;

	// loop ends by returning on error or when the terminate flag is set
	while( TRUE) {
		dwWait = WaitForSingleObject( COMMEVENT, TIMEOUT);
		while( dwWait!=WAIT_OBJECT_0)	{	
			// check for error
			if( dwWait==WAIT_ABANDONED_0 || dwWait == WAIT_FAILED) {
				ResetEvent( COMMEVENT);
				PurgeComm( HPORT, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
				return 1;
			}

			// check for termination
			if( TERMINATE_FLAG) {
				ResetEvent( COMMEVENT);
				PurgeComm( HPORT, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
				return 0;
			}

			dwWait = WaitForSingleObject( COMMEVENT, TIMEOUT);
		}

		CCommRequest* pRequest = (((CCommDevice*)lParam)->m_pRequest);

		switch( pRequest->m_dwRequestType) {
			case REQUEST_READ:
			{
				// clear request error field
				pRequest->m_dwRequestError = ERR_NOERROR;

				DWORD dwBytesRead = READFILE( pRequest->m_lpszResult, 
																			pRequest->m_dwResultLength, 
																			pRequest->m_dwMaxTries,
																			pRequest->m_dwTimeout); 
				pRequest->m_dwResultLength = dwBytesRead;
				pRequest->m_dwRequestError = COMMSTATE;
				COMMSTATE = ERR_NOERROR;

				((CCommDevice*)lParam)->m_pRequest = NULL;
				ResetEvent( COMMEVENT);

				if( !dwBytesRead) {
					switch( pRequest->m_dwRequestError) {
						case ERR_PORTERROR:
						case ERR_NOPORTLOCK:
						case ERR_NODATALOCK:
						{
							if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
								pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
							else if( pRequest->m_pCompleteFunction)
								(*pRequest->m_pCompleteFunction)( pRequest);
							return 1;
						}
						case ERR_TERMINATED:
						{
							if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
								pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
							else if( pRequest->m_pCompleteFunction)
								(*pRequest->m_pCompleteFunction)( pRequest);
							return 0;
						}
					}
				}

				if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
					pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
				else if( pRequest->m_pCompleteFunction)
					(*pRequest->m_pCompleteFunction)( pRequest);
				break;
			}
			case REQUEST_WRITE:
			{
				// clear request error field
				pRequest->m_dwRequestError = ERR_NOERROR;

				DWORD dwBytesWritten = WRITEFILE( pRequest->m_lpszData, 
																					pRequest->m_dwDataLength);
				pRequest->m_dwDataLength = dwBytesWritten;
				pRequest->m_dwRequestError = COMMSTATE;
				COMMSTATE = ERR_NOERROR;

				((CCommDevice*)lParam)->m_pRequest = NULL;
				ResetEvent( COMMEVENT);

				if( !dwBytesWritten) {
					switch( pRequest->m_dwRequestError) {
						case ERR_PORTERROR:
						case ERR_NOPORTLOCK:
						{
							if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
								pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
							else if( pRequest->m_pCompleteFunction)
								(*pRequest->m_pCompleteFunction)( pRequest);
							return 1;
						}
						case ERR_TERMINATED:
						{
							if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
								pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
							else if( pRequest->m_pCompleteFunction)
								(*pRequest->m_pCompleteFunction)( pRequest);
							return 0;
						}
					}
				}

				if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
					pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
				else if( pRequest->m_pCompleteFunction)
					(*pRequest->m_pCompleteFunction)( pRequest);
				break;
			}
			case REQUEST_COMMAND:
			{
				// clear request error field
				pRequest->m_dwRequestError = ERR_NOERROR;

				DWORD dwBytesRead = COMMAND( pRequest->m_lpszData,
																		 pRequest->m_dwDataLength,
																		 pRequest->m_lpszResult,
																		 pRequest->m_dwResultLength,
																		 pRequest->m_dwMaxTries,
																		 pRequest->m_dwTimeout);
				pRequest->m_dwResultLength = dwBytesRead;
				pRequest->m_dwRequestError = COMMSTATE;
				COMMSTATE = ERR_NOERROR;

				((CCommDevice*)lParam)->m_pRequest = NULL;
				ResetEvent( COMMEVENT);

				if( !dwBytesRead) {
					switch( pRequest->m_dwRequestError) {
						case ERR_PORTERROR:
						case ERR_NOPORTLOCK:
						case ERR_NODATALOCK:
						{
							if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
								pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
							else if( pRequest->m_pCompleteFunction)
								(*pRequest->m_pCompleteFunction)( pRequest);
							return 1;
						}
						case ERR_TERMINATED:
						{
							if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
								pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
							else if( pRequest->m_pCompleteFunction)
								(*pRequest->m_pCompleteFunction)( pRequest);
							return 0;
						}
					}
				}

				if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
					pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
				else if( pRequest->m_pCompleteFunction)
					(*pRequest->m_pCompleteFunction)( pRequest);
				break;
			}
			case REQUEST_SPECIAL:
			{
				// clear request error field
				pRequest->m_dwRequestError = ERR_NOERROR;

				HANDLESPECIALREQUEST( pRequest);
				COMMSTATE = ERR_NOERROR;
				
				((CCommDevice*)lParam)->m_pRequest = NULL;
				ResetEvent( COMMEVENT);

				switch( pRequest->m_dwRequestError) {
					case ERR_PORTERROR:
					case ERR_NOPORTLOCK:
					case ERR_NODATALOCK:
					{
						if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
							pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
						else if( pRequest->m_pCompleteFunction)
							(*pRequest->m_pCompleteFunction)( pRequest);
						return 1;
					}
					case ERR_TERMINATED:
					{
						if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
							pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
						else if( pRequest->m_pCompleteFunction)
							(*pRequest->m_pCompleteFunction)( pRequest);
						return 0;
					}
				}

				if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
					pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
				else if( pRequest->m_pCompleteFunction)
					(*pRequest->m_pCompleteFunction)( pRequest);
				break;
			}
			case REQUEST_CALLBACK:
			default:
			{
				// clear any previous request errors
				pRequest->m_dwRequestError = ERR_NOERROR;

				((CCommDevice*)lParam)->m_pRequest = NULL;
				ResetEvent( COMMEVENT);

				if( pRequest->m_pWindow && ::IsWindow( pRequest->m_pWindow->m_hWnd))
					pRequest->m_pWindow->PostMessage( WM_COMM_REQUEST_COMPLETE, 0, (long)pRequest);
				else if( pRequest->m_pCompleteFunction)
					(*pRequest->m_pCompleteFunction)( pRequest);
				break;
			}
		}
	}
}

/******************************************************************************
* Name: CCommDevice::GetComPorts
*
* Purpose: Find the valid COM ports and store the numbers in buf.
*
*	Returns: The number of COM ports found.  This will always be between 0 and 10   
******************************************************************************/
int CCommDevice::GetComPorts(unsigned char *buf)
{

	int rv = 0;
	CString port;
	HANDLE hComm;
	for (unsigned char i=0; i<10; i++)	{
		port.Format("COM%d", (int)i);
		hComm = CreateFile((LPCTSTR)port, GENERIC_READ | GENERIC_WRITE, 0, 
												NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
		if (hComm != INVALID_HANDLE_VALUE)	{
			CloseHandle(hComm);
			buf[rv++] = i;
		}
		else	{
			if (::GetLastError() != ERROR_FILE_NOT_FOUND)	
				buf[rv++] = i;
		}
	}	
	return rv;

}
