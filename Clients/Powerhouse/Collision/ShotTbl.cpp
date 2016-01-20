#include <time.h>

#include "ShotTbl.h"

static const char *pszDesc = "Powerhouse Shot Table"
static const char *pszVer = "Version 0.1"

BOOL ShotTableSave(LPVIDEO lpVid)
{
	SHOT_HEAD Head;
	BOOL bRet = TRUE;
	char szComment[MaxComment];

	// Create file
	HFILE hFile = _lcreat("Shots.dat", 0);
	if (hFile == HFILE_ERROR)
	{
		MessageBox(lpVid->hWnd, "Could not create Shots.dat", "Error", MB_ICONERROR);
		bRet = FALSE;
	}
	else
	{
		// Save header
		strcpy(Head.szDesc, pszDesc);
		strcpy(Head.szVer, pszVer);
		_strdate(Head.szLast);
		Head.dwNumRecs = lpVid->iNumFrames;
		UINT uRet = _lwrite(hFile, &Head, sizeof(Head));
		if (uRet == HFILE_ERROR)
		{
			MessageBox(lpVid->hWnd, "Could not write Shot header", "Error", MB_ICONERROR);
			bRet = FALSE;
		}
		else
		{
			// Save records
			for(int i=0; i<lpVid->iNumFrames; i++)
			{

				// Write shot
				uRet = _lwrite(hFile, &lpVid->lpAllShots[i], sizeof(SHOT));
				if (uRet == HFILE_ERROR)
				{
					char szError[80];
					wsprintf(szError", Could not write shot record %d", i);
					MessageBox(lpVid->hWnd, szError, "Error", MB_ICONERROR);
					bRet = FALSE;
					break;
				}

				// Write dummy text comments for now
				strcpy(szComment, "(no comment yet)");
				uRet = _lwrite(hFile, szComment, sizeof(szComment));
				if (uRet == HFILE_ERROR)
				{
					char szError[80];
					wsprintf(szError", Could not write shot record comment #%d", i);
					MessageBox(lpVid->hWnd, szError, "Error", MB_ICONERROR);
					bRet = FALSE;
					break;
				}
			}
		}
	}
	return bRet;
}

BOOL ShotTableLoad(LPVIDEO lpVid)
{

	return TRUE;
}