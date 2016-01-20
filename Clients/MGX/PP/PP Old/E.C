// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/************************************************************************/
/* extname.c    Extended name handling routines				*/
/************************************************************************/

#define EXT_NAME "POUCH.TXT"
#define GetPouchPath GetExtNamePath
#define GetFullPathName GetExtFilePath
#define GetExtFile CreateFileName

extern HANDLE hInstAstral;
extern HWND hWndAstral;

static FILE *fp;
static FNAME szAddExtName;
static WORD wAddType;

/***********************************************************************/
void InitExtName( hWnd, idControl, idStatic, lpExtName, wType )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
{
int i;
LPTR lp;
FNAME szExtName, szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return;
bCombo = IsCombo( hControl );
SendMessage( hControl, (bCombo ? CB_RESETCONTENT : LB_RESETCONTENT), 0, 0L );
if ( bCombo )
	ListExtFiles( wType, AddToComboBox, (long)hControl );
else	{
	if ( AstralStr( IDS_UNTITLED, &lp ) )
		{
		szFileName[0] = '-';
		lstrcpy( &szFileName[1], lp );
		SendMessage( hControl, LB_ADDSTRING, 0, (long)(LPTR)szFileName);
		}
	ListExtFiles( wType, AddToListBox, (long)hControl );
	}

i = SendMessage( hControl, (bCombo ? CB_GETCOUNT : LB_GETCOUNT), 0, 0L);
while ( --i >= 0 )
	{
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
		i, (long)(LPTR)szExtName);
	if ( !i || StringsEqual(lpExtName, szExtName) )
		{
		SendMessage( hControl, (bCombo ? CB_SETCURSEL : LB_SETCURSEL),
			i, 0L);
		break;
		}
	}

SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
	i, (long)lpExtName );
if ( !idStatic )
	return;
if ( LookupExtFile( lpExtName, szFileName, wType ) )
	SetDlgItemText( hWnd, idStatic, Lowercase(stripdir(szFileName)) );
else	SetDlgItemText( hWnd, idStatic, "" );
}


/***********************************************************************/
void GetExtName( hWnd, idControl, idStatic, lpExtName, wType, wMsg )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
WORD wMsg;
{
int item;
FNAME szFileName;
HWND hControl;
BOOL bCombo;

if ( !(hControl = GetDlgItem(hWnd, idControl)) )
	return;

bCombo = IsCombo( hControl );
if ( bCombo && wMsg == CBN_EDITCHANGE )
	GetDlgItemText( hWnd, idControl, lpExtName, MAX_FNAME_LEN );
else
if ( (bCombo && wMsg == CBN_SELCHANGE) || (!bCombo && wMsg == LBN_SELCHANGE) )
	{
	*lpExtName = '\0';
	item = SendMessage( hControl, (bCombo ? CB_GETCURSEL : LB_GETCURSEL),
		0, 0L );
	SendMessage( hControl, (bCombo ? CB_GETLBTEXT : LB_GETTEXT),
		item, (long)lpExtName);
	if ( !idStatic )
		return;
	if ( LookupExtFile( lpExtName, szFileName, wType ) )
	      SetDlgItemText( hWnd, idStatic, Lowercase(stripdir(szFileName)) );
	else  SetDlgItemText( hWnd, idStatic, "" );
	}
}


/***********************************************************************/
void ExtNameManager( hWnd, idControl, idStatic, lpExtName, wType, idFunction, fLinkedFiles )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic, idFunction;
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
switch ( idFunction )
    {
    case IDC_ADDEXT:
	AddExtName( hWnd, idControl, idStatic, lpExtName, wType );
	break;
    case IDC_DELETEEXT:
	DeleteExtName( hWnd, idControl, idStatic, lpExtName, wType,
		fLinkedFiles);
	break;
    case IDC_RENAMEEXT:
	RenameExtName( hWnd, idControl, idStatic, lpExtName, wType,
		fLinkedFiles);
	break;
    default:
	break;
    }
}


/***********************************************************************/
static void AddExtName( hWnd, idControl, idStatic, lpExtName, wType )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
{
FNAME szFileName;
WORD idDlg;

if ( wType == IDN_TEXTURE || wType == IDN_CLIPBOARD )
	idDlg = IDD_OPEN;
else	idDlg = IDD_EXTOPEN;
if ( !DoOpenDlg( hWnd, idDlg, wType, szFileName, NO ) )
	return;
wAddType = wType;
lstrcpy( szAddExtName, Lowercase(stripdir(szFileName)) );
stripext( szAddExtName );
if ( islower(*szAddExtName) )
	*szAddExtName -= ('a'-'A');
if ( !AstralDlg( NO, hInstAstral, hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return;
if ( !AddExtFile( szAddExtName, szFileName, wType ) )
	return;
lstrcpy( lpExtName, szAddExtName ); // pass back the name
InitExtName( hWnd, idControl, idStatic, lpExtName, wType );
}


/***********************************************************************/
static void RenameExtName( hWnd, idControl, idStatic, lpExtName, wType, fLinkedFiles )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
FNAME szFileName;

if ( !LookupExtFile( lpExtName, szFileName, wType ) )
	return;
wAddType = wType;
lstrcpy( szAddExtName, lpExtName );
if ( !AstralDlg( NO, hInstAstral, hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return;
if ( !RenameExtFile( lpExtName, szAddExtName, wType, fLinkedFiles) )
	return;
lstrcpy( lpExtName, szAddExtName ); // pass back the name
InitExtName( hWnd, idControl, idStatic, lpExtName, wType );
}


/***********************************************************************/
static void DeleteExtName( hWnd, idControl, idStatic, lpExtName, wType, fLinkedFiles )
/***********************************************************************/
HWND hWnd;
WORD idControl, idStatic;
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
int retc;
FNAME szFileName;

if ( !LookupExtFile( lpExtName, szFileName, wType ) )
	return;
retc = AstralOKCancel( IDS_DELETEEXTFILE, lpExtName, Lowercase(szFileName));
if ( retc != IDOK )
	return;
DeleteExtFile( lpExtName, wType, fLinkedFiles );
InitExtName( hWnd, idControl, idStatic, lpExtName, wType );
}


/***********************************************************************/
BOOL PromptForExtName(hWnd, lpExtName, lpFileName, wType )
/***********************************************************************/
HWND hWnd;
LPTR lpExtName, lpFileName;
WORD wType;
{
if ( *lpExtName )
	lstrcpy( szAddExtName, lpExtName );
else	lstrcpy( szAddExtName, "" );
wAddType = wType;
if ( !AstralDlg( NO, hInstAstral, hWnd, IDD_ADDEXTNAME, DlgAddExtNameProc ) )
	return(FALSE);
lstrcpy( lpExtName, szAddExtName );
if ( !LookupExtFile( lpExtName, lpFileName, wType ) )
	if ( !CreateFileName( lpExtName, NULL, wType, lpFileName ) )
		return( FALSE );
return(TRUE);
}


/***********************************************************************/
BOOL FAR PASCAL DlgAddExtNameProc( hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
BOOL Bool;
LPTR lpTitle;
FNAME szFileName;

switch (msg)
    { // This proc requires that wAddType and szAddExtName be initialized
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	if ( AstralStr( wAddType + MAX_FILETYPES, &lpTitle ) )
		SetWindowText( hDlg, lpTitle );
	szFileName[0] = '\0';
	InitExtName( hDlg, IDC_EXTNAMES, NULL, szFileName, wAddType);
	SetDlgItemText( hDlg, IDC_EXTNAMES, szAddExtName );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, NULL, szAddExtName, wAddType,
			HIWORD(lParam) );
		break;

	    case IDOK:
		if ( LookupExtFile( szAddExtName, szFileName, wAddType ) )
			{
			if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			    (LPTR)szAddExtName) == IDCANCEL)
				break;
			}
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }
    default:
	return( FALSE );
    }

return( TRUE );
}


/************************************************************************/
BOOL AddExtFile(lpExtName, lpFileName, wType)
/************************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
{
char buffer[256];
FNAME outfile, path, outpath;
int err;

if ( LookupExtFile(lpExtName, outfile, wType) )
	{ // If the name exists already...
	if ( StringsEqual( outfile, lpFileName ) )
		return( TRUE );
	DeleteExtFile(lpExtName, wType, YES);
	}
lstrcpy(outpath, lpFileName);
stripfile(outpath);

if (!GetPouchPath(path))
	return(FALSE);
if ( StringsEqual(outpath, path) )
	{
	lstrcpy(outfile, lpFileName);
	}
else	{ // If its not already in the pouch...
	if (!CreateFileName(lpExtName, lpFileName, 0, outfile))
		return(FALSE);
	if (!CopyFile(lpFileName, outfile))
		return(FALSE);
	}

WritePrivateProfileString( GetTypeString(wType), lpExtName, stripdir(outfile),
	path );
return( TRUE );
}


#ifdef CONVERTOVER

#define KEY_LENGTH	512   /* Allows for at least 20 keynames */
static BYTE KeyBuffer[KEY_LENGTH];
/************************************************************************/
BOOL LookupExtFile(lpExtName, lpFileName, wType)
/************************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
{
FNAME name, file_name;
WORD type;
LPTR lpName;

if ( !lstrlen( lpExtName ) )
	return( FALSE );

if (!GetPouchPath(name))
	return(FALSE);
// get all ext name for wType - loop on them
GetProfileString( GetTypeString(wType), NULL, "\0\0", KeyBuffer,
	KEY_LENGTH, name );
lpName = KeyBuffer;
while ( *lpName )
	{
	if ( StringsEqual( lpName, lpExtName ) )
		{
		GetProfileString( GetTypeString(wType), lpExtName, "\0",
			file_name, sizeof(STRING) ) )
		GetFullPathName(file_name);
		lstrcpy(lpFileName, file_name);
		return(TRUE);
		}
	lpName += (lstrlen(lpName) + 1);
	}

return(FALSE);
}

/************************************************************************/
BOOL DeleteExtFile(lpExtName, wType, fLinkedFiles)
/************************************************************************/
LPTR lpExtName;
WORD wType;
BOOL fLinkedFiles;
{
FNAME file_name, ext_name, name;
WORD type;
int iFileCount;

if ( !fLinkedFiles )
	{ // Check for the existance of a linked file
	while (type = GetExtNameData(ext_name, file_name))
		{ // see if we can find same name, different type
		if ( type == wType )
			continue;
		if (!StringsEqual(ext_name, lpExtName))
			continue;
		Print("Can't delete linked files");
		return(FALSE);
		}
	}

while (type = GetExtNameData(ext_name, file_name))
	{ // Delete all occurances
	if (!StringsEqual(ext_name, lpExtName))
		continue;
	if (!DeleteExtFileEntry(ext_name, type))
		return(FALSE);
	// Look for another usage of this file before deleting it
	GetFullPathName(file_name);
	strcpy(name, file_name);
	iFileCount = NO;
	while (GetExtNameData(ext_name, file_name))
		{
		GetFullPathName(file_name);
		if (StringsEqual(file_name, name))
			{
			iFileCount = YES;
			break;
			}
		}
	if (!iFileCount)
        	unlink(name);
	}

return(TRUE);
}

/************************************************************************/
BOOL RenameExtFile(lpExtName, lpNewExtName, wType, fLinkedFiles)
/************************************************************************/
LPTR lpExtName;
LPTR lpNewExtName;
WORD wType;
BOOL fLinkedFiles;
{
FNAME file_name, ext_name;
WORD type;

if ( !fLinkedFiles )
	{ // Check for the existance of a linked file
	while (type = GetExtNameData(ext_name, file_name))
		{ // see if we can find same name, different type
		if ( type == wType )
			continue;
		if (!StringsEqual(ext_name, lpExtName))
			continue;
		Print("Can't rename linked files");
		return(FALSE);
		}
	}

while (type = GetExtNameData(ext_name, file_name))
	{ // Rename all occurances
	if (!StringsEqual(ext_name, lpExtName))
		continue;
	GetFullPathName(file_name);
    	if ( !AddExtFile( lpNewExtName, file_name, type ) )
		return(FALSE);
    	if (!DeleteExtFileEntry( lpExtName, type ))
		return(FALSE);
    	}

return(TRUE);
}

/************************************************************************/
BOOL ListExtFiles(wType, lpAddFunction, lData)
/************************************************************************/
WORD wType;
LPROC lpAddFunction;
long lData;
{
FNAME file_name, ext_name;
WORD type;
int num_files = 0;

while (type = GetExtNameData(ext_name, file_name))
	{
	if ( type != wType )
		continue;
        (*lpAddFunction)((LPTR)ext_name, (LPTR)file_name, (WORD)type, lData);
        ++num_files;
	}

return( num_files != 0 );
}

#endif


/************************************************************************/
static WORD GetExtNameData(ext_name, file_name)
/************************************************************************/
char *ext_name, *file_name;
{
char buffer[256];
BOOL found = FALSE;
STRING TypeName;
char *p1, *p2, *p3;
FNAME name;
WORD wType;

if (!GetPouchPath(name))
	return(FALSE);
wType = 0; //???????
GetPrivateProfileString( GetTypeString(wType), ext_name, "", file_name,
	sizeof(STRING), name );
return( wType );
}

/************************************************************************/
static BOOL DeleteExtFileEntry( lpExtName, wType )
/************************************************************************/
LPTR lpExtName;
WORD wType;
{
FNAME name, oldname, path, ext_name, file_name;
WORD type;
FILE *newfp, *oldfp;
char buffer[256];

if (!GetPouchPath(name))
	return(FALSE);
WritePrivateProfileString( GetTypeString(wType), lpExtName, NULL, name );
return( TRUE );
}


/************************************************************************/
BOOL CreateFileName(lpExtName, lpFileName, wType, lpNewFileName)
/************************************************************************/
LPTR lpExtName, lpFileName;
int wType;
LPTR lpNewFileName;
{
FNAME szExtension;
char *p;
FILE *ofp;
FNAME outfile, num_string, path, outname;
int num, i, j, len;

if (!GetPouchPath(path))
	return(FALSE);

if (lpFileName) // If a file name was passed in...
	{
	lstrcpy(outname, stripdir(lpFileName));
	GetExtension(outname, szExtension);
	}
else	{
	lstrcpy(outname, stripdir(lpExtName));
	if ( !LookupExtension( wType, szExtension ) )
		return(FALSE);
	if ((p = strchr(szExtension, '.')))
		lstrcpy(szExtension, p);
	}

stripext(outname);
for (i = 0, j = 0; i < lstrlen(outname); ++i)
    if (!isspace(outname[i]))
	outname[j++] = outname[i];

num = 0;
num_string[0] = '\0';
len = lstrlen( outname );
while ( 1 )
	{
	while ( len && (len + lstrlen(num_string) > 8) )
		outname[--len] = '\0';
	if ( !len )
		return(FALSE);
	lstrcpy(outfile, path);
	lstrcat(outfile, outname);
	lstrcat(outfile, num_string);
	lstrcat(outfile, szExtension);
	if ( !(ofp = fopen(outfile, "r")) )
		break;
	fclose(ofp);
	sprintf(num_string, "%d", num);
	num++;
	}

lstrcpy(lpNewFileName, outfile);
return(TRUE);
}


/************************************************************************/
static VOID GetFullPathName(lpFileName)
/************************************************************************/
LPTR lpFileName;
{
FNAME path, file_name;

lstrcpy(file_name, lpFileName);
if (!strchr(file_name, '\\') && !strchr(file_name, ':'))
	{
	if (GetPouchPath(path))
		{
		strcat(path, file_name);
		strcpy(file_name, path);
		}
	}
lstrcpy(lpFileName, file_name);
}

/************************************************************************/
static LPTR stripfile( pathname )
/************************************************************************/
LPTR pathname;
{
LPTR ptr;

ptr = pathname;
ptr += lstrlen( pathname );
while( ptr >= pathname && *ptr != '\\' && *ptr != ':' )
	ptr--;
ptr++;
*ptr = '\0';
return( pathname );
}

/************************************************************************/
static VOID stripext(file_name)
/************************************************************************/
char *file_name;
{
char *p;

if ((p = strchr(file_name, '.')))
    *p = '\0';
}


/************************************************************************/
static BOOL GetExtension(file_name, extension)
/************************************************************************/
char *file_name;
char *extension;
{
char *p;

if ((p = strchr(file_name, '.')))
	{
	lstrcpy(extension, p);
	return(TRUE);
	}
else	{
	lstrcpy(extension, "");
	return(FALSE);
	}
}


/************************************************************************/
static BOOL GetPouchPath(path)
/************************************************************************/
char *path;
{
int length;

lstrcpy(path, Control.PouchPath);
if ( !(length = lstrlen(path)) )
	return(FALSE);
while (length && path[length-1] == '\n' || path[length-1] == '\r')
	path[--length] = '\0';
if (!length)
	return(FALSE);
if (path[length-1] != '\\')
	lstrcat(path, "\\");
return(TRUE);
}


/***********************************************************************/
static PTR GetTypeString( wType )
/***********************************************************************/
WORD wType;
{
switch ( wType )
    {
    case IDN_MAP:
	return( "Map" );
    case IDN_BGMAP:
	return( "BGMap" );
    case IDN_CALMAP:
	return( "CalMap" );
    case IDN_SCANMAP:
	return( "ScanMap" );
    case IDN_TEXTURE:
	return( "Texture" );
    case IDN_SMOOTH:
	return( "Smooth" );
    case IDN_SHARP:
	return( "Sharp" );
    case IDN_EDGE:
	return( "Edge" );
    case IDN_SPECIAL:
	return( "Special" );
    case IDN_SCANNER:
	return( "Scanner" );
    case IDN_GRABBER:
	return( "Grabber" );
    case IDN_STYLE:
	return( "Style" );
    case IDN_CLIPBOARD:
	return( "Clipboard" );
    case IDN_MASK:
	return( "Mask" );
    case IDN_CUSTOM:
	return( "Custom" );
    default:
	return( "Unknown" );
    }
}


/***********************************************************************/
static WORD GetTypeWord( lpType )
/***********************************************************************/
LPTR lpType;
{
if ( StringsEqual( lpType, "Map" ) )
	return( IDN_MAP );
if ( StringsEqual( lpType, "BGMap" ) )
	return( IDN_BGMAP );
if ( StringsEqual( lpType, "CalMap" ) )
	return( IDN_CALMAP );
if ( StringsEqual( lpType, "ScanMap" ) )
	return( IDN_SCANMAP );
if ( StringsEqual( lpType, "Texture" ) )
	return( IDN_TEXTURE );
if ( StringsEqual( lpType, "Smooth" ) )
	return( IDN_SMOOTH );
if ( StringsEqual( lpType, "Sharp" ) )
	return( IDN_SHARP );
if ( StringsEqual( lpType, "Edge" ) )
	return( IDN_EDGE );
if ( StringsEqual( lpType, "Special" ) )
	return( IDN_SPECIAL );
if ( StringsEqual( lpType, "Scanner" ) )
	return( IDN_SCANNER );
if ( StringsEqual( lpType, "Grabber" ) )
	return( IDN_GRABBER );
if ( StringsEqual( lpType, "Style" ) )
	return( IDN_STYLE );
if ( StringsEqual( lpType, "Clipboard" ) )
	return( IDN_CLIPBOARD );
if ( StringsEqual( lpType, "Mask" ) )
	return( IDN_MASK );
if ( StringsEqual( lpType, "Custom" ) )
	return( IDN_CUSTOM );
return( 1 ); // An invalid type
}


/***********************************************************************/
static BOOL IsCombo( hWnd )
/***********************************************************************/
HWND hWnd;
{
return( GetWindowLong( hWnd, GWL_STYLE ) & CBS_AUTOHSCROLL );
}


/***********************************************************************/
static void AddToComboBox(lpExtName, lpFileName, wType, lData)
/***********************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
long lData;
{
SendMessage( (HWND)lData, CB_ADDSTRING, 0, (long)lpExtName );
}


/***********************************************************************/
static void AddToListBox(lpExtName, lpFileName, wType, lData)
/***********************************************************************/
LPTR lpExtName, lpFileName;
WORD wType;
long lData;
{
SendMessage( (HWND)lData, LB_ADDSTRING, 0, (long)lpExtName );
}


