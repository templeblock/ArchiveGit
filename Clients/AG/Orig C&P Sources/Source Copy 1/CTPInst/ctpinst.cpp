
#include <windows.h>
#include "resource.h"

#define MSGBOX_TITLE	"Installer"
#define SRCFILE			"npctp.dl_"
#define DESTFILE		"NpCtp.dll"


//
// Eliminate the need for linking in the CRT libraries
//
extern "C" void WinMainCRTStartup()
{
	//
	// Get the Navigator plugin directory
	//
	char szInstallDir[_MAX_PATH] = "";

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Netscape\\Netscape Navigator\\Main",
		0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(szInstallDir);
		RegQueryValueEx(hKey, "Install Directory", NULL, NULL, (BYTE *)szInstallDir, &dwSize);
		RegCloseKey(hKey);
	}

	if (!szInstallDir[0])
	{
		MessageBox(NULL, "Netscape Navigator 3 installation directory registry entry not found.",
					MSGBOX_TITLE, MB_OK);
		ExitProcess(0);
	}
	else
	{
		if (szInstallDir[lstrlen(szInstallDir) - 1] != '\\')
			lstrcat(szInstallDir, "\\");
		lstrcat(szInstallDir, "program\\plugins");
	}


	//
	// Tell the user that we're going to install the plug-in
	//
	char szMsg[512];
	wsprintf(szMsg, "This is the American Greetings Create and Print Plug-in installer\n"
				"for Netscape Navigator version 3\n\n"
				"It will install the plug-in into -\n%s\n\n"
				"Press OK to install.",
				szInstallDir);

	if (MessageBox(NULL, szMsg, MSGBOX_TITLE, MB_OKCANCEL) == IDOK)
	{
		//
		// Copy the compressed plug-in to the temp directory
		//
		char szSrcDir[_MAX_PATH];
		GetTempPath(sizeof(szSrcDir), szSrcDir);

		char szCompressedFile[_MAX_PATH];
		lstrcpy(szCompressedFile, szSrcDir);
		if (szCompressedFile[lstrlen(szCompressedFile) - 1] != '\\')
			lstrcat(szCompressedFile, "\\");
		lstrcat(szCompressedFile, SRCFILE);

		OFSTRUCT of;
		HANDLE hFile = (HANDLE)OpenFile(szCompressedFile, &of, OF_CREATE | OF_WRITE);
		if (hFile == (HANDLE)HFILE_ERROR)
		{
			MessageBox(NULL, "Error extracting plug-in to temp directory.",
						MSGBOX_TITLE, MB_OK);
			ExitProcess(0);
		}

		HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_COMPRESSED), "COMPRESSED");
		HGLOBAL hResData = LoadResource(NULL, hResInfo);
		DWORD dwSize = SizeofResource(NULL, hResInfo);
		void *pData = LockResource(hResData);
		DWORD dwBytesWritten;
		WriteFile(hFile, pData, dwSize, &dwBytesWritten, NULL);
		CloseHandle(hFile);

		//
		// Decompress the plug-in to the Netscape plug-in directory
		//
		char szTempFile[_MAX_PATH] = "";
		UINT uTmpFileLen = 0;
		DWORD dwResult = 0;

		dwResult = VerInstallFile(0, SRCFILE, DESTFILE, szSrcDir, szInstallDir, "",
									szTempFile, &uTmpFileLen);

		if (dwResult == 0)
			MessageBox(NULL, "The plug-in was successfully installed.", MSGBOX_TITLE, MB_OK);
		else if (dwResult & VIF_SRCOLD)
		{
			MessageBox(NULL, "You already have a newer version of the plug-in installed.",
						MSGBOX_TITLE, MB_OK);
		}
		else if (dwResult & VIF_FILEINUSE)
		{
			MessageBox(NULL, "You already have a version of the plug-in installed and it is currently in use.\n"
						"You will have to exit Netscape before installing the new version",
						MSGBOX_TITLE, MB_OK);
		}
		else if (dwResult & VIF_OUTOFSPACE)
		{
			MessageBox(NULL, "You do not have enough disk space to install the plug-in.",
						MSGBOX_TITLE, MB_OK);
		}
		else if ((dwResult & VIF_WRITEPROT) || (dwResult & VIF_TEMPFILE))
		{
	    	dwResult = VerInstallFile(VIFF_FORCEINSTALL, SRCFILE, DESTFILE, szSrcDir,
										szInstallDir, "", szTempFile, &uTmpFileLen);
			if (dwResult & VIF_WRITEPROT)
			{
				MessageBox(NULL,
							"You already have a version of the plug-in installed and it is write protected.\n"
							"You will have to remove the read only attribute before installing the new version.",
							MSGBOX_TITLE, MB_OK);
			}
			else if (dwResult)
			{
				char szMsg[50];
				wsprintf(szMsg, "Installation failed.  Error %x", dwResult);
				MessageBox(NULL, szMsg, MSGBOX_TITLE, MB_OK);
			}
			else
				MessageBox(NULL, "The plug-in was successfully installed.", MSGBOX_TITLE, MB_OK);
		}
		else
		{
			char szMsg[50];
			wsprintf(szMsg, "Installation failed.  Error %x", dwResult);
			MessageBox(NULL, szMsg, MSGBOX_TITLE, MB_OK);
		}

		//
		// Clean up the temporary files
		//
		DeleteFile(szCompressedFile);
		if (szTempFile[0])
			DeleteFile(szTempFile);
	}
	else
		MessageBox(NULL, "Installation aborted.", MSGBOX_TITLE, MB_OK);

	ExitProcess(0);
}

