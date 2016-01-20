#include "afxwin.h"
#include "Message.h"

/////////////////////////////////////////////////////////////////////////////
int Message( LPCTSTR lpFormat, ... )
{
	// Put up a modal message box with arguments specified exactly like printf
	LPCTSTR lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
	TCHAR szBuffer[1024];
	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
	HWND hFocusWindow = GetFocus();
	// Load app name resource
	int retc = ::MessageBox( NULL/*MessageParent*/, szBuffer, AfxGetAppName(), MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OK );
	SetFocus( hFocusWindow );
	return( retc );
}

/////////////////////////////////////////////////////////////////////////////
int MessageOKCancel( LPCTSTR lpFormat, ... )
{
	// Put up a modal message box with arguments specified exactly like printf
	LPCTSTR lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
	TCHAR szBuffer[1024];
	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
	HWND hFocusWindow = GetFocus();
	// Load app name resource
	int retc = ::MessageBox( NULL/*MessageParent*/, szBuffer, AfxGetAppName(), MB_TASKMODAL | MB_ICONEXCLAMATION | MB_OKCANCEL );
	SetFocus( hFocusWindow );
	return( retc );
}

/////////////////////////////////////////////////////////////////////////////
int MessageConfirm( LPCTSTR lpFormat, ... )
{
	// Put up a modal message box with arguments specified exactly like printf
	LPCTSTR lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
	TCHAR szBuffer[1024];
	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
	HWND hFocusWindow = GetFocus();
	// Load app name resource
	int retc = ::MessageBox( NULL/*MessageParent*/, szBuffer, AfxGetAppName(), MB_TASKMODAL | MB_ICONEXCLAMATION | MB_YESNOCANCEL );
	SetFocus( hFocusWindow );
	return( retc );
}

/////////////////////////////////////////////////////////////////////////////
void MessageDebug( LPCTSTR lpFormat, ... )
{
	// Put up a modal message box with arguments specified exactly like printf
	LPCTSTR lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
	TCHAR szBuffer[1024];
	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
	OutputDebugString(szBuffer);
	OutputDebugString("\n");
}

