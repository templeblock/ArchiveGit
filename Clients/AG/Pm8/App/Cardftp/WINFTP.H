/*
  MODULE: WINFTP.H   (main program header file)
*/

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#ifndef MAXPACKET
#define MAXPACKET 4096
#endif

// some miscellaneous definitions that we use

#define SCRLWND      7531

#define FTP_PRELIM   1
#define FTP_COMPLETE 2
#define FTP_CONTINUE 3
#define FTP_RETRY    4
#define FTP_ERROR    5
#define FTP_ABORT    6

#define INCL_RESOURCES

// **** misc definitions
#define TYPE_I 'I'
#define TYPE_A 'A'
#define TYPE_E 'E'
#define TYPE_L 'L'

#define FORM_N 'N'
#define FORM_T 'T'
#define FORM_C 'C'

#define MODE_S 'S'
#define MODE_B 'B'
#define MODE_C 'C'

// **** function prototypes

// in ws_child.c
int CreateSubWindows (HWND,HWND);
int GetLocalDirForWnd (HWND);
int GetRemoteDirForWnd(HWND);
int ReadProcessHelp (HWND, SOCKET);
int DoMainPaint (HWND hWnd);
int GetButtonStatus (int);
int GetButtonMsgStatus (int);
int SetButtonStatus (int, int);
int FindButtonClicked (int, int);
int DoPaintButton (HWND, int, int);
int GetButtonID (int);
int SetStatus (HWND, LPSTR);
int ScrollStatus (HWND, int);
int UnsetStatusLines();
int GetChildWindowID (LPARAM);
int GetMaxStatusLines();

BOOL SetButtonEnabledStatus (int, BOOL);
BOOL GetButtonEnabledStatus (int);

void ShowOurFlags (void);
void CreateButtonPens (void);
void DeleteButtonPens (void);
void BoxIt (HDC, int, int, int, int, BOOL);

LPSTR FindName (LPSTR);
LPSTR GetStatusLine (int nLine);

// in ws_dir.c
int ProcessRemoteDirList (HWND hWnd);

// in ws_con.c
int command (SOCKET socket, LPCSTR pCommand, int* pRetCode = NULL);
int DoSTAT (SOCKET);
int DoCHMOD (SOCKET,LPSTR,LPSTR);
int DoCWD (SOCKET,LPSTR);
int DoSystemCommand (SOCKET);
int DoDirList (SOCKET,LPSTR);
int DoDELE (SOCKET,LPSTR);
int DoDisconnect (SOCKET);
int DoMKD (SOCKET,LPSTR);
int DoPWD (SOCKET);
int DoQUOTE (SOCKET,LPSTR);
int DoRMD (SOCKET,LPSTR);
int DoDirList (SOCKET,LPSTR);
int SendFile (SOCKET, LPCSTR, LPCSTR, char);
int RetrieveFile (SOCKET,LPSTR,LPSTR,char);
int SendPacket (SOCKET,LPCSTR);
int ForcePacket (SOCKET,LPSTR);
int ReadDisplayLine (SOCKET, int* pRetCode = NULL);
int ReadLine (SOCKET Socket, int* pRetCode = NULL);
int ReadALine (SOCKET Socket, int* pRetCode = NULL);
int SendMass (SOCKET Socket, LPCSTR filename, BOOL fBinary);
int ReadMass (SOCKET,LPSTR filename,BOOL);
int ForceCommand (SOCKET s, LPSTR lpCmd, ...);
int SetTransferType(SOCKET Socket, char TransferType);
SOCKET DoClose (SOCKET sockfd);
int DoConnect (LPCSTR, SOCKET*);
int GetFTPListenSocket (SOCKET sockfd, SOCKET* pListenSocket);

// in ws_ip.c
int connectsock (LPCSTR host,LPCSTR service,LPCSTR protocol, SOCKET* pSocket);
int connectTCP (LPCSTR host,LPCSTR service, SOCKET* pSocket);
int connectUDP (LPCSTR host,LPCSTR service, SOCKET* pSocket);
int sendstr (SOCKET sockfd,LPSTR ptr,int nbytes, int *lpCode);

// in ws_debug.c
int CreateDebugWindow (HWND hMainWnd,HWND hInst);
LRESULT CALLBACK DebugWndProc (HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SetDebugWindowText (LPSTR);

// in ws_error.c
LPSTR ReturnWSError(UINT Err,LPSTR lpszBuf);
void ReportWSError(LPSTR lpszMsg,UINT Err);

// in ws_main.c

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WS_AboutMsgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WS_HostMsgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WS_InputMsgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WS_DeleteProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WS_StatMsgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WS_FireWallSetupProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

int SubProcessAsync (HWND, UINT, WPARAM, LPARAM);
int InitInstance (HINSTANCE);
int ExecFTPApp (int);
int CheckMsgQueue (HWND);
int FindViewerIndex (LPSTR);
int AddDirToList (HWND, int, LPSTR);
int OnCmdLocalChangeDirLst (HWND);
int OnCmdLocalChangeDir (HWND);
int OnCmdLocalToRemote (HWND);
int OnCmdLocalMakeDir (HWND);
int OnCmdLocalRemoveDir (HWND);
int OnCmdLocalDeleteFile (HWND);
int OnCmdLocalRenameFile (HWND);
int OnCmdLocalHistory (HWND);
int OnCmdLocalDisplay (HWND);
int OnCmdLButtonDblClkLocal (HWND);
int OnCmdLocalRefresh (HWND);
int OnCmdRemoteMakeDir (HWND);
int OnCmdRemoteRemoveDir (HWND);
int OnCmdRemoteDeleteFile (HWND);
int OnCmdRemoteRenameFile (HWND);
int OnCmdRemoteChangeDir (HWND);
int OnCmdRemoteChangeDirLst (HWND);
int OnCmdRemoteToLocal (HWND);
int OnCmdRemoteRefresh (HWND, LPARAM);
int OnCmdRemoteHistory (HWND);
int OnCmdRemoteDisplay (HWND);
int OnCmdLButtonDblClkRemote (HWND);
int OnShowTransferType (HWND);
int OnCmdConnectHost (HWND, WORD);
int OnCmdListHost (HWND);
int OnCmdNListHost (HWND, WORD);
int OnCmdLogToFile (HWND, WORD);
int OnCmdLogFileName (HWND, WORD);
int OnCmdHelpHost (HWND);
int OnCmdStatusHost (HWND);
int OnCmdPWDHost (HWND);
int OnCmdDirOpHost (HWND, WORD);
int OnCmdGetFileHost (HWND);
int OnCmdPutFileHost (HWND);
int OnCmdQuoteHost (HWND);
int OnCmdGetCwdHost (HWND);
int OnCmdAbort (HWND);
int OnCmdAbout (HWND);
int OnCmdShowDebug (HWND, WORD);
int OnCmdSaveDir (HWND, WORD);
int OnCmdGetHostAddr (HWND, WORD);
int OnCmdLFileType (HWND, WORD);
int OnCmdRFileType (HWND, WORD);
int OnCmdCloseOrExit (HWND, WPARAM);
int nCwRegisterClasses (void );

void CleanupTempFiles (HWND);
void CreateTempFileNames (LPSTR);
void GetViewerInfo (HWND);
void SetViewerInfo (HWND, int);
void cwCenter (HWND, int);
void CwUnRegisterClasses (void );

LPSTR GetViewer (LPSTR, LPSTR, LPSTR);
LPSTR FindViewerName (LPSTR);

LRESULT OnCreate (HWND, UINT, WPARAM, LPARAM);
LRESULT OnTimer (HWND, UINT, WPARAM, LPARAM);
LRESULT OnWmCmd (HWND, UINT, WPARAM, LPARAM);
LRESULT OnSetCursor (HWND, UINT, WPARAM, LPARAM);
LRESULT OnPaint (HWND, UINT, WPARAM, LPARAM);
LRESULT OnRButtonDown (HWND, UINT, WPARAM, LPARAM);
LRESULT OnRButtonUp (HWND, UINT, WPARAM, LPARAM);
LRESULT OnLButtonDown (HWND, UINT, WPARAM, LPARAM);
LRESULT OnLButtonUp (HWND, UINT, WPARAM, LPARAM);
LRESULT OnMouseMove (HWND, UINT, WPARAM, LPARAM);
LRESULT OnVScroll (HWND, UINT, WPARAM, LPARAM);
LRESULT OnCtlColor (HWND, UINT, WPARAM, LPARAM);
LRESULT OnClose (HWND, UINT, WPARAM, LPARAM);
LRESULT OnSetFocus (HWND, UINT, WPARAM, LPARAM);
LRESULT OnKillFocus (HWND, UINT, WPARAM, LPARAM);
LRESULT OnParentNotify (HWND, UINT, WPARAM, LPARAM);
LRESULT OnDragDropFile (HWND, UINT, WPARAM, LPARAM);

// in ws_paint.c
void ReleaseDisplayMem (VOID);
void DoAddLine (LPSTR);
void DoPrintf (char *fmt,...);
void DoPaint (HWND);
int GetLocalInfo (void);  

//in ws_host.c
int  MakeLocalName(LPSTR szLocalName, LPSTR szRemoteName);
int  ConvertTargetDir (LPSTR lpDir, int nSiz);
int  ConvertSourceDir (LPSTR lpBuf, LPSTR lpDir, int nSiz);
int RetrieveDirList (HWND);
int SaveDirList (HWND);
int UpdateDirListOpt (HWND, BOOL);
void SaveUserInfo();
void LoadUserInfo();
void SetViewTyp (int, char);
LPSTR GetHostName (int nI);
LPSTR GetHostType (int nI);
LPSTR GetHostTypeValue (int nI);
LPSTR lpVuExt (int);
LPSTR lpVuPgm (int);
LPSTR lpVuDir (int);
LPSTR ReAllocViewer (int);
char GetViewTyp (int);
BOOL CALLBACK WS_SetHostTypeProc (HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);

//in ws_local.c
int OnCmdDropFiles (HWND hWnd, LPSTR *lParam);
int OnCmdDisplayHostList (HWND hWnd, WORD wCtlID, WORD wNotifyCode);
int DoRunScript (HWND hWnd, LPSTR lpScript);
int OnCmdZoomInfo (HWND);
void ClearZoomInfo();
void AddMessageToView (LPSTR lpBuf, LPSTR lpHdr);
void CreateUniqueName (LPSTR localname, LPSTR szName, LPSTR Ext);
void CreateUniqueNameNTFS (LPSTR localname);
//void SetTotalBytes (unsigned LONG lBytes);
//void SetXmitBytes (unsigned LONG lBytes);
void OnPaintXfer (HWND hWnd);
void CreateXferWindow();
void DeleteXferWindow();
void SetXferWindowText (LPSTR lpStr);
LRESULT CALLBACK WndXferProc (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndMsgProc (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

//in ws_icmp.c
BOOL CALLBACK WS_FindHostProc (HWND, UINT, WPARAM, LPARAM);

#ifdef WIN32
   #define setdisk(x)                  _chdrive (x)
#else
   #define setdisk(x)                  { int nDrv; _dos_setdrive (x, &nDrv); }   
#endif

#define UNREFERENCED_PARAMETER(x)   (x)

#define DLG_HOST_ANONY              101
#define DLG_HOST_SAVE               102
#define DLG_HOST_PWD                103

#define DLG_HOST_DIR                105

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

