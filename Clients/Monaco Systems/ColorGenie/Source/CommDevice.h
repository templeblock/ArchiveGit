#ifndef _COMMDEVICE_H_
#define _COMMDEVICE_H_

#include "ErrorMessage.h"

#define WM_COMM_REQUEST_COMPLETE (WM_USER + 101)

/******************************************************************************
* Macros
******************************************************************************/
// error messages returned by CCommDevice and derived classes
// the upper word of the error message is a bitmap used to identify the
// device with 0x0000 being reserved for this class.  The lower
// word may be any value identifying the unique error

/*
#define ERR_NOERROR					0x00000000
#define ERR_NOTHREAD				0x00000001
#define ERR_NOPORT					0x00000002
#define ERR_OPENPORT				0x00000003
#define ERR_NODEVICE				0x00000004
#define ERR_CANCELLED				0x00000005
#define ERR_TERMINATED			0x00000006
*/

#define ERR_NODATALOCK			0x00000007
#define ERR_NOPORTLOCK			0x00000008
#define ERR_PORTERROR				0x00000009
//#define ERR_MAXTRIES				0x0000000A
#define ERR_UNKNOWNSPECIAL	0x0000000B
#define ERR_UNSUPPORTEDFUNC	0x0000000C

// error messages with 0x1000???? are used for the CProfileDevice class
// error messages with 0x2000???? are used for the CXRiteDTP5x class
// error messages with 0x3000???? are used for the CXRiteDTP22 class
// error messages with 0x4000???? are used for the CXRiteRCIDevice class
// error messages with 0x5000???? are used for the CXRite968 class

// request values
#define	REQUEST_READ				0x00000001
#define REQUEST_WRITE				0x00000002
#define REQUEST_COMMAND			0x00000003
#define REQUEST_CALLBACK		0x00000004
#define REQUEST_SPECIAL			0x00000005

// default timeout settings
#define COM_READINTERVALTIMEOUT					0
#define COM_READTOTALTIMEOUTMULTIPLIER	100
#define COM_READTOTALTIMEOUTCONSTANT		500
#define COM_WRITETOTALTIMEOUTMULTIPLIER	100
#define COM_WRITETOTALTIMEOUTCONSTANT		500
#define COM_OPERATIONTIMEOUT						500

//device type
#define GRETAG_SCAN		0
#define TECKON_TABLE	1
#define XRITE_408		2

/******************************************************************************
* Forwards
******************************************************************************/
class CCommRequest;

/******************************************************************************
* Class
******************************************************************************/

/******************************************************************************
* Class: CCommRequest
* 
* Purpose: Request object for use with a CCommDevice
******************************************************************************/
// The CCommRequest class contains information about the request.  On a successful
// REQUEST_READ the information read from the COM port is stored in pszData.  For a
// REQUEST_WRITE the information stored in pszData is written to the port.
typedef LRESULT (CDECL *LPREQUESTPROC) (CCommRequest* r);
class CCommRequest : public CObject
{
	DECLARE_DYNCREATE( CCommRequest)

		// data members
	public:
		DWORD					m_dwRequestType;			// request type
		DWORD					m_dwRequestSpecial;		// special request flags
		DWORD					m_dwRequestError;			// request error code
		LPTSTR				m_lpszData;						// data (command) buffer
		DWORD					m_dwDataLength;				// length of the data (command) buffer
		LPTSTR				m_lpszResult;					// result buffer
		DWORD					m_dwResultLength;			// length of the result buffer
		CWnd*					m_pWindow;						// pointer to window to receive message upon request completion
		LPREQUESTPROC	m_pCompleteFunction;	// pointer to a function to call upon request completion
		DWORD					m_dwTimeout;					// timeout between polls to port
		DWORD					m_dwMaxTries;					// maximum number of attempts to read from port

		// construction
	public:
		CCommRequest();
		~CCommRequest();
};

/******************************************************************************
* Class
******************************************************************************/

/******************************************************************************
* Class: CCommDevice
* 
* Purpose: Interface for the com port
******************************************************************************/
class CCommDevice : public CObject
{
	DECLARE_DYNCREATE( CCommDevice)

// Construction
public:
	CCommDevice();

// Atributes
public:
	DWORD m_dwReadIntervalTimeout;
	DWORD m_dwReadTotalTimeoutMultiplier;
	DWORD m_dwReadTotalTimeoutConstant;
	DWORD m_dwWriteTotalTimeoutMultiplier;
	DWORD m_dwWriteTotalTimeoutConstant;

protected:
	HANDLE m_hPortMutex;
	HANDLE m_hPort;
	HANDLE m_hThread;
	HANDLE m_hEvent;
	CWinThread* m_pThread;
	CCommRequest* m_pRequest;
	BOOL m_bTerminate;
	BOOL m_bCancel;
	DWORD m_dwCommState;
	DWORD m_dwRequestTimeout;
	BOOL m_bParseCRLF;
	char m_EOLChar;
	char m_LFChar;

// Operations
public:
	virtual BOOL Open( INT port, DWORD baud=9600, CHAR parity='N', INT data=8, INT stop=1);
	virtual BOOL Close();
	virtual BOOL Reset() { return TRUE;}
	virtual BOOL Request( CCommRequest* pRequest);
	virtual BOOL RequestWrite( LPCTSTR lpcszData, CWnd* pWnd);
	virtual BOOL RequestWrite( LPCTSTR lpcszData, LPREQUESTPROC pProc);
	virtual BOOL RequestRead( DWORD dwResultLen, CWnd* pWnd);
	virtual BOOL RequestRead( DWORD dwResultLen, LPREQUESTPROC pProc);
	virtual BOOL RequestCommand( LPCTSTR lpcszData, DWORD dwResultLen, CWnd* pWnd);
	virtual BOOL RequestCommand( LPCTSTR lpcszData, DWORD dwResultLen, LPREQUESTPROC pProc);
	virtual BOOL CancelOperation();
	virtual CCommRequest* NewRequest();
	virtual DWORD Write( LPCTSTR lpcszData, DWORD dwDataLength);
	virtual DWORD Read( LPTSTR lpszResult, DWORD dwResultLength, DWORD dwMaxTries=1, DWORD dwTimeout=COM_OPERATIONTIMEOUT);
	virtual DWORD Command( LPCTSTR lpcszData, DWORD dwDataLength, LPTSTR lpszResult, DWORD dwResultLen, DWORD dwMaxTries=1, DWORD dwTimeout=COM_OPERATIONTIMEOUT);
	virtual void HandleSpecialRequest( CCommRequest* pRequest) { pRequest->m_dwRequestError = ERR_UNKNOWNSPECIAL; }
	virtual DWORD GetLastError();
	virtual operator HANDLE() { return m_hPort;}
	virtual BOOL IsOpen() { return (m_hPort!=INVALID_HANDLE_VALUE);}
	virtual BOOL PurgeComm(HANDLE hFile, DWORD dwFlags) { return ::PurgeComm(hFile, dwFlags);}

//	Static Operations
	static int GetComPorts(unsigned char *buf);

// Implementation
public:
	virtual ~CCommDevice();

protected:
	virtual HANDLE OpenPort( int port);
	virtual BOOL InitPort( HANDLE hPort, int baud, char parity, int data, int stop);

	// This is the worker thread function
	friend UINT AFX_CDECL ProcessCommRequests( LPVOID lParam);
};

#endif