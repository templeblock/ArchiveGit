#include "windows.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int PASCAL WinMain(HINSTANCE hInstance,
						 HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine,
						 int nCmdShow)
{
	// Get the Windows directory.
	char cbBuffer[_MAX_PATH];
	GetWindowsDirectory(cbBuffer, sizeof(cbBuffer)-1);

	// Append a backslash so we can add the path.
	int nLength = strlen(cbBuffer);
	if (cbBuffer[nLength-1] != '\\')
	{
		cbBuffer[nLength] = '\\';
		nLength++;
	}

	// Append the file name.
	strcpy(cbBuffer+nLength, "MSREGUSR.INI");

	// Do the remove.
	remove(cbBuffer);

	// Report the result.
	MessageBox(NULL, "You are now unregistered.", "Information", MB_OK|MB_ICONINFORMATION);

	// Done
	return 0;
}
