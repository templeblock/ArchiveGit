#include <windows.h>

/***********************************************************************/
int Print( LPSTR lpFormat, ... )
/***********************************************************************/
{
char szBuffer[512];
LPSTR lpArguments;
int retc;
HWND hFocusWindow;

/* Put up a modal message box with arguments specified exactly like printf */
lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
hFocusWindow = GetFocus();
// Load app name resource
retc = MessageBox( NULL/*MessageParent*/, szBuffer, "Error", MB_OK | MB_TASKMODAL );
SetFocus( hFocusWindow );
return( retc );
}

//***********************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
//***********************************************************************
{
	LPSTR lpClassName, lpExeName;

	if ( !lpCmdLine )
		return( FALSE );

	// Skip over white space
	while ( *lpCmdLine && *lpCmdLine <= ' ' )
		lpCmdLine++;
	// If no argument get out
	if ( !(*lpCmdLine) )
		return( FALSE );
	// Here's the argument
	lpClassName = lpCmdLine;
	// Find the end of the string
	while ( *lpCmdLine && *lpCmdLine != ' ' )
		lpCmdLine++;
	// Zap the end of string
	*lpCmdLine++ = '\0';

	// Skip over white space
	while ( *lpCmdLine && *lpCmdLine <= ' ' )
		lpCmdLine++;
	// If no argument get out
	if ( !(*lpCmdLine) )
		return( FALSE );
	// Here's the argument
	lpExeName = lpCmdLine;
	// Find the end of the string
	while ( *lpCmdLine && *lpCmdLine != ' ' )
		lpCmdLine++;
	// Zap the end of string
	*lpCmdLine = '\0';

	// If a window of that class is running, get out
	if ( FindWindow( lpClassName, NULL ) )
		return( FALSE );

	UINT err = WinExec(lpExeName, nCmdShow);
	if (err < 32)
		Print("Failed to start '%s'\nError code %d.", lpExeName, err);
	return 0;
}
