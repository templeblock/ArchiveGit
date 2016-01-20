#include <windows.h>
//#include "proto.h"
//#include "commonid.h"

#define PROFILE_NAME "LAUNCH.INI"
#define SECTION "LAUNCH"
#define PROGRAM_ENTRY "PROGRAM"
#define VOLUME_ENTRY "VOLUME"

HINSTANCE hAppInstance;

#ifndef WIN32
#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6
#endif

//************************************************************************
LPSTR FileName( LPSTR lpPath )
//************************************************************************
{
	LPSTR lp;
	
	lp = lpPath;
	lp += lstrlen( lpPath );
	while( *lp != '\\' && *lp != ':' )
	    {
	    if ( (--lp) < lpPath )
	        return( lpPath );
	    }
	lp++;
	return( lp ); // the file name
}

//************************************************************************
LPSTR StripFile( LPSTR lpPath )
//************************************************************************
{
	LPSTR lp;
	
	if ( lp = FileName( lpPath ) )
	    *lp = '\0';
	return( lpPath );
}

#ifndef WIN32
//************************************************************************/
BOOL IsCDROM (int DriveNumber)
//************************************************************************/
{
    BOOL bCDfound = FALSE;

    _asm    mov     ax,01500h
    _asm    xor     bx,bx
    _asm    int     02Fh
    _asm    or      bx,bx
    _asm    jz      Done
    _asm    mov     ax,0150bh
    _asm    mov     cx,DriveNumber
    _asm    int     02Fh
    _asm    or      ax,ax
    _asm    jz      Done
    bCDfound = TRUE;
Done:
    return bCDfound;
}

/***************************************************************************
 *
 *        Name:  Get31ExtendedDriveType
 *
 *     Purpose:  Determines the type of the specified drive.  Extends
 *               standard Windows GetDriveType by identifying RAM drives and
 *               CD ROM drives.
 *
 *  Parameters:  Drive - Drive Number to check (A=0,B=1,etc...)
 *                       If Drive is an alphabetic character,
 *                       then it will be automatically converted to
 *                       the correct drive number (e.g. 'B' is same as 1,
 *                       'c' is same as 2).
 *
 *     Returns:  0 for invalid or unknown drive, or one of the following:
 *               DRIVE_REMOVABLE        // Same as Windows
 *               DRIVE_FIXED            // Same as Windows
 *               DRIVE_REMOTE           // Same as Windows
 *               DRIVE_CDROM            // Extended Type
 *               DRIVE_RAM              // Extended Type
 */
//************************************************************************/
UINT Get31ExtendedDriveType (int Drive)
//************************************************************************/
{
    UINT DriveType;

    DriveType = GetDriveType (Drive);
    switch (DriveType)
    {
	    case DRIVE_REMOTE:
    	    if (IsCDROM (Drive))
    	    	DriveType = DRIVE_CDROM;
        break;
    }
    return DriveType;
}

#endif // WIN32

//************************************************************************/
UINT GetExtendedDriveType(int i)
//************************************************************************/
{
	#ifdef WIN32
		FNAME szPath;
	
		*szPath = i + 'a';
		*MyCharNextN( szPath, 1 ) = ':';
		*MyCharNextN( szPath, 2 ) = '\\';
		*MyCharNextN( szPath, 3 ) = '\0';
		return( GetDriveType( szPath ));
	#else
		return(Get31ExtendedDriveType (i));
	#endif
}

/***********************************************************************/
int Print( LPSTR lpFormat, ... )
/***********************************************************************/
{
	char szBuffer[512];
	LPSTR lpArguments;
	STRING szTitle;
	
	/* Put up a modal message box with arguments specified exactly like printf */
	lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
	HWND hFocusWindow = GetFocus();
	// Load app name resource
	#ifdef NO_PHAPP
	HINSTANCE hInstance = hAppInstance;
	#else
	HINSTANCE hInstance = GetApp()->GetInstance();
	#endif
	LoadString( hInstance, IDS_TITLE, szTitle, sizeof(szTitle) );
	int retc = MessageBox( NULL/*MessageParent*/, szBuffer, szTitle, MB_OK | MB_TASKMODAL );
	SetFocus( hFocusWindow );
	return( retc );
}

/***********************************************************************/
int OkCancel( LPSTR lpFormat, ... )
/***********************************************************************/
{
	char szBuffer[512];
	LPSTR lpArguments;
	STRING szTitle;
	
	/* Put up a modal message box with arguments specified exactly like printf */
	lpArguments = (LPSTR)&lpFormat + sizeof(lpFormat);
	wvsprintf( szBuffer, lpFormat, (LPSTR)lpArguments );
	HWND hFocusWindow = GetFocus();
	// Load app name resource
	#ifdef NO_PHAPP
	HINSTANCE hInstance = hAppInstance;
	#else
	HINSTANCE hInstance = GetApp()->GetInstance();
	#endif
	LoadString( hInstance, IDS_TITLE, szTitle, sizeof(szTitle) );
	int retc = MessageBox( NULL/*MessageParent*/, szBuffer, szTitle, MB_OKCANCEL | MB_TASKMODAL );
	SetFocus( hFocusWindow );
	return( retc );
}

//***********************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
//***********************************************************************
{
	FNAME szPath, szProgram, szVolume;
    OFSTRUCT of;

	hAppInstance = hInstance;
	GetModuleFileName( hInstance, szPath, sizeof( szPath ) );
	StripFile( szPath );
	lstrcat(szPath, PROFILE_NAME);
	// see if launch ini file exists
    if ( OpenFile( szPath, &of, OF_EXIST ) == HFILE_ERROR )
    {
		Print("Program not installed properly.\nCannot find file '%s'.", (LPSTR)szPath);
		SetErrorMode(errmode);
		return(1);
    }
	// get program entry
	if (!GetPrivateProfileString(SECTION, PROGRAM_ENTRY, "", szProgram, sizeof(szProgram), szPath))
	{
		Print("Program not installed properly.\nCannot find entry '%s' in section '%s' of '%s'.", (LPSTR)PROGRAM_ENTRY, (LPSTR)SECTION, (LPSTR)szPath);
		SetErrorMode(errmode);
		return(2);
	}
	// get volume name entry
	if (!GetPrivateProfileString(SECTION, VOLUME_ENTRY, "", szVolume, sizeof(szVolume), szPath))
	{
		Print("Program not installed properly.\nCannot find entry '%s' in section '%s' of '%s'.", (LPSTR)VOLUME_ENTRY, (LPSTR)SECTION, (LPSTR)szPath);
		SetErrorMode(errmode);
		return(3);
	}

	UINT errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	// see if installed path is cool
    if ( OpenFile( szProgram, &of, OF_EXIST ) == HFILE_ERROR )
    {
		// keep trying to we get it or user gives up
		while (TRUE)
		{
			// see if there is a CDROM drive that exists with our program on it.
			for (int drive = 0; drive < 26; ++drive)
			{
				UINT type = GetExtendedDriveType(drive);
				if (type == DRIVE_CDROM)
				{
					szProgram[0] = drive + 'a';
				    if ( OpenFile( szProgram, &of, OF_EXIST ) != HFILE_ERROR )
						break;
				}
				else
				if (type == 1) 
				{
					drive = 26;
					break;
				}
			}
			// see if we found the file somewhere
			if (drive < 26)
				break; // file was found
			// ask user to insert CD into cdrom drive
			if (OkCancel("Please insert CDROM with volume label '%s' into your CDROM drive.", (LPSTR) szVolume) == IDCANCEL)
			{
				SetErrorMode(errmode);
				return(4);
			}
			// keep trying
		}
    }
									
	UINT err = WinExec(szProgram, nCmdShow);
	if (err < 32)
		Print("Failed to start program '%s';  Error code = %d.", (LPSTR)szProgram, err);

	SetErrorMode(errmode);
	return 0;
}
