// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HANDLE hInstAstral;

static FNAME szFileName;
static FNAME szFileSpec;
static WORD idFileType;
static WORD wFileAttr;
static BOOL fSaving;

#define CHDRIVE(drive) (_chdrive((drive)+1) )
#define CHDIR(dir) ( chdir((PTR)(dir)) )
#define CURRENTDRIVE (_getdrive()-1)
#define CURRENTDIR(dir,len) ( getcwd((PTR)dir,len) )
#define UP_ARROW 171
#define DOWN_ARROW 187

/************************************************************************/
int DoOpenDlg( hDlg, idDlg, idType, szFileNameOut, fSavingIn)
/************************************************************************/
HWND hDlg;
int idDlg;
WORD idType;
PTR szFileNameOut;
BOOL fSavingIn;
{
int iReturn;
STRING tmp;

if ( !(idFileType = idType) )
	{ // use the current image type
	if ( idDlg == IDD_SAVEAS && lpImage )
		idFileType = lpImage->FileType;
	else
	if ( idDlg == IDD_SAVEAS || idDlg == IDD_OPEN )
		idFileType = Control.ImageType;
	else	idFileType = idDlg;
	}

GetFileLocation( idFileType, szFileSpec );

/* Initialize some values used in DlgOpenProc */
wFileAttr = 0;
fSaving = fSavingIn;

if ( iReturn = AstralDlg( NO, hInstAstral, hDlg, idDlg, DlgOpenProc) )
	{
	if ( szFileSpec[ lstrlen(szFileSpec)-1 ] != '\\' )
		lstrcat( szFileSpec, "\\" );
	lstrcpy( szFileNameOut, szFileSpec );
	lstrcat( szFileNameOut, szFileName );
	}
else	szFileNameOut[0] = '\0';

SetFileLocation( idFileType, szFileSpec );
if ( idDlg == IDD_OPEN )
	{
	Control.ImageType = idFileType;
	PutDefaultString( "Control.ImageType",
		itoa( Control.ImageType - IDC_SAVETIFF, tmp, 10 ) );
	}

return( iReturn );
}


/************************************************************************/
BOOL FAR PASCAL DlgOpenProc( hDlg, iMessage, wParam, lParam )
/************************************************************************/
HWND hDlg;
unsigned iMessage;
WORD wParam;
LONG lParam;
{
WORD wMsg, wIndex;
int i, l, fp;
OFSTRUCT of;
FNAME szDriveNDir;
char cChar;

switch(iMessage)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	szFileName[0] = '\0';
	if ( !DlgDirList(hDlg, szFileSpec, IDC_FLIST, IDC_FPATH, wFileAttr) )
		{
		lstrcpy( szFileSpec, strchr( szFileSpec, '*' ) );
		DlgDirList(hDlg, szFileSpec, IDC_FLIST, IDC_FPATH, wFileAttr);
		}
	CheckComboItem( hDlg, IDC_FILETYPES, IDC_SAVETIFF, IDC_SAVEPCX,
		idFileType );
	SendDlgItemMessage(hDlg, IDC_FNAME, EM_LIMITTEXT, MAX_FNAME_LEN-1, 0L);
//	CheckComboItem( hDlg, IDC_FNAME, IDC_SAVETIFF, IDC_SAVEPCX,
//		idFileType );
//	SendDlgItemMessage(hDlg, IDC_FNAME, CB_LIMITTEXT, MAX_FNAME_LEN-1, 0L);
	SetDlgItemText(hDlg, IDC_FNAME, szFileSpec);
	LoadComboWithDrives( hDlg, IDC_DRIVES );
	LoadComboWithDirectories( hDlg, IDC_DIRS, NO );
	CheckDlgButton( hDlg, IDC_SAVECOMPRESS, Save.Compressed );
	if ( lpImage )
		{
		ControlEnable( hDlg, IDC_SAVECT, YES );
		ControlEnable( hDlg, IDC_SAVELA, YES );
		ControlEnable( hDlg, IDC_SAVESP, YES );
		ControlEnable( hDlg, IDC_SAVE8BITCOLOR, YES );
		ControlEnable( hDlg, IDC_SAVE24BITCOLOR, YES );
		ControlEnable( hDlg, IDC_SAVECOMPRESS,
			idFileType == IDC_SAVETIFF );

		CheckComboItem( hDlg, IDC_DATATYPES, IDC_SAVECT,
			IDC_SAVE24BITCOLOR, lpImage->DataType );
		}
	SetFocus( GetDlgItem( hDlg, IDC_FNAME ) );
	SendDlgItemMessage(hDlg, IDC_FNAME, EM_SETSEL,
		NULL, MAKELONG(0, 0x7fff));
//	SendDlgItemMessage(hDlg, IDC_FNAME, CB_SETEDITSEL,
//		NULL, MAKELONG(0, 0x7fff));
	return( FALSE );

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch(wParam)
	    {
	    case IDC_DRIVES:
		wMsg = HIWORD(lParam);
//		if ( wMsg != CBN_SELCHANGE && wMsg != CBN_DBLCLK )
		if ( wMsg != CBN_SELCHANGE )
			break;
		wIndex = SendDlgItemMessage( hDlg, wParam, CB_GETCURSEL, 0, 0L);
		SendDlgItemMessage( hDlg, wParam, CB_GETLBTEXT, wIndex,
			(LONG)(LPTR)szDriveNDir );
		CHDRIVE( *szDriveNDir - 'a' );
		lstrcpy( szFileName, szFileSpec );
		SetDlgItemText( hDlg, IDC_FNAME, szFileSpec );
		DlgDirList( hDlg, szFileSpec, IDC_FLIST, IDC_FPATH, wFileAttr );
		LoadComboWithDrives( hDlg, IDC_DRIVES );
		LoadComboWithDirectories( hDlg, IDC_DIRS, YES );
		break;

	    case IDC_DIRS:
		wMsg = HIWORD(lParam);
//		if ( wMsg != LBN_SELCHANGE && wMsg != LBN_DBLCLK )
		if ( wMsg != LBN_SELCHANGE )
			break;
		wIndex = SendDlgItemMessage( hDlg, wParam, LB_GETCURSEL, 0, 0L);
		for ( i=0; i<=wIndex; )
			{
			SendDlgItemMessage( hDlg, wParam, LB_GETTEXT, i,
				(LONG)(LPTR)szDriveNDir );
			CHDIR( &szDriveNDir[1] ); // Skip over the arrow
			if ( szDriveNDir[0] == ' ' && i != wIndex )
				i = wIndex;
			else	i++;
			}
		lstrcpy( szFileName, szFileSpec );
		SetDlgItemText( hDlg, IDC_FNAME, szFileSpec );
		DlgDirList( hDlg, szFileSpec, IDC_FLIST, IDC_FPATH, wFileAttr );
		LoadComboWithDirectories( hDlg, IDC_DIRS, YES );
//		SendDlgItemMessage( hDlg, wParam, CB_SHOWDROPDOWN, 1, 0L );
		break;

	    case IDC_FLIST:
		wMsg = HIWORD(lParam);
		if ( wMsg == LBN_DBLCLK )
			{
			SendMessage(hDlg, WM_COMMAND, IDOK, 0L);
			break;
			}
		if ( wMsg != LBN_SELCHANGE )
			break;
		/* If a directory is selected... */
		if (DlgDirSelect(hDlg, szFileName, wParam))
			lstrcat(szFileName, szFileSpec);
		SetDlgItemText(hDlg, IDC_FNAME, szFileName);
		SendDlgItemMessage(hDlg, IDC_FNAME, EM_SETSEL,
			NULL, MAKELONG(0, 0x7fff));
//		SendDlgItemMessage(hDlg, IDC_FNAME, CB_SETEDITSEL,
//			NULL, MAKELONG(0, 0x7fff));
		break;

	    case IDC_FNAME:
		/* If the name is changed, disable OK if its length goes 0 */
		if (HIWORD(lParam) == EN_CHANGE)
//		if (HIWORD(lParam) == CBN_EDITCHANGE)
			{
			ControlEnable( hDlg, IDOK,
				(BOOL)SendDlgItemMessage( hDlg, wParam,
				WM_GETTEXTLENGTH, 0, 0L));
			break;
			}
		break;

	    case IDC_FILETYPES:
//	    case IDC_SAVETIFF:
//	    case IDC_SAVEEPS:
//	    case IDC_SAVETARGA:
//	    case IDC_SAVEGIF:
//	    case IDC_SAVEBMP:
//	    case IDC_SAVEPCX:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		idFileType = wParam;
		if ( !LookupExtension( idFileType, szFileSpec ) )
			break;
		SetDlgItemText( hDlg, IDC_FNAME, szFileSpec );
		SendMessage( hDlg, WM_COMMAND, IDOK, 0L );
		if ( lpImage )
			{
			lpImage->FileType = idFileType;
			ControlEnable( hDlg, IDC_SAVECOMPRESS,
				idFileType == IDC_SAVETIFF );
			}
		break;

	    case IDC_DATATYPES:
//	    case IDC_SAVECT:
//	    case IDC_SAVELA:
//	    case IDC_SAVESP: // scatterprint
//	    case IDC_SAVE8BITCOLOR:
//	    case IDC_SAVE24BITCOLOR:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		if ( lpImage )
			lpImage->DataType = wParam;
		break;

	    case IDC_SAVECOMPRESS:
		Save.Compressed = !Save.Compressed;
		CheckDlgButton( hDlg, IDC_SAVECOMPRESS, Save.Compressed );
		break;

	    case IDC_FPATH:
		szFileName[0] = '\0';
		GetDlgItemText(hDlg, wParam, szFileName,sizeof(szFileName));
		goto ChangeDir;

	    case IDOK:
		GetDlgItemText(hDlg, IDC_FNAME, szFileName,sizeof(szFileName));
	    ChangeDir:
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
			DlgDirList(hDlg, szDriveNDir, IDC_FLIST, IDC_FPATH,
			    wFileAttr);
			lstrcpy( szFileName, &szFileName[l] );
			break;
			}
		   }

		/* Try to display a new list box */
		if ( !szFileName[0] )
			lstrcat(szFileName, szFileSpec);
		if (DlgDirList(hDlg, szFileName, IDC_FLIST, IDC_FPATH,
		    wFileAttr))
			{ /* A wildcard was found and a new list displayed */
			lstrcpy(szFileSpec, szFileName);
			SetDlgItemText(hDlg, IDC_FNAME, szFileSpec);
			LoadComboWithDrives( hDlg, IDC_DRIVES );
			LoadComboWithDirectories( hDlg, IDC_DIRS, YES );
			break;
			}

		LoadComboWithDrives( hDlg, IDC_DRIVES );
		LoadComboWithDirectories( hDlg, IDC_DIRS, YES );

		/* No wildcards, and the drive and dir have been changed */
		/* Check to see if the file exists... */
		of.szPathName[0] = '\0';
		if ( (fp = OpenFile(szFileName, &of, OF_READ)) < 0 )
			{ /* The file does not exist */
			if ( !fSaving )
				{
				MessageBeep(0);
				break;
				}
			}
		else	_lclose( fp );

		lstrcpy(szFileName,
			AnsiNext(strrchr(of.szPathName, '\\')));
		OemToAnsi(szFileName, szFileName);
		GetDlgItemText(hDlg, IDC_FPATH, szFileSpec,sizeof(szFileSpec));
		AstralDlgEnd(hDlg, TRUE);
		break;

	    case IDCANCEL:
		GetDlgItemText(hDlg, IDC_FPATH, szFileSpec,sizeof(szFileSpec));
		AstralDlgEnd(hDlg, FALSE);
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
LPTR RemoveDots( lpFileName )
/***********************************************************************/
LPTR	lpFileName;
{
BYTE	c;
LPTR	lp, lpFile;

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
LPTR DriveName( iDrive, lpName )
/************************************************************************/
int iDrive;
LPTR lpName;
{
BYTE szString[MAX_STR_LEN];
struct find_t vol;

lpName[0] = 'a' + iDrive;
lpName[1] = ':';
lpName[2] = '\0';
lstrcpy( szString, lpName );
lstrcat( szString, "\\*.*" ); /* wildcard search of root dir */

lstrcat( lpName, "  [" );
if ( GetDriveType(iDrive) == DRIVE_REMOVABLE && iDrive != CURRENTDRIVE )
	lstrcat( lpName, "unknown?" );
else	{
	if ( !_dos_findfirst( szString, _A_VOLID, &vol ) )
		lstrcat( lpName, RemoveDots(vol.name) );
	else	lstrcat( lpName, "unlabeled" );
	}
lstrcat( lpName, "]" );

return( lpName );
}


/************************************************************************/
LPTR DriveSize( iDrive, lpSize )
/************************************************************************/
int iDrive;
LPTR lpSize;
{
BYTE szString[MAX_STR_LEN];
DWORD lSize;
struct diskfree_t DiskFree;

if ( GetDriveType(iDrive) == DRIVE_REMOVABLE && iDrive != CURRENTDRIVE )
	*lpSize = '\0';
else	{
	if ( !_dos_getdiskfree( iDrive+1, &DiskFree ) )
		{
		lSize = (DWORD)DiskFree.avail_clusters *
				DiskFree.sectors_per_cluster * 
				DiskFree.bytes_per_sector;
		SizeToAscii( lSize, szString );
		lstrcpy( lpSize, szString );
		lstrcat( lpSize, " / " );
		lSize = (DWORD)DiskFree.total_clusters *
				DiskFree.sectors_per_cluster * 
				DiskFree.bytes_per_sector;
		SizeToAscii( lSize, szString );
		lstrcat( lpSize, szString );
		}
	}

return( lpSize );
}


/************************************************************************/
LPTR DirectoryName( iDirectory, lpName )
/************************************************************************/
int iDirectory;
LPTR lpName;
{
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

return( NULL );
}


/************************************************************************/
void LoadComboWithDrives( hWnd, idComboBox )
/************************************************************************/
HWND hWnd;
WORD idComboBox;
{
int i, count, drive;
WORD wType;
BYTE szString[MAX_STR_LEN];

drive = CURRENTDRIVE;
SetDlgItemText( hWnd, IDC_DISKSPACE, DriveSize(drive,szString) );
count = 0;
SendDlgItemMessage( hWnd, idComboBox, CB_RESETCONTENT, 0, 0L );
for ( i=0; i<=26; i++ )
	{
	wType = GetDriveType( i );
	if ( !wType || wType == 1 )
		continue;
	SendDlgItemMessage( hWnd, idComboBox, CB_ADDSTRING, 0,
		(LONG)Lowercase(DriveName(i,szString)) );
	if ( i == drive )
		SendDlgItemMessage( hWnd, idComboBox, CB_SETCURSEL, count, 0L);
	count++;
	}
}


/************************************************************************/
void LoadComboWithDirectories( hWnd, idComboBox, bHide )
/************************************************************************/
HWND hWnd;
WORD idComboBox;
{
int i, count;
BYTE szString[MAX_STR_LEN], szRoot[4];
LPTR lpDir, lp;
int retc;
struct find_t dir;

if ( bHide )
	ShowControl( hWnd, idComboBox, idComboBox, NO, NO /*Delay*/ );

CURRENTDIR( szString, MAX_STR_LEN );
lpDir = &szString[2]; // Skip over the drive, colon and point to the slash
*lpDir = UP_ARROW; // Overwrite the slash
lp = lpDir + 1;

SendDlgItemMessage( hWnd, idComboBox, LB_RESETCONTENT, 0, 0L );
szRoot[0] = ( *lp ? UP_ARROW : ' ' );
szRoot[1] = '\\';
szRoot[2] = '\0';
SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0, (LONG)(LPTR)szRoot );
count = 0;

while ( *lp )
	{
	if ( *lp == '\\' )
		{
		*lp = '\0'; // Overwrite the slash
		SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0,
			(LONG)Lowercase(lpDir) );
		count++;
		lpDir = lp;
		*lpDir = UP_ARROW; // Overwrite the slash
		}
	lp++;
	}

if ( lp > lpDir + 1 )
	{
	*lpDir = ' ';
	SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0,
		(LONG)Lowercase(lpDir) );
	count++;
	}

SendDlgItemMessage( hWnd, idComboBox, LB_SETCURSEL, count, 0L);

szString[0] = DOWN_ARROW;
retc = _dos_findfirst( "*.*", _A_SUBDIR, &dir );
for ( i=0; ; i++ )
	{
	if ( retc )
		break;
	if ( dir.attrib & _A_SUBDIR && dir.name[0] != '.' )
		{ // If this is a subdirectory
		lstrcpy( &szString[1], dir.name );
		SendDlgItemMessage( hWnd, idComboBox, LB_ADDSTRING, 0,
			(LONG)Lowercase(szString) );
		}
	retc = _dos_findnext( &dir );
	}

if ( bHide )
	ShowControl( hWnd, idComboBox, idComboBox, YES, NO /*Delay*/ );
}


/************************************************************************/
BOOL LookupExtension( idFileType, lpExtension )
/************************************************************************/
WORD idFileType;
LPTR lpExtension;
{
LPTR lpFileType;

if ( !AstralStr( idFileType, &lpFileType ) )
	lstrcpy( lpExtension, "*.*" );
else	lstrcpy( lpExtension, lpFileType );
return( YES );
}


/************************************************************************/
BOOL GetFileLocation( idFileType, lpPath )
/************************************************************************/
WORD idFileType;
LPTR lpPath;
{
FNAME szExtension;

if ( !LookupExtension( idFileType, szExtension ) )
	return( NO );

/* Find out where the user was last, so we can change over to that directory */
GetProfileString( ID_APPL, szExtension, szExtension, lpPath, sizeof(FNAME) );

#ifdef UNUSED
if ( DemoRunning() )
	{
	GetProfileString( ID_APPL, "*.SHO", "*.SHO", lpPath, sizeof(FNAME) );
	p1 = lstrchr( lpPath, '.' );
	p2 = lstrchr( szExtension, '.' );
	if ( p1 && p2 )
		lstrcpy( p1, p2 );
	}
#endif
return( YES );
}


/************************************************************************/
BOOL SetFileLocation( idFileType, lpPath )
/************************************************************************/
WORD idFileType;
LPTR lpPath;
{
FNAME szExtension;

#ifdef UNUSED
if ( DemoRunning() )
	return( YES );
#endif

if ( !LookupExtension( idFileType, szExtension ) )
	return( NO );

/* Whatever disk and directory the user changed to, */
/* store it away so we come back to the same place next time */
if ( lpPath[ lstrlen(lpPath)-1 ] != '\\' )
	lstrcat( lpPath, "\\" );
lstrcat( lpPath, szExtension );
WriteProfileString( ID_APPL, szExtension, lpPath );
}


/************************************************************************/
LPTR SizeToAscii( lSize, lpString )
/************************************************************************/
DWORD lSize;
LPTR lpString;
{
int l;
BYTE c, num[32];

if ( lSize > 1000000L )
	{ // Megabytes
	lSize +=  50000L;
	lSize /= 100000L;
	itoa( (int)lSize, num, 10 );
	l = lstrlen( num );
	c = num[l-1]; // get the decimal place value
	if ( l > 3 || c == '0' ) // has 4 significant digits or no decimal value
		num[l-1] = '\0'; // Don't do any decimal places
	else	{
		num[l-1] = '.';
		num[l] = c;
		num[l+1] = '\0';
		}
	lstrcat( num, "MB" );
	}
else
if ( lSize > 1000L )
	{ // Kilobytes
	lSize +=  50L;
	lSize /= 100L;
	itoa( (int)lSize, num, 10 );
	l = lstrlen( num );
	c = num[l-1]; // get the decimal place value
	if ( l > 3 || c == '0' ) // has 4 significant digits or no decimal value
		num[l-1] = '\0'; // Don't do any decimal places
	else	{
		num[l-1] = '.';
		num[l] = c;
		num[l+1] = '\0';
		}
	lstrcat( num, "KB" );
	}
else	{
	itoa( (int)lSize, num, 10 );
	}

lstrcpy( lpString, num );
return( lpString );
}
