//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <dos.h>
#include <direct.h>
#include <string.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static  void AddSpaces(LPSTR lpString,int iCount);
static  LPSTR GetFilePath(HWND hDlg, WORD idCtl, LPSTR lpPath, LPSTR lpCurrentDir);

extern HINSTANCE hInstAstral;

static FNAME szFileSpec;
static FNAME szSaveName;
static FNAME szFileTemp;
static FNAME szDirTemp;
static WORD idFileType;
static WORD wFileAttr;
static BOOL fSaving, fUntitled;
static WORD idDataType;
static BOOL bNavigated;
static ITEMID idOpen;
static int iFilesProcessed;
static int iFilesToBeProcessed;
static BOOL bAbort, bCancelled, bImageOpen;

typedef struct _rdrdef {
	char	szDLL[13];
	BOOL	bTypes[6];	// IDC_SAVECT, IDC_SAVELA, IDC_SAVESP,
				// IDC_SAVE4BITCOLOR, IDC_SAVE8BITCOLOR, IDC_SAVE24BITCOLOR
	BOOL	bOpen;		// can we open these files
	BOOL	bSave[4];  	// Save Depths supported
	BOOL	bCompressed;	// Optional compression?
	} RDRDEF;
static RDRDEF Readers[] = {
"",
YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,	// ART (PP)

"",
YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,	// TIFF

"",
YES,YES,YES,YES,YES,YES,YES,YES,YES,YES,YES, NO,	// BMP

};

#ifdef _MAC
#define CHDRIVE(drive) 'C'
#define CHDIR(dir) ( chdir((PSTR)(dir)) )
#define CURRENTDRIVE 2
#define CURRENTDIR(dir,len) ( getcwd((PSTR)dir,len) )
#else
#define CHDRIVE(drive) (_chdrive((drive)+1) )
#define CHDIR(dir) ( chdir((PSTR)(dir)) )
#define CURRENTDRIVE (_getdrive()-1)
#define CURRENTDIR(dir,len) ( getcwd((PSTR)dir,len) )
#endif

/************************************************************************/
long DoOpenDlg(
/************************************************************************/
HWND 	hDlg,
int 	idDlg,
WORD 	idType,
LPSTR 	pFileNameOut,
BOOL 	fSavingIn)
{
int iReturn;
FNAME szExtension, szStartDir;
int iStartDrive;

fSaving = fSavingIn;

idDataType = NULL;
bImageOpen = idDlg == IDD_OPEN || idDlg == IDD_SAVEAS;
idFileType = idType;
if (bImageOpen)
	{
	idFileType = bound(idFileType, IDN_ART, IDN_BMP);
	if (fSaving)
		{
		if (!Readers[idFileType-IDN_ART].bSave[FrameDepth(ImgGetBaseEditFrame(lpImage))])
			idFileType = IDN_ART;
		}
	else
		{
		if (!Readers[idFileType-IDN_ART].bOpen)
			idFileType = IDN_ART;
		}
	}
fUntitled = lpImage && lpImage->fUntitled;

if (bImageOpen && fSaving && !fUntitled)
	{
	lstrcpy( szSaveName, pFileNameOut );
	lstrcpy( szFileSpec, pFileNameOut );
	stripfile( szFileSpec );
	FixPath( szFileSpec );
	LookupExtension( idFileType, szExtension );
	lstrcat( szFileSpec, szExtension );
	}
else
	{
	GetFileLocation( idFileType, szFileSpec );
	}

/* Initialize some values used in DlgOpenProc */
wFileAttr = 0;

iStartDrive = CURRENTDRIVE;
CURRENTDIR( szStartDir, sizeof(FNAME) );
idOpen = 0;
do
	{
	if (idOpen)
		idDlg = idOpen;
	else
		idOpen = idDlg;
	iReturn = AstralDlg( NO|2, hInstAstral, hDlg, idDlg, DlgOpenProc );
	}
while (idOpen != idDlg);
CHDRIVE( iStartDrive );
//CHDIR( szStartDir );

if ( !iReturn )
	{
	pFileNameOut[0] = '\0';
	return( NULL );
	}

lstrcpy( pFileNameOut, szFileSpec );
SetFileLocation( idFileType, szFileSpec );

return( MAKELONG( idFileType, idDataType ) );
}


/************************************************************************/
BOOL WINPROC EXPORT DlgOpenProc(
/************************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
BOOL Bool;
ITEMID id;
WORD wMsg, wIndex;
int i, ii, l, fp, hi, lo;
FNAME szDriveNDir, szFileName, szExtension;
HMENU hMenu;
STRING szString;
char cChar;
BOOL bModifySaveName;
static BOOL bDisableCancel;

switch (msg)
    {
    case WM_INITDIALOG:
	SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
	CenterPopup( hDlg );
	if ( !DlgDirList(hDlg, szFileSpec, IDC_FLIST, IDC_FPATH, wFileAttr) )
		{
		lstrcpy( szFileSpec, lstrchr( szFileSpec, '*' ) );
		DlgDirList(hDlg, szFileSpec, IDC_FLIST, IDC_FPATH, wFileAttr);
		}
	SetFileName(hDlg, IDC_FNAME, szFileSpec, szSaveName, fSaving);
	CheckComboItem( hDlg, IDC_FILETYPES, IDC_ART, IDC_BMP,
		IDC_ART + (idFileType-IDN_ART) );
	SendDlgItemMessage(hDlg, IDC_FNAME, EM_LIMITTEXT, MAX_FNAME_LEN-1, 0L);

	LoadComboWithDrives( hDlg, IDC_DRIVES );
	LoadComboWithDirectories( hDlg, IDC_DIRS, NO );
	CheckDlgButton( hDlg, IDC_SAVEPATH, Save.OKtoSavePath );
	CheckDlgButton( hDlg, IDC_SAVECOMPRESS, Save.Compressed );
	for (i = IDC_ART; bImageOpen && i <= IDC_BMP; ++i)
		{
		ii = i-IDC_ART;
		if (lstrlen(Readers[ii].szDLL))
			{
			lstrcpy(szString, Control.ProgHome);
			lstrcat(szString, Readers[ii].szDLL);
			if (!FileExists(szString))
				{
				if ( GetPrivateProfileString( "Micrografx", "Libraries", "",
					szString, sizeof(STRING), "MGX.INI" ) > 2 )
						FixPath( szString );
				lstrcat(szString, Readers[ii].szDLL);
				if (!FileExists(szString))
					{
					ControlEnable( hDlg, i, NO);
					continue;
					}
				}
			}
		if (fSaving)
			ControlEnable( hDlg, i, lpImage &&
				Readers[ii].bSave[FrameDepth(ImgGetBaseEditFrame(lpImage))] );
		else
			ControlEnable( hDlg, i, Readers[ii].bOpen );
		}
	if ( bImageOpen && lpImage )
		{
		idDataType = lpImage->DataType;
		for (i = IDC_SAVECT; i <= IDC_SAVE24BITCOLOR; ++i)
			ControlEnable( hDlg, i,
			Readers[idFileType-IDN_ART].bTypes[i-IDC_SAVECT]);
		CheckComboItem( hDlg, IDC_DATATYPES, IDC_SAVECT,
			IDC_SAVE24BITCOLOR, idDataType );
		ControlEnable( hDlg, IDC_SAVECOMPRESS,
			Readers[idFileType-IDN_ART].bCompressed );
		}

	SetFocus( GetDlgItem( hDlg, IDC_FNAME ) );
	SendDlgItemMessage(hDlg, IDC_FNAME, EM_SETSEL,
		NULL, MAKELONG(0, 0x7fff));
	bNavigated = NO;
	bDisableCancel = NO;
	return( FALSE );

    case WM_PALETTECHANGED:
	break;

    case WM_MENUSELECT:
	lo = LOWORD(lParam);
	hi = HIWORD(lParam);
	if (hi == 0)
		break;
	if (lo == -1)
		break;
	if (lo & MF_SYSMENU)
		break;
	if (lo & MF_POPUP)
		{
		hMenu = (HMENU)wParam;
		while (GetSubMenu(hMenu, 0))
			hMenu = GetSubMenu(hMenu, 0);
		id = GetMenuItemID(hMenu, 0);
		if (id <= 0)
			break;
		wParam = id - 1;
		}
	HintLine( wParam );
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, idOpen ) );

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE|2 );
	break;

    case WM_MEASUREITEM:
    case WM_DRAWITEM:
	id = ((LPDRAWITEMSTRUCT)lParam)->CtlID;
	Bool = ( id == IDC_DRIVES || id == IDC_DIRS );
	return( OwnerDraw( hDlg, msg, lParam, Bool ) );
//	break;

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	if (wParam != IDCANCEL)
		bDisableCancel = NO;
	switch(wParam)
	    {
	    case IDC_FILETYPES:
//	    case IDC_ART:
//	    case IDC_TIFF:
//	    case IDC_BMP:
		if ( !(wParam = HandleCombo( hDlg, wParam, lParam )) )
			break;
		idFileType = IDN_ART + (wParam-IDC_ART);
		if (bNavigated)
			{
			LookupExtension( idFileType, szFileSpec );
			}
		else	GetFileLocation( idFileType, szFileSpec );
		for (i = IDC_SAVECT; i <= IDC_SAVE24BITCOLOR; ++i)
			ControlEnable( hDlg, i,
			Readers[idFileType-IDN_ART].bTypes[i-IDC_SAVECT]);
		ControlEnable( hDlg, IDC_SAVECOMPRESS,
			Readers[idFileType-IDN_ART].bCompressed );
		SetFileName(hDlg, IDC_FNAME, szFileSpec, szSaveName, NO );
		SendMessage( hDlg, WM_COMMAND, IDOK, 0L );
		break;

	    case IDC_DATATYPES:
//	    case IDC_SAVECT:
//	    case IDC_SAVELA:
//	    case IDC_SAVESP: // scatterprint
//	    case IDC_SAVE8BITCOLOR:
//	    case IDC_SAVE24BITCOLOR:
		if ( !(wParam = HandleCombo( hDlg, wParam, lParam )) )
			break;
		idDataType = wParam;
		break;

	    case IDC_SAVEPATH:
		Save.OKtoSavePath = (BOOL)SendDlgItemMessage (hDlg, IDC_SAVEPATH,
		  BM_GETCHECK, 0, 0L);
//		Save.OKtoSavePath = !Save.OKtoSavePath;
//		CheckDlgButton( hDlg, IDC_SAVEPATH, Save.OKtoSavePath );
		break;

	    case IDC_SAVECOMPRESS:
		Save.Compressed = (BOOL)SendDlgItemMessage (hDlg, IDC_SAVECOMPRESS,
		  BM_GETCHECK, 0, 0L);
//		Save.Compressed = !Save.Compressed;
//		CheckDlgButton( hDlg, IDC_SAVECOMPRESS, Save.Compressed );
		break;

	    case IDC_DRIVES:
		wMsg = HIWORD(lParam);
		if ( wMsg != CBN_SELCHANGE )
			break;
		wIndex = SendDlgItemMessage( hDlg, wParam, CB_GETCURSEL, 0, 0L);
		SendDlgItemMessage( hDlg, wParam, CB_GETLBTEXT, wIndex,
			(long)(LPSTR)szDriveNDir );
		id = ExtractStringID( szDriveNDir );
		if (CHDRIVE( *szDriveNDir - 'a' ))
			{
			LoadComboWithDrives(hDlg, IDC_DRIVES);
			break;
			}
		if (!CURRENTDIR(szString, sizeof(szString)))
			{
			GetDlgItemText(hDlg, IDC_FPATH, szString,
				 sizeof(szString));
			CHDRIVE(*szString - 'a');
			LoadComboWithDrives(hDlg, IDC_DRIVES);
			}
//12/15		SetFileName(hDlg, IDC_FNAME, szFileSpec, szSaveName, NO);
		DlgDirList( hDlg, szFileSpec, IDC_FLIST, IDC_FPATH, wFileAttr );
		LoadComboWithDirectories( hDlg, IDC_DIRS, YES );
		SetDlgItemText( hDlg, IDC_DISKSPACE,
			DriveSize( *szDriveNDir - 'a', szString ) );
		SetFileName(hDlg, IDC_FNAME, szFileSpec, szSaveName, fSaving );
		bNavigated = YES;
		break;

	    case IDC_DIRS:
		wMsg = HIWORD(lParam);
		if ( wMsg == LBN_DBLCLK )
			{
			SendMessage(hDlg, WM_COMMAND, IDOK, 1L);
			break;
			}
		if ( wMsg != LBN_SELCHANGE )
			break;
		wIndex = SendDlgItemMessage( hDlg, wParam, LB_GETCURSEL, 0, 0L);
		// Figure out how to build the path name based on the selection
		SendDlgItemMessage( hDlg, wParam, LB_GETTEXT, wIndex,
			(long)(LPSTR)szDriveNDir );
		id = ExtractStringID( szDriveNDir );
		if ( id == IDC_PATHICON_OPEN )
			i = 0; // Must start building the path from the root
		else
		if ( id == IDC_PATHICON_ACTIVE )
			i = 9999; // Don't build any path - we're there
		else	i = wIndex; // OK to build a relative path
		szFileName[0] = '\0';
		for ( ; i<=wIndex; i++ )
			{
			SendDlgItemMessage( hDlg, wParam, LB_GETTEXT, i,
				(long)(LPSTR)szDriveNDir );
			id = ExtractStringID( szDriveNDir );
			if ( id == IDC_PATHICON_CLOSED && i != wIndex )
				continue;
			lstrcat( szFileName, SkipSpaces(szDriveNDir) );
			if ( id != IDC_PATHICON_ROOT )
				lstrcat( szFileName, "\\" );
			}
		lstrcat( szFileName, szFileSpec );
		SetFileName(hDlg, IDC_FNAME, szFileName, szSaveName, NO );
		bNavigated = YES;
		break;

	    case IDC_FLIST:
		wMsg = HIWORD(lParam);
		if ( wMsg == LBN_DBLCLK )
			{
			SendMessage(hDlg, WM_COMMAND, IDOK, 1L);
			break;
			}
		if ( wMsg != LBN_SELCHANGE )
			break;
		/* If a directory is selected... */
		if (DlgDirSelectEx(hDlg, szFileName, sizeof(szFileName), wParam))
			lstrcat(szFileName, szFileSpec);
		// 1-2-92 - TMR - always use SetFileName for all FNAME sets
// 		SetDlgItemText(hDlg, IDC_FNAME, szFileName);
		SetFileName(hDlg, IDC_FNAME, szFileName, szSaveName, NO );
		SendDlgItemMessage(hDlg, IDC_FNAME, EM_SETSEL,
			NULL, MAKELONG(0, 0x7fff));
//		SendDlgItemMessage(hDlg, IDC_FNAME, CB_SETEDITSEL,
//			NULL, MAKELONG(0, 0x7fff));
		break;

	    case IDC_FNAME:
		/* If the name is changed, disable OK if its length goes 0 */
		if (HIWORD(lParam) != EN_CHANGE)
//		if (HIWORD(lParam) != CBN_EDITCHANGE)
			break;
		ControlEnable( hDlg, IDOK,
			(BOOL)SendDlgItemMessage( hDlg, wParam,
			WM_GETTEXTLENGTH, 0, 0L));
		
		// 1-2-92 - TMR - make sure Edit Box has focus to make sure
		// that szSaveName only gets overwritten from user input
		// 1-3-92 - TMR - move this after ControlEnable
		if (GetFocus() != GetDlgItem(hDlg, IDC_FNAME))
			break;
		if (fSaving)
			{
			GetDlgItemText(hDlg, IDC_FNAME, szSaveName,
				MAX_FNAME_LEN);
			fUntitled = NO;
			}
		break;

	    case IDC_FPATH:
		wMsg = HIWORD(lParam);
		if ( wMsg == BN_DOUBLECLICKED )
			{
			SendMessage(hDlg, WM_COMMAND, IDOK, 1L);
			break;
			}
		if ( wMsg != BN_CLICKED )
			break;
 		GetDlgItemText(hDlg, wParam, szFileName, sizeof(szFileName));
		if ( !szFileName[0] )
			break;
		FixPath( szFileName );
		lstrcat( szFileName, szFileSpec );
		SetFileName(hDlg, IDC_FNAME, szFileName, szSaveName, NO );
		bNavigated = YES;
		break;

	    case IDOK:
		GetDlgItemText(hDlg, IDC_FNAME, szFileName,sizeof(szFileName));
		bModifySaveName = fSaving && StringsEqual(szFileName,
			szSaveName);
		/* Strip off the drive and directory to make */
		/* a DlgDirlist() call to switch over to them */
		/* Loop backwards over the file name */
		l = lstrlen(szFileName);
		while( --l >= 0 )
		   {
		   cChar = szFileName[l];
		   /* If we find a wildcard, the next DlgDirList() takes */
		   /* care of drive and directory switching; so get out */
		   if ( cChar == '?' || cChar == '*' )
			break;
		   /* If we find a drive or directory, handle it and get out */
		   if ( cChar == '\\' || cChar == ':' )
			{
			lstrcpy(szDriveNDir, szFileName);
			l++;
			szDriveNDir[l] = '\0';
			lstrcat(szDriveNDir, szFileSpec);
			// 1-3-92 - TMR - Handle directory change error
			if (DlgDirList(hDlg, szDriveNDir,
					 IDC_FLIST, IDC_FPATH, wFileAttr))
				lstrcpy( szFileName, &szFileName[l] );
			else
				{
				szDriveNDir[l] = '\0';
				Message(IDS_EDIRECTORYCHANGE,
					 Lowercase(szDriveNDir));
				szFileName[0] = '\0';
				}
			break;
			}
		   }

		// 1-3-92 - TMR add extension if none present
		/* Check to see if the file has an extension... */
		if ( !lstrchr( szFileName, '.' ) ) // if no extension...
		    if ( LookupExtension( idFileType, szExtension ) )
			{
			if (lstrlen(szFileName))
			    lstrcat( szFileName, extension(szExtension) );
			else
			    lstrcat( szFileName, szExtension);
			}
		if (bModifySaveName)
			lstrcpy(szSaveName, szFileName);

		/* Try to display a new list box */
		if ( !szFileName[0] )
			lstrcat(szFileName, szFileSpec);
		if (DlgDirList(hDlg, szFileName, IDC_FLIST, IDC_FPATH,
		    wFileAttr))
			{ /* A wildcard was found and a new list displayed */
			lstrcpy(szFileSpec, szFileName);
			SetFileName(hDlg, IDC_FNAME, szFileSpec, szSaveName,
				fSaving );
			LoadComboWithDrives( hDlg, IDC_DRIVES );
			LoadComboWithDirectories( hDlg, IDC_DIRS, YES );
			
			break;
			}

		// If there is still a path or wildcards in the name, the
		// file specification must be invalid
		if (lstrchr(szFileName, '\\') || lstrchr(szFileName, ':') ||
		    lstrchr(szFileName, '?') || lstrchr(szFileName, '*'))
			{
			lstrcpy(szString, szFileName);
			stripfile(szString);
			Message(IDS_EDIRECTORYCHANGE, Lowercase(szString));
			lstrcpy(szFileSpec, filename(szFileName));
			lstrcpy(szFileName, szFileSpec); // is this needed?
			SetFileName(hDlg, IDC_FNAME, szFileSpec, szSaveName,
				fSaving );
			break;
			}

		/* No wildcards, and the drive and dir have been changed */
		LoadComboWithDrives( hDlg, IDC_DRIVES );
		LoadComboWithDirectories( hDlg, IDC_DIRS, YES );

		/* Check to see if the file has 8 characters or less... */
		if ( fSaving )
			RemoveWhiteSpace( szFileName );
		FixFileName( szFileName );

		/* Check to see if the file has an extension... */
		if ( !lstrchr( szFileName, '.' ) ) // if no extension...
			if ( LookupExtension( idFileType, szExtension ) )
				lstrcat( szFileName, extension(szExtension) );

		// Build the fully qualified path name
		GetDlgItemText( hDlg, IDC_FPATH, szString, sizeof(szString) );
		FixPath( szString );
		lstrcat( szString, szFileName );

		/* Check to see if the file exists... */
		if ( (fp = _lopen( szString, OF_READ ) ) < 0 )
			{ /* The file does not exist */
			if ( !fSaving )
				{
				Message(IDS_EOPEN, Lowercase(szString));
				break;
				}
			}
		else	{
			_lclose( fp );
			if ( fSaving )
				{
				if ( !AstralAffirm( IDS_OVERWRITEIMAGE,
					Lowercase(szString) ) )
					break;
				}
			}

		lstrcpy( szFileSpec, szString );
		AstralDlgEnd(hDlg, TRUE|2);
		break;

	    case IDC_CANCEL:
	    case IDCANCEL:
		if ( bDisableCancel && !LOWORD(lParam) )
			break;
		GetDlgItemText(hDlg, IDC_FPATH, szFileSpec,sizeof(szFileSpec));
		AstralDlgEnd(hDlg, FALSE|2);
		break;

	    default:
		return( FALSE );
	    }
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}

/***********************************************************************/
LPSTR GetFileName(
/***********************************************************************/
HWND	hDlg,
LPSTR	lpFileName)
{
FNAME	szFileName;
STRING	szString;

GetDlgItemText( hDlg, IDC_FNAME, szFileName, sizeof(szFileName) );
FixFileName(szFileName);
GetDlgItemText( hDlg, IDC_FPATH, szString, sizeof(szString) );
FixPath( szString );
lstrcat( szString, szFileName );
Lowercase(szString);
lstrcpy(lpFileName, szString);
return(lpFileName);
}

/***********************************************************************/
void SetFileName(
/***********************************************************************/
HWND	hDlg,
int		idCtl,
LPSTR	lpFileSpec,
LPSTR	lpSaveName,
BOOL	bSaving)
{
FNAME szName, szSpec, szExt;

if (bSaving && !fUntitled)
	{
	lstrcpy( szName, stripdir(lpSaveName) );
	stripext( szName );
	lstrcpy( szExt, extension(lpFileSpec) );
	lstrcpy( szSpec, lpFileSpec );
	stripfile( szSpec );
	FixPath( szSpec );
	lstrcat( szSpec, szName );
	lstrcat( szSpec, szExt );
	SetDlgItemText( hDlg, idCtl, Lowercase(szSpec) );
	}
else	SetDlgItemText( hDlg, idCtl, Lowercase(lpFileSpec) );
}

/***********************************************************************/
LPSTR RemoveDots( LPSTR lpFileName )
/***********************************************************************/
{
BYTE	c;
LPSTR	lp, lpFile;

lp = lpFile = lpFileName;
while( c = *lpFile++ )
	{
	if ( c != '.' )
		*lp++ = c;
	}
*lp = '\0';
return( lpFileName );
}


/************************************************************************/
LPSTR DriveName(
/************************************************************************/
int 	iDrive,
LPSTR 	lpName)
{
#ifdef WIN32

#else
STRING szString;
struct find_t vol;
WORD	wType;

lpName[0] = 'a' + iDrive;
lpName[1] = ':';
lpName[2] = '\0';
lstrcpy( szString, lpName );
lstrcat( szString, "\\*.*" ); /* wildcard search of root dir */

lstrcat( lpName, " [" );
wType = GetExtendedDriveType(iDrive);
if ( iDrive != CURRENTDRIVE &&
	(wType == DRIVE_REMOVABLE || wType == DRIVE_CDROM) )
	lstrcat( lpName, "unknown" );
else
	{
	if ( !_dos_findfirst( szString, _A_VOLID, &vol ) )
		lstrcat( lpName, RemoveDots(vol.name) );
	else	lstrcat( lpName, "unlabeled" );
	}
lstrcat( lpName, "]" );
#endif

return( lpName );
}


/************************************************************************/
LPSTR DriveSize(
/************************************************************************/
int 	iDrive,
LPSTR 	lpSize)
{
#ifdef WIN32

#else
char szString[MAX_STR_LEN];
DWORD dwSize;
WORD wType;
struct diskfree_t DiskFree;

wType = GetExtendedDriveType(iDrive);
if ( iDrive != CURRENTDRIVE &&
	(wType == DRIVE_REMOVABLE || wType == DRIVE_CDROM) )
	*lpSize = '\0';
else	{
	if ( !_dos_getdiskfree( iDrive+1, &DiskFree ) )
		{
		dwSize = (DWORD)DiskFree.avail_clusters *
				DiskFree.sectors_per_cluster * 
				DiskFree.bytes_per_sector;
		SizeToAscii( dwSize, szString );
		lstrcpy( lpSize, szString );
		lstrcat( lpSize, " / " );
		dwSize = (DWORD)DiskFree.total_clusters *
				DiskFree.sectors_per_cluster * 
				DiskFree.bytes_per_sector;
		SizeToAscii( dwSize, szString );
		lstrcat( lpSize, szString );
		}
	}
	
#endif
return( lpSize );
}


/************************************************************************/
LPSTR DirectoryName(
/************************************************************************/
int 	iDirectory,
LPSTR 	lpName)
{
#ifdef WIN32

#else
int retc;
struct find_t dir;

retc = _dos_findfirst( "*.*", _A_SUBDIR, &dir );
while ( !retc )
	{
	if ( dir.attrib & _A_SUBDIR && dir.name[0] != '.' )
		{ // If this is a subdirectory
		if ( --iDirectory < 0 )
			{
			lstrcpy( lpName, dir.name );
			return( lpName );
			}
		}
	retc = _dos_findnext( &dir );
	}

#endif
return( NULL );
}


/************************************************************************/
void LoadComboWithDrives(
/************************************************************************/
HWND hWnd,
WORD idComboBox)
{
#ifdef WIN32

#else
int i, count, drive;
WORD wType;
char szString[MAX_STR_LEN];

drive = CURRENTDRIVE;
SetDlgItemText( hWnd, IDC_DISKSPACE, DriveSize(drive,szString) );
count = 0;
SendDlgItemMessage( hWnd, idComboBox, CB_RESETCONTENT, 0, 0L );
for ( i=0; i<=26; i++ )
	{
	if ( !(wType = GetDriveType( i )) )
		continue;
	DriveName( i, szString );
	CombineStringID( szString, IDC_PATHICON_DRIVE );
	SendDlgItemMessage( hWnd, idComboBox, CB_ADDSTRING, 0,
		(long)Lowercase(szString) );
	if ( i == drive )
		SendDlgItemMessage( hWnd, idComboBox, CB_SETCURSEL, count, 0L);
	count++;
	}
#endif  // WIN32
}


/************************************************************************/
void LoadComboWithDirectories(
/************************************************************************/
HWND hWnd,
WORD idComboBox,
BOOL bHide)
{
#ifdef WIN32

#else
HWND hControl;
int i, count;
STRING szString, szDirName;
LPSTR lpDir, lp;
int retc, iMaxSubDirs;
struct find_t dir;

hControl = GetDlgItem( hWnd, idComboBox );

if ( bHide )
//	ShowControl( hWnd, idComboBox, idComboBox, NO, YES /*Delay*/ );
	SetWindowPos( hControl, 0, /* Same z order */
		0, 0, /* Same location */
		0, 0, /* Same size */
		SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		SWP_NOREDRAW );

CURRENTDIR( szString, MAX_STR_LEN );
lpDir = &szString[3]; // Skip over the drive, colon, and slash
lp = lpDir;
iMaxSubDirs = 32;

SendDlgItemMessage( hWnd, idComboBox, LB_RESETCONTENT, 0, 0L );
AddSpaces( szDirName, iMaxSubDirs-- ); // Add spaces to defeat the sort
lstrcat( szDirName, "\\" );
CombineStringID( szDirName, IDC_PATHICON_ROOT );
SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0, (long)(LPSTR)szDirName );
count = 0;

while ( *lp )
	{
	if ( *lp != '\\' )
		{
		lp++;
		continue;
		}
	*lp++ = '\0'; // Overwrite the slash and skip past it
	AddSpaces( szDirName, iMaxSubDirs-- ); // Add spaces to defeat the sort
	lstrcat( szDirName, lpDir );
	CombineStringID( szDirName, IDC_PATHICON_OPEN );
	SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0,
		(long)Lowercase(szDirName) );
	count++;
	lpDir = lp;
	}

if ( lp > lpDir )
	{
	AddSpaces( szDirName, iMaxSubDirs-- ); // Add spaces to defeat the sort
	lstrcat( szDirName, lpDir );
	CombineStringID( szDirName, IDC_PATHICON_ACTIVE );
	SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0,
		(long)Lowercase(szDirName) );
	count++;
	}

retc = _dos_findfirst( "*.*", _A_SUBDIR, &dir );
for ( i=0; ; i++ )
	{
	if ( retc )
		break;
	if ( dir.attrib & _A_SUBDIR && dir.name[0] != '.' )
		{ // If this is a subdirectory
		lstrcpy( szDirName, dir.name );
		CombineStringID( szDirName, IDC_PATHICON_CLOSED );
		SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0,
			(long)Lowercase(szDirName) );
		}
	retc = _dos_findnext( &dir );
	}

//SendDlgItemMessage( hWnd, idComboBox, LB_SETCURSEL, count, 0L);
SendDlgItemMessage( hWnd, idComboBox, LB_SETCURSEL, -1, 0L);

if ( bHide )
	{
//	ShowControl( hWnd, idComboBox, idComboBox, YES, NO /*Delay*/ );
	SetWindowPos( hControl, 0, /* Same z order */
		0, 0, /* Same location */
		0, 0, /* Same size */
		SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER );
//		SWP_NOREDRAW );
	}
#endif  // WIN32
}


/************************************************************************/
BOOL LookupExtension(
/************************************************************************/
WORD idFileType,
LPSTR lpExtension)
{
LPSTR lpFileType;

if ( !AstralStr( idFileType, &lpFileType ) )
	lstrcpy( lpExtension, "*.*" );
else	lstrcpy( lpExtension, lpFileType );
return( YES );
}


/************************************************************************/
void LoadFileLocations()
/************************************************************************/
{
FNAME szFileName, szExtension;
int i;
static int items[] =
{ IDN_ART, IDN_TIFF, IDN_BMP,
  IDN_BGMAP, IDN_CALMAP, IDN_TEXTURE, IDN_STYLE,
  IDN_CLIPBOARD, IDN_MASK, IDN_CUSTOM, IDN_PALETTE };

for ( i=0; i<sizeof(items)/sizeof(int); i++ )
	{
	if ( !LookupExtension( items[i], szExtension ) )
		continue;
	GetDefaultString( szExtension, "", szFileName, sizeof(FNAME) );
	lstrcat( szExtension, "temp" );
	PutDefaultString( szExtension, szFileName );
	}
}


/************************************************************************/
BOOL GetFileLocation(
/************************************************************************/
WORD 	idFileType,
LPSTR 	lpPath)
{
FNAME szExtension, szFileName, szTemp;

if ( !LookupExtension( idFileType, szExtension ) )
	return( NO );

/* Find out where the user was last, so we can change over to that directory */
lstrcpy( szTemp, szExtension );
lstrcat( szTemp, "temp" );
GetDefaultString( szTemp, szExtension, szFileName, sizeof(FNAME) );
stripfile( szFileName );
FixPath( szFileName );
lstrcat( szFileName, szExtension );
lstrcpy( lpPath, szFileName );
return( YES );
}


/************************************************************************/
BOOL SetFileLocation(
/************************************************************************/
WORD 	idFileType,
LPSTR 	lpPath)
{
FNAME szFileName, szExtension;

if ( !LookupExtension( idFileType, szExtension ) )
	return( NO );

/* Whatever disk and directory the user changed to, */
/* store it away so we come back to the same place next time */
lstrcpy( szFileName, lpPath );
stripfile( szFileName );
FixPath( szFileName );
lstrcat( szFileName, szExtension );
if ( Save.OKtoSavePath )
	PutDefaultString( szExtension, szFileName );
lstrcat( szExtension, "temp" );
PutDefaultString( szExtension, szFileName );
return( TRUE );
}


/************************************************************************/
LPSTR SizeToAscii(
/************************************************************************/
DWORD 	dwSize,
LPSTR 	lpString)
{
int l;
char c, num[32];
DWORD dwDivisor;
LPSTR lpLabel;
#define GIG (1L<<30)
#define MEG (1L<<20)
#define KB (1L<<10)

if ( dwSize < KB )
	{
	itoa( (int)dwSize, num, 10 );
	lstrcpy( lpString, num );
	if ( AstralStr( IDS_BYTES, &lpLabel ) )
		lstrcat( lpString, lpLabel );
	return( lpString );
	}

if ( dwSize >= GIG )
	{
	dwDivisor = GIG;
	if ( !AstralStr( IDS_GIGABYTES, &lpLabel ) )
		lpLabel = "";
	}
else
if ( dwSize >= MEG )
	{
	dwDivisor = MEG;
	if ( !AstralStr( IDS_MEGABYTES, &lpLabel ) )
		lpLabel = "";
	}
else
//if ( dwSize >= KB )
	{
	dwDivisor = KB;
	if ( !AstralStr( IDS_KILOBYTES, &lpLabel ) )
		lpLabel = "";
	}

dwSize += (dwDivisor/20);
dwSize /= (dwDivisor/10);
itoa( (int)dwSize, num, 10 );
l = lstrlen( num );
c = num[l-1]; // get the decimal place value
if ( l > 3 || c == '0' ) // has 4 significant digits or no decimal value
	num[l-1] = '\0'; // Don't do any decimal places
else	{
	num[l-1] = '.';
	num[l] = c;
	num[l+1] = '\0';
	}
lstrcpy( lpString, num );
lstrcat( lpString, lpLabel );
return( lpString );
}


/************************************************************************/
static void AddSpaces(
/************************************************************************/
LPSTR 	lpString,
int 	iCount)
{
while ( --iCount >= 0 )
	*lpString++ = ' ';
*lpString = '\0';
}

/***********************************************************************/
LPSTR GetFilePath(
/***********************************************************************/
HWND	hDlg,
WORD	idCtl,
LPSTR	lpPath,
LPSTR	lpCurrentDir)
{
// get file name specification from edit box
GetDlgItemText(hDlg, idCtl, lpPath, MAX_FNAME_LEN);
return(lpPath);
}

/************************************************************************/
BOOL CopyFile1(
/************************************************************************/
LPSTR 	lpInFile,
LPSTR	lpOutFile,
BOOL 	bClock,
BOOL	bEscapable)
{
int i, ifp, ofp;
char Buffer[2048];
int	nTotal, nCount;

nTotal = FileSize(lpInFile) / 2048;
lstrcpy( Buffer, lpInFile );
if ((ifp = _lopen( Buffer, OF_READ )) < 0)
	return( FALSE );
lstrcpy( Buffer, lpOutFile );
if ((ofp = _lcreat( Buffer, 0 )) < 0)
	{
	_lclose( ifp );
	return( FALSE );
	}

i = sizeof(Buffer);
nCount = 0;
while( 1 )
	{
	if (bClock)
		{
		if (AstralClockCursor(nCount, nTotal, bEscapable))
			{
			_lclose( ifp );
			_lclose( ofp );
			lstrcpy( Buffer, lpOutFile );
			unlink(Buffer);
			return( FALSE );
			}
		}
	if ( (i = _lread(ifp, Buffer, i)) <= 0 )
		{
		_lclose( ifp );
		_lclose( ofp );
		return( i == 0 );
		}
	if ( _lwrite(ofp, Buffer, i) != i )
		{
		_lclose( ifp );
		_lclose( ofp );
		lstrcpy( Buffer, lpOutFile );
		unlink(Buffer);
		return( FALSE );
		}
	++nCount;
	}
}

/*[2*************************************************************************
 *
 *        Name:  PathUp                       (Supports double-byte chars.)
 *
 *     Purpose:  Removes the last directory or filename from a path name.
 *
 *  Parameters:  lpDest   - resulting truncated path
 *               lpSource - original path name
 *
 *       Notes:  lpDest may equal lpSource
 *
 *    Examples:  PathUp (A,"here\\there\\yonder")   =>  A="here\\there"
 *
 *               PathUp (A,A)                       =>  A="here"
 *
 *************************************************************************{]*/

// ATTENTION:   This function was added here because the same function
//              in MGXUTILS is broken!

void FAR PASCAL PathUp (LPSTR lpDest,LPCSTR lpSource)
{
    if (lpDest)
        if (!lpSource || !(*lpSource))
        {
            *lpDest = 0;
        }
        else
        {
            LPCSTR lpLastSlash = lpSource;
            LPCSTR c           = lpSource+lstrlen (lpSource);

            /************************************************************
             *  Always truncate at least one character from end.
             *  This guarantees that a directory will be removed if
             *  the last character of the path name is a "\" or ":".
             ************************************************************/
            c = AnsiPrev (lpSource,c);
            while (c > lpSource)
            {
                c = AnsiPrev (lpSource,c);
                if ((*c=='\\') || (*c==':'))
                {
                    lpLastSlash = c;
                    break;
                }
            }

            /************************************************************
             *  Truncate the path.
             ************************************************************/
            if (lpDest != lpSource)
                _fstrncpy (lpDest,lpSource,lpLastSlash-lpSource);
            lpDest[lpLastSlash-lpSource] = 0; // cuz lpLastSlash is const
        }
}
