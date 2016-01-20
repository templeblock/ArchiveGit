// PSGFTP.cpp : Defines the initialization routines for the DLL.
//

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <stdlib.h>
#include <time.h>

#include "winftp.h"				// Local FTP definitions
#include "cardftp.h"				// Real-world interface

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// VOID CALLBACK mytimerproc(wind,msg,idevent,systime);
// lgk new globals so we can close open files on an abort or close
int uiFtpPort = 21;

char szMsgBuf[MAXPACKET];              // main i/o buffer
int sendstr(SOCKET sSocket, LPCSTR pBuf, int nBytesToWrite);
// lgk need a new szmsgbuf since one is already being used during downloads and conflicts
// with local commands
u_char szSendPkt[MAXPACKET];           // output transfer buffer


BOOL receivingfile = FALSE;
int receivefilehandle = -1;
int bConnected=0;          // connected flag
int bSendPort=1;           // use PORT commands (must be 1!!!)
#define  UC(b)  (((int)b)&0xff)

int iCode;                 // return code from last command(..)

struct sockaddr_in saDestAddr;     
struct sockaddr_in saSockAddr;         // endpoint address
struct sockaddr_in saSockAddr1;        // used when bSendPort==0
struct sockaddr_in saCtrlAddr;

int DoConnect2(LPCSTR ftp_host, SOCKET* pSocket);

int Sendfiles(LPCSTR pszCardDir, LPCSTR pszURL);

// Notify variables.
CARDFTP_NOTIFY m_pNotify;
LPVOID m_pNotifyData;

void DoNotify(int nNotify)
{
	if (m_pNotify != NULL)
	{
		m_pNotify(nNotify, m_pNotifyData);
	}
}

#ifdef WIN32
extern "C" BOOL WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
	// Like LibMain.
//		m_hInstance = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	// Like WEP.
	}

	return TRUE;    // ok
}
#else
extern "C"
int CALLBACK LibMain(HINSTANCE hInstance, WORD, WORD, LPSTR)
{
//	m_hInstance = hInstance;
	return 1;   // ok
}
#endif

//
// Entry point.
//

#ifdef WIN32
extern "C" __declspec(dllexport) int WINAPI send_files (LPCSTR thedir, LPCSTR pszURL, CARDFTP_NOTIFY pNotify, LPVOID pNotifyData)
#else
extern "C" int pascal _export send_files (LPCSTR thedir, LPCSTR pszURL, CARDFTP_NOTIFY pNotify, LPVOID pNotifyData)
#endif
{
	// Set the global notify.
	m_pNotify = pNotify;
	m_pNotifyData = pNotifyData;

	// Send the files.
   return Sendfiles(thedir, pszURL);
}

//
// Main function.
//

int Sendfiles(LPCSTR pszCardDir, LPCSTR pszURL)
{
	static struct
	{
		LPCSTR pszName;
		char nMode;
		int nNotify;
	} Files[] =
	{
		{ "front.gif", TYPE_I, CARDFTP_SendingFront },
		{ "inside.gif", TYPE_I, CARDFTP_SendingInside },
		{ "info.ini", TYPE_A, CARDFTP_SendingTextFile }
	};

	int err;
	WSADATA WSAData;
	char cbSourceFile[_MAX_PATH];
	char cbDestFile[_MAX_PATH];
	int iRetCode = 0;

	long lTime = GetTickCount();

	DoNotify(CARDFTP_Initializing);
	// Start up Windows sockets.
   err = WSAStartup((WORD)0x0101, &WSAData);  // register task with
	if (err == 0)
	{
		// Connect to our host.
		DoNotify(CARDFTP_Connecting);
		SOCKET CtrlSocket;
		err = DoConnect(pszURL, &CtrlSocket);
		if (err == 0)
		{
			// Send all the files.
			for (int i = 0; i < sizeof(Files)/sizeof(Files[0]); i++)
			{
				// Notify what we are going to do.
				DoNotify(Files[i].nNotify);

				// Build the source file name.
				sprintf(cbSourceFile,"%s\\%s", pszCardDir, Files[i].pszName);
				// Build the destination file name.
				sprintf(cbDestFile,"%ld%s", lTime, Files[i].pszName);
				if ((err = SendFile(CtrlSocket, cbSourceFile, cbDestFile, Files[i].nMode)) != 0)
				{
					break;
				}
			}

			// Say goodbye to the server.
			if (err == 0)
			{
				// Notify that we are shutting down.
				DoNotify(CARDFTP_Closing);
				command(CtrlSocket, "QUIT");
			}
			else
			{
				// Notify that we are aborting.
				DoNotify(CARDFTP_Aborting);
			}

			// Close our control socket.
			CtrlSocket = DoClose(CtrlSocket);
		}

		// Close up shop.
		WSACleanup();
	}
	return err;
}

int SendFile(SOCKET CtrlSocket, LPCSTR pszSrcFile, LPCSTR pszDstFile, char TransferType)
{
	char tmp[_MAX_PATH+10];		// Path + command length (plus a little slop).

	SOCKET ListenSocket;
	int iErr = GetFTPListenSocket(CtrlSocket, &ListenSocket);
   if (iErr != 0)
      return iErr;

   // Set the transfer type (e.g. Binary, Ascii).
	SetTransferType(CtrlSocket, TransferType);

	// Send the actual STOR command.
   sprintf(tmp, "STOR %s", pszDstFile);
	int iRetCode;
	if ((iErr = command(CtrlSocket, tmp, &iRetCode)) != 0)
	{
//		OutputDebugString("STOR failed\r\n");
		return iErr;
	}

	// If we got a good reponse back, proceed.
	if (iRetCode == FTP_PRELIM) 
   {
		int iLength = sizeof(saSockAddr1);
		SOCKET DataSocket = accept(ListenSocket,
											(struct sockaddr far *)&saSockAddr1,
											(int far *)&iLength);
		// See if got a data socket.
		if (DataSocket == INVALID_SOCKET)
		{
//			OutputDebugString("accept failed\r\n");
			iErr = WSAGetLastError();
		}

		// Close the listening socket.
      ListenSocket = DoClose(ListenSocket);

		// If we got a data socket, use it!
		if (DataSocket != INVALID_SOCKET)
		{
			iErr = SendMass(DataSocket, pszSrcFile, TransferType == TYPE_I);

			LINGER ntlinger;
			ntlinger.l_onoff  = (u_short)TRUE;
			ntlinger.l_linger = (u_short)5;			// Linger for 5 seconds if necessary.
			setsockopt(DataSocket,
						  SOL_SOCKET,
						  SO_LINGER,
						  (LPSTR)&ntlinger,
						  sizeof(ntlinger));

			DataSocket = DoClose(DataSocket);

			if (iErr == 0)
			{
				// Read the reply.
				iErr = ReadDisplayLine(CtrlSocket);
				if (iErr != 0)
				{
//					OutputDebugString("ReadDisplayLine after SendMass failed\r\n");
				}
			}
			else
			{
//				OutputDebugString("SendMass failed\r\n");
			}
		}
   }
	else
	{
		// Close the listen socket.
		ListenSocket = DoClose(ListenSocket);
		iErr = WSAEPROTOTYPE;		// Signal protocol error.
//		OutputDebugString("Protocol error\r\n");
	}
   return iErr;
}

// initial connection
int DoConnect(LPCSTR ftp_host, SOCKET* pSocket)
{
	int iErr = DoConnect2(ftp_host, pSocket);
	if (iErr == 0)
	{
		bConnected = TRUE;
	}
	return iErr;
} 

// initial connection
int DoConnect2(LPCSTR ftp_host, SOCKET* pSocket)
{
	*pSocket = NULL;			// Until we succeed...
	SOCKET ctrl_skt;

	// create a connected socket
   int iErr = connectTCP(ftp_host,"ftp", &ctrl_skt);
   if (iErr != 0)
   {
      return iErr;
   }

	// get information about local end of the connection
	int iLength = sizeof (saCtrlAddr);
	int iFlag=1;
	if (getsockname(ctrl_skt,(struct sockaddr *)&saCtrlAddr, &iLength) == SOCKET_ERROR
		 || setsockopt(ctrl_skt, SOL_SOCKET, SO_OOBINLINE, (LPSTR)&iFlag, sizeof(iFlag)) == SOCKET_ERROR)
	{
		iErr = WSAGetLastError();
		closesocket(ctrl_skt);
		return iErr;
	}

	// Read the initial response line from the FTP server.
	// Send the user name.
	// Send the password.
	if ((iErr = ReadDisplayLine(ctrl_skt)) != 0
		 || (iErr = command(ctrl_skt,"USER pmcard")) != 0
		 || (iErr = command(ctrl_skt,"PASS printM22")) != 0)
	{
		closesocket(ctrl_skt);
		return iErr;
	}

	// Return the socket with no error.
	*pSocket = ctrl_skt;
   return iErr;
}

/*
// send a message on the control socket, read and display the resulting
// message and return the result value
*/

int command(SOCKET ctrl_skt, LPCSTR cmdstring, int* pRetCode /*=NULL*/)
{
	int iErr = 0;

   iCode=0;
	if ((iErr = SendPacket(ctrl_skt, cmdstring)) == 0)
	{
		iErr = ReadDisplayLine(ctrl_skt, pRetCode);
	}

   return iErr;
}

#if 0
int SendFile(SOCKET ctrl_skt,LPSTR szCMD,LPSTR localfile,char stype)
{
  int iRetCode = 0;
  int iFlag = 1;

  iCode=0;
  // if we don't have a valid control socket, can't do anything
  if(ctrl_skt==INVALID_SOCKET)
    return(0);
  iRetCode=SendMass(data_socket,localfile,stype==TYPE_I);
   return(iRetCode);
}
#endif

// based on WINTEL (ftp.c) and BSD (ftp.c)
int GetFTPListenSocket(SOCKET ctrl_skt, SOCKET* pListenSocket)
{
	*pListenSocket = INVALID_SOCKET;		// Until we succeed...

	SOCKET listen_skt;
	// create a data socket
	if((listen_skt=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}

	if(bSendPort) 
	{
		saCtrlAddr.sin_port=htons(0);
		saCtrlAddr.sin_family=AF_INET;
		saCtrlAddr.sin_addr.s_addr=0;
	}
	else
	{
		int iFlag=1;
		if (setsockopt(listen_skt,SOL_SOCKET,SO_REUSEADDR,
							 (char *)&iFlag,sizeof(iFlag))==SOCKET_ERROR)
		{
			int iErr = WSAGetLastError();
			closesocket(listen_skt);
			return iErr;
		}
	}

	// Bind name to socket
	// get the port name that we got for later transmission in PORT cmd
	// invoke listener
	int iLength=sizeof(saCtrlAddr);
	if (bind((SOCKET)listen_skt,(LPSOCKADDR)&saCtrlAddr,
            (int)sizeof(struct sockaddr)) == SOCKET_ERROR
			|| getsockname(listen_skt,(struct sockaddr *)&saCtrlAddr,&iLength)<0
			|| listen(listen_skt,1) != 0)
	{
		int iErr = WSAGetLastError();
		closesocket(listen_skt);
		return iErr;
	}

	// inform remote end about our port that we created.
	if(bSendPort)
	{
		struct sockaddr_in saTmpAddr;

		int iLength = sizeof (saTmpAddr);
		if (getsockname(ctrl_skt,(LPSOCKADDR)&saTmpAddr, &iLength) == SOCKET_ERROR)
		{
			int iErr = WSAGetLastError();
			closesocket(listen_skt);
			return iErr;
		}

		char tmp[300];
		char* a = (char *)&saTmpAddr.sin_addr;
		char* p = (char *)&saCtrlAddr.sin_port;
		sprintf(tmp,"PORT %d,%d,%d,%d,%d,%d",
				  UC(a[0]), UC(a[1]),
				  UC(a[2]), UC(a[3]),
				  UC(p[0]), UC(p[1]));
//		OutputDebugString(tmp);

		int iRetCode;
		command(ctrl_skt, tmp, &iRetCode);
		if (iRetCode != FTP_COMPLETE)
		{
			// Remote end did not understand our port command.
		}
	}
	*pListenSocket = listen_skt;
	return 0;
}

// send a file through the data socket
int SendMass(SOCKET sockfd, LPCSTR filename, BOOL binaryflag)
{
	// if we don't have a socket, return an error  
	if (sockfd == INVALID_SOCKET)
		return WSAENOTSOCK;
	if (!bConnected)				// Global variables?
		return WSAENOTCONN;

	// initialize some vars
	int iErr = 0;

	// this forces binary mode at the moment
	int iFileHandle;
	if ((iFileHandle = _lopen(filename, OF_READ)) == -1)
	{
//		OutputDebugString("File not found\r\n");
		return WSAENOENT;
	}
	else
	{
		int iNumBytes;
		// loop to send output to remote end
		while ((iNumBytes = _lread(iFileHandle,szMsgBuf,512)) > 0)
		{
			if (sendstr(sockfd, szMsgBuf, iNumBytes) <= 0)
			{
				iErr = WSAGetLastError();
//				OutputDebugString("sendstr failed\r\n");
				break;
			}
		}
		_lclose(iFileHandle);
	}
	return iErr;
}

//
// Send a packet. The error value is returned (0 on success).
//

int SendPacket(SOCKET sockfd, LPCSTR msg)
{
	if (sockfd == INVALID_SOCKET)
		return WSAENOTSOCK;

	int i = strlen(msg);
	strcpy((char *)szSendPkt,msg);
	// append a CRLF to the end of outgoing messages
	szSendPkt[i++]='\r';
	szSendPkt[i++]='\n';
	szSendPkt[i]=0;  
	i = sendstr(sockfd, (char *)szSendPkt, i);

	return (i <= 0) ? WSAGetLastError() : 0;
}
// user close routine
SOCKET DoClose(SOCKET sockfd)
{
	if (sockfd!=INVALID_SOCKET) 
	{
		if(WSAIsBlocking())
			WSACancelBlockingCall();
		shutdown(sockfd,2);
		closesocket(sockfd);
	}

  return INVALID_SOCKET;
}

int sendstr(SOCKET sSocket, LPCSTR pBuf,int nBytesToWrite)
{
	int nBytesLeft,nWritten;

	nBytesLeft = nBytesToWrite;
	while(nBytesLeft > 0) 
	{
		nWritten = send(sSocket, pBuf, ((nBytesLeft>1024)?1024:nBytesLeft),0); // write what we can
		if(nWritten <= 0)
			return(nWritten);  // error occured
		nBytesLeft -= nWritten;                 // count what we wrote
		pBuf   += nWritten;                     // adjust buffer pointer
	}
	return (nBytesToWrite - nBytesLeft);       // return count of bytes written
}

int connectTCP(LPCSTR host, LPCSTR service, SOCKET* pSocket)
{
  return connectsock(host,service,"tcp", pSocket);
}

int connectUDP(LPCSTR host, LPCSTR service, SOCKET* pSocket)
{
  return connectsock(host, service, "udp", pSocket);
}

int connectsock(LPCSTR host,LPCSTR service,LPCSTR protocol, SOCKET* pSocket)
{
	memset((char *)&saSockAddr,0,sizeof(saSockAddr));
	saSockAddr.sin_family=AF_INET;

	struct servent FAR *pServiceEntry;   // pointer to service entry
	if(pServiceEntry=getservbyname(service,protocol))
	{
		if (uiFtpPort != 21)
			saSockAddr.sin_port=htons((u_short)uiFtpPort);
		else saSockAddr.sin_port=pServiceEntry->s_port;
	}
	else if((saSockAddr.sin_port=htons((u_short)atoi(service)))==0)
	{
		if (uiFtpPort != 21)
			saSockAddr.sin_port=htons((u_short)uiFtpPort);
		else
		{
			if(strcmpi(service,"FTP")==0)
				saSockAddr.sin_port=htons(21);
			else
				return WSAGetLastError();
		}
	}

	struct hostent *pHostEntry;      // pointer to host entry
	if (pHostEntry=gethostbyname(host))
	{
		memcpy((char *)&saSockAddr.sin_addr,pHostEntry->h_addr, pHostEntry->h_length);
	}
	else
	{
		if ((saSockAddr.sin_addr.s_addr=inet_addr(host)) == INADDR_NONE)
			return WSAGetLastError();
	}

	struct protoent FAR *pProtoEntry;    // pointer to protocol entry
	short pproto;
	if((pProtoEntry=getprotobyname(protocol))==0)
	{
		if(strcmpi(protocol,"TCP")==0)
			pproto=6;
		else
			return WSAGetLastError();
	}
	else
		pproto=pProtoEntry->p_proto;

	// Compute the socket type.
	int nSocketType;                 // socket type
	if(strcmp(protocol,"udp")==0)
		nSocketType=SOCK_DGRAM;
	else
		nSocketType=SOCK_STREAM;

	// Allocate a new socket.
	SOCKET sSocket=socket(AF_INET, nSocketType, pproto);
	if (sSocket==INVALID_SOCKET) 
	{
		return WSAGetLastError();
	}

	int iFlag=1;
	if (setsockopt(sSocket,SOL_SOCKET,SO_REUSEADDR,
         (char *)&iFlag,sizeof(iFlag)) == SOCKET_ERROR)
	{
		int iErr = WSAGetLastError();
		closesocket(sSocket);
		return iErr;
	}
   
	memcpy((LPSTR)&saSockAddr1,(LPSTR)&saSockAddr,sizeof(saSockAddr));
	saSockAddr1.sin_port=htons(20);
	if (connect(sSocket,(struct sockaddr *)&saSockAddr,sizeof(saSockAddr)) == SOCKET_ERROR)
	{
		int iErr = WSAGetLastError();
		closesocket(sSocket);
		return iErr;
	}

	// Return the socket we opened with no error.
	*pSocket = sSocket;
	return 0;
} 

// read a reply (may be multi line) and display in debug window
int ReadDisplayLine(SOCKET sockfd, int* pRetCode /*=NULL*/)
{
	iCode = 0;
	// can't do anything if we don't have a socket
	if (sockfd == INVALID_SOCKET)
		return WSAENOTSOCK;

	// go read the line
	int iErr = ReadLine(sockfd, pRetCode);
	if (iErr != 0)
	{
//		OutputDebugString("ReadLine from ReadDisplayLine failed\r\n");
		return iErr;
	}

	if (pRetCode != NULL)
	{
		// Munge the return code.
		if (*pRetCode == 226)
		{
			*pRetCode = 550;
		}
		else
		{
			if((iCode==0) && *pRetCode>99 && *pRetCode<600)
				iCode = *pRetCode;
			if (iCode==550)
				*pRetCode = iCode;
			else if (iCode>99 && iCode<600)
				*pRetCode = (iCode/100);
			else
				*pRetCode = 0;
		}
	}

	return iErr;
}

// read a reply line back in from the sockfd and return the
// value at the beginning of the first line.
int ReadLine(SOCKET sockfd, int* pRetCode /*=NULL*/)
{
	int iErr;
	do
	{
		iErr = ReadALine(sockfd, pRetCode);
	} while (iErr == 0 && szMsgBuf[3] == '-');

	return iErr;
}

// read a reply line back in from the sockfd and return the
// value at the beginning of the first line.
int ReadALine(SOCKET sockfd, int* pRetCode /*=NULL*/)
{
	int nIndex;
	int i;
	char c;

	// can't do anything if we don't have a socket
	if (sockfd == INVALID_SOCKET)
		return WSAENOTSOCK;

	memset(szMsgBuf, 0, 4096);

	LPSTR szBuf=szMsgBuf;
	int iBytesRead=0;

	int iNumBytes;

	// Peek into the recv stream to scan for end-of-line.
	while (iBytesRead < 4000 && (iNumBytes = recv(sockfd, (LPSTR)szBuf, 82, MSG_PEEK)) > 0)
	{
//		char b[80];
//		sprintf(b, "Received %d bytes\r\n", iNumBytes);
//		OutputDebugString(b);

		szBuf[iNumBytes]=0;
		for(nIndex=0;nIndex<iNumBytes;nIndex++)
		{
			if(szBuf[nIndex]==0 || szBuf[nIndex]==0x0a)
			{
				iNumBytes=nIndex+1;
				break;
			}
		}
		// Do the actual read.
		iNumBytes=recv(sockfd, szBuf, iNumBytes, 0);
		if (iNumBytes <= 0)
		{
//			OutputDebugString("Reading ");
			break;
		}
		// again, terminate the string
		szBuf[iNumBytes]=0;
		szBuf+=iNumBytes;
		// count the bytes that we have read so far
		iBytesRead+=iNumBytes;
		// if the last character read was a LF, then stop.
		if (*(szBuf-1)==0x0a)
			break;
	}

	if (iNumBytes <= 0)
	{
//		OutputDebugString(iNumBytes == 0 ? "recv returned 0\r\n" : "recv returned SOCKET_ERROR\r\n");
		int iErr = WSAGetLastError();
		return (iErr == 0) ? 13000 : iErr;
	}

	*szBuf=0;

	// find the retcode at the beginning of the line
	if (pRetCode != NULL)
	{
		c=szMsgBuf[3];
		szMsgBuf[3]=0;
		*pRetCode=atoi(szMsgBuf);
		szMsgBuf[3]=c;
	}

	// Strip off unwanted trailing things.
	while((i=strlen(szMsgBuf))>2 &&
			(szMsgBuf[i-1]==0x0a || szMsgBuf[i-1]==0x0d || szMsgBuf[i-1]==' '))
	{
		szMsgBuf[i-1]=0;
	}

#ifdef _DEBUG
  OutputDebugString(szMsgBuf);
  OutputDebugString("\n");
#endif

  return 0;
}

int SetTransferType(SOCKET Socket, char TransferType)
{
   char cbBuffer[10];
   sprintf(cbBuffer, "type %c", TransferType);
	return command(Socket, cbBuffer);
}
